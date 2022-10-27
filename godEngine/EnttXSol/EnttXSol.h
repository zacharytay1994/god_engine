#pragma once

#define SOL_ALL_SAFETIES_ON 1

#pragma warning(disable: 4201)
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#pragma warning(default: 4201)

#include <entt.hpp>
#include <sol/sol.hpp>

#include <godUtility/TemplateManipulation.h>
#include <godUtility/FileIO.h>
#include <godUtility/Internal/RapidJSONWrapper.h>
#include <godUtility/Internal/RecycleVector.h>
#include <godUtility/Math.h>

#include "EngineComponents/EngineComponents.h"
#include "EngineComponents/EC_All.h"

#include "Internal/godEntity.h"
#include "LuaFunctionDefinitions.h"

#include <string>
#include <unordered_map>
#include <vector>
#include <tuple>
#include <iostream>
#include <functional>
#include <type_traits>
#include <queue>

namespace god
{
	using namespace entt::literals;
	struct EnttXSol
	{
		using Entities = RecycleVector<Entity_>;

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
		EnttXSol ();
		void Update ( EngineResources& engineResources );
		void ClearEntt ( EngineResources& engineResources );

		void NewScriptTemplate ( std::string const& newScript );
		void LoadScriptsFromFolder ();
		void LoadScript ( std::string const& scriptFile );
		void ReloadScript ( std::string const& scriptFile );
		void UnloadScript ( std::string const& scriptName );
		void DeleteScript ( std::string const& scriptFile );

		template<typename ENGINE_COMPONENTS>
		void BindEngineComponents ();
		template<typename T , typename ...ARGS>
		void RegisterLuaType ( std::string const& name , ARGS...args );
		template<typename FUNCTION>
		void RegisterLuaFunction ( std::string const& name , FUNCTION fn );
		template<typename ...T>
		void RunEngineSystem ( EngineResources& engineResources , void( *system )( EnttXSol& , EngineResources& , std::tuple<T...> ) );
		void BindEngineSystemUpdate (
			void( *update )( EnttXSol& , EngineResources& , bool ) ,
			void( *init )( EnttXSol& , EngineResources& ) ,
			void( *cleanup )( EnttXSol& , EngineResources& )
		);

		Entities::ID CreateEntity ( std::string const& name = "" , Entities::ID parent = Entities::Null );
		Entities::ID GetEntity ( std::string const& name );
		void RemoveEntity ( EntityGrid& grid , Entities::ID entity );

		template<typename ENGINE_COMPONENTS , typename EDITOR_RESOURCES>
		void SerializeEngineComponents ( Entities::ID entity , int& imguiUniqueID , EDITOR_RESOURCES& resources );
		template<typename T>
		using SerializeFunction = void( * )( T& val , int i , std::string const& name );
		void SerializeScriptComponents ( Entities::ID entity , int imguiUniqueID ,
			void( *Header )( std::string const& name ) ,
			SerializeFunction<bool> SerializeBool ,
			SerializeFunction<int> SerializeInt ,
			SerializeFunction<float> SerializeFloat ,
			SerializeFunction<std::string> SerializeString ,
			void( *RemoveCallback )( entt::registry& , entt::entity , int i , std::string const& name ) );

		template<typename T>
		void AttachComponent ( Entities::ID id );
		template<typename T>
		void RemoveComponent ( Entities::ID id );
		template<typename T>
		void AttachScript ( Entities::ID entity , std::string const& script );
		template<typename T>
		void AttachScriptSystem ( Entities::ID entity , std::string const& scriptSystem );
		void AttachEngineComponent ( Entities::ID entity , uint32_t componentID );

		template<typename T>
		T* GetEngineComponent ( Entities::ID entity );
		template<typename T>
		T* GetEngineComponent ( entt::entity e );
		template<typename T>
		void RemoveEngineComponent ( Entities::ID entity );

