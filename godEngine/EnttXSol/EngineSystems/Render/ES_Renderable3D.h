#pragma once

#include "../../EngineComponents/EC_All.h"
#include "../../EnttXSol.h"

#include "../../../Window/DeltaTimer.h"

namespace god
{
	void Renderable3DUpdate ( EnttXSol& entt , EngineResources& engineResources , std::tuple<Renderable3D&> components )
	{
		( entt );
		( engineResources );

		// update frames if any
		auto& [renderable] = components;
		// if y > 1 means there are multiple frames
		if ( renderable.m_spritesheet_data.y > 1 )
		{
			if ( renderable.m_framerate_counter < renderable.m_framerate )
			{
				renderable.m_framerate_counter += DeltaTimer::m_dt;
			}
			else
			{
				// increment frame
				renderable.m_framerate_counter = 0.0f;
				renderable.m_spritesheet_data.x = ++renderable.m_spritesheet_data.x % renderable.m_spritesheet_data.y;
			}
		}
	}
}