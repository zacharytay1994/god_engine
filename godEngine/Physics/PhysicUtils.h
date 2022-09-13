#pragma once

#include "PxPhysicsAPI.h"
#include "../EnttXSol/EngineComponents/EC_Transform.h"
#define BIT(x) (1u << x)

namespace god
{
	
	//enum class CookingResult
	//{
	//	Success,
	//	TestZeroAreaFailed,
	//	ReachedPolygonLimit,
	//	LargeTriangle,
	//	Failure
	//};

	enum class ForceMode : uint8_t
	{
		Force = 0,
		Impulse,
		VelocityChange,
		Acceleration
	};

	enum class ActorLockFlag
	{
		TranslationX = BIT(0), TranslationY = BIT(1), TranslationZ = BIT(2), Translation = TranslationX | TranslationY | TranslationZ,
		RotationX = BIT(3), RotationY = BIT(4), RotationZ = BIT(5), Rotation = RotationX | RotationY | RotationZ
	};

	physx::PxTransform ConvertToPhysXTransform(const Transform& transform);
	//PxTransform ConvertToPhysXTransform(const glm::mat4& transform);
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

	/*CookingResult FromPhysXCookingResult(PxConvexMeshCookingResult::Enum cookingResult);
	CookingResult FromPhysXCookingResult(PxTriangleMeshCookingResult::Enum cookingResult);*/
}