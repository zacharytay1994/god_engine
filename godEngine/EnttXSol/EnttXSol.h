#pragma once

#define SOL_ALL_SAFETIES_ON 1

#pragma warning(disable: 4201)
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#pragma warning(default: 4201)

#include <entt.hpp>
#include <sol/sol.hpp>

#include "../../godUtility/TemplateManipulation.h"
#include "../../godUtility/FileIO.h"
#include "../../godUtility/Internal/RapidJSONWrapper.h"
#include "../../godUtility/Internal/RecycleVector.h"

#include "EngineComponents/EngineComponents.h"
#include "EngineComponents/EC_All.h"

#include "Internal/godEntity.h"

#include <string>
#include <unordered_map>
#include <vector>
#include <stack>
#include <optional>
#include <tuple>
#include <iostream>
#include <functional>

namespace god
{
	using namespace entt::literals;
	struct EnttXSol
	{
		/*using Entity = uint32_t;
		static constexpr uint32_t NullEntity = static_cast< uint32_t >( -1 );*/

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
		EnttXSol ( std::vector<std::string> scriptFiles );
		void Update ();
		template<typename ENGINE_COMPONENTS>
		void BindEngineComponents ();
		template<typename T , typename ...ARGS>
		void RegisterLuaType ( std::string const& name , ARGS...args );
		template<typename ...T>
		void RunEngineSystem ( void( *system )( T... ) );
		void BindEngineSystemUpdate ( void( *update )( EnttXSol& ) );

		/*entt::entity operator[]( Entity entity );
		entt::entity GetEntity ( Entity entity );*/
		/*Entity CreateEntity ( std::string const& name = "" , Entity parent = NullEntity );
		Entity LoadEntity ( std::string const& name = "" , Entity parent = NullEntity );
		void RemoveEntity ( Entity entity , uint32_t childIndex = NullEntity );*/
		Entities::ID CreateEntity ( std::string const& name = "" , bool root = false , Entities::ID parent = Entities::Null );
		Entities::ID CreatePrefab ( std::string const& name = "" , bool root = false , Entities::ID parent = Entities::Null );
		Entities::ID LoadEntity ( std::string const& name = "" , Entities::ID parent = Entities::Null );
		void RemoveEntity ( Entities::ID entity );

		template<typename ENGINE_COMPONENTS , typename EDITOR_RESOURCES>
		void SerializeEngineComponents ( Entities::ID entity , int& imguiUniqueID , EDITOR_RESOURCES& resources );
		template<typename T>
		using SerializeFunction = void( * )( T& val , int i , std::string const& name );
		void SerializeScriptComponents ( Entities::ID entity , int imguiUniqueID ,
			void( *Header )( std::string const& name ) ,
			SerializeFunction<bool> SerializeBool ,
			SerializeFunction<int> SerializeInt ,
			SerializeFunction<float> SerializeFloat ,
			SerializeFunction<std::string> SerializeString );

		template<typename T>
		void AttachComponent ( Entities::ID id );
		template<typename T>
		void AttachScript ( Entities::ID entity , std::string const& script );
		template<typename T>
		void AttachScriptSystem ( Entities::ID entity , std::string const& scriptSystem );
		void AttachEngineComponent ( Entities::ID entity , uint32_t componentID );

		std::vector<std::optional<entt::entity>> const& GetEntities () const;

		/*struct EntityData
		{
			std::string m_name { "" };
			Entity m_parent { NullEntity };
			std::vector<Entity> m_children;
			std::vector<uint32_t> m_prefab_children;
		};*/
		//std::vector<EntityData> const& GetEntityData () const;

		std::unordered_map<std::string , Script> const& GetScripts () const;

		//using EntityPack = std::tuple<entt::entity , EntityData>;
		//using Prefab = std::tuple<std::string , std::vector<EntityPack>>;

		// S = scene, T = transform, R = renderable
		template <typename S , typename T , typename R>
		void PopulateScene ( S& scene );
		template <typename S , typename T , typename R>
		void RecursivePopulateScene ( S& scene , Entities::ID e , glm::mat4 parentTransform = glm::mat4 ( 1.0f ) );

		//template <typename S , typename T , typename R>
		//void RecursivePopulateScenePrefab ( S& scene , Prefab const& prefab , glm::mat4 parentTransform = glm::mat4 ( 1.0f ) , uint32_t i = 0 );

		void SerializeState ( EngineResources& engineResources , std::string const& filePath );
		void DeserializeState ( EngineResources& engineResources , std::string const& filePath );

		// saving an entity and all its children as a prefab file
		void SerializeEntity ( EngineResources& engineResources , rapidjson::Document& document , Entities::ID entity , int parent , int& count );
		void SavePrefab ( EngineResources& engineResources , Entities::ID root , std::string const& filePath );
		// loading a prefab file, attaching it to an entity
		Entities::ID LoadPrefab ( EngineResources& engineResources , std::string const& fileName , Entities::ID parent = Entities::Null );
		// removing prefab
		//void RemovePrefab ( uint32_t id );

