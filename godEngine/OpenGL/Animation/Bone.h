#pragma once

/* Container for bone data */

#include <vector>
#include <assimp/scene.h>
#include <list>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include "AssimpGLMHelpers.h"

namespace god
{
	namespace Animation3D
	{
		struct KeyPosition
		{
			glm::vec3 position{ glm::vec3( 0.0f ) };
			float timeStamp{ 0.0f };
		};

		struct KeyRotation
		{
			glm::quat orientation{};
			float timeStamp{ 0.0f };
		};

		struct KeyScale
		{
			glm::vec3 scale{ glm::vec3( 0.0f ) };
			float timeStamp{ 0.0f };
		};

		class Bone
		{
		public:
			Bone( const std::string& name, int ID, const aiNodeAnim* channel );

			void Update( float animationTime );

			glm::mat4 GetLocalTransform();

			std::string GetBoneName() const;

			int GetBoneID();

			int GetPositionIndex	( float animationTime );

			int GetRotationIndex	( float animationTime );

			int GetScaleIndex		( float animationTime );

		private:

			std::vector<KeyPosition> m_Positions;
			std::vector<KeyRotation> m_Rotations;
			std::vector<KeyScale> m_Scales;

			int m_NumPositions{ 0 };
			int m_NumRotations{ 0 };
			int m_NumScalings{ 0 };

			glm::mat4 m_LocalTransform;
			std::string m_Name;
			int m_ID;

			float GetScaleFactor( float lastTimeStamp, float nextTimeStamp, float animationTime );

			glm::mat4 InterpolatePosition( float animationTime );

			glm::mat4 InterpolateRotation( float animationTime );

			glm::mat4 InterpolateScaling( float animationTime );
		};
	}
}