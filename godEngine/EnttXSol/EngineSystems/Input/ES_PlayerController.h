#pragma once

#include "../../EngineComponents/EC_All.h"
#include "../../EnttXSol.h"

namespace god
{
	void PlayerControllerSystem ( EnttXSol& entt , EngineResources& engineResources , std::tuple<PhysicsController& , PlayerController&> components )
	{
		( entt );
		( components );
		( engineResources );
		// some code here ...
		//std::cout << "Update" << std::endl;
		auto& [physics_controller , player_controller] = components;
		auto& window = engineResources.Get<GLFWWindow> ().get ();
		player_controller.m_horizontal_input = { 0.0f, 0.0f };
		if ( window.KeyDown ( player_controller.m_forward ) )
		{
			player_controller.m_horizontal_input.y -= 1.0f;
		}
		if ( window.KeyDown ( player_controller.m_backward ) )
		{
			player_controller.m_horizontal_input.y += 1.0f;
		}
		if ( window.KeyDown ( player_controller.m_left ) )
		{
			player_controller.m_horizontal_input.x -= 1.0f;
		}
		if ( window.KeyDown ( player_controller.m_right ) )
		{
			player_controller.m_horizontal_input.x += 1.0f;
		}
	}

	//void PlayerControllerSystemFrameStart ( EnttXSol& entt , EngineResources& engineResources , std::tuple<EntityData& , Transform&> components )
	//{
	//	( entt );
	//	( components );
	//	( engineResources );
	//	// some code here ...
	//	//std::cout << "Start Frame" << std::endl;
	//}

	//void PlayerControllerSystemFrameEnd ( EnttXSol& entt , EngineResources& engineResources , std::tuple<EntityData& , Transform&> components )
	//{
	//	( entt );
	//	( components );
	//	( engineResources );
	//	// some code here ...
	//	//std::cout << "End Frame" << std::endl;
	//}

	//void PlayerControllerSystemInit ( EnttXSol& entt , EngineResources& engineResources , std::tuple<EntityData& , Transform&> components )
	//{
	//	( entt );
	//	( components );
	//	( engineResources );
	//	//some code here ...
	//   //std::cout << "Init - Entities with EntityData & Transform." << std::endl;
	//}

	//void PlayerControllerSystemCleanup ( EnttXSol& entt , EngineResources& engineResources , std::tuple<EntityData& , Transform&> components )
	//{
	//	( entt );
	//	( components );
	//	( engineResources );
	//	// some code here ...
	//	//std::cout << "Cleanup - Entities with EntityData & Transform." << std::endl;
	//}
}