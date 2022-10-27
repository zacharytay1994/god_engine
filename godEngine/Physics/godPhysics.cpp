#include "../pch.h"
#include "godPhysics.h"
#include "../editor/engineresources.h"

#include <assert.h>
namespace god
{
	PhysicsSystem::PhysicsSystem() 
	{
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
		mScene->release();
		mDispatcher->release();
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
		// Fouundation (required)
		mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, mDefaultAllocatorCallback, mDefaultErrorCallback);
		if (!mFoundation) throw("PxCreateFoundation failed!");

		mWindow = window;
		mCamera = cam;

		CreatePVD();
	
		mToleranceScale.length = 1;        // typical length of an object
		mToleranceScale.speed = 98.1;         // typical speed of an object, gravity*1s is a reasonable choice
		mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, mToleranceScale, true, mPvd);


		if (!mPhysics)
			std::cerr << "Failed to Create PhysX Instance" << std::endl;
		else
		{
			mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, physx::PxCookingParams(mToleranceScale));
			if (!mCooking)
				std::cerr << "PxCreateCooking failed!" << std::endl;
			
			physx::PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
			sceneDesc.gravity = physx::PxVec3(0.0f, -98.11f, 0.0f);
			mDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
			if (!mDispatcher)
				std::cerr << "PxDefaultCpuDispatcherCreate failed!" << std::endl;


			sceneDesc.cpuDispatcher = mDispatcher;
			sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
			sceneDesc.broadPhaseType = physx::PxBroadPhaseType::eABP;//Automatic box pruning

			mScene = mPhysics->createScene(sceneDesc);

			SetupPVD();

			std::cout << "Bound Physics" << std::endl;
			
		}
	}

	void PhysicsSystem::Update(float dt , bool pause)
	{
		if (pause)
			return;
		//mStepSize is 1/60 Physics at 60fps by default
		mAccumulator += dt;
		if (mAccumulator < mStepSize)
			return;

		mStepSize = 1.0f / (numSteps * 60.0f);

		for (uint16_t i = 0; i < numSteps; ++i)
		{
			mAccumulator -= mStepSize;
			mScene->simulate(mStepSize);
			mScene->fetchResults(true);
			Raycast();
		}
		if (mAccumulator > 1.0f)
		{
			numSteps = 2;
		}
		else if (mAccumulator < 0.1f)
		{
			numSteps = 1;
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
		}
		else
		{
			mRayCastMouse = nullptr;
		}
	}

	physx::PxRigidActor* const PhysicsSystem::GetRayCastMouse() const
	{
		return mRayCastMouse;
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



}