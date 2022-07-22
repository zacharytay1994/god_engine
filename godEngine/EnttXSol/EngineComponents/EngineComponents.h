#pragma once

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include <entt.hpp>
#include "../../imgui/imgui.h"
#include "../../imgui/imgui_stdlib.h"

#include "../../Editor/EngineResources.h"

#include <tuple>
#include <string>
#include <array>

namespace god
{
	template <typename T>
	void NewLuaType ( sol::state& luaState , std::string const& name ) {}

	struct ComponentInspector
	{
		template<typename T>
		void operator()( entt::entity entity , entt::registry& registry , int& imguiUniqueID , EngineResources& editorResources ) {}
	};

	template<typename T , typename ...Args>
	void RegisterLuaType ( sol::state& luaState , std::string const& name , Args... args )
	{
		luaState.new_usertype<T> ( name , sol::constructors<T ()> () , args... );
	}

	template<typename T , typename U>
	void RegisterInspector ( entt::entity entity , entt::registry& registry , int& imguiUniqueID , U& editorResources , void( *gui )( T& , U& ) )
	{
		if ( registry.any_of<T> ( entity ) )
		{
			ImGui::PushID ( imguiUniqueID++ );
			auto& component = registry.get<T> ( entity );
			gui ( component , editorResources );
			ImGui::PopID ();
		}
	}
}