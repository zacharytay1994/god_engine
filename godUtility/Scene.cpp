#include "pch.h"
#include "Scene.h"
#include "Internal/Miscellaneous.h"

#include <assert.h>

namespace god
{
	Scene::Scene ()
	{
	}

	void Scene::AddInstancedObject ( InstancedRenderData const& data , glm::mat4 const& transform )
	{
		m_instanced_render_data[ data ].push_back ( transform );
	}

	void Scene::ClearInstancedScene ()
	{
		for ( auto& instance : m_instanced_render_data )
		{
			instance.second.clear ();
		}
	}

	size_t Scene::InstancedRenderData::operator()( InstancedRenderData const& data ) const
	{
		size_t seed { 0 };
		HashCombine ( seed , data.m_model_id );
		HashCombine ( seed , data.m_diffuse_id );
		HashCombine ( seed , data.m_specular_id );
		HashCombine ( seed , data.m_shininess );
		return seed;
	}

	bool Scene::InstancedRenderData::operator==( InstancedRenderData const& rhs ) const
	{
		return m_model_id == rhs.m_model_id &&
			m_diffuse_id == rhs.m_diffuse_id &&
			m_specular_id == rhs.m_specular_id &&
			m_shininess == rhs.m_shininess;
	}
}