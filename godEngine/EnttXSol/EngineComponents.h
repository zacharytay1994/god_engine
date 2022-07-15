#pragma once

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include <entt.hpp>
#include "../imgui/imgui.h"

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
		void operator()( entt::entity entity , entt::registry& registry , int& imguiUniqueID ) {}
	};

	template <typename T>
	struct EngineComponents
	{
		using Components = T;
		EngineComponents ( std::array<std::string , std::tuple_size_v<Components>> const& componentNames );
		Components const m_components;
		std::array<std::string , std::tuple_size_v<Components>> const m_component_names;
	};

	template<typename T>
	inline EngineComponents<T>::EngineComponents ( std::array<std::string , std::tuple_size_v<Components>> const& componentNames )
		:
		m_component_names ( componentNames )
	{
	}

	template<typename T , typename ...Args>
	void RegisterLuaType ( sol::state& luaState , std::string const& name , Args... args )
	{
		luaState.new_usertype<T> ( name , sol::constructors<T ()> () , args... );
	}

	template<typename T>
	void RegisterInspector ( entt::entity entity , entt::registry& registry , int& imguiUniqueID , void( *gui )( T& ) )
	{
		if ( registry.any_of<T> ( entity ) )
		{
			ImGui::PushID ( imguiUniqueID++ );
			auto& component = registry.get<T> ( entity );
			gui ( component );
			ImGui::PopID ();
		}
	}
}