#pragma once

#include "PxPhysicsAPI.h"
#include <glm/glm/gtx/quaternion.hpp>

namespace god
{

	physx::PxTransform ConvertToPhysXTransform(const glm::vec3& translation, const glm::vec3& rotation);
	physx::PxMat44 ConvertToPhysXMatrix(const glm::mat4& matrix);
	const physx::PxVec3& ConvertToPhysXVector(const glm::vec3& vector);
	const physx::PxVec4& ConvertToPhysXVector(const glm::vec4& vector);
	physx::PxQuat ConvertToPhysXQuat(const glm::quat& quat);

	glm::mat4 ConvertToGlmTransform(const physx::PxTransform& transform);
	glm::mat4 ConvertToGlmMatrix(const physx::PxMat44& matrix);
	glm::vec3 ConvertToGlmVector(const physx::PxVec3& vector);
	glm::vec4 ConvertToGlmVector(const physx::PxVec4& vector);
	glm::quat ConvertToGlmQuat(const physx::PxQuat& quat);

	std::ostream& operator<<(std::ostream& os, const physx::PxVec3& v3);
}