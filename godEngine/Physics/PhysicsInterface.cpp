#pragma once
#include "../pch.h"
#include "PhysicsInterface.h"
#include "godPhysics.h"

namespace god
{
	using namespace physx;


	PhysicsSystem* PhysicsAPI::p_psys;

	void PhysicsAPI::SetForce(physx::PxRigidBody* rb, glm::vec3 const& force)
	{
		while (p_psys->GetisRunning())
			;

		rb->clearForce();
		rb->addForce(force);
		
	}

	physx::PxVec3 const& PhysicsAPI::GetPhysicsPos(physx::PxRigidBody* rb)
	{
		while (p_psys->GetisRunning())
			;
		PxVec3 a;
		a = rb->getGlobalPose().p;
		return a;
	}

	void PhysicsAPI::SetPhysicsPos(physx::PxRigidBody* rb, glm::vec3 const& pos, glm::vec3 const& rot)
	{
		while (p_psys->GetisRunning())
			;
		rb->setGlobalPose(ConvertToPhysXTransform(pos,rot));
	}

}