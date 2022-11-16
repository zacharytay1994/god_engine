#pragma once

#include "../EngineComponents.h"

namespace god
{
	/* ENGINE COMPONENTS */
	struct Billboard
	{
		bool facing_h { false };
	};
	template <>
	inline void NewLuaType<Billboard> ( sol::state& luaState , std::string const& name )
	{
		RegisterLuaType<Billboard> ( luaState , name ,
			"facing_h" , &Billboard::facing_h );
	}
	template<>
	inline void ComponentInspector::operator() < Billboard > ( entt::entity entity , entt::registry& registry , int& imguiUniqueID , EngineResources& editorResources )
	{
		RegisterInspector<Billboard , EngineResources> ( entity , registry , imguiUniqueID , editorResources ,
			[]( Billboard& component , EngineResources& resources )
			{
				( resources );
				ImGui::Separator ();
				ImGui::Text ( "Billboard" );
				ImGui::Separator ();


			} );
	}

	template<>
	inline void JSONify<Billboard> ( EngineResources& engineResources , rapidjson::Document& document , rapidjson::Value& value , Billboard& component )
	{
		( engineResources );
		// serialize
		RapidJSON::JSONifyToValue ( value , document , "facing_h" , component.facing_h );
	}

	template<>
	inline void DeJSONify<Billboard> ( EngineResources& engineResources , Billboard& component , rapidjson::Value& jsonObj )
	{
		( engineResources );
		// deserialize
		AssignIfExist ( jsonObj , component.facing_h , "facing_h" );
	}
}