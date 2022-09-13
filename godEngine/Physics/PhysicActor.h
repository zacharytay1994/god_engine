#pragma once

#include "PhysicUtils.h"
#include "../EnttXSol/Internal/godEntity.h"
#include "../EnttXSol/EngineComponents/EC_RigidBody.h"

namespace god
{
	class PhysicActor
	{
	public:
		PhysicActor(Entity_ entity);
		~PhysicActor();

		glm::vec3 getTranslation() const;
		glm::vec3 GetRotation() const;
		void setTranslation(const glm::vec3& translation, bool autowake = true);
		void SetRotation(const glm::vec3& rotation, bool autowake = true);
		
		void Rotate(const glm::vec3& rotation, bool autowake = true);

		void WakeUp();
		void PutToSleep();

		float GetMass() const;
		void SetMass(float mass);

		void AddForce(const glm::vec3& force, ForceMode forceMode);
		void AddTorque(const glm::vec3& torque, ForceMode forceMode);

		glm::vec3 GetLinearVelocity() const;
		void SetLinearVelocity(const glm::vec3& velocity);
		glm::vec3 GetAngularVelocity() const;
		void SetAngularVelocity(const glm::vec3& velocity);

		float GetMaxLinearVelocity() const;
		void SetMaxLinearVelocity(float maxVelocity);
		float GetMaxAngularVelocity() const;
		void SetMaxAngularVelocity(float maxVelocity);

		void SetLinearDrag(float drag) const;
		void SetAngularDrag(float drag) const;

		glm::vec3 GetKinematicTargetPosition() const;
		glm::vec3 GetKinematicTargetRotation() const;
		void SetKinematicTarget(const glm::vec3& targetPosition, const glm::vec3& targetRotation) const;
	private:
		Entity_ m_entity;
		RigidBody m_rigidBody;
		unsigned int m_flag = 0;

		physx::PxRigidActor* m_rigidActor;

	};
}