		template<typename T>
		bool HasEngineComponent ( entt::entity e );
		bool HasEngineComponent ( entt::entity e , std::string const& name );
		bool HasScriptComponent ( entt::entity e , std::string const& name );
		bool HasComponent ( entt::entity e , std::string const& name );

		void GetEntitiesWithScriptComponent ( std::string const& name , std::vector<entt::entity>& container );
		void GetEntitiesWithEngineComponent ( std::string const& name , std::vector<entt::entity>& container );
		void GetEntitiesWithComponent ( std::string const& name , std::vector<entt::entity>& container );

		std::unordered_map<std::string , Script> const& GetScripts () const;

		// S = scene, T = transform, R = renderable
		template <typename S , typename T , typename R>
		void PopulateScene ( S& scene );
		template <typename S , typename T , typename R>
		void RecursivePopulateScene ( S& scene , Entities::ID e , glm::mat4 parentTransform = glm::mat4 ( 1.0f ) );

		void SerializeStateV2 ( EngineResources& engineResources , std::string const& fileName );
		void SerializeStateV2Recurse ( EngineResources& engineResources , Entities::ID entity , rapidjson::Document& document , rapidjson::Value& value );

		void DeserializeStateV2 ( EngineResources& engineResources , std::string const& fileName , EntityGrid* grid = nullptr );
		void DeserializeStateV2Recurse ( EngineResources& engineResources , rapidjson::Value& value , std::string const& name , Entities::ID parent , EntityGrid* grid = nullptr );

		void SavePrefabV2 ( EngineResources& engineResources , Entities::ID root , std::string const& fileName );
		void SavePrefabV2Recurse ( EngineResources& engineResources , Entities::ID entity , rapidjson::Document& document , rapidjson::Value& value , bool root = false );

		EnttXSol::Entities::ID LoadPrefabV2 ( EngineResources& engineResources , std::string const& fileName , Entities::ID parent = Entities::Null , bool persist = true , EntityGrid* grid = nullptr );
		EnttXSol::Entities::ID LoadPrefabV2Recurse ( EngineResources& engineResources , rapidjson::Value& value , std::string const& name , Entities::ID parent , bool root = false , EntityGrid* grid = nullptr );
		EnttXSol::Entities::ID AddPrefabToScene ( EngineResources& engineResources , std::string const& fileName , Entities::ID parent = Entities::Null , glm::vec3 const& position = { 0,0,0 } , bool persist = true );

		void QueueInstancePrefab ( std::string const& name , float x = 0.0f , float y = 0.0f , float z = 0.0f , Entities::ID parent = Entities::Null , bool grid = false );
		EnttXSol::Entities::ID InstancePrefab ( EngineResources& engineResources , std::string const& name , Entities::ID parent = Entities::Null );

		template<typename...COMPONENTS>
		auto GetView ();

		friend void RegisterLuaCPP ( EnttXSol& entt , EngineResources& engineResources );

		// helper functor to attach script components
		struct AttachEngineComponentFunctor
		{
			template<typename T>
			void operator () ( EnttXSol* enttxsol , Entities::ID e );
		};

		struct CheckEngineComponentFunctor
		{
			template<typename T>
			void operator()( EnttXSol* enttxsol , entt::entity e , bool& b );
		};

		Entities m_entities;
		bool m_pause { true };

	private:
		sol::state m_lua;

		sol::load_result m_copy_table;

		entt::registry m_registry;

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
		std::unordered_map<std::string , Entities::ID> m_entity_pool;

		void( *m_engine_update )( EnttXSol& , EngineResources& engineResources , bool ) = nullptr;
		void( *m_engine_init )( EnttXSol& , EngineResources& engineResources ) = nullptr;
		void( *m_engine_cleanup )( EnttXSol& , EngineResources& engineResources ) = nullptr;

