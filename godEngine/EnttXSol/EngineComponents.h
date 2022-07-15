#pragma once

#include <entt.hpp>

#include <tuple>
#include <string>
#include <array>

namespace sol
{
	class state;
}
namespace god
{
	template <typename T>
	void NewLuaType ( sol::state& luaState , std::string const& name ) {}

	struct ComponentInspector
	{
		template<typename T>
		void operator()( entt::entity entity , entt::registry& registry ) {}
	};

	/* ENGINE COMPONENTS */
	struct Position
	{
		float x , y , z;
	};
	template <>
	void NewLuaType<Position> ( sol::state& luaState , std::string const& name );
	template<>
	void ComponentInspector::operator() < Position > ( entt::entity entity , entt::registry& registry );

	template <typename...T>
	struct EngineComponents
	{
		using Components = std::tuple<T...>;
		EngineComponents ( std::array<std::string , std::tuple_size_v<Components>> const& componentNames );
		Components const m_components;
		std::array<std::string , std::tuple_size_v<Components>> const m_component_names;
	};

	template<typename ...T>
	inline EngineComponents<T...>::EngineComponents ( std::array<std::string , std::tuple_size_v<Components>> const& componentNames )
		:
		m_component_names ( componentNames )
	{
	}
}