#include "pch.h"
#include "Scene.h"

#include <assert.h>

namespace god
{
	Scene::RenderData::RenderData ( uint32_t model , glm::vec3 const& position , glm::vec3 const& rotation , glm::vec3 const& scale )
		:
		m_model_id { model } ,
		m_position { position } ,
		m_rotation { rotation } ,
		m_scale { scale } ,
		m_active { true }
	{
	}

	bool Scene::RenderData::Active () const
	{
		return m_active;
	}

	Scene::Scene ()
	{
	}

	SceneObjectID Scene::AddSceneObject ( uint32_t model , glm::vec3 const& position , glm::vec3 const& rotation , glm::vec3 const& scale )
	{
		if ( m_free_render_data.empty () )
		{
			m_render_data.emplace_back ( model , position , rotation , scale );
			return static_cast< SceneObjectID >( m_render_data.size () - 1 );
		}
		SceneObjectID free_id = m_free_render_data.top ();
		m_free_render_data.pop ();

		m_render_data[ free_id ].m_model_id = model;
		m_render_data[ free_id ].m_position = position;
		m_render_data[ free_id ].m_rotation = rotation;
		m_render_data[ free_id ].m_scale = scale;
		m_render_data[ free_id ].m_active = true;

		return free_id;
	}

	void Scene::RemoveSceneObject ( SceneObjectID id )
	{
		assert ( id < m_render_data.size () );
		m_free_render_data.push ( id );
		m_render_data[ id ].m_active = false;
	}

	Scene::RenderData& Scene::GetSceneObject ( SceneObjectID id )
	{
		// check if valid id and entity active
		assert ( id < m_render_data.size () && m_render_data[ id ].m_active );
		return m_render_data[ id ];
	}
}