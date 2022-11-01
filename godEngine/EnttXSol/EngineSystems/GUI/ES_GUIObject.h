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

		if ( entity_data.m_parent_id != EnttXSol::Entities::Null && entt.GetEngineComponent<GUIObject> ( entt.m_entities[ entity_data.m_parent_id ].m_id ) )
		{
			transform.m_position.x = -1.0f + gui.m_position.x * 2.0f;
			transform.m_position.y = -1.0f + gui.m_position.y * 2.0f;
			transform.m_position.z = 1.0f + gui.m_layer;
			transform.m_scale.x = gui.m_size.x;
			if ( gui.m_fixed_aspect_ratio )
			{
				transform.m_scale.y = transform.m_scale.x * gui.m_aspect_ratio * ( transform.m_parent_transform[ 0 ].x / transform.m_parent_transform[ 1 ].y );
				transform.m_scale.z = transform.m_scale.y;
			}
			else
			{
				transform.m_scale.y = gui.m_size.y;
			}
		}
		else
		{
			transform.m_position.x = gui.m_position.x * window.GetWindowWidth ();
			transform.m_position.y = gui.m_position.y * window.GetWindowHeight ();
			transform.m_position.z = -251 + gui.m_layer;
			transform.m_scale.x = gui.m_size.x * window.GetWindowWidth () / 2.0f;
			if ( gui.m_fixed_aspect_ratio )
			{
				transform.m_scale.y = transform.m_scale.x * gui.m_aspect_ratio;
				transform.m_scale.z = transform.m_scale.y;
			}
			else
			{
				transform.m_scale.y = gui.m_size.y * window.GetWindowHeight () / 2.0f;
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