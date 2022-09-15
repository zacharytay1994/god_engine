#pragma once

#include "PxPhysicsAPI.h"
//#include "PhysicScene.h"
//#include "PhysicSettings.h"
//#include "PhysicActor.h"

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






		 void Shutdown();

		 //PhysicScene& getScene() { return m_scene; }

		 void CreateScene();
		 void DestroyScene();

		 //void CreateActors(Scene& scene);
		 /*PhysicActor& CreateActor(Entity_ entity);

		 PhysicSettings& getSettings() { return m_settings; }*/

		 void SetupPVD();
		 physx::PxPhysics* const GetPhysics() const;
		 //PhysX Visual Debugger
	private:
		 /*PhysicScene m_scene;
		 PhysicSettings m_settings;*/

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

