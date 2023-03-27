#pragma once

#include "../EngineComponents.h"

namespace god
{
	/* ENGINE COMPONENTS */
	struct PlayerController
	{
		int m_forward { GLFW_KEY_W };
		int m_backward { GLFW_KEY_S };
		int m_left { GLFW_KEY_A };
		int m_right { GLFW_KEY_D };

		glm::vec2 m_horizontal_input { 0.0f };

		bool operator==( PlayerController const& rhs )
		{
			( rhs );
			return true;
		}
	};
	template <>
	inline void NewLuaType<PlayerController> ( sol::state& luaState , std::string const& name )
	{
		RegisterLuaType<PlayerController> ( luaState , name ,
			"hinput" , &PlayerController::m_horizontal_input );
	}
	template<>
	inline void ComponentInspector::operator() < PlayerController > ( entt::entity entity , entt::registry& registry , int& imguiUniqueID , EngineResources& editorResources )
	{
		RegisterInspector<PlayerController , EngineResources> ( entity , registry , imguiUniqueID , editorResources ,
			[]( PlayerController& component , EngineResources& resources )
			{
				( resources );
				ImGui::Separator ();
				ImGui::Text ( "Player Controller" );
				ImGui::Separator ();

				/*ImGui::Text ( "This is an example engine component" );
				ImGui::InputFloat ( "a float" , &component.f );
				ImGui::InputText ( "a string" , &component.s );*/
			} );
	}

	template<>
	inline void JSONify<PlayerController> ( EngineResources& engineResources , rapidjson::Document& document , rapidjson::Value& value , PlayerController& component )
	{
		( engineResources );
		// serialize
		/*RapidJSON::JSONifyToValue ( value , document , "i" , component.i );
		RapidJSON::JSONifyToValue ( value , document , "f" , component.f );
		RapidJSON::JSONifyToValue ( value , document , "s" , component.s );*/
	}

	template<>
	inline void DeJSONify<PlayerController> ( EngineResources& engineResources , PlayerController& component , rapidjson::Value& jsonObj )
	{
		( engineResources );
		// deserialize
		/*AssignIfExist ( jsonObj , component.i , "i" );
		AssignIfExist ( jsonObj , component.f , "f" );
		AssignIfExist ( jsonObj , component.s , "s" );*/
	}
}