#pragma once

#include "../../EngineComponents/EC_All.h"
#include "../../EnttXSol.h"

#include "../../../Window/DeltaTimer.h"

namespace god
{
	void _350LevelManagerUpdate ( EnttXSol& entt , EngineResources& engineResources , std::tuple<EntityData& , _350LevelManager& , Transform&> components )
	{
		( entt );
		( components );
		( engineResources );
		// some code here ...
		//std::cout << "Update" << std::endl;
		auto& [entity_data , level_manager , transform] = components;

		// if current initializing lvl is valid
		if ( level_manager.m_currently_initializing < level_manager.m_level_names.size () )
		{
			if ( !level_manager.m_initializing )
			{
				// start initializing next level in list
				auto level_prefab_name = level_manager.m_level_names[ level_manager.m_currently_initializing ];
				level_manager.m_level_ids.push_back ( entt.InstancePrefab ( engineResources , level_prefab_name , entity_data.m_id ) );

				// offset lvls in direction
				Transform* level_transform = entt.GetEngineComponent<Transform> ( level_manager.m_level_ids.back () );
				if ( level_transform )
				{
					/*glm::vec2 rot = RotateVector ( glm::vec2 ( level_manager.m_offset.x , 0.0f ) , -45 );
					level_transform->m_position = { rot.x, level_manager.m_offset.y, rot.y };*/
					level_transform->m_position = level_manager.m_offset;
					level_manager.m_offset += level_manager.m_offset_direction;
				}

				level_manager.m_initializing = true;
			}
			else
			{
				_350Level* level = entt.GetEngineComponent<_350Level> ( level_manager.m_level_ids[ level_manager.m_currently_initializing ] );
				// when current lvl ready
				if ( level && level->m_grid.m_ready )
				{
					// unfocus current lvl
					level->m_focused = false;
					// initialize next lvl
					level_manager.m_initializing = false;
					++level_manager.m_currently_initializing;
				}
			}
		}
		// finish readying all lvls, focus on first lvl and start it
		else if ( level_manager.m_currently_playing < level_manager.m_level_ids.size () )
		{
			_350Level* level = entt.GetEngineComponent<_350Level> ( level_manager.m_level_ids[ level_manager.m_currently_playing ] );
			if ( level && level->m_grid.m_ready )
			{
				// activate tutorial gui for first lvl
				if ( level_manager.m_currently_playing == 0 && !level->m_grid.m_combat_paused )
				{
					EnttXSol::Entities::ID tut_camera = entt.GetEntity ( "TutCamera" );
					EnttXSol::Entities::ID tut_interact = entt.GetEntity ( "TutInteract" );
					if ( tut_camera != EnttXSol::Entities::Null && tut_interact != EnttXSol::Entities::Null )
					{
						GUIObject* tut_cam_gui = entt.GetEngineComponent<GUIObject> ( tut_camera );
						GUIObject* tut_int_gui = entt.GetEngineComponent<GUIObject> ( tut_interact );
						if ( tut_cam_gui && tut_int_gui )
						{
							tut_cam_gui->m_active = true;
							tut_int_gui->m_active = true;
						}
					}
				}

				level->m_grid.m_start = true;
				level->m_focused = true;

				// if lvl is done, go next lvl
				if ( level->m_grid.m_won )
				{
					level->m_focused = false;
					++level_manager.m_currently_playing;

					// deactivate tutorial gui
					EnttXSol::Entities::ID tut_camera = entt.GetEntity ( "TutCamera" );
					EnttXSol::Entities::ID tut_interact = entt.GetEntity ( "TutInteract" );
					if ( tut_camera != EnttXSol::Entities::Null && tut_interact != EnttXSol::Entities::Null )
					{
						GUIObject* tut_cam_gui = entt.GetEngineComponent<GUIObject> ( tut_camera );
						GUIObject* tut_int_gui = entt.GetEngineComponent<GUIObject> ( tut_interact );
						if ( tut_cam_gui && tut_int_gui )
						{
							tut_cam_gui->m_active = false;
							tut_int_gui->m_active = false;
						}
					}
				}
				// if lose and needs to reset
				if ( level->m_grid.m_to_restart )
				{
					// remove old lvl
					Transform* old_lvl = entt.GetEngineComponent<Transform> ( level_manager.m_level_ids[ level_manager.m_currently_playing ] );
					glm::vec3 pos { 0.0f };
					if ( old_lvl )
					{
						pos = old_lvl->m_position;
					}
					entt.QueueDelete ( level_manager.m_level_ids[ level_manager.m_currently_playing ] );
					// create new lvl
					level_manager.m_level_ids[ level_manager.m_currently_playing ] = entt.InstancePrefab ( engineResources , level_manager.m_level_names[ level_manager.m_currently_playing ] , entity_data.m_id );
					Transform* new_lvl = entt.GetEngineComponent<Transform> ( level_manager.m_level_ids[ level_manager.m_currently_playing ] );
					if ( new_lvl )
					{
						new_lvl->m_position = pos;
					}
				}
			}
		}
	}

	void _350LevelManagerFrameStart ( EnttXSol& entt , EngineResources& engineResources , std::tuple<EntityData& , _350LevelManager& , Transform&> components )
	{
		( entt );
		( components );
		( engineResources );
		// some code here ...
		//std::cout << "Start Frame" << std::endl;
	}

	void _350LevelManagerFrameEnd ( EnttXSol& entt , EngineResources& engineResources , std::tuple<EntityData& , Transform&> components )
	{
		( entt );
		( components );
		( engineResources );
		// some code here ...
		//std::cout << "End Frame" << std::endl;
	}

	void _350LevelManagerInit ( EnttXSol& entt , EngineResources& engineResources , std::tuple<_350LevelManager& , Transform&> components )
	{
		( entt );
		( components );
		( engineResources );
		//some code here ...
		//std::cout << "Init - Entities with EntityData & Transform." << std::endl;
		auto& [level_manager , transform] = components;

		std::string s;
		std::stringstream ss;
		ss << level_manager.m_level_names_serialize;
		while ( ss >> s )
		{
			level_manager.m_level_names.push_back ( s );
		}
	}

	void _350LevelManagerCleanup ( EnttXSol& entt , EngineResources& engineResources , std::tuple<EntityData& , Transform&> components )
	{
		( entt );
		( components );
		( engineResources );
		// some code here ...
		//std::cout << "Cleanup - Entities with EntityData & Transform." << std::endl;
	}
}