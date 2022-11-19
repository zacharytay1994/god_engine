#pragma once

#include "../EngineComponents.h"

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

namespace god
{
	/* ENGINE COMPONENTS */
	struct PointLight
	{
		glm::vec3 m_colour { 1.0f };
		glm::vec3 m_ambient { 1.0f };
		glm::vec3 m_diffuse { 1.0f };
		glm::vec3 m_specular { 1.0f };

		bool operator==( PointLight const& rhs )
		{
			return m_colour == rhs.m_colour &&
				m_ambient == rhs.m_ambient &&
				m_diffuse == rhs.m_diffuse &&
				m_specular == rhs.m_specular;
		}
	};

	template <>
	inline void NewLuaType<PointLight> ( sol::state& luaState , std::string const& name )
	{
		RegisterLuaType<PointLight> ( luaState , name ,
			"colour" , &PointLight::m_colour ,
			"ambient" , &PointLight::m_ambient ,
			"diffuse" , &PointLight::m_diffuse ,
			"specular" , &PointLight::m_specular );
	}
	template<>
	inline void ComponentInspector::operator() < PointLight > ( entt::entity entity , entt::registry& registry , int& imguiUniqueID , EngineResources& editorResources )
	{
		RegisterInspector<PointLight , EngineResources> ( entity , registry , imguiUniqueID , editorResources ,
			[]( PointLight& component , EngineResources& resources )
			{
				( resources );
				ImGui::Separator ();
				ImGui::Text ( "Point Light" );
				ImGui::Separator ();

				ImGui::DragFloat3 ( "Colour" , glm::value_ptr ( component.m_colour ) , 1.0f , 0.0f , 100.0f );
				ImGui::DragFloat3 ( "Ambient" , glm::value_ptr ( component.m_ambient ) , 1.0f , 0.0f , 100.0f );
				ImGui::DragFloat3 ( "Diffuse" , glm::value_ptr ( component.m_diffuse ) , 1.0f , 0.0f , 100.0f );
				ImGui::DragFloat3 ( "Specular" , glm::value_ptr ( component.m_specular ) , 1.0f , 0.0f , 100.0f );
			} );
	}

	template<>
	inline void JSONify<PointLight> ( EngineResources& engineResources , rapidjson::Document& document , rapidjson::Value& value , PointLight& component )
	{
		( engineResources );
		// serialize
		RapidJSON::JSONifyToValue ( value , document , "colour_x" , component.m_colour.x );
		RapidJSON::JSONifyToValue ( value , document , "colour_y" , component.m_colour.y );
		RapidJSON::JSONifyToValue ( value , document , "colour_z" , component.m_colour.z );

		RapidJSON::JSONifyToValue ( value , document , "ambient_x" , component.m_ambient.x );
		RapidJSON::JSONifyToValue ( value , document , "ambient_y" , component.m_ambient.y );
		RapidJSON::JSONifyToValue ( value , document , "ambient_z" , component.m_ambient.z );

		RapidJSON::JSONifyToValue ( value , document , "diffuse_x" , component.m_diffuse.x );
		RapidJSON::JSONifyToValue ( value , document , "diffuse_y" , component.m_diffuse.y );
		RapidJSON::JSONifyToValue ( value , document , "diffuse_z" , component.m_diffuse.z );

		RapidJSON::JSONifyToValue ( value , document , "specular_x" , component.m_specular.x );
		RapidJSON::JSONifyToValue ( value , document , "specular_y" , component.m_specular.y );
		RapidJSON::JSONifyToValue ( value , document , "specular_z" , component.m_specular.z );
	}

	template<>
	inline void DeJSONify<PointLight> ( EngineResources& engineResources , PointLight& component , rapidjson::Value& jsonObj )
	{
		( engineResources );
		// deserialize
		AssignIfExist ( jsonObj , component.m_colour.x , "colour_x" );
		AssignIfExist ( jsonObj , component.m_colour.y , "colour_y" );
		AssignIfExist ( jsonObj , component.m_colour.z , "colour_z" );

		AssignIfExist ( jsonObj , component.m_ambient.x , "ambient_x" );
		AssignIfExist ( jsonObj , component.m_ambient.y , "ambient_y" );
		AssignIfExist ( jsonObj , component.m_ambient.z , "ambient_z" );

		AssignIfExist ( jsonObj , component.m_diffuse.x , "diffuse_x" );
		AssignIfExist ( jsonObj , component.m_diffuse.y , "diffuse_y" );
		AssignIfExist ( jsonObj , component.m_diffuse.z , "diffuse_z" );

		AssignIfExist ( jsonObj , component.m_specular.x , "specular_x" );
		AssignIfExist ( jsonObj , component.m_specular.y , "specular_y" );
		AssignIfExist ( jsonObj , component.m_specular.z , "specular_z" );
	}
}