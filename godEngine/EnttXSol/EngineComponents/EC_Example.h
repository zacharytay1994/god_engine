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
	void ComponentInspector::operator() < ExampleComponent > ( entt::entity entity , entt::registry& registry , int& imguiUniqueID )
	{
		RegisterInspector<ExampleComponent> ( entity , registry , imguiUniqueID ,
			[]( ExampleComponent& component )
			{
				ImGui::Text ( "Example Component" );
				ImGui::Separator ();
				ImGui::Text ( "This is an example engine component" );
				ImGui::InputFloat ( "a float" , &component.f );
				ImGui::InputText ( "a string" , &component.s );
			} );
	}
}