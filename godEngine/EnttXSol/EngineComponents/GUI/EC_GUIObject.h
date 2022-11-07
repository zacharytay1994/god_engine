#pragma once

#include "../EngineComponents.h"

#pragma warning(disable: 4201)
#include <glm/glm.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#pragma warning(default: 4201)

namespace god
{
	/* ENGINE COMPONENTS */
	struct GUIObject
	{
		glm::vec2 m_position { 0,0 };
		int m_layer { 0 };
		glm::vec3 m_size { 0.1f,0.1f,1.0f };
		bool m_fixed_aspect_ratio { false };
		float m_aspect_ratio { 1.0f };
		std::string m_text { "" };

		bool m_hovered { false };
		bool m_pressed { false };
		bool m_down { false };
		bool m_released { false };
		bool m_enter { false };
		bool m_exit { false };
	};
	template <>
	inline void NewLuaType<GUIObject> ( sol::state& luaState , std::string const& name )
	{
		RegisterLuaType<GUIObject> ( luaState , name ,
			"position" , &GUIObject::m_position ,
			"layer" , &GUIObject::m_layer ,
			"size" , &GUIObject::m_size ,
			"fixed_ar" , &GUIObject::m_fixed_aspect_ratio ,
			"ar" , &GUIObject::m_aspect_ratio ,
			"text" , &GUIObject::m_text ,
			"hovered" , &GUIObject::m_hovered ,
			"pressed" , &GUIObject::m_pressed ,
			"down" , &GUIObject::m_down ,
			"up" , &GUIObject::m_released ,
			"enter" , &GUIObject::m_enter ,
			"exit" , &GUIObject::m_exit );
	}
	template<>
	inline void ComponentInspector::operator() < GUIObject > ( entt::entity entity , entt::registry& registry , int& imguiUniqueID , EngineResources& editorResources )
	{
		RegisterInspector<GUIObject , EngineResources> ( entity , registry , imguiUniqueID , editorResources ,
			[]( GUIObject& component , EngineResources& resources )
			{
				( resources );
				ImGui::Separator ();
				ImGui::Text ( "GUIObject" );
				ImGui::Separator ();

				ImGui::DragFloat2 ( "Pos" , glm::value_ptr ( component.m_position ) , 0.01f , 0.0f , 1.0f );
				ImGui::DragFloat2 ( "Size" , glm::value_ptr ( component.m_size ) , 0.01f , 0.0f , 1.0f );
				ImGui::DragInt ( "Z-Layer" , &component.m_layer , 1 , -250 , 250 );

				ImGui::Separator ();
				ImGui::Checkbox ( "Fixed Aspect-Ratio (Y=X*AR)" , &component.m_fixed_aspect_ratio );
				if ( component.m_fixed_aspect_ratio )
				{
					ImGui::DragFloat ( "Aspect-Ratio" , &component.m_aspect_ratio , 0.05f , 0.0f );
				}

				ImGui::Separator ();
				ImGui::InputText ( "Text" , &component.m_text );
			} );
	}

	template<>
	inline void JSONify<GUIObject> ( EngineResources& engineResources , rapidjson::Document& document , rapidjson::Value& value , GUIObject& component )
	{
		( engineResources );
		// serialize
		RapidJSON::JSONifyToValue ( value , document , "position_x" , component.m_position.x );
		RapidJSON::JSONifyToValue ( value , document , "position_y" , component.m_position.y );
		RapidJSON::JSONifyToValue ( value , document , "z_layer" , component.m_layer );
		RapidJSON::JSONifyToValue ( value , document , "size_x" , component.m_size.x );
		RapidJSON::JSONifyToValue ( value , document , "size_y" , component.m_size.y );
		RapidJSON::JSONifyToValue ( value , document , "size_z" , component.m_size.z );
		RapidJSON::JSONifyToValue ( value , document , "fixed_ar" , component.m_fixed_aspect_ratio );
		RapidJSON::JSONifyToValue ( value , document , "ar" , component.m_aspect_ratio );

	}

	template<>
	inline void DeJSONify<GUIObject> ( EngineResources& engineResources , GUIObject& component , rapidjson::Value& jsonObj )
	{
		( engineResources );
		// deserialize
		AssignIfExist ( jsonObj , component.m_position.x , "position_x" );
		AssignIfExist ( jsonObj , component.m_position.y , "position_y" );
		AssignIfExist ( jsonObj , component.m_layer , "z_layer" );
		AssignIfExist ( jsonObj , component.m_size.x , "size_x" );
		AssignIfExist ( jsonObj , component.m_size.y , "size_y" );
		AssignIfExist ( jsonObj , component.m_size.z , "size_z" );
		AssignIfExist ( jsonObj , component.m_fixed_aspect_ratio , "fixed_ar" );
		AssignIfExist ( jsonObj , component.m_aspect_ratio , "ar" );
	}
}