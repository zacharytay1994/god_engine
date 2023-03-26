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

			EnttXSol::Entities::ID next_level_button_id = entt.GetEntity ( "AchievementNextLevel" );
			if ( next_level_button_id != EnttXSol::Entities::Null )
			{
				GUIObject* next_level_button = entt.GetEngineComponent<GUIObject> ( next_level_button_id );
				if ( next_level_button && next_level_button->m_released )
				{
					level.m_grid.m_won = true;
					// delete 3 star achievement
					if ( !level.m_grid.m_achievement.empty () )
					{
						EnttXSol::Entities::ID achievement_id = entt.GetEntity ( level.m_grid.m_achievement );
						if ( achievement_id != EnttXSol::Entities::Null )
						{
							entt.QueueDelete ( achievement_id );
						}
						level.m_grid.m_achievement = "";
					}
				}
			}

			EnttXSol::Entities::ID try_again_button_id = entt.GetEntity ( "AchievementTryAgain" );
			if ( try_again_button_id != EnttXSol::Entities::Null )
			{
				GUIObject* try_again_button = entt.GetEngineComponent<GUIObject> ( try_again_button_id );
				if ( try_again_button && try_again_button->m_released )
				{
					level.m_grid.m_to_restart = true;
					// delete 3 star achievement
					if ( !level.m_grid.m_achievement.empty () )
					{
						EnttXSol::Entities::ID achievement_id = entt.GetEntity ( level.m_grid.m_achievement );
						if ( achievement_id != EnttXSol::Entities::Null )
						{
							entt.QueueDelete ( achievement_id );
						}
						level.m_grid.m_achievement = "";
					}
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