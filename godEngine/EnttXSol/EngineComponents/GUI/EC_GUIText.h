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
		float m_padding_left { 0.0f };
		float m_padding_right { 0.0f };
		float m_padding_top { 0.0f };
		float m_size { 0.2f };

		bool operator==( GUIText const& rhs )
		{
			return m_text == rhs.m_text &&
				m_alignment == rhs.m_alignment &&
				m_padding_left == rhs.m_padding_left &&
				m_padding_right == rhs.m_padding_right &&
				m_padding_top == rhs.m_padding_top &&
				m_size == rhs.m_size;
		}
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
				( resources );
				ImGui::Separator ();
				ImGui::Text ( "GUIText" );
				ImGui::Separator ();

				ImGui::Separator ();
				ImGui::InputText ( "Text" , &component.m_text );

				ImGui::DragFloat ( "Size" , &component.m_size , 0.01f , 0.01f , 2.0f );

				ImGui::Text ( "Alignment" );
				ImGui::RadioButton ( "Left" , &component.m_alignment , static_cast< int >( TextAlignment::LEFT ) );
				ImGui::SameLine ();
				ImGui::RadioButton ( "Center" , &component.m_alignment , static_cast< int >( TextAlignment::CENTER ) );
				ImGui::SameLine ();
				ImGui::RadioButton ( "Right" , &component.m_alignment , static_cast< int >( TextAlignment::RIGHT ) );

				ImGui::DragFloat ( "Pad Left" , &component.m_padding_left , 0.01f , 0.0f , 2.0f );
				ImGui::DragFloat ( "Pad Right" , &component.m_padding_right , 0.01f , 0.0f , 2.0f );
				ImGui::DragFloat ( "Pad Top" , &component.m_padding_top , 0.01f , 0.0f , 2.0f );
			} );
	}

	template<>
	inline void JSONify<GUIText> ( EngineResources& engineResources , rapidjson::Document& document , rapidjson::Value& value , GUIText& component )
	{
		( engineResources );
		// serialize
		RapidJSON::JSONifyToValue ( value , document , "text" , component.m_text );
		RapidJSON::JSONifyToValue ( value , document , "alignment" , component.m_alignment );
		RapidJSON::JSONifyToValue ( value , document , "paddingleft" , component.m_padding_left );
		RapidJSON::JSONifyToValue ( value , document , "paddingright" , component.m_padding_right );
		RapidJSON::JSONifyToValue ( value , document , "paddingtop" , component.m_padding_top );
		RapidJSON::JSONifyToValue ( value , document , "size" , component.m_size );

	}

	template<>
	inline void DeJSONify<GUIText> ( EngineResources& engineResources , GUIText& component , rapidjson::Value& jsonObj )
	{
		( engineResources );
		// deserialize
		AssignIfExist ( jsonObj , component.m_text , "text" );
		AssignIfExist ( jsonObj , component.m_alignment , "alignment" );
		AssignIfExist ( jsonObj , component.m_padding_left , "paddingleft" );
		AssignIfExist ( jsonObj , component.m_padding_right , "paddingright" );
		AssignIfExist ( jsonObj , component.m_padding_top , "paddingtop" );
		AssignIfExist ( jsonObj , component.m_size , "size" );
	}
}