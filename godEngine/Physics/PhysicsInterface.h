#pragma once
#include "PxPhysicsAPI.h"
namespace god
{
	using namespace physx;
	class PhysicsSystem;
	class PhysicsAPI
	{

	public:
		
		static void SetForce(physx::PxRigidBody* rb, glm::vec3 const& force);
		static physx::PxVec3 const& GetPhysicsPos(physx::PxRigidBody* rb);
		static void SetPhysicsPos(physx::PxRigidBody* rb, glm::vec3 const& pos, glm::vec3 const& rot);

		static PhysicsSystem* p_psys;

	private:

		
	};








}