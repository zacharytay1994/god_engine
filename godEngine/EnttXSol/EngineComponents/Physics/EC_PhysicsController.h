#pragma once

#include "../EngineComponents.h"

namespace god
{
	/* ENGINE COMPONENTS */
	struct PhysicsController
	{
		float m_speed { 0.0f };
		glm::vec3 m_heading { 0,0,0 };

		float m_static_friction { 0.02f };
		float m_dynamic_friction { 0.05f };
		float m_restitution { 0.1f };

		// used for hacking jump behaviour
		float m_jump { 0.0f };
		float m_max_fall_speed { -0.35f };
		float m_old_foot_y_position { 0.0f };

		bool m_controller_initialized { false };
		physx::PxController* m_controller { nullptr };

		bool operator==( PhysicsController const& rhs )
		{
			( rhs );
			return true;
		}
	};
	template <>
	inline void NewLuaType<PhysicsController> ( sol::state& luaState , std::string const& name )
	{
		RegisterLuaType<PhysicsController> ( luaState , name ,
			"speed" , &PhysicsController::m_speed ,
			"jump" , &PhysicsController::m_jump ,
			"heading" , &PhysicsController::m_heading );
	}
	template<>
	inline void ComponentInspector::operator() < PhysicsController > ( entt::entity entity , entt::registry& registry , int& imguiUniqueID , EngineResources& editorResources )
	{
		RegisterInspector<PhysicsController , EngineResources> ( entity , registry , imguiUniqueID , editorResources ,
			[]( PhysicsController& component , EngineResources& resources )
			{
				( resources );
				ImGui::Separator ();
				ImGui::Text ( "Physics Controller" );
				ImGui::Separator ();

				ImGui::DragFloat ( "Speed" , &component.m_speed , 0.1f , 0.0f , 10'000.0f );
			} );
	}

	template<>
	inline void JSONify<PhysicsController> ( EngineResources& engineResources , rapidjson::Document& document , rapidjson::Value& value , PhysicsController& component )
	{
		( engineResources );
		// serialize
		RapidJSON::JSONifyToValue ( value , document , "speed" , component.m_speed );
	}

	template<>
	inline void DeJSONify<PhysicsController> ( EngineResources& engineResources , PhysicsController& component , rapidjson::Value& jsonObj )
	{
		( engineResources );
		// deserialize
		AssignIfExist ( jsonObj , component.m_speed , "speed" );
	}
}