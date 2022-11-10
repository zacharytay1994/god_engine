#pragma once

#include "../EngineComponents.h"

namespace god
{
	/* ENGINE COMPONENTS */
	enum class TextAlignment
	{
		LEFT = 0 ,
		CENTER ,
		RIGHT ,
		COUNT
	};

	struct GUIText
	{
		std::string m_text { "" };
		int m_alignment { static_cast< int >( TextAlignment::LEFT ) };
	};
	template <>
	inline void NewLuaType<GUIText> ( sol::state& luaState , std::string const& name )
	{
		RegisterLuaType<GUIText> ( luaState , name ,
			"text" , &GUIText::m_text ,
			"alignment" , &GUIText::m_alignment );
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

				ImGui::Text ( "Alignment" );
				ImGui::RadioButton ( "Left" , &component.m_alignment , static_cast< int >( TextAlignment::LEFT ) );
				ImGui::SameLine ();
				ImGui::RadioButton ( "Center" , &component.m_alignment , static_cast< int >( TextAlignment::CENTER ) );
				ImGui::SameLine ();
				ImGui::RadioButton ( "Right" , &component.m_alignment , static_cast< int >( TextAlignment::RIGHT ) );
			} );
	}

	template<>
	inline void JSONify<GUIText> ( EngineResources& engineResources , rapidjson::Document& document , rapidjson::Value& value , GUIText& component )
	{
		( engineResources );
		// serialize
		RapidJSON::JSONifyToValue ( value , document , "text" , component.m_text );
		RapidJSON::JSONifyToValue ( value , document , "alignment" , component.m_alignment );

	}

	template<>
	inline void DeJSONify<GUIText> ( EngineResources& engineResources , GUIText& component , rapidjson::Value& jsonObj )
	{
		( engineResources );
		// deserialize
		AssignIfExist ( jsonObj , component.m_text , "text" );
		AssignIfExist ( jsonObj , component.m_alignment , "alignment" );
	}
}