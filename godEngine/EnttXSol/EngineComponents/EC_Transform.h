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
		glm::vec3 m_scale;
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
}