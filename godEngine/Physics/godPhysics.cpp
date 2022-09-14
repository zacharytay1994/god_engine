#include "../pch.h"
#include "godPhysics.h"

namespace god
{
	PhysicsSystem::PhysicsSystem()
	{
		mDispatcher = NULL;
		physx::PxFoundation* mFoundation = NULL;
		physx::PxPhysics* mPhysics = NULL;

		physx::PxScene* mScene = NULL;
		physx::PxMaterial* mMaterial = NULL;

		physx::PxPvd* mPvd = NULL;

		std::cout << "Physics Constructed" << std::endl;
	}
	PhysicsSystem::~PhysicsSystem()
	{
	}
	void PhysicsSystem::Init()
	{
		// init physx
		mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, mDefaultAllocatorCallback, mDefaultErrorCallback);
		if (!mFoundation) throw("PxCreateFoundation failed!");
		mPvd = PxCreatePvd(*mFoundation);
		physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
		mPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
		//mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, PxTolerancesScale(),true, mPvd);
		mToleranceScale.length = 100;        // typical length of an object
		mToleranceScale.speed = 981;         // typical speed of an object, gravity*1s is a reasonable choice
		mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, mToleranceScale, true, mPvd);
		//mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, mToleranceScale);

		physx::PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
		sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
		mDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
		sceneDesc.cpuDispatcher = mDispatcher;
		sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
		mScene = mPhysics->createScene(sceneDesc);



		// create simulation
		mMaterial = mPhysics->createMaterial(0.5f, 0.5f, 0.6f);
		physx::PxRigidStatic* groundPlane = PxCreatePlane(*mPhysics, physx::PxPlane(0, 1, 0, 50), *mMaterial);
		mScene->addActor(*groundPlane);

		float halfExtent = .5f;
		physx::PxShape* shape = mPhysics->createShape(physx::PxBoxGeometry(halfExtent, halfExtent, halfExtent), *mMaterial);
		physx::PxU32 size = 30;
		physx::PxTransform t(physx::PxVec3(0));
		for (physx::PxU32 i = 0; i < size; i++) {
			for (physx::PxU32 j = 0; j < size - i; j++) {
				physx::PxTransform localTm(physx::PxVec3(physx::PxReal(j * 2) - physx::PxReal(size - i), physx::PxReal(i * 2 + 1), 0) * halfExtent);
				physx::PxRigidDynamic* body = mPhysics->createRigidDynamic(t.transform(localTm));
				body->attachShape(*shape);
				physx::PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
				mScene->addActor(*body);
			}
		}
		shape->release();

		std::cout << "Physics Initialised" << std::endl;


	}

	void PhysicsSystem::Update(float fps)
	{
		mScene->simulate(1.0f / fps);
		mScene->fetchResults(true);

	}


	void PhysicsSystem::Shutdown()
	{
	}
	void PhysicsSystem::CreateScene()
	{
	}
	void PhysicsSystem::DestroyScene()
	{
	}
	void PhysicsSystem::CreateActors(Scene& scene)
	{
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

}