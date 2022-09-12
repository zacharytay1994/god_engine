#pragma once

#include "PxPhysicsAPI.h"
#include "../EnttXSol/EngineComponents/EC_Transform.h"
#define BIT(x) (1u << x)

using namespace physx;

namespace god
{
	
	enum class CookingResult
	{
		Success,
		TestZeroAreaFailed,
		ReachedPolygonLimit,
		LargeTriangle,
		Failure
	};

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

	physx::PxTransform ToPhysXTransform(const Transform& transform);
	physx::PxTransform ToPhysXTransform(const glm::mat4& transform);
	physx::PxTransform ToPhysXTransform(const glm::vec3& translation, const glm::vec3& rotation);
	physx::PxMat44 ToPhysXMatrix(const glm::mat4& matrix);
	const physx::PxVec3& ToPhysXVector(const glm::vec3& vector);
	const physx::PxVec4& ToPhysXVector(const glm::vec4& vector);
	physx::PxQuat ToPhysXQuat(const glm::quat& quat);

	glm::mat4 FromPhysXTransform(const physx::PxTransform& transform);
	glm::mat4 FromPhysXMatrix(const physx::PxMat44& matrix);
	glm::vec3 FromPhysXVector(const physx::PxVec3& vector);
	glm::vec4 FromPhysXVector(const physx::PxVec4& vector);
	glm::quat FromPhysXQuat(const physx::PxQuat& quat);

	CookingResult FromPhysXCookingResult(physx::PxConvexMeshCookingResult::Enum cookingResult);
	CookingResult FromPhysXCookingResult(physx::PxTriangleMeshCookingResult::Enum cookingResult);
}