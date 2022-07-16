#pragma once

#define SOL_ALL_SAFETIES_ON 1

#include <entt.hpp>
#include <sol/sol.hpp>

#include "../../godUtility/TemplateManipulation.h"
//#include "EngineComponents/EC_All.h"
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
			std::vector<std::string> m_used_script_components;
			std::vector<std::string> m_used_engine_components;
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
		template<typename T>
		void AttachScript ( Entity entity , std::string const& script , T const& engineComponents );

		std::vector<std::optional<entt::entity>> const& GetEntities () const;
		std::vector<std::string> const& GetEntityNames () const;
		std::unordered_map<std::string , Script> const& GetScripts () const;

		// helper functor to attach script components
		struct AttachEngineComponent
		{
			template<typename T>
			void operator () ( EnttXSol* enttxsol , Entity e );
		};
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
	inline void EnttXSol::AttachScript ( Entity entity , std::string const& script , T const& engineComponents )
	{
		assert ( entity < m_entities.size () && m_entities[ entity ].has_value () );
		if ( m_scripts.find ( script ) != m_scripts.end () )
		{
			// for each system in the script
			for ( auto const& system : m_scripts.at ( script ).m_systems )
			{
				// add all script components used by this system to the entity
				for ( auto const& component : system.second.m_used_script_components )
				{
					AttachComponent ( entity , component );
				}
				// add all engine components used by this system to the entity
				auto const& engine_component_names = engineComponents.m_component_names;
				for ( auto const& component : system.second.m_used_engine_components )
				{
					for ( int i = 0; i < engine_component_names.size (); ++i )
					{
						if ( engine_component_names[ i ] == component )
						{
							T_Manip::RunOnType ( T::Components () , i , AttachEngineComponent () , this , entity );
						}
					}
				}
			}
		}
		else
		{
			std::cerr << "EnttXSol::AttachScript - Script not found " << script << std::endl;
		}
	}

	template<typename T>
	inline auto&& EnttXSol::GetStorage ( std::string const& name )
	{
		return m_registry.storage<T> ( entt::hashed_string ( name.c_str () ) );
	}

	using Entity = EnttXSol::Entity;
	template<typename T>
	inline void EnttXSol::AttachEngineComponent::operator()( EnttXSol* enttxsol , Entity e )
	{
		enttxsol->AttachComponent<T> ( e );
	}
}