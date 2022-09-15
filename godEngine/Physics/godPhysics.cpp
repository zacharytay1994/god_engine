#include "../pch.h"
#include "godPhysics.h"
#include <assert.h>
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
		mPhysics->release();
		mFoundation->release();

	}
	void PhysicsSystem::Init()
	{
		// Fouundation (required)
		mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, mDefaultAllocatorCallback, mDefaultErrorCallback);
		if (!mFoundation) throw("PxCreateFoundation failed!");


		//PVD
		mPvd = PxCreatePvd(*mFoundation);
		physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
		mPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);



	
		mToleranceScale.length = 1;        // typical length of an object
		mToleranceScale.speed = 98.1;         // typical speed of an object, gravity*1s is a reasonable choice
		mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, mToleranceScale, true, mPvd);
		if (!mPhysics)
			std::cerr << "Failed to Create PhysX Instance" << std::endl;



		physx::PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
		sceneDesc.gravity = physx::PxVec3(0.0f, 0.0f, -9.81f);
		mDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
		if (!mDispatcher)
			std::cerr << "PxDefaultCpuDispatcherCreate failed!" << std::endl;


		sceneDesc.cpuDispatcher = mDispatcher;
		sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
		sceneDesc.broadPhaseType = physx::PxBroadPhaseType::eABP;
		//Automatic box pruning
		mScene = mPhysics->createScene(sceneDesc);


		// create table material
		mMaterial = mPhysics->createMaterial(0.3f, 0.3f, 0.7f);
		//create plane
		physx::PxRigidStatic* groundPlane = PxCreatePlane(*mPhysics, physx::PxPlane(0, 0, 1, 50), *mMaterial);
		mScene->addActor(*groundPlane);

		
		//Shape can be attached to multiple actors
		//Shape needs to contain PxGeometry and ref to PxMaterial
		physx::PxShape* tableshape = mPhysics->createShape(physx::PxBoxGeometry(500.f, 500.f, 2.f), *mMaterial);
		
		//physx::PxRigidActorExt::createExclusiveShape()
		//table
		physx::PxTransform t(physx::PxVec3(0));
		physx::PxRigidStatic* table = mPhysics->createRigidStatic(physx::PxTransform(0.f, 10.f, 0.f));
		table->attachShape(*tableshape);
		
		mScene->addActor(*table);
		tableshape->release();

		//dice
		// create dice material
		physx::PxMaterial* DiceMaterial = mPhysics->createMaterial(0.3f, 0.3f, 0.8f);
		physx::PxShape* diceshape = mPhysics->createShape(physx::PxBoxGeometry(15.f, 15.f, 15.f), *DiceMaterial);
		

		physx::PxRigidDynamic* body = mPhysics->createRigidDynamic(physx::PxTransform(0.f, 0.f, 200.f));

		body->setAngularVelocity(physx::PxVec3(1.4f,1.4f,1.4f), true);
		body->attachShape(*diceshape);
		physx::PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
		mScene->addActor(*body);

		diceshape->release();

		std::cout << "Physics Initialised" << std::endl;


	}

	void PhysicsSystem::Update(float dt)
	{


		mAccumulator += dt;
		if (mAccumulator < mStepSize)
			return;

		mAccumulator -= mStepSize;

		mScene->simulate(mStepSize);
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
	/*void PhysicsSystem::CreateActors(Scene& scene)
	{
	}*/

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

}