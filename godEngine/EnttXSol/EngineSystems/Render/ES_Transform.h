#pragma once

#include "../../EngineComponents/EC_All.h"
#include "../../EnttXSol.h"

namespace god
{
	void TransformDirty ( EnttXSol& entt , EngineResources& engineResources , std::tuple<Transform&> components )
	{
		( entt );
		( components );
		( engineResources );

		auto& [transform] = components;

		if ( transform.m_prev_position != transform.m_position ||
			transform.m_prev_rotation != transform.m_rotation ||
			transform.m_prev_scale != transform.m_scale )
		{
			transform.m_changed = true;
			transform.m_prev_position = transform.m_position;
			transform.m_prev_rotation = transform.m_rotation;
			transform.m_prev_scale = transform.m_scale;
		}
		else
		{
			transform.m_changed = false;
		}
	}
}