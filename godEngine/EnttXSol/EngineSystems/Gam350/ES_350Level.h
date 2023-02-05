#pragma once

#include "../../EngineComponents/EC_All.h"
#include "../../EnttXSol.h"

#include "../../../Window/DeltaTimer.h"

namespace god
{
	void _350LevelUpdate ( EnttXSol& entt , EngineResources& engineResources , std::tuple<EntityData& , _350Level& , Transform&> components )
	{
		( entt );
		( components );
		( engineResources );
		// some code here ...
		//std::cout << "Update" << std::endl;
		auto& [entity_data , level , transform] = components;

		GLFWWindow& window = engineResources.Get<GLFWWindow> ().get ();
		if ( window.KeyPressed ( GLFW_KEY_P ) )
		{
			level.m_focused = !level.m_focused;
		}

		if ( !level.m_grid.m_deserialized )
		{
			level.m_grid.Initialize ( entt , engineResources , entity_data , transform , level.m_level_layout , level.m_enemy_layout , level.m_playable_layout );
		}

		if ( level.m_focused )
		{
			level.m_grid.Update ( DeltaTimer::m_dt , entt , engineResources , entity_data , transform );
			
			EnttXSol::Entities::ID reset_button = entt.GetEntity ( "ResetButton" );
			if ( reset_button != EnttXSol::Entities::Null )
			{
				GUIObject* rb_gui = entt.GetEngineComponent<GUIObject> ( reset_button );
				if ( rb_gui && rb_gui->m_released )
				{
					level.m_grid.m_to_restart = true;
				}
			}
		}
	}

	void _350LevelFrameStart ( EnttXSol& entt , EngineResources& engineResources , std::tuple<EntityData& , _350Level& , Transform&> components )
	{
		( entt );
		( components );
		( engineResources );
		// some code here ...
		//std::cout << "Start Frame" << std::endl;
	}

	void _350LevelFrameEnd ( EnttXSol& entt , EngineResources& engineResources , std::tuple<EntityData& , Transform&> components )
	{
		( entt );
		( components );
		( engineResources );
		// some code here ...
		//std::cout << "End Frame" << std::endl;
	}

	void _350LevelInit ( EnttXSol& entt , EngineResources& engineResources , std::tuple<EntityData& , _350Level& , Transform&> components )
	{
		( entt );
		( components );
		( engineResources );
		//some code here ...
		//std::cout << "Init - Entities with EntityData & Transform." << std::endl;
		//auto& [entity_data , level , transform] = components;
	}

	void _350LevelCleanup ( EnttXSol& entt , EngineResources& engineResources , std::tuple<EntityData& , Transform&> components )
	{
		( entt );
		( components );
		( engineResources );
		// some code here ...
		//std::cout << "Cleanup - Entities with EntityData & Transform." << std::endl;
	}
}