		//std::vector<Prefab> const& GetPrefabs ();

		// helper functor to attach script components
		struct AttachEngineComponentFunctor
		{
			template<typename T>
			void operator () ( EnttXSol* enttxsol , Entities::ID e );
		};

		Entities m_entities;

	private:
		sol::state m_lua;
		entt::registry m_registry;

		/*std::vector<std::optional<entt::entity>> m_entities;
		std::vector<EntityData> m_entity_data;
		std::stack<Entity> m_free_ids;*/


		// for prefabs
		/*std::vector<Prefab> m_prefabs;
		std::stack<uint32_t> m_free_prefabs_ids;*/

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

		void( *m_engine_update )( EnttXSol& ) = nullptr;

		void LoadScript ( std::string const& scriptFile );
		void LoadSystem ( std::string const& name );
		void AttachComponent ( Entities::ID id , std::string const& name );
		void AttachComponent ( entt::entity id , std::string const& name );
		template <typename T>
		auto&& GetStorage ( std::string const& name );
		entt::runtime_view GetView ( std::vector<std::string> const& components , std::vector<std::string> const& engineComponents );

		void RecursiveRemoveEntity ( Entities::ID entity );

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
				if ( ptr )
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
				registry.emplace<T> ( entity );

				DeJSONify ( engineResources , registry.get<T> ( entity ) , jsonObj );
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

	template<typename ...T>
	inline void EnttXSol::RunEngineSystem ( void( *system )( T... ) )
	{
		auto view = m_registry.view<std::remove_reference<T>::type...> ();
		view.each ( system );
	}

	template<typename ENGINE_COMPONENTS , typename EDITOR_RESOURCES>
	inline void EnttXSol::SerializeEngineComponents ( Entities::ID entity , int& imguiUniqueID , EDITOR_RESOURCES& editorResources )
	{
		// register all engine components as lua types and bind their calling functions
		for ( auto i = 0; i < std::tuple_size_v<ENGINE_COMPONENTS::Components>; ++i )
		{
			T_Manip::RunOnType ( ENGINE_COMPONENTS::Components () , i , ComponentInspector () , m_entities[ entity ].m_id , std::ref ( m_registry ) , imguiUniqueID , editorResources );
		}
	}

