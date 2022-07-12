#pragma once

#define SOL_ALL_SAFETIES_ON 1

#include <entt.hpp>
#include <sol/sol.hpp>

#include "InBuiltComponents.h"

#include <string>
#include <unordered_map>
#include <vector>
#include <stack>
#include <optional>
#include <tuple>

namespace god
{
	using namespace entt::literals;
	struct EnttXSol
	{
		using Entity = uint32_t;

		EnttXSol ( std::vector<std::string> scriptFiles );
		void Update ();

		entt::entity operator[]( Entity entity );
		entt::entity GetEntity ( Entity entity );
		Entity CreateEntity ( std::string const& name = "" );
		void RemoveEntity ( Entity entity );

		void AttachScript ( Entity entity , std::string const& script );

		std::vector<std::optional<entt::entity>> const& GetEntities ();
		std::vector<std::string> const& GetEntityNames ();

	private:
		sol::state m_lua;
		entt::registry m_registry;

		std::vector<std::optional<entt::entity>> m_entities;
		std::vector<std::string> m_entity_names;
		std::stack<Entity> m_free_ids;

		std::string const m_identifier_component { "--[IsComponent]" };
		std::string const m_identifier_system { "--[IsSystem]" };

		struct ComponentData
		{

		};
		struct SystemData
		{
			std::vector<std::string> m_used_components;
		};
		struct Script
		{
			std::unordered_map<std::string , ComponentData> m_components;
			std::unordered_map<std::string , SystemData> m_systems;
		};
		std::unordered_map<std::string , Script> m_scripts;
		std::unordered_map<std::string , sol::function> m_sol_functions;

		void LoadScript ( std::string const& scriptFile );
		void LoadSystem ( std::string const& name );
		void AttachComponent ( Entity id , std::string const& name );
		template <typename T>
		auto&& GetStorage ( std::string const& name );
		entt::runtime_view GetView ( std::vector<std::string> const& components );

		using EngineComponents = std::tuple<Position , Velocity>;
	};

	template<typename T>
	inline auto&& EnttXSol::GetStorage ( std::string const& name )
	{
		return m_registry.storage<T> ( entt::hashed_string ( name.c_str () ) );
	}

	using Entity = EnttXSol::Entity;
}