#pragma once

#include "../../EngineComponents/EC_All.h"
#include "../../EnttXSol.h"

namespace god
{
	void PhysicsControllerSystem ( EnttXSol& entt , EngineResources& engineResources , std::tuple<PhysicsController&> components )
	{
		( entt );
		( components );
		( engineResources );
		// some code here ...
		//std::cout << "Update" << std::endl;

		auto& [physics_controller] = components;

		glm::vec3 vel = physics_controller.m_heading * physics_controller.m_speed;
		if ( glm::length2 ( vel ) > 0.01f )
		{
			physics_controller.m_controller->move ( { vel.x , vel.y , vel.z } , 0.1f , DeltaTimer::m_dt , 0 );
		}
	}

	//void PhysicsControllerFrameStart ( EnttXSol& entt , EngineResources& engineResources , std::tuple<EntityData& , Transform&> components )
	//{
	//	( entt );
	//	( components );
	//	( engineResources );
	//	// some code here ...
	//	//std::cout << "Start Frame" << std::endl;
	//}

	void PhysicsControllerFrameEnd ( EnttXSol& entt , EngineResources& engineResources , std::tuple<Transform& , PhysicsController&> components )
	{
		( entt );
		( components );
		( engineResources );
		// some code here ...

		auto& [transform , physics_controller] = components;
		auto controller_position = physics_controller.m_controller->getPosition ();
		transform.m_position = { controller_position.x, controller_position.y, controller_position.z };
	}

	void PhysicsControllerInit ( EnttXSol& entt , EngineResources& engineResources , std::tuple<Transform& , PhysicsController&> components )
	{
		( entt );
		( components );
		( engineResources );
		//some code here ...

		auto& [transform , physics_controller] = components;
		auto& physics_controller_manager = engineResources.Get<PX::PhysicsController> ().get ();
		physics_controller.m_controller = physics_controller_manager.CreateController ( PX::PhysicsController::ControllerShape::CAPSULE ,
			engineResources.Get<PhysicsSystem> ().get ().GetPhysics ()->createMaterial ( physics_controller.m_static_friction , physics_controller.m_dynamic_friction , physics_controller.m_restitution ) ,
			{ transform.m_position.x, transform.m_position.y, transform.m_position.y } );
	}

	//void PhysicsControllerCleanup ( EnttXSol& entt , EngineResources& engineResources , std::tuple<EntityData& , Transform&> components )
	//{
	//	( entt );
	//	( components );
	//	( engineResources );
	//	// some code here ...
	//	//std::cout << "Cleanup - Entities with EntityData & Transform." << std::endl;
	//}
}