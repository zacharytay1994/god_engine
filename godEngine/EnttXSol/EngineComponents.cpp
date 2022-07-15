#include "../pch.h"
#include "../imgui/imgui.h"

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include "EngineComponents.h"

namespace god
{
	template <>
	void NewLuaType<Position> ( sol::state& luaState , std::string const& name )
	{
		luaState.new_usertype<Position> ( name , sol::constructors<Position ()> () ,
			"x" , &Position::x ,
			"y" , &Position::y ,
			"z" , &Position::z
			);
	}

	template<>
	void ComponentInspector::operator()<Position> ( entt::entity entity , entt::registry& registry )
	{
		if ( registry.any_of<Position> ( entity ) )
		{
			auto& component = registry.get<Position> ( entity );
			ImGui::InputFloat ( "testx" , &component.x );
			ImGui::InputFloat ( "testy" , &component.y );
			ImGui::InputFloat ( "testz" , &component.z );
		}
	}
}