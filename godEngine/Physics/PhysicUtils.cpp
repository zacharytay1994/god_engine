#include "../pch.h"
#include "PhysicUtils.h"
#include <glm/glm/gtx/quaternion.hpp>

namespace god
{
	PxTransform ConvertToPhysXTransform(const Transform& transform)
	{
		PxQuat rot = ConvertToPhysXQuat(glm::quat(transform.m_rotation));
		PxVec3 vec = ConvertToPhysXVector(transform.m_position);
		return physx::PxTransform(vec, rot);
	}

	PxTransform ConvertToPhysXTransform(const glm::vec3& translation, const glm::vec3& rotation)
	{
		return PxTransform(ConvertToPhysXVector(translation), ConvertToPhysXQuat(glm::quat(rotation)));
	}
	PxMat44 ConvertToPhysXMatrix(const glm::mat4& matrix)
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
	PxQuat ConvertToPhysXQuat(const glm::quat& quat)
	{
		return physx::PxQuat(quat.x, quat.y, quat.z, quat.w);
	}
	glm::mat4 ConvertToGlmTransform(const PxTransform& transform)
	{
		glm::quat rotation = ConvertToGlmQuat(transform.q);
		glm::vec3 position = ConvertToGlmVector(transform.p);
		return glm::translate(glm::mat4(1.0F), position) * glm::toMat4(rotation);
	}
	glm::mat4 ConvertToGlmMatrix(const PxMat44& matrix)
	{
		return *(glm::mat4*)&matrix;
	}
	glm::vec3 ConvertToGlmVector(const PxVec3& vector)
	{
		return *(glm::vec3*)&vector;
	}
	glm::vec4 ConvertToGlmVector(const PxVec4& vector)
	{
		return *(glm::vec4*)&vector;
	}
	glm::quat ConvertToGlmQuat(const PxQuat& quat)
	{
		return *(glm::quat*)&quat;
	}
}