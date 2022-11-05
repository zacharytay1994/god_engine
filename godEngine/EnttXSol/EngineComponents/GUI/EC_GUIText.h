#pragma once

#include "../EngineComponents.h"

namespace god
{
	/* ENGINE COMPONENTS */
	struct GUIText
	{
		std::string m_text { "" };
	};
	template <>
	inline void NewLuaType<GUIText> ( sol::state& luaState , std::string const& name )
	{
		RegisterLuaType<GUIText> ( luaState , name ,
			"text" , &GUIText::m_text );
	}
	template<>
	inline void ComponentInspector::operator() < GUIText > ( entt::entity entity , entt::registry& registry , int& imguiUniqueID , EngineResources& editorResources )
	{
		RegisterInspector<GUIText , EngineResources> ( entity , registry , imguiUniqueID , editorResources ,
			[]( GUIText& component , EngineResources& resources )
			{
				ImGui::Separator ();
				ImGui::Text ( "GUIText" );
				ImGui::Separator ();

				ImGui::Separator ();
				ImGui::InputText ( "Text" , &component.m_text );
			} );
	}

	template<>
	inline void JSONify<GUIText> ( EngineResources& engineResources , rapidjson::Document& document , rapidjson::Value& value , GUIText& component )
	{
		( engineResources );
		// serialize
		RapidJSON::JSONifyToValue ( value , document , "text" , component.m_text );

	}

	template<>
	inline void DeJSONify<GUIText> ( EngineResources& engineResources , GUIText& component , rapidjson::Value& jsonObj )
	{
		( engineResources );
		// deserialize
		AssignIfExist ( jsonObj , component.m_text , "text" );
	}
}