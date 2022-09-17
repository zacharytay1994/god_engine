#pragma once

#include "PxPhysicsAPI.h"
#include <map>

#define PVD_HOST "127.0.0.1"


namespace god
{
	struct PhysicsSystem
	{
	public:

		PhysicsSystem();
		~PhysicsSystem();
		 void Init();
		 void Update(float fps);




		 void SetupPVD();//PhysX Visual Debugger


		 physx::PxPhysics* const GetPhysics() const;
		 physx::PxScene* const GetPhysicsScene() const;
		 std::map < std::string, physx::PxMaterial*>& GetMaterialContainer();

		 
	private:
		std::map < std::string, physx::PxMaterial*> MaterialContainer;

		physx::PxDefaultAllocator      mDefaultAllocatorCallback;
		physx::PxDefaultErrorCallback  mDefaultErrorCallback;
		physx::PxDefaultCpuDispatcher* mDispatcher;
		physx::PxTolerancesScale       mToleranceScale;

		physx::PxFoundation* mFoundation ;
		physx::PxPhysics* mPhysics;

		physx::PxScene* mScene;
		physx::PxMaterial* mMaterial;

		physx::PxPvd* mPvd;


		float mAccumulator = 0.0f;
		float mStepSize = 1.0f / 60.0f;

	};

}

