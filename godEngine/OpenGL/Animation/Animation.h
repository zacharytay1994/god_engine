#pragma once

#include <vector>
#include <functional>
#include <map>

#include <glm/glm.hpp>
#include <assimp/scene.h>

#include "AnimationBoneInfo.h"
#include "AnimationModel3D.h"
#include "Bone.h"

namespace god
{
	namespace Animation3D
	{
		struct AssimpNodeData
		{
			glm::mat4 transformation;
			std::string name;
			int childrenCount;
			std::vector<AssimpNodeData> children;
		};

		class Animation
		{
		public:
			Animation() = default;

			Animation( const std::string& animationPath, Model* model );

			~Animation()
			{}

			Bone* FindBone( const std::string& name );

			float GetTicksPerSecond();

			float GetDuration();

			const AssimpNodeData& GetRootNode();

			const std::map<std::string, BoneInfo>& GetBoneIDMap();

		private:

			double m_Duration;
			double m_TicksPerSecond;
			std::vector<Bone> m_Bones;
			AssimpNodeData m_RootNode;
			std::map<std::string, BoneInfo> m_BoneInfoMap;

			void ReadMissingBones( const aiAnimation* animation, Model& model );

			void ReadHeirarchyData( AssimpNodeData& dest, const aiNode* src );

		};
	}
}