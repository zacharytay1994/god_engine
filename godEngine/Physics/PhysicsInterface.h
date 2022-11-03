#pragma once
#include "PxPhysicsAPI.h"
namespace god
{
	using namespace physx;
	class PhysicsSystem;
	class PhysicsAPI
	{

	public:
		physx::PxVec3 const& GetForce(physx::PxRigidBody* rb);
		physx::PxVec3 const& SetForce(physx::PxRigidBody* rb);

		physx::PxVec3 const& GetPhysicsPos(physx::PxRigidBody* rb);
		physx::PxVec3 const& SetPhysicsPos(physx::PxRigidBody* rb);


		PhysicsAPI();
		void init(PhysicsSystem* psys);
		~PhysicsAPI();
	private:

		PhysicsSystem* mPhysics;
	};








}