		void LoadSystem ( std::string const& name );
		bool AttachComponent ( Entities::ID id , std::string const& name );
		bool AttachComponent ( entt::entity id , std::string const& name );
	public:
		template <typename T>
		auto&& GetStorage ( std::string const& name );
	private:
		entt::runtime_view GetView ( std::vector<std::string> const& components , std::vector<std::string> const& engineComponents );

		void RecursiveRemoveEntity ( EntityGrid& grid , Entities::ID entity );
		//void RecursiveRemoveEntityFromGrid ( EntityGrid& grid , Entities::ID entity );

		std::queue<std::tuple<std::string , Entities::ID , float , float , float , bool>> m_instance_queue;
		void SetEntityActive ( EnttXSol::Entities::ID entity , bool active );
		void PrefabSetMaster ( EngineResources& engineResources , std::string const& fileName );
		EnttXSol::Entities::ID InstancePrefabFromMaster ( std::string const& fileName , Entities::ID parent = Entities::Null );
		Entities::ID MakeEntityCopy ( Entities::ID src , Entities::ID parent );

		// register engine components with lua
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
					return registry.get<T> ( e );
				};
				std::cout << "Bound " << name << std::endl;
			}
		};

		// builds the view with engine components
		struct AppendEngineComponentToView
		{
			template <typename T>
			void operator()( entt::registry& registry , entt::runtime_view& view )
			{
				view.iterate ( registry.storage<T> () );
			}
		};

		template <typename ENGINE_RESOURCES>
		struct SerializeEngineComponent
		{
			template <typename T>
			void operator()( entt::registry& registry , rapidjson::Document& document , rapidjson::Value& engineComponents , entt::entity const& entity , std::string const& name , ENGINE_RESOURCES& engineResources )
			{
				auto ptr = registry.try_get<T> ( entity );
				if ( ptr && !std::is_same<T , EntityData> () )
				{
					rapidjson::Value engine_component;
					engine_component.SetObject ();

					JSONify ( engineResources , document , engine_component , *ptr );

					RapidJSON::JSONifyToValue ( engineComponents , document , name , engine_component );
				}
			}
		};

		template <typename ENGINE_RESOURCES>
		struct DeserializeEngineComponent
		{
			template <typename T>
			void operator()( entt::registry& registry , entt::entity& entity , rapidjson::Value& jsonObj , ENGINE_RESOURCES& engineResources )
			{
				if ( !std::is_same<T , EntityData> () )
				{
					registry.emplace<T> ( entity );

					DeJSONify ( engineResources , registry.get<T> ( entity ) , jsonObj );
				}
			}
		};
	};

	template<typename ENGINE_COMPONENTS>
	inline void EnttXSol::BindEngineComponents ()
	{
		// register all engine components as lua types and bind their calling functions
		for ( auto i = 0; i < std::tuple_size_v<ENGINE_COMPONENTS::Components>; ++i )
		{
			T_Manip::RunOnType ( ENGINE_COMPONENTS::Components () , i , BindCTypeToLua () , std::ref ( m_lua ) , std::ref ( m_registry ) , ENGINE_COMPONENTS::m_component_names[ i ] );
		}
	}

	template<typename T , typename ...ARGS>
	inline void EnttXSol::RegisterLuaType ( std::string const& name , ARGS ...args )
	{
		m_lua.new_usertype<T> ( name , sol::constructors<T ()> () , args... );
	}

	template<typename FUNCTION>
	inline void EnttXSol::RegisterLuaFunction ( std::string const& name , FUNCTION fn )
	{
		m_lua[ name ] = fn;
	}

	template<typename ...T>
	inline void EnttXSol::RunEngineSystem ( EngineResources& engineResources , void( *system )( EnttXSol& , EngineResources& , std::tuple<T...> ) )
	{
		auto view = m_registry.view<std::remove_reference<T>::type...> ();
		//view.each ( system );

		for ( auto entity : view )
		{
			system ( *this , engineResources , view.get ( entity ) );
		}
	}

	template<typename ENGINE_COMPONENTS , typename EDITOR_RESOURCES>
	inline void EnttXSol::SerializeEngineComponents ( Entities::ID entity , int& imguiUniqueID , EDITOR_RESOURCES& editorResources )
	{
		// register all engine components as lua types and bind their calling functions
		for ( auto i = 0; i < std::tuple_size_v<ENGINE_COMPONENTS::Components>; ++i )
		{
			T_Manip::RunOnType ( ENGINE_COMPONENTS::Components () , i , ComponentInspector () , m_entities[ entity ].m_id , std::ref ( m_registry ) , imguiUniqueID , editorResources );
			++imguiUniqueID;
		}
	}

	template<typename T>
	inline void EnttXSol::AttachComponent ( Entities::ID id )
	{
		if ( !m_registry.storage<T> ().contains ( m_entities[ id ].m_id ) )
		{
			m_registry.emplace<T> ( m_entities[ id ].m_id );
		}
	}

	template<typename T>
	inline void EnttXSol::RemoveComponent ( Entities::ID id )
	{
		m_registry.remove<T> ( m_entities[ id ].m_id );
	}

	template<typename T>
	inline void EnttXSol::AttachScript ( Entities::ID entity , std::string const& script )
	{
		assert ( m_entities.Valid ( entity ) );

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
				for ( auto const& component : system.second.m_used_engine_components )
				{
					for ( int i = 0; i < T::m_component_names.size (); ++i )
					{
						if ( T::m_component_names[ i ] == component )
						{
							T_Manip::RunOnType ( T::Components () , i , AttachEngineComponentFunctor () , this , entity );
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
	inline void EnttXSol::AttachScriptSystem ( Entities::ID entity , std::string const& scriptSystem )
	{
		assert ( m_entities.Valid ( entity ) );

		for ( auto const& script : m_scripts )
		{
			// for each system in the script
			for ( auto const& system : script.second.m_systems )
			{
				if ( scriptSystem == system.first )
				{
					// add all script components used by this system to the entity
					for ( auto const& component : system.second.m_used_script_components )
					{
						AttachComponent ( entity , component );
					}
					// add all engine components used by this system to the entity
					for ( auto const& component : system.second.m_used_engine_components )
					{
						for ( int i = 0; i < T::m_component_names.size (); ++i )
						{
							if ( T::m_component_names[ i ] == component )
							{
								T_Manip::RunOnType ( T::Components () , i , AttachEngineComponentFunctor () , this , entity );
							}
						}
					}
					return;
				}
			}
		}
		std::cerr << "EnttXSol::AttachScriptSystem - System not found " << system << std::endl;
	}

	template<typename T>
	inline T* EnttXSol::GetEngineComponent ( Entities::ID entity )
	{
		if ( m_entities.Valid ( entity ) )
		{
			return m_registry.try_get<T> ( m_entities[ entity ].m_id );
		}
		return nullptr;
	}

	template<typename T>
	inline T* EnttXSol::GetEngineComponent ( entt::entity e )
	{
		return m_registry.try_get<T> ( e );
	}

	template<typename T>
	inline void EnttXSol::RemoveEngineComponent ( Entities::ID entity )
	{
		if ( m_entities.Valid ( entity ) )
		{
			m_registry.remove<T> ( m_entities[ entity ].m_id );
		}
	}

	template<typename T>
	inline bool EnttXSol::HasEngineComponent ( entt::entity e )
	{
		T* component = m_registry.try_get<T> ( e );
		return component != nullptr;
	}

	template<typename S , typename T , typename R>
	inline void EnttXSol::PopulateScene ( S& scene )
	{
		// add objects to scene
		scene.ClearInstancedScene ();
		for ( uint32_t i = 0; i < m_entities.Size (); ++i )
		{
			if ( m_entities.Valid ( i ) && m_entities[ i ].m_parent_id == Entities::Null && m_registry.storage<ActiveComponent> ().contains ( m_entities[ i ].m_id ) )
			{
				RecursivePopulateScene<S , T , R> ( scene , i );
			}
		}

		// add point light to scene
		scene.m_point_light_data.clear ();
		for ( auto&& [entity , active , transform , pointlight] : GetView<Transform , PointLight> ().each () )
		{
			scene.AddPointLight ( { glm::vec3 ( transform.m_parent_transform * glm::vec4 ( transform.m_position, 1.0f ) ),
				pointlight.m_colour, pointlight.m_ambient, pointlight.m_diffuse, pointlight.m_specular } );
		}

		// add directional light to scene
		scene.m_directional_light_data.clear ();
		for ( auto&& [entity , active , transform , directionallight] : GetView<Transform , DirectionalLight> ().each () )
		{
			scene.AddDirectionalLight ( { glm::vec3 ( transform.m_parent_transform * glm::vec4 ( transform.m_position, 1.0f ) ),
				directionallight.m_colour, directionallight.m_ambient, directionallight.m_diffuse, directionallight.m_specular } );
		}
	}

	template<typename S , typename T , typename R>
	inline void EnttXSol::RecursivePopulateScene ( S& scene , Entities::ID e , glm::mat4 parentTransform )
	{
		// if parent has both transform and renderable component
		if ( m_registry.all_of<T , R> ( m_entities[ e ].m_id ) )
		{
			auto [transform , renderable] = m_registry.get<T , R> ( m_entities[ e ].m_id );

			glm::mat4 model_transform = BuildModelMatrixRotDegrees ( transform.m_position , transform.m_rotation , transform.m_scale );

			transform.m_parent_transform = parentTransform;
			transform.m_local_transform = model_transform;

			auto model_xform_cat = parentTransform * model_transform;

			// add to scene
			if ( renderable.m_model_id != -1 )
			{
				scene.AddInstancedObject ( { static_cast< uint32_t >( renderable.m_model_id ) ,
					renderable.m_diffuse_id , renderable.m_specular_id , renderable.m_shininess } , model_xform_cat );
			}

			// populate scene with children
			for ( auto const& child : m_entities[ e ].m_children )
			{
				RecursivePopulateScene<S , T , R> ( scene , child , model_xform_cat );
			}
		}
		// if only transform component
		else if ( m_registry.all_of<T> ( m_entities[ e ].m_id ) )
		{
			auto& transform = m_registry.get<T> ( m_entities[ e ].m_id );

			glm::mat4 model_transform = BuildModelMatrixRotDegrees ( transform.m_position , transform.m_rotation , transform.m_scale );

			transform.m_parent_transform = parentTransform;
			transform.m_local_transform = model_transform;

			auto model_xform_cat = parentTransform * model_transform;

			// populate scene with children
			for ( auto const& child : m_entities[ e ].m_children )
			{
				RecursivePopulateScene<S , T , R> ( scene , child , model_xform_cat );
			}
		}
		// if neither, take the previous transform in the hierarchy, default identity matrix
		else
		{
			// populate scene with children
			for ( auto const& child : m_entities[ e ].m_children )
			{
				RecursivePopulateScene<S , T , R> ( scene , child , parentTransform );
			}
		}
	}

	template<typename ...COMPONENTS>
	inline auto EnttXSol::GetView ()
	{
		return m_registry.view<ActiveComponent , COMPONENTS...> ();
	}

	template<typename T>
	inline auto&& EnttXSol::GetStorage ( std::string const& name )
	{
		return m_registry.storage<T> ( entt::hashed_string ( name.c_str () ) );
	}

	template<typename T>
	inline void EnttXSol::AttachEngineComponentFunctor::operator()( EnttXSol* enttxsol , Entities::ID e )
	{
		enttxsol->AttachComponent<T> ( e );
	}

	template<typename T>
	inline void EnttXSol::CheckEngineComponentFunctor::operator()( EnttXSol* enttxsol , entt::entity e , bool& b )
	{
		b = enttxsol->HasEngineComponent<T> ( e );
	}
}