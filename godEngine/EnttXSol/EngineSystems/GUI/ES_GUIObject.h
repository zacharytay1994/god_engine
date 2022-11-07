#pragma once

#include "../../EngineComponents/EC_All.h"
#include "../../EnttXSol.h"

namespace god
{
	void GUIObjectUpdate ( EnttXSol& entt , EngineResources& engineResources , std::tuple<EntityData& , Transform& , GUIObject&> components )
	{
		( entt );
		( components );
		( engineResources );
		// some code here ...
		//std::cout << "Update" << std::endl;
		auto&& [entity_data , transform , gui] = components;
		auto& window = engineResources.Get<GLFWWindow> ().get ();

		// reset input 
		gui.m_enter = false;
		gui.m_exit = false;

		if ( entity_data.m_parent_id != EnttXSol::Entities::Null && entt.GetEngineComponent<GUIObject> ( entt.m_entities[ entity_data.m_parent_id ].m_id ) )
		{
			transform.m_position.x = -1.0f + gui.m_position.x * 2.0f;
			transform.m_position.y = -1.0f + gui.m_position.y * 2.0f;
			transform.m_position.z = 1.0f + gui.m_layer;
			transform.m_scale.x = gui.m_size.x;
			if ( gui.m_fixed_aspect_ratio )
			{
				transform.m_scale.y = transform.m_scale.x * gui.m_aspect_ratio * ( transform.m_parent_transform[ 0 ].x / transform.m_parent_transform[ 1 ].y );
				//transform.m_scale.z = transform.m_scale.y;
			}
			else
			{
				transform.m_scale.y = gui.m_size.y;
			}

			double mouse_x { window.ViewportMouseX () } , mouse_y { window.GetWindowHeight () - window.ViewportMouseY () };
			glm::vec3 world_position = transform.m_parent_transform * glm::vec4 ( transform.m_position , 1.0f );
			glm::vec3 world_scale = glm::mat3 ( transform.m_parent_transform ) * transform.m_scale;
			if ( !( mouse_x < world_position.x - world_scale.x || mouse_x > world_position.x + world_scale.x || mouse_y < world_position.y - world_scale.y || mouse_y > world_position.y + world_scale.y ) )
			{
				if ( !gui.m_hovered )
				{
					gui.m_enter = true;
				}
				gui.m_hovered = true;
				gui.m_pressed = window.MouseLPressed ( 1 );
				gui.m_down = window.MouseLDown ( 1 );
				gui.m_released = window.MouseLUp ( 1 );
			}
			else
			{
				if ( gui.m_hovered )
				{
					gui.m_exit = true;
					gui.m_hovered = false;
					gui.m_down = false;
					gui.m_released = false;
				}
			}
		}
		else
		{
			transform.m_position.x = gui.m_position.x * window.GetWindowWidth ();
			transform.m_position.y = gui.m_position.y * window.GetWindowHeight ();
			transform.m_position.z = static_cast< float >( -251 + gui.m_layer );
			transform.m_scale.x = gui.m_size.x * window.GetWindowWidth () / 2.0f;
			if ( gui.m_fixed_aspect_ratio )
			{
				transform.m_scale.y = transform.m_scale.x * gui.m_aspect_ratio;
				//transform.m_scale.z = transform.m_scale.y;
			}
			else
			{
				transform.m_scale.y = gui.m_size.y * window.GetWindowHeight () / 2.0f;
			}

			// bounding check
			double mouse_x { window.ViewportMouseX () } , mouse_y { window.GetWindowHeight () - window.ViewportMouseY () };
			if ( !( mouse_x < transform.m_position.x - transform.m_scale.x || mouse_x > transform.m_position.x + transform.m_scale.x || mouse_y < transform.m_position.y - transform.m_scale.y || mouse_y > transform.m_position.y + transform.m_scale.y ) )
			{
				if ( !gui.m_hovered )
				{
					gui.m_enter = true;
				}
				gui.m_hovered = true;
				gui.m_pressed = window.MouseLPressed ();
				gui.m_down = window.MouseLDown ();
				gui.m_released = window.MouseLUp ();
			}
			else
			{
				if ( gui.m_hovered )
				{
					gui.m_exit = true;
					gui.m_hovered = false;
				}
			}
		}
	}

	void GUIObjectFrameStart ( EnttXSol& entt , EngineResources& engineResources , std::tuple<EntityData& , Transform&> components )
	{
		( entt );
		( components );
		( engineResources );
		// some code here ...
		//std::cout << "Start Frame" << std::endl;
	}

	void GUIObjectFrameEnd ( EnttXSol& entt , EngineResources& engineResources , std::tuple<EntityData& , Transform&> components )
	{
		( entt );
		( components );
		( engineResources );
		// some code here ...
		//std::cout << "End Frame" << std::endl;
	}

	void GUIObjectInit ( EnttXSol& entt , EngineResources& engineResources , std::tuple<EntityData& , Transform&> components )
	{
		( entt );
		( components );
		( engineResources );
		//some code here ...
	   //std::cout << "Init - Entities with EntityData & Transform." << std::endl;
	}

	void GUIObjectCleanup ( EnttXSol& entt , EngineResources& engineResources , std::tuple<EntityData& , Transform&> components )
	{
		( entt );
		( components );
		( engineResources );
		// some code here ...
		//std::cout << "Cleanup - Entities with EntityData & Transform." << std::endl;
	}
}