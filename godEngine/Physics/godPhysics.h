#pragma once

#include "PxPhysicsAPI.h"
#include "PhysicUtils.h"
#include "Callbacks.h"
#include "PhysicsInterface.h"

#include "../snippetutils/SnippetUtils.h"
#include <godCamera/Camera.h>
#include "../Window/GLFWWindow.h"


#define PVD_HOST "127.0.0.1"


namespace god
{
	using namespace physx;



	struct PhysicsSystem
	{
		static constexpr uint32_t Null = static_cast<uint32_t>(-1);
	public:

		PhysicsSystem();
		~PhysicsSystem();
		void Init(GLFWWindow* window, Camera* cam);
		void Update(float dt, bool pause);
		
		
		physx::PxRigidActor* const GetRayCastMouse() const;
		void SetRCMid(uint32_t id);
		uint32_t getRCMid();

		bool GetisRunning() const;
		physx::PxPhysics* const GetPhysics() const;
		physx::PxCooking* const GetCooking() const;
		physx::PxScene* const GetPhysicsScene() const;

		
		ContactReportCallback& getCRCB();

		bool debugdraw;
	private:
	

		
		void Raycast();

		//PhysX Visual Debugger
		void CreatePVD();
		void SetupPVD();


		// Data members

		uint32_t RayCastid;
		ContactReportCallback gContactReportCallback;

		CallbackFinishTask callbackFinishTask;

		GLFWWindow* mWindow;
		Camera* mCamera;


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

