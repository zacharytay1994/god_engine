#pragma once
#include "../pch.h"
#include "PhysicsInterface.h"
#include "godPhysics.h"

namespace god
{
	using namespace physx;


	

	PhysicsAPI::PhysicsAPI()
	{
		mPhysics = nullptr;
	}

	void PhysicsAPI::init(PhysicsSystem* psys)
	{
		mPhysics = psys;
	}

	PhysicsAPI::~PhysicsAPI()
	{
	}

	physx::PxVec3 const& PhysicsAPI::GetForce(physx::PxRigidBody* rb)
	{
		PxVec3 a;
		return a;
	}

	physx::PxVec3 const& PhysicsAPI::SetForce(physx::PxRigidBody* rb)
	{
		PxVec3 a;
		return a;
	}

	physx::PxVec3 const& PhysicsAPI::GetPhysicsPos(physx::PxRigidBody* rb)
	{
		PxVec3 a;
		return a;
	}

	physx::PxVec3 const& PhysicsAPI::SetPhysicsPos(physx::PxRigidBody* rb)
	{
		PxVec3 a;
		return a;
	}

}