#pragma once

#include "EngineComponents.h"

namespace god
{
	/* ENGINE COMPONENTS */
	struct ExampleComponent
	{
		int i { 2 };
		float f { 3.14f };
		std::string s { "example" };
	};
	template <>
	inline void NewLuaType<ExampleComponent> ( sol::state& luaState , std::string const& name )
	{
		RegisterLuaType<ExampleComponent> ( luaState , name ,
			"i" , &ExampleComponent::i ,
			"f" , &ExampleComponent::f ,
			"s" , &ExampleComponent::s );
	}
	template<>
	inline void ComponentInspector::operator() < ExampleComponent > ( entt::entity entity , entt::registry& registry , int& imguiUniqueID , EngineResources& editorResources )
	{
		RegisterInspector<ExampleComponent , EngineResources> ( entity , registry , imguiUniqueID , editorResources ,
			[]( ExampleComponent& component , EngineResources& resources )
			{
				( resources );
				ImGui::Separator ();
				ImGui::Text ( "Example Component" );
				ImGui::Separator ();

				ImGui::Text ( "This is an example engine component" );
				ImGui::InputFloat ( "a float" , &component.f );
				ImGui::InputText ( "a string" , &component.s );
			} );
	}

	template<>
	inline void JSONify<ExampleComponent> ( EngineResources& engineResources , rapidjson::Document& document , rapidjson::Value& value , ExampleComponent& component )
	{
		( engineResources );
		// serialize
		RapidJSON::JSONifyToValue ( value , document , "i" , component.i );
		RapidJSON::JSONifyToValue ( value , document , "f" , component.f );
		RapidJSON::JSONifyToValue ( value , document , "s" , component.s );
	}

	template<>
	inline void DeJSONify<ExampleComponent> ( EngineResources& engineResources , ExampleComponent& component , rapidjson::Value& jsonObj )
	{
		( engineResources );
		// deserialize
		AssignIfExist ( jsonObj , component.i , "i" );
		AssignIfExist ( jsonObj , component.f , "f" );
		AssignIfExist ( jsonObj , component.s , "s" );
	}
}