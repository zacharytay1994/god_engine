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

		if ( level.m_focused )
		{
			level.m_grid.Update ( DeltaTimer::m_dt , entt , engineResources , entity_data , transform );
			// restart the level
			if ( level.m_grid.m_to_restart )
			{
				entt.QueueDelete ( entity_data.m_id );
				auto id = entt.AddPrefabToScene ( engineResources , entt.m_entities[ entity_data.m_id ].m_name );
				EntityData* new_entity_data = entt.GetEngineComponent<EntityData> ( id );
				_350Level* new_level = entt.GetEngineComponent<_350Level> ( id );
				Transform* new_transform = entt.GetEngineComponent<Transform> ( id );
				if ( new_entity_data && new_level && new_transform )
				{
					new_transform->m_position = transform.m_position;
					new_level->m_grid.Initialize ( entt , engineResources , *new_entity_data , *new_transform ,
						new_level->m_level_layout , new_level->m_enemy_layout , new_level->m_playable_layout );
				}
			}

			//level.m_grid.UpdateCameraControls ( DeltaTimer::m_dt , engineResources );
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
		auto& [entity_data , level , transform] = components;
		level.m_grid.Initialize ( entt , engineResources , entity_data , transform , level.m_level_layout , level.m_enemy_layout , level.m_playable_layout );
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