#pragma once

#include <glm/glm.hpp>
#include <map>
#include <vector>


#ifdef _DEBUG
#pragma comment (lib, "assimp-vc142-mtd.lib")
#else
#pragma comment (lib, "assimp-vc142-mt.lib")
#endif

#include <assimp/scene.h>
#include <assimp/Importer.hpp>

#include "Bone.h"
#include "Animation.h"

namespace god
{
	namespace Animation3D
	{
		class Animator
		{
		public:
			Animator () = default;

			Animator ( Animation* animation );

			void UpdateAnimation ( float dt , std::vector<std::vector<glm::mat4>> const& cachedTransforms );

			void PlayAnimation ( Animation* pAnimation );

			void CalculateBoneTransform ( uint32_t frame , const AssimpNodeData* node , std::vector<std::vector<glm::mat4>> const& cachedTransforms );

			std::vector<glm::mat4> GetFinalBoneMatrices ();

			float m_startTime { 0.0f };
			float m_endTime { 1.0f };

		private:
			std::vector<glm::mat4> m_FinalBoneMatrices;
			Animation* m_CurrentAnimation;
			float m_CurrentTime;
			float m_DeltaTime;
		};
	}
}