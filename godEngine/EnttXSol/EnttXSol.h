#pragma once

#define SOL_ALL_SAFETIES_ON 1

#include <entt.hpp>
#include <sol/sol.hpp>

#include "../../godUtility/TemplateManipulation.h"
#include "EngineComponents.h"

#include <string>
#include <unordered_map>
#include <vector>
#include <stack>
#include <optional>
#include <tuple>
#include <iostream>

namespace god
{
	using namespace entt::literals;
	struct EnttXSol
	{
		using Entity = uint32_t;
		static constexpr uint32_t NullEntity = static_cast< uint32_t >( -1 );

	private:
		enum class AttributeTypes
		{
			BOOL ,
			INT ,
			FLOAT ,
			STRING
		};
		struct ComponentData
		{
			std::vector<std::tuple<std::string , AttributeTypes>> m_serialize_attributes;
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

	public:
		EnttXSol ( std::vector<std::string> scriptFiles );
		void Update ();
		template<typename EngineComponentsType>
		void BindEngineComponents ( EngineComponentsType const& components );
		template<typename T , typename ...Args>
		void RegisterLuaType ( std::string const& name , Args...args );

		entt::entity operator[]( Entity entity );
		entt::entity GetEntity ( Entity entity );
		Entity CreateEntity ( std::string const& name = "" );
		void RemoveEntity ( Entity entity );

		template<typename EngineComponentsType>
		void SerializeEngineComponents ( Entity entity , int& imguiUniqueID , EngineComponentsType const& components );
		template<typename T>
		using SerializeFunction = void( * )( T& val , int i , std::string const& name );
		void SerializeScriptComponents ( Entity entity , int imguiUniqueID ,
			void( *Header )( std::string const& name ) ,
			SerializeFunction<bool> SerializeBool ,
			SerializeFunction<int> SerializeInt ,
			SerializeFunction<float> SerializeFloat ,
			SerializeFunction<std::string> SerializeString );

		template<typename T>
		void AttachComponent ( Entity id );
		void AttachScript ( Entity entity , std::string const& script );

		std::vector<std::optional<entt::entity>> const& GetEntities () const;
		std::vector<std::string> const& GetEntityNames () const;
		std::unordered_map<std::string , Script> const& GetScripts () const;

	private:
		sol::state m_lua;
		entt::registry m_registry;

		std::vector<std::optional<entt::entity>> m_entities;
		std::vector<std::string> m_entity_names;
		std::stack<Entity> m_free_ids;

		// script identifiers
		std::string const m_identifier_component { "--[IsComponent]" };
		std::string const m_identifier_system { "--[IsSystem]" };

		std::string const m_identifier_serialize_input_bool { "--[SerializeBool]" };
		std::string const m_identifier_serialize_input_int { "--[SerializeInt]" };
		std::string const m_identifier_serialize_input_float { "--[SerializeFloat]" };
		std::string const m_identifier_serialize_input_string { "--[SerializeString]" };

		std::string const m_identifier_GetScriptComponent { "GetComponent" };
		std::string const m_identifier_GetEngineComponent { "GetEngineComponent" };

		std::unordered_map<std::string , Script> m_scripts;
		std::unordered_map<std::string , sol::function> m_sol_functions;

		void LoadScript ( std::string const& scriptFile );
		void LoadSystem ( std::string const& name );
		void AttachComponent ( Entity id , std::string const& name );
		template <typename T>
		auto&& GetStorage ( std::string const& name );
		entt::runtime_view GetView ( std::vector<std::string> const& components );

		// engine components
		struct BindCTypeToLua
		{
			template <typename T>
			void operator()( sol::state& luaState , entt::registry& registry , std::string const& name )
			{
				// set the type in lua
				NewLuaType<T> ( luaState , name );

				// set the calling function to get the type
				std::string key { "Get" };
				luaState[ ( key + name ).c_str () ] =
					[&registry]( entt::entity e )->T&
				{
					auto& component = registry.get<T> ( e );
					std::cout << component.m_position.x << std::endl;
					return registry.get<T> ( e );
				};
				std::cout << "Bound " << name << std::endl;
			}
		};
	};

	template<typename EngineComponentsType>
	inline void EnttXSol::BindEngineComponents ( EngineComponentsType const& components )
	{
		// register all engine components as lua types and bind their calling functions
		for ( auto i = 0; i < std::tuple_size_v<EngineComponentsType::Components>; ++i )
		{
			T_Manip::RunOnType ( EngineComponentsType::Components () , i , BindCTypeToLua () , std::ref ( m_lua ) , std::ref ( m_registry ) , components.m_component_names[ i ] );
		}
	}

	template<typename T , typename ...Args>
	inline void EnttXSol::RegisterLuaType ( std::string const& name , Args ...args )
	{
		m_lua.new_usertype<T> ( name , sol::constructors<T ()> () , args... );
	}

	template<typename EngineComponentsType>
	inline void EnttXSol::SerializeEngineComponents ( Entity entity , int& imguiUniqueID , EngineComponentsType const& components )
	{
		// register all engine components as lua types and bind their calling functions
		for ( auto i = 0; i < std::tuple_size_v<EngineComponentsType::Components>; ++i )
		{
			T_Manip::RunOnType ( EngineComponentsType::Components () , i , ComponentInspector () , GetEntity ( entity ) , std::ref ( m_registry ) , imguiUniqueID );
		}
	}

	template<typename T>
	inline void EnttXSol::AttachComponent ( Entity id )
	{
		m_registry.emplace<T> ( GetEntity ( id ) );
	}

	template<typename T>
	inline auto&& EnttXSol::GetStorage ( std::string const& name )
	{
		return m_registry.storage<T> ( entt::hashed_string ( name.c_str () ) );
	}

	using Entity = EnttXSol::Entity;
}