	template<typename T>
	inline void EnttXSol::AttachComponent ( Entities::ID id )
	{
		if ( !m_registry.all_of<T> ( m_entities[ id ].m_id ) )
		{
			m_registry.emplace<T> ( m_entities[ id ].m_id );
		}
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
				//auto const& engine_component_names = engineComponents.m_component_names;
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
					//auto const& engine_component_names = engineComponents.m_component_names;
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

	template<typename S , typename T , typename R>
	inline void EnttXSol::PopulateScene ( S& scene )
	{
		scene.ClearScene ();

		for ( auto i = 0; i < m_entities.Size (); ++i )
		{
			if ( m_entities.Valid ( i ) && m_entities[ i ].m_parent_id == Entities::Null )
			{
				RecursivePopulateScene<S , T , R> ( scene , i );
			}
		}

		/*for ( auto i = 0; i < m_prefabs.size (); ++i )
		{
			if ( std::get<1> ( std::get<1> ( m_prefabs[ i ] )[ 0 ] ).m_parent == NullEntity )
			{
				RecursivePopulateScenePrefab<S , T , R> ( scene , m_prefabs[ i ] );
			}
		}*/
	}

	template<typename S , typename T , typename R>
	inline void EnttXSol::RecursivePopulateScene ( S& scene , Entities::ID e , glm::mat4 parentTransform )
	{
		// if parent has both transform and renderable component
		if ( m_registry.all_of<T , R> ( m_entities[ e ].m_id ) )
		{
			auto const& [transform , renderable] = m_registry.get<T , R> ( m_entities[ e ].m_id );

			glm::mat4 model_transform = glm::mat4 ( 1.0f );
			model_transform = glm::translate ( model_transform , transform.m_position );
			model_transform = glm::rotate ( model_transform , transform.m_rotation.x , glm::vec3 ( 1.0f , 0.0f , 0.0f ) );
			model_transform = glm::rotate ( model_transform , transform.m_rotation.y , glm::vec3 ( 0.0f , 1.0f , 0.0f ) );
			model_transform = glm::rotate ( model_transform , transform.m_rotation.z , glm::vec3 ( 0.0f , 0.0f , 1.0f ) );
			model_transform = glm::scale ( model_transform , transform.m_scale );

			auto model_xform_cat = parentTransform * model_transform;

			// add to scene
			if ( renderable.m_model_id != -1 )
			{
				auto& object = scene.GetSceneObject ( scene.AddSceneObject ( renderable.m_model_id , model_xform_cat ) );
				object.m_diffuse_id = renderable.m_diffuse_id;
				object.m_specular_id = renderable.m_specular_id;
				object.m_shininess = renderable.m_shininess;
			}

			// populate scene with children
			for ( auto const& child : m_entities[ e ].m_children )
			{
				RecursivePopulateScene<S , T , R> ( scene , child , model_xform_cat );
			}

			// populate scene with prefab children
			/*for ( auto const& child : m_entity_data[ e ].m_prefab_children )
			{
				RecursivePopulateScenePrefab<S , T , R> ( scene , m_prefabs[ child ] , model_xform_cat );
			}*/
		}
		// if only transform component
		else if ( m_registry.all_of<T> ( m_entities[ e ].m_id ) )
		{
			auto const& transform = m_registry.get<T> ( m_entities[ e ].m_id );

			glm::mat4 model_transform = glm::mat4 ( 1.0f );
			model_transform = glm::translate ( model_transform , transform.m_position );
			model_transform = glm::rotate ( model_transform , transform.m_rotation.x , glm::vec3 ( 1.0f , 0.0f , 0.0f ) );
			model_transform = glm::rotate ( model_transform , transform.m_rotation.y , glm::vec3 ( 0.0f , 1.0f , 0.0f ) );
			model_transform = glm::rotate ( model_transform , transform.m_rotation.z , glm::vec3 ( 0.0f , 0.0f , 1.0f ) );
			model_transform = glm::scale ( model_transform , transform.m_scale );

			auto model_xform_cat = parentTransform * model_transform;

			// populate scene with children
			for ( auto const& child : m_entities[ e ].m_children )
			{
				RecursivePopulateScene<S , T , R> ( scene , child , model_xform_cat );
			}

			// populate scene with prefab children
			/*for ( auto const& child : m_entity_data[ e ].m_prefab_children )
			{
				RecursivePopulateScenePrefab<S , T , R> ( scene , m_prefabs[ child ] , model_xform_cat );
			}*/
		}
		// if neither, take the previous transform in the hierarchy, default identity matrix
		else
		{
			// populate scene with children
			for ( auto const& child : m_entities[ e ].m_children )
			{
				RecursivePopulateScene<S , T , R> ( scene , child , parentTransform );
			}

			// populate scene with prefab children
			/*for ( auto const& child : m_entity_data[ e ].m_prefab_children )
			{
				RecursivePopulateScenePrefab<S , T , R> ( scene , m_prefabs[ child ] , parentTransform );
			}*/
		}
	}

	//template<typename S , typename T , typename R>
	//inline void EnttXSol::RecursivePopulateScenePrefab ( S& scene , Prefab const& prefab , glm::mat4 parentTransform , uint32_t i )
	//{
	//	auto& entity = std::get<1> ( prefab )[ i ];
	//	auto& entt_id = std::get<0> ( entity );
	//	auto& entity_data = std::get<1> ( entity );

	//	// if parent has both transform and renderable component
	//	if ( m_registry.all_of<T , R> ( entt_id ) )
	//	{
	//		auto const& [transform , renderable] = m_registry.get<T , R> ( entt_id );

	//		glm::mat4 model_transform = glm::mat4 ( 1.0f );
	//		model_transform = glm::translate ( model_transform , transform.m_position );
	//		model_transform = glm::rotate ( model_transform , transform.m_rotation.x , glm::vec3 ( 1.0f , 0.0f , 0.0f ) );
	//		model_transform = glm::rotate ( model_transform , transform.m_rotation.y , glm::vec3 ( 0.0f , 1.0f , 0.0f ) );
	//		model_transform = glm::rotate ( model_transform , transform.m_rotation.z , glm::vec3 ( 0.0f , 0.0f , 1.0f ) );
	//		model_transform = glm::scale ( model_transform , transform.m_scale );

	//		auto model_xform_cat = parentTransform * model_transform;

	//		// add to scene
	//		if ( renderable.m_model_id != -1 )
	//		{
	//			auto& object = scene.GetSceneObject ( scene.AddSceneObject ( renderable.m_model_id , model_xform_cat ) );
	//			object.m_diffuse_id = renderable.m_diffuse_id;
	//			object.m_specular_id = renderable.m_specular_id;
	//			object.m_shininess = renderable.m_shininess;
	//		}

	//		// populate scene with children
	//		for ( auto const& child : entity_data.m_children )
	//		{
	//			RecursivePopulateScenePrefab<S , T , R> ( scene , prefab , model_xform_cat , child );
	//		}
	//	}
	//}

	template<typename T>
	inline auto&& EnttXSol::GetStorage ( std::string const& name )
	{
		return m_registry.storage<T> ( entt::hashed_string ( name.c_str () ) );
	}

	//using Entity = EnttXSol::Entity;
	template<typename T>
	inline void EnttXSol::AttachEngineComponentFunctor::operator()( EnttXSol* enttxsol , Entities::ID e )
	{
		enttxsol->AttachComponent<T> ( e );
	}
}