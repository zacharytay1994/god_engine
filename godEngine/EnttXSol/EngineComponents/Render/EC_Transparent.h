#pragma once

#include "../EngineComponents.h"

namespace god
{
	/* ENGINE COMPONENTS */
	struct Transparent
	{
		bool m_facing_horizontal { true };

		bool operator==( Transparent const& rhs )
		{
			return m_facing_horizontal == rhs.m_facing_horizontal;
		}
	};
	template <>
	inline void NewLuaType<Transparent> ( sol::state& luaState , std::string const& name )
	{
		RegisterLuaType<Transparent> ( luaState , name ,
			"facing_horizontal" , &Transparent::m_facing_horizontal );
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

				ImGui::Checkbox ( "Face Camera [H]" , &component.m_facing_horizontal );
			} );
	}

	template<>
	inline void JSONify<Transparent> ( EngineResources& engineResources , rapidjson::Document& document , rapidjson::Value& value , Transparent& component )
	{
		( engineResources );
		// serialize
		//RapidJSON::JSONifyToValue ( value , document , "facing_h" , component.facing_h );
		RapidJSON::JSONifyToValue ( value , document , "facing_horizontal" , component.m_facing_horizontal );
	}

	template<>
	inline void DeJSONify<Transparent> ( EngineResources& engineResources , Transparent& component , rapidjson::Value& jsonObj )
	{
		( engineResources );
		// deserialize
		//AssignIfExist ( jsonObj , component.facing_h , "facing_h" );
		AssignIfExist ( jsonObj , component.m_facing_horizontal , "facing_horizontal" );
	}
}