#pragma once

#include "../../EngineComponents/EC_All.h"
#include "../../EnttXSol.h"

#include <godUtility/Math.h>

namespace god
{
	void FaceCamera ( EnttXSol& entt , EngineResources& engineResources , std::tuple<Transform& , Transparent&> components )
	{
		( entt );
		( components );

		auto& [transform , transparent] = components;
		if ( transparent.m_facing_horizontal )
		{
			glm::vec3 world_position = transform.m_parent_transform * glm::vec4 ( transform.m_position , 1.0f );
			float new_rotation = transform.m_rotation.y + HorizontalFaceCameraDegrees ( engineResources.Get<Camera> ().get ().m_position , world_position );
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