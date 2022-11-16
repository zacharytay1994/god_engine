#pragma once

#include "../EngineComponents.h"

namespace god
{
	/* ENGINE COMPONENTS */
	struct Transparent
	{
		bool facing_h { false };
	};
	template <>
	inline void NewLuaType<Transparent> ( sol::state& luaState , std::string const& name )
	{
		RegisterLuaType<Transparent> ( luaState , name ,
			"facing_h" , &Transparent::facing_h );
	}
	template<>
	inline void ComponentInspector::operator() < Transparent > ( entt::entity entity , entt::registry& registry , int& imguiUniqueID , EngineResources& editorResources )
	{
		RegisterInspector<Transparent , EngineResources> ( entity , registry , imguiUniqueID , editorResources ,
			[]( Transparent& component , EngineResources& resources )
			{
				( resources );
				ImGui::Separator ();
				ImGui::Text ( "Transparent" );
				ImGui::Separator ();


			} );
	}

	template<>
	inline void JSONify<Transparent> ( EngineResources& engineResources , rapidjson::Document& document , rapidjson::Value& value , Transparent& component )
	{
		( engineResources );
		// serialize
		RapidJSON::JSONifyToValue ( value , document , "facing_h" , component.facing_h );
	}

	template<>
	inline void DeJSONify<Transparent> ( EngineResources& engineResources , Transparent& component , rapidjson::Value& jsonObj )
	{
		( engineResources );
		// deserialize
		AssignIfExist ( jsonObj , component.facing_h , "facing_h" );
	}
}