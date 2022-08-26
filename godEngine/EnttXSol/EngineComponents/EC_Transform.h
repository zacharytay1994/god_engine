#pragma once

#include "EngineComponents.h"

#include <glm/glm/glm.hpp>

namespace god
{
	/* ENGINE COMPONENTS */
	struct Transform
	{
		glm::vec3 m_position;
		glm::vec3 m_rotation;
		glm::vec3 m_scale { 1.0f, 1.0f,1.0f };

		glm::mat4 m_parent_transform { glm::mat4 ( 1.0f ) };
		glm::mat4 m_local_transform { glm::mat4 ( 1.0f ) };
	};
	template <>
	inline void NewLuaType<Transform> ( sol::state& luaState , std::string const& name )
	{
		RegisterLuaType<Transform> ( luaState , name ,
			"position" , &Transform::m_position ,
			"rotation" , &Transform::m_rotation ,
			"scale" , &Transform::m_scale );
	}
	template<>
	inline void ComponentInspector::operator() < Transform > ( entt::entity entity , entt::registry& registry , int& imguiUniqueID , EngineResources& editorResources )
	{
		RegisterInspector<Transform , EngineResources> ( entity , registry , imguiUniqueID , editorResources ,
			[]( Transform& component , EngineResources& resources )
			{
				( resources );
				ImGui::Separator ();
				ImGui::Text ( "Transform" );
				ImGui::Separator ();

				auto width = ImGui::GetWindowWidth ();
				ImGui::SetNextItemWidth ( width / 4 );
				ImGui::InputFloat ( "##x" , &component.m_position.x );
				ImGui::SameLine ();
				ImGui::SetNextItemWidth ( width / 4 );
				ImGui::InputFloat ( "##y" , &component.m_position.y );
				ImGui::SameLine ();
				ImGui::SetNextItemWidth ( width / 4 );
				ImGui::InputFloat ( "Position" , &component.m_position.z );

				ImGui::SetNextItemWidth ( width / 4 );
				ImGui::InputFloat ( "##rx" , &component.m_rotation.x );
				ImGui::SameLine ();
				ImGui::SetNextItemWidth ( width / 4 );
				ImGui::InputFloat ( "##ry" , &component.m_rotation.y );
				ImGui::SameLine ();
				ImGui::SetNextItemWidth ( width / 4 );
				ImGui::InputFloat ( "Rotation" , &component.m_rotation.z );

				ImGui::SetNextItemWidth ( width / 4 );
				ImGui::InputFloat ( "##sx" , &component.m_scale.x );
				ImGui::SameLine ();
				ImGui::SetNextItemWidth ( width / 4 );
				ImGui::InputFloat ( "##sy" , &component.m_scale.y );
				ImGui::SameLine ();
				ImGui::SetNextItemWidth ( width / 4 );
				ImGui::InputFloat ( "Scale" , &component.m_scale.z );
			} );
	}

	template<>
	inline void JSONify<Transform> ( EngineResources& engineResources , rapidjson::Document& document , rapidjson::Value& value , Transform& component )
	{
		( engineResources );
		// serialize position
		RapidJSON::JSONifyToValue ( value , document , "position_x" , component.m_position.x );
		RapidJSON::JSONifyToValue ( value , document , "position_y" , component.m_position.y );
		RapidJSON::JSONifyToValue ( value , document , "position_z" , component.m_position.z );
		// serialize rotation
		RapidJSON::JSONifyToValue ( value , document , "rotation_x" , component.m_rotation.x );
		RapidJSON::JSONifyToValue ( value , document , "rotation_y" , component.m_rotation.y );
		RapidJSON::JSONifyToValue ( value , document , "rotation_z" , component.m_rotation.z );
		// serialize scale
		RapidJSON::JSONifyToValue ( value , document , "scale_x" , component.m_scale.x );
		RapidJSON::JSONifyToValue ( value , document , "scale_y" , component.m_scale.y );
		RapidJSON::JSONifyToValue ( value , document , "scale_z" , component.m_scale.z );
	}

	template<>
	inline void DeJSONify<Transform> ( EngineResources& engineResources , Transform& component , rapidjson::Value& jsonObj )
	{
		( engineResources );
		// deserialize position
		AssignIfExist ( jsonObj , component.m_position.x , "position_x" );
		AssignIfExist ( jsonObj , component.m_position.y , "position_y" );
		AssignIfExist ( jsonObj , component.m_position.z , "position_z" );
		// deserialize rotation
		AssignIfExist ( jsonObj , component.m_rotation.x , "rotation_x" );
		AssignIfExist ( jsonObj , component.m_rotation.y , "rotation_y" );
		AssignIfExist ( jsonObj , component.m_rotation.z , "rotation_z" );
		// deserialize scale
		AssignIfExist ( jsonObj , component.m_scale.x , "scale_x" );
		AssignIfExist ( jsonObj , component.m_scale.y , "scale_y" );
		AssignIfExist ( jsonObj , component.m_scale.z , "scale_z" );
	}
}