#pragma once

#include "../../EngineComponents/EC_All.h"
#include "../../EnttXSol.h"

#include <godUtility/Math.h>

namespace god
{
	void PopulateTransparent ( EnttXSol& entt , EngineResources& engineResources , std::tuple<EntityData& , Transform& , Renderable3D& , Transparent& , ActiveComponent&> components )
	{
		( entt );
		( components );

		auto& [entity_data , transform , renderable , transparent , active] = components;

		// return if not valid
		if ( !entt.m_entities.Valid ( entity_data.m_id ) )
		{
			return;
		}

		Scene& scene = engineResources.Get<Scene> ().get ();

		bool changed { false };
		glm::mat4 parentTransform { 1.0f };
		if ( entt.m_entities[ entity_data.m_id ].m_parent_id != EnttXSol::Entities::Null )
		{
			//changed = entt.m_entities[ entity_data.m_id ].m_parent_changed;
			parentTransform = entt.m_entities[ entity_data.m_id ].m_parent_transform;
			changed = transform.m_parent_transform != parentTransform;
		}

		bool world_changed { false };
		if ( changed )
		{
			transform.m_parent_transform = parentTransform;
			world_changed = true;
		}
		if ( transform.m_changed || transparent.m_changed )
		{
			glm::mat4 model_transform = BuildModelMatrixRotDegrees ( transform.m_position , { transform.m_rotation.x, transparent.m_facing_rotation_y, transform.m_rotation.z } , transform.m_scale );
			transform.m_local_transform = model_transform;
			world_changed = true;
		}
		if ( world_changed )
		{
			transform.m_world_transform = transform.m_parent_transform * transform.m_local_transform;
		}

		// add to scene
		if ( renderable.m_model_id != -1 && renderable.m_visible )
		{
			scene.AddBillboard ( { static_cast< uint32_t >( renderable.m_model_id ) ,
					renderable.m_diffuse_id , renderable.m_specular_id , renderable.m_shininess , renderable.m_emissive } , transform.m_world_transform );
		}

		// set children variables
		for ( auto const& child : entt.m_entities[ entity_data.m_id ].m_children )
		{
			//entt.m_entities[ child ].m_parent_changed = world_changed;
			entt.m_entities[ child ].m_parent_transform = transform.m_world_transform;
		}
	}
}