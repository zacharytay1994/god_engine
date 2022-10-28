#pragma once



#include "PxPhysicsAPI.h"
#include "PhysicUtils.h"
#include "../PhysX/physx/snippets/snippetutils/SnippetUtils.h"
#include <godCamera/Camera.h>
#include "../Window/GLFWWindow.h"


#define PVD_HOST "127.0.0.1"


namespace god
{
	using namespace physx;


	struct PhysicsSystem
	{
	public:

		PhysicsSystem();
		~PhysicsSystem();
		void Init(GLFWWindow* window, Camera* cam);
		void Update(float dt, bool pause);
		
		void Raycast();
		physx::PxRigidActor* const GetRayCastMouse() const;
		bool GetisRunning() const;
		physx::PxPhysics* const GetPhysics() const;
		physx::PxCooking* const GetCooking() const;
		physx::PxScene* const GetPhysicsScene() const;

		bool debugdraw;
	private:

		GLFWWindow* mWindow; 
		Camera* mCamera;
		

		//PhysX Visual Debugger
		void CreatePVD();
		void SetupPVD();

		physx::PxRigidActor* mRayCastMouse;
		

		physx::PxDefaultAllocator      mDefaultAllocatorCallback;
		physx::PxDefaultErrorCallback  mDefaultErrorCallback;

		physx::PxDefaultCpuDispatcher* mDispatcher;
		physx::PxTolerancesScale       mToleranceScale;

		physx::PxFoundation* mFoundation ;
		physx::PxCooking* mCooking;
		physx::PxPhysics* mPhysics;

		physx::PxScene* mScene;
		

		physx::PxPvd* mPvd;

		bool mRunning;
		float mAccumulator = 0.0f;
		uint16_t numSteps = 1;
		float mStepSize = 1.0f / 60.0f;
		

	};


	


}

