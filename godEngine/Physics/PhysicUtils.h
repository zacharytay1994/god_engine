#pragma once

#include "PxPhysicsAPI.h"
#pragma warning(disable: 4201)
#include <glm/glm/gtx/quaternion.hpp>
#pragma warning(default: 4201)

namespace god
{

	physx::PxTransform ConvertToPhysXTransform(const glm::vec3& translation, const glm::vec3& rotation);
	physx::PxMat44 ConvertToPhysXMatrix(const glm::mat4& matrix);
	physx::PxVec3 ConvertToPhysXVector(const glm::vec3& vector);
	physx::PxVec4 ConvertToPhysXVector(const glm::vec4& vector);
	physx::PxQuat ConvertToPhysXQuat(const glm::quat& quat);

	glm::mat4 ConvertToGlmTransform(const physx::PxTransform& transform);
	glm::mat4 ConvertToGlmMatrix(const physx::PxMat44& matrix);
	glm::vec3 ConvertToGlmVector(const physx::PxVec3& vector);
	glm::vec4 ConvertToGlmVector(const physx::PxVec4& vector);
	glm::quat ConvertToGlmQuat(const physx::PxQuat& quat);


	std::ostream& operator<<(std::ostream& os, const physx::PxVec2& v2);
	std::ostream& operator<<(std::ostream& os, const physx::PxVec3& v3);
	std::ostream& operator<<(std::ostream& os, const physx::PxVec4& v3);
}