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
					// transition sound
					entt.QueueInstancePrefab ( "SFX_Wave" , 0.f , 0.f , 0.f );

					if ( level_manager.m_currently_initializing < level_manager.m_level_names.size () )
					{
						// move directional light too
						auto directional_light_id = entt.GetEntity ( "DirectionalLight" );
						if ( directional_light_id != static_cast< uint32_t >( -1 ) )
						{
							Transform* dl_transform = entt.GetEngineComponent<Transform> ( directional_light_id );
							DirectionalLight* directional_light = entt.GetEngineComponent<DirectionalLight> ( directional_light_id );
							if ( dl_transform && directional_light )
							{
								/*transform->m_position.x += level_manager.m_offset_direction.x;
								transform->m_position.z += level_manager.m_offset_direction.z;*/
								dl_transform->m_position += static_cast< glm::vec3 >( transform.m_local_transform * glm::vec4 ( level_manager.m_offset_direction , 1.0f ) );
								directional_light->m_specular = transform.m_local_transform * glm::vec4 ( level_manager.m_offset_direction * static_cast< float >( level_manager.m_currently_initializing ) , 1.0f );
							}
						}
					}
					else
					{
						// move directional light too
						auto directional_light_id = entt.GetEntity ( "DirectionalLight" );
						if ( directional_light_id != static_cast< uint32_t >( -1 ) )
						{
							Transform* dl_transform = entt.GetEngineComponent<Transform> ( directional_light_id );
							DirectionalLight* directional_light = entt.GetEngineComponent<DirectionalLight> ( directional_light_id );
							if ( dl_transform && directional_light )
							{
								//reset to original position
								dl_transform->m_position -= static_cast< glm::vec3 >( transform.m_local_transform * glm::vec4 ( level_manager.m_offset_direction * static_cast< float >( level_manager.m_currently_initializing - 1 ) , 1.0f ) );
								directional_light->m_specular = transform.m_local_transform * glm::vec4 ( level_manager.m_offset_direction * 0.0f , 1.0f );
							}
						}
					}
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

					entt.QueueDelete ( level_manager.m_level_ids[ level_manager.m_currently_playing ] );
					++level_manager.m_currently_playing;

					// transition sound
					entt.QueueInstancePrefab ( "SFX_Wave" , 0.f , 0.f , 0.f );

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

					// move directional light too
					auto directional_light_id = entt.GetEntity ( "DirectionalLight" );
					if ( directional_light_id != static_cast< uint32_t >( -1 ) )
					{
						Transform* dl_transform = entt.GetEngineComponent<Transform> ( directional_light_id );
						DirectionalLight* directional_light = entt.GetEngineComponent<DirectionalLight> ( directional_light_id );
						if ( dl_transform && directional_light )
						{
							/*transform->m_position.x += level_manager.m_offset_direction.x;
							transform->m_position.z += level_manager.m_offset_direction.z;*/
							dl_transform->m_position += static_cast< glm::vec3 >( transform.m_local_transform * glm::vec4 ( level_manager.m_offset_direction , 1.0f ) );
							directional_light->m_specular = transform.m_local_transform * glm::vec4 ( level_manager.m_offset_direction * static_cast< float >( level_manager.m_currently_playing ) , 1.0f );
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
		else
		{
			// change back to splash screen
			level_manager.m_done = true;

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