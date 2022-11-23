#include "../pch.h"
#include "godPhysics.h"
#include "../Editor/EngineResources.h"
#include "../EnttXSol/EngineComponents/EC_All.h"


#include <assert.h>
namespace god
{

	//physx::PxSimulationFilterShader

	
	PxFilterFlags contactReportFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
		PxFilterObjectAttributes attributes1, PxFilterData filterData1,
		PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
	{
		PX_UNUSED(attributes0);
		PX_UNUSED(attributes1);
		PX_UNUSED(filterData0);
		PX_UNUSED(filterData1);
		PX_UNUSED(constantBlockSize);
		PX_UNUSED(constantBlock);

		// let triggers through
		if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
		{
			pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
			return PxFilterFlag::eDEFAULT;
		}
		// generate contacts for all that were not filtered above
		pairFlags = PxPairFlag::eCONTACT_DEFAULT;

			
		// all initial and persisting reports for everything, with per-point data
		pairFlags = PxPairFlag::eSOLVE_CONTACT | PxPairFlag::eDETECT_DISCRETE_CONTACT
			| PxPairFlag::eNOTIFY_TOUCH_FOUND
			| PxPairFlag::eNOTIFY_TOUCH_PERSISTS
			| PxPairFlag::eNOTIFY_CONTACT_POINTS;
		return PxFilterFlag::eDEFAULT;
	}

	PhysicsSystem::PhysicsSystem() 

	{
		mStepSize = 1.f / 30.f;

		RayCastid = Null;
		mRunning = false;
		mCamera = nullptr;
		mWindow = nullptr;
		mDispatcher = nullptr;
		mFoundation = nullptr;
		mCooking = nullptr;
		mPhysics = nullptr;
		mRayCastMouse = nullptr;
		mScene = nullptr;
		mPvd = nullptr;

		debugdraw = false;

		std::cout << "Physics Constructed" << std::endl;
	}

	PhysicsSystem::~PhysicsSystem()
	{
		while (mRunning)
			;

		callbackFinishTask.free();
		mRayCastMouse = nullptr;
		mScene->flushSimulation(false);
		mScene->release();
		mDispatcher->release();
		//PxCloseExtensions();
		mPhysics->release();
		mCooking->release();
		if (mPvd)
		{
			physx::PxPvdTransport* transport = mPvd->getTransport();
			mPvd->release();
			mPvd = nullptr;
			transport->release();
		}
		
		mFoundation->release();
		
	}

	void PhysicsSystem::Init(GLFWWindow* window, Camera* cam)
	{
		
		// Foundation (required)
		mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, mDefaultAllocatorCallback, mDefaultErrorCallback);
		if (!mFoundation) throw("PxCreateFoundation failed!");

		mWindow = window;
		mCamera = cam;

		CreatePVD();
	
		mToleranceScale.length = 1;        // typical length of an object
		mToleranceScale.speed = static_cast<physx::PxReal>(9.81);         // typical speed of an object, gravity*1s is a reasonable choice
		mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, mToleranceScale, true, mPvd);


		if (!mPhysics)
			std::cerr << "Failed to Create PhysX Instance" << std::endl;
		else
		{
			mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, physx::PxCookingParams(mToleranceScale));
			if (!mCooking)
				std::cerr << "PxCreateCooking failed!" << std::endl;
			
			physx::PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
			sceneDesc.gravity = physx::PxVec3(0.0f, -9.81, 0.0f);
			mDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
			if (!mDispatcher)
				std::cerr << "PxDefaultCpuDispatcherCreate failed!" << std::endl;

			
			sceneDesc.cpuDispatcher = mDispatcher;
			sceneDesc.filterShader = contactReportFilterShader;
			sceneDesc.broadPhaseType = physx::PxBroadPhaseType::eABP;//Automatic box pruning

			mScene = mPhysics->createScene(sceneDesc);
			mScene->setSimulationEventCallback(&gContactReportCallback);
			

			SetupPVD();
			
			PhysicsAPI::p_psys = this;

			std::cout << "Bound Physics" << std::endl;
			
		}
	}

	void PhysicsSystem::SimpleMovingAverageDeltaTime(float dt)
	{
		sma_dt[sma_index++] = dt;
		if (sma_index == sma_dt.size())
			sma_index = 0;

		mStepSize = (sma_dt[0] + sma_dt[1] + sma_dt[2] + sma_dt[3] + sma_dt[4]) / 5.f;

	}

	void PhysicsSystem::CalculateNumSteps()
	{

		if (mAccumulator < 1.0f)
		{
			numSteps = 1;
		}
		
		if (mAccumulator > 2.0f)
		{
			numSteps = 2;
		}
		if (mAccumulator > 3.0f)
		{
			numSteps = 3;
		}
		

		if(mAccumulator>5.0f)
		{
			mStepSize = 1.f/15.f;
		}
	}
	void PhysicsSystem::Update(float dt , bool pause)
	{
		if (!mWindow->WindowsMinimized())
			Raycast();
		if (pause)
			return;
	
		mAccumulator += dt;
		if (mAccumulator < mStepSize)
			return;


		CalculateNumSteps();
		for (uint16_t i = 0; i < numSteps; ++i)
		{
			//mAccumulator -= dt;
			mRunning = true;
			mScene->simulate(mStepSize);

			//Call fetchResultsStart. Get the set of pair headers
			const physx::PxContactPairHeader* pairHeader;
			physx::PxU32 nbContactPairs;

			mScene->fetchResultsStart(pairHeader, nbContactPairs, true);

			//Set up continuation task to be run after callbacks have been processed in parallel
			callbackFinishTask.setContinuation(*mScene->getTaskManager(), NULL);
			callbackFinishTask.reset();

			//process the callbacks
			mScene->processCallbacks(&callbackFinishTask);

			callbackFinishTask.removeReference();

			callbackFinishTask.wait();

			mScene->fetchResultsFinish();
			mRunning = false;		
		}

		
	}

	void PhysicsSystem::CreatePVD()
	{
		//PVD
		mPvd = PxCreatePvd(*mFoundation);
		physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
		mPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
	}

	void PhysicsSystem::SetupPVD()
	{
		physx::PxPvdSceneClient* pvdClient = mScene->getScenePvdClient();
		if (pvdClient)
		{
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		}
	}

	void PhysicsSystem::Raycast()
	{
		glm::vec3 ray_dir = ViewportToWorldRay(
			{ mWindow->ViewportMouseX(), mWindow->ViewportMouseY() },
			mWindow->GetWindowWidth(),
			mWindow->GetWindowHeight(),
			mCamera->GetPerpectiveProjectionMatrix(),
			mCamera->GetCameraViewMatrix());

		
		physx::PxVec3 origin = mCamera->m_position;                 // [in] Ray origin
		physx::PxVec3 unitDir = ray_dir;                // [in] Normalized ray direction
		physx::PxReal maxDistance = 1000.f;            // [in] Raycast max distance
		physx::PxRaycastBuffer hit;                 // [out] Raycast results
		
		// Raycast against all static & dynamic objects (no filtering)
		// The main result from this call is the closest hit, stored in the 'hit.block' structure
		bool status = mScene->raycast(origin, unitDir, maxDistance, hit);
		if (hit.hasBlock)
		{
			mRayCastMouse = hit.block.actor;
			//SetRCMid(reinterpret_cast<EntityData*>(mRayCastMouse->userData)->m_id);
		}
		else
		{
			mRayCastMouse = nullptr;
			//SetRCMid(Null);
		}
	}

	physx::PxRigidActor* const PhysicsSystem::GetRayCastMouse() const
	{
		return mRayCastMouse;
	}

	const uint32_t PhysicsSystem::getRCMid() const
	{

		if (!mRayCastMouse)
			return Null;
		else
			return RayCastid;
		 
	}

	void PhysicsSystem::setRCMid(uint32_t id)
	{
		RayCastid = id;
	}

	bool PhysicsSystem::GetisRunning() const
	{


		return mRunning;
	}

	physx::PxPhysics* const PhysicsSystem::GetPhysics() const
	{
		return mPhysics;
	}


	physx::PxCooking* const PhysicsSystem::GetCooking() const
	{
		return mCooking;
	}

	physx::PxScene* const PhysicsSystem::GetPhysicsScene() const
	{
		return mScene;
	}

	ContactReportCallback& PhysicsSystem::getContactReportCallback()
	{
		return gContactReportCallback;
	}



}//god