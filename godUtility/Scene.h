#pragma once
#include "godUtility.h"
#include "Internal/RecycleVector.h"

#include <glm/glm/glm.hpp>

#include <vector>
#include <queue>
#include <unordered_map>

namespace god
{
	using SceneObjectID = uint32_t;

	struct Scene
	{
		struct InstancedRenderData
		{
			size_t		operator()( InstancedRenderData const& data ) const;
			bool		operator==( InstancedRenderData const& rhs ) const;
			uint32_t	m_model_id { 0 };
			uint32_t	m_diffuse_id { 0 };
			uint32_t	m_specular_id { 0 };
			float		m_shininess { 0.0f };
		};

		std::unordered_map<InstancedRenderData , std::vector<glm::mat4> , InstancedRenderData> m_instanced_render_data;

		GODUTILITY_API		Scene ();
		GODUTILITY_API void	AddInstancedObject ( InstancedRenderData const& data , glm::mat4 const& transform );
		GODUTILITY_API void	ClearInstancedScene ();
	};
}