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

	void Scene::Add2DInstancedObject ( InstancedRenderData const& data , glm::mat4 const& transform )
	{
		m_2D_instanced_render_data[ data ].push_back ( transform );
	}

	void Scene::AddCharacter ( InstancedRenderData const& data , glm::mat4 const& transform )
	{
		m_characters[ data ].push_back ( transform );
	}

	void Scene::AddBillboard ( InstancedRenderData const& data , glm::mat4 const& transform )
	{
		m_billboard_sprites[ data ].push_back ( transform );
	}

	void Scene::AddBillboardNoLighting ( InstancedRenderData const& data , glm::mat4 const& transform )
	{
		m_billboard_sprites_no_lighting[ data ].push_back ( transform );
	}

	void Scene::AddAnimation ( InstancedRenderData const& data , glm::mat4 const& transform )
	{
		m_animation_data.push_back ( { data, transform } );
	}

	void Scene::ClearInstancedScene ()
	{
		for ( auto& instance : m_instanced_render_data )
		{
			instance.second.clear ();
		}
		for ( auto& instance : m_2D_instanced_render_data )
		{
			instance.second.clear ();
		}
		for ( auto& instance : m_characters )
		{
			instance.second.clear ();
		}
		for ( auto& instance : m_billboard_sprites )
		{
			instance.second.clear ();
		}
		for ( auto& instance : m_billboard_sprites_no_lighting )
		{
			instance.second.clear ();
		}
		m_animation_data.clear ();
	}

	void Scene::AddPointLight ( PointLightData const& pld )
	{
		m_point_light_data.push_back ( pld );
	}

	void Scene::AddDirectionalLight ( DirectionalLightData const& dld )
	{
		m_directional_light_data.push_back ( dld );
	}

	size_t Scene::InstancedRenderData::operator()( InstancedRenderData const& data ) const
	{
		size_t seed { 0 };
		HashCombine ( seed , data.m_model_id );
		HashCombine ( seed , data.m_diffuse_id );
		HashCombine ( seed , data.m_specular_id );
		HashCombine ( seed , data.m_shininess );
		HashCombine	( seed , data.m_emissive );
		HashCombine ( seed , data.m_tint.x );
		HashCombine ( seed , data.m_tint.y );
		HashCombine ( seed , data.m_tint.z );
		HashCombine ( seed , data.m_tint.w );
		HashCombine ( seed , data.m_outlined );
		return seed;
	}

	bool Scene::InstancedRenderData::operator==( InstancedRenderData const& rhs ) const
	{
		return m_model_id == rhs.m_model_id &&
			m_diffuse_id == rhs.m_diffuse_id &&
			m_specular_id == rhs.m_specular_id &&
			m_shininess == rhs.m_shininess &&
			m_emissive == rhs.m_emissive &&
			m_tint == rhs.m_tint &&
			m_outlined == rhs.m_outlined;
	}

	Scene::PointLightData& Scene::PointLightData::operator=( PointLightData const& rhs )
	{
		m_position = rhs.m_position;
		m_colour = rhs.m_colour;
		m_ambient = rhs.m_ambient;
		m_diffuse = rhs.m_diffuse;
		m_specular = rhs.m_specular;
		return *this;
	}

	Scene::DirectionalLightData& Scene::DirectionalLightData::operator=( DirectionalLightData const& rhs )
	{
		m_position = rhs.m_position;
		m_colour = rhs.m_colour;
		m_ambient = rhs.m_ambient;
		m_diffuse = rhs.m_diffuse;
		m_specular = rhs.m_specular;
		return *this;
	}
}