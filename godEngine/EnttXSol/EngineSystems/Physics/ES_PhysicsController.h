#pragma once

#include "../../EngineComponents/EC_All.h"
#include "../../EnttXSol.h"
#include <../godUtility/Math.h>

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

		// set velocity to moving
		physics_controller.m_velocity = physics_controller.m_heading * physics_controller.m_speed;
		// add velocity by force on body
		physics_controller.m_velocity += physics_controller.m_acceleration * DeltaTimer::m_dt;
		// dampen acceleration by lerping to 0 by a value
		physics_controller.m_acceleration = glm::mix ( physics_controller.m_acceleration , glm::vec3 ( 1.0f ) , physics_controller.m_acceleration_damping );

		if ( glm::length2 ( physics_controller.m_velocity ) > 0.01f )
		{
			physics_controller.m_controller->move ( { physics_controller.m_velocity.x , physics_controller.m_velocity.y , physics_controller.m_velocity.z } , 0.001f , 1.0f / 60.0f , 0 );
		}
		if ( physics_controller.m_jump != 0.0f )
		{
			physics_controller.m_controller->move ( { 0 , physics_controller.m_jump , 0 } , 0.001f , 1.0f / 60.0f , 0 );
		}

		// falling
		if ( physics_controller.m_jump > physics_controller.m_max_fall_speed )
		{
			physics_controller.m_jump -= DeltaTimer::m_dt;
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

		// initialize controller if not yet initialized
		if ( !physics_controller.m_controller_initialized )
		{
			auto& physics_controller_manager = engineResources.Get<PX::PhysicsController> ().get ();
			physics_controller.m_controller = physics_controller_manager.CreateController ( PX::PhysicsController::ControllerShape::CAPSULE ,
				engineResources.Get<PhysicsSystem> ().get ().GetPhysics ()->createMaterial ( physics_controller.m_static_friction , physics_controller.m_dynamic_friction , physics_controller.m_restitution ) ,
				{ transform.m_position.x, transform.m_position.y, transform.m_position.z } );
			physics_controller.m_controller_initialized = true;
		}

		auto const& controller_position = physics_controller.m_controller->getPosition ();
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
		physics_controller.m_controller_initialized = true;
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