#pragma once

#include "../../EngineComponents/EC_All.h"
#include "../../EnttXSol.h"

#include <godUtility/Math.h>

namespace god
{
	void FaceCamera ( EnttXSol& entt , EngineResources& engineResources , std::tuple<EntityData& , Transform& , Transparent&> components )
	{
		( entt );
		( components );

		auto& [entity_data , transform , transparent] = components;
		if ( transparent.m_facing_horizontal )
		{
			glm::vec3 world_position = transform.m_parent_transform * glm::vec4 ( transform.m_position , 1.0f );
			float new_rotation = transform.m_rotation.y + HorizontalFaceCameraDegrees ( engineResources.Get<Camera> ().get ().m_position , world_position );
			// hacky sln to offset rotation of only 1 parent
			if ( entity_data.m_parent_id != EnttXSol::Entities::Null )
			{
				Transform* transform = entt.GetEngineComponent<Transform> ( entity_data.m_parent_id );
				if ( transform )
				{
					new_rotation -= transform->m_rotation.y;
				}
			}
			if ( new_rotation != transparent.m_facing_rotation_y )
			{
				transparent.m_changed = true;
				transparent.m_facing_rotation_y = new_rotation;
			}
			else
			{
				transparent.m_changed = false;
			}
		}
		else
		{
			transparent.m_facing_rotation_y = transform.m_rotation.y;
			transparent.m_changed = false;
		}
	}
}