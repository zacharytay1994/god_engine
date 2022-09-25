#include "../pch.h"
#include "godPhysics.h"
#include <assert.h>
namespace god
{
	PhysicsSystem::PhysicsSystem() 
	{
		mDispatcher = nullptr;
		mFoundation = nullptr;
		mPhysics = nullptr;

		mScene = nullptr;
		mPvd = nullptr;

		std::cout << "Physics Constructed" << std::endl;
	}
	PhysicsSystem::~PhysicsSystem()
	{
		mScene->release();
		mDispatcher->release();
		mPhysics->release();

		if (mPvd)
		{
			physx::PxPvdTransport* transport = mPvd->getTransport();
			mPvd->release();
			mPvd = nullptr;
			transport->release();
		}
		mFoundation->release();

	}
	void PhysicsSystem::Init()
	{
		// Fouundation (required)
		mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, mDefaultAllocatorCallback, mDefaultErrorCallback);
		if (!mFoundation) throw("PxCreateFoundation failed!");

		CreatePVD();
	
		mToleranceScale.length = 1;        // typical length of an object
		mToleranceScale.speed = 98.1;         // typical speed of an object, gravity*1s is a reasonable choice
		mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, mToleranceScale, true, mPvd);
		if (!mPhysics)
			std::cerr << "Failed to Create PhysX Instance" << std::endl;
		else
		{


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


			std::cout << "Physics Initialised" << std::endl;
			

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

	physx::PxPhysics* const PhysicsSystem::GetPhysics() const
	{
		return mPhysics;
	}

	physx::PxScene* const PhysicsSystem::GetPhysicsScene() const
	{
		return mScene;
	}

	std::map<std::string, physx::PxMaterial*>& PhysicsSystem::GetMaterialContainer()
	{
		return MaterialContainer;
	}

}