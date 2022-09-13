#include "../pch.h"
#include "PhysicUtils.h"
#include <glm/glm/gtx/quaternion.hpp>

namespace god
{
	physx::PxTransform ConvertToPhysXTransform(const Transform& transform)
	{
		physx::PxQuat rot = ConvertToPhysXQuat(glm::quat(transform.m_rotation));
		physx::PxVec3 vec = ConvertToPhysXVector(transform.m_position);
		return physx::PxTransform(vec, rot);
	}

	physx::PxTransform ConvertToPhysXTransform(const glm::vec3& translation, const glm::vec3& rotation)
	{
		return physx::PxTransform(ConvertToPhysXVector(translation), ConvertToPhysXQuat(glm::quat(rotation)));
	}
	physx::PxMat44 ConvertToPhysXMatrix(const glm::mat4& matrix)
	{
		return *(physx::PxMat44*)&matrix;
	}
	const physx::PxVec3& ConvertToPhysXVector(const glm::vec3& vector)
	{
		return *(physx::PxVec3*)&vector;
	}
	const physx::PxVec4& ConvertToPhysXVector(const glm::vec4& vector)
	{
		return *(physx::PxVec4*)&vector;
	}
	physx::PxQuat ConvertToPhysXQuat(const glm::quat& quat)
	{
		return physx::PxQuat(quat.x, quat.y, quat.z, quat.w);
	}
	glm::mat4 ConvertToGlmTransform(const physx::PxTransform& transform)
	{
		glm::quat rotation = ConvertToGlmQuat(transform.q);
		glm::vec3 position = ConvertToGlmVector(transform.p);
		return glm::translate(glm::mat4(1.0F), position) * glm::toMat4(rotation);
	}
	glm::mat4 ConvertToGlmMatrix(const physx::PxMat44& matrix)
	{
		return *(glm::mat4*)&matrix;
	}
	glm::vec3 ConvertToGlmVector(const physx::PxVec3& vector)
	{
		return *(glm::vec3*)&vector;
	}
	glm::vec4 ConvertToGlmVector(const physx::PxVec4& vector)
	{
		return *(glm::vec4*)&vector;
	}
	glm::quat ConvertToGlmQuat(const physx::PxQuat& quat)
	{
		return *(glm::quat*)&quat;
	}
}