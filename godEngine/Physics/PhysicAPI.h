#pragma once

#include "PxPhysicsAPI.h"

namespace god
{
	class PhysicErrorCallback : public physx::PxErrorCallback
	{
	public:
		virtual void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) override;
	};

	class PhysicAssertHandler : public physx::PxAssertHandler
	{
		virtual void operator()(const char* exp, const char* file, int line, bool& ignore);
	};

	struct PhysicData
	{
		physx::PxFoundation*			m_PhysXFoundation;
		physx::PxDefaultCpuDispatcher*	m_PhysXCpuDispatcher;
		physx::PxPhysics*				m_PhysX;

		physx::PxDefaultAllocator		Allocator;
		PhysicErrorCallback				ErrorCallback;
		PhysicAssertHandler				AssertHandler;
	};

	class PhysicAPI
	{
	public:
		static void Init();
		static void Shutdown();

		static physx::PxFoundation& GetFoundation();
		static physx::PxPhysics& GetPhysX();
		static physx::PxCpuDispatcher* GetCpuDispatcher();
		static physx::PxDefaultAllocator& GetAllocator();

		static physx::PxFilterFlags FilterShader(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0, physx::PxFilterObjectAttributes attributes1,
			physx::PxFilterData filterData1, physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize);

		//static physx::PxBroadPhaseType::Enum HazelToPhysXBroadphaseType(BroadphaseType type);
		//static physx::PxFrictionType::Enum HazelToPhysXFrictionType(FrictionType type);
	};
}
