#pragma once
#include "godUtility.h"

#include <glm/glm/glm.hpp>

#include <vector>
#include <queue>

namespace god
{
	using SceneObjectID = uint32_t;

	struct Scene
	{
		struct RenderData
		{
			uint32_t	m_model_id { 0 };
			glm::vec3	m_position { 0.0f,0.0f,0.0f };
			glm::vec3	m_rotation { 0.0f,0.0f,0.0f };
			glm::vec3	m_scale { 1.0f,1.0f,1.0f };

			// material data
			uint32_t	m_diffuse_id { 0 };
			uint32_t	m_specular_id { 0 };
			float		m_shininess { 0.0f };

			GODUTILITY_API RenderData () = default;
			GODUTILITY_API RenderData ( uint32_t model , glm::vec3 const& position , glm::vec3 const& rotation , glm::vec3 const& scale );

			bool GODUTILITY_API Active () const;

			friend struct Scene;

		private:
			bool m_active { false };
		};

		std::vector<RenderData> m_render_data;
		using PQueue = std::priority_queue<SceneObjectID , std::vector<SceneObjectID> , std::greater<SceneObjectID>>;
		PQueue m_free_render_data;

		GODUTILITY_API					Scene ();
		SceneObjectID GODUTILITY_API	AddSceneObject (
			uint32_t model = 0 ,
			glm::vec3 const& position = { 0.0f,0.0f,0.0f } ,
			glm::vec3 const& rotation = { 0.0f,0.0f,0.0f } ,
			glm::vec3 const& scale = { 1.0f,1.0f,1.0f } );

		void GODUTILITY_API				RemoveSceneObject ( SceneObjectID id );
		void GODUTILITY_API				ClearScene ();
		RenderData GODUTILITY_API&		GetSceneObject ( SceneObjectID id );
	};
}