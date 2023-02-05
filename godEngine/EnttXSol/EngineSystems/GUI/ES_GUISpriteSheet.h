#pragma once

#include "../../EngineComponents/EC_All.h"
#include "../../EnttXSol.h"

namespace god
{
	void GUISpriteSheetUpdate ( EnttXSol& entt , EngineResources& engineResources , std::tuple<Renderable3D& , GUIObject&> components )
	{
		( entt );
		( components );
		( engineResources );
		// some code here ...

		auto& [renderable , gui_object] = components;

		if ( renderable.m_spritesheet_data.y > 1 )
		{
			renderable.m_spritesheet_data.x = 0;

			if ( gui_object.m_hovered )
			{
				renderable.m_spritesheet_data.x = 1;
				if ( gui_object.m_down )
				{
					renderable.m_spritesheet_data.x = 2;
				}
			}
		}
	}
}