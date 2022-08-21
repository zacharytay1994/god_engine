#include "../pch.h"
#include "EnttXSol.h"

namespace god
{
	EnttXSol::EnttXSol ( std::vector<std::string> scriptFiles )
	{
		m_lua.open_libraries ( sol::lib::base );
		for ( auto file : scriptFiles )
		{
			LoadScript ( file );
		}

		m_lua.set ( "sol_table" , sol::table () );
		m_lua[ m_identifier_GetScriptComponent ] = [this]( entt::entity e , std::string const& s )->sol::table
		{
			return GetStorage<sol::table> ( s ).get ( e );
		};
	}

	void EnttXSol::Update ()
	{
		// update engine systems
		if ( m_engine_update )
		{
			m_engine_update ( *this );
		}
		// update script systems
		// for each script in loaded scripts
		for ( auto const& script : m_scripts )
		{
			// run system with views created by their used components,
			// i.e. process all entities
			for ( auto const& system : script.second.m_systems )
			{
				GetView ( system.second.m_used_script_components , system.second.m_used_engine_components ).each ( m_sol_functions[ system.first ] );
			}
		}
	}

	void EnttXSol::BindEngineSystemUpdate ( void( *update )( EnttXSol& ) )
	{
		m_engine_update = update;
	}

	entt::entity EnttXSol::operator[]( Entity entity )
	{
		assert ( entity < m_entities.size () && m_entities[ entity ].has_value () );
		return m_entities[ entity ].value ();
	}

	entt::entity EnttXSol::GetEntity ( Entity entity )
	{
		assert ( entity < m_entities.size () && m_entities[ entity ].has_value () );
		return m_entities[ entity ].value ();
	}

	EnttXSol::Entity EnttXSol::CreateEntity ( std::string const& name , Entity parent )
	{
		// check if parent exists and is a valid entity
		if ( parent != NullEntity )
		{
			assert ( parent < m_entities.size () && m_entities[ parent ].has_value () );
		}
		if ( m_free_ids.empty () )
		{
			m_entities.emplace_back ( m_registry.create () );
			auto id = static_cast< Entity >( m_entities.size () ) - 1;
			if ( !name.empty () )
			{
				if ( m_entity_data.size () <= id )
				{
					m_entity_data.resize ( static_cast< size_t >( id ) + 1 );
				}
				m_entity_data[ id ] = { name , parent };
			}
			// assign parent newly created child if there is parent
			if ( parent != NullEntity )
			{
				m_entity_data[ parent ].m_children.push_back ( id );
			}
			return id;
		}
		else
		{
			auto id = m_free_ids.top ();
			m_free_ids.pop ();
			m_entities[ id ] = m_registry.create ();
			if ( !name.empty () )
			{
				if ( m_entity_data.size () <= id )
				{
					m_entity_data.resize ( static_cast< size_t >( id ) + 1 );
				}
				m_entity_data[ id ] = { name , parent };
			}
			// assign parent newly created child
			if ( parent != NullEntity )
			{
				m_entity_data[ parent ].m_children.push_back ( id );
			}
			return id;
		}
	}

	Entity EnttXSol::LoadEntity ( std::string const& name , Entity parent )
	{
		m_entities.emplace_back ( m_registry.create () );
		auto id = static_cast< Entity >( m_entities.size () ) - 1;

		if ( id >= m_entity_data.size () )
		{
			m_entity_data.resize ( id + 1 );
		}
		if ( parent != NullEntity )
		{
			if ( parent >= m_entity_data.size () )
			{
				m_entity_data.resize ( parent + 1 );
			}
			m_entity_data[ parent ].m_children.push_back ( id );
		}

		m_entity_data[ id ].m_name = name;
		m_entity_data[ id ].m_parent = parent;

		return id;
	}

	void EnttXSol::RemoveEntity ( Entity entity , uint32_t childIndex )
	{
		assert ( entity < m_entities.size () && m_entities[ entity ].has_value () );
		m_registry.destroy ( GetEntity ( entity ) );
		m_entities[ entity ].reset ();
		m_free_ids.push ( entity );

		// remove all children
		auto& entity_data = m_entity_data[ entity ];
		for ( auto const& child : entity_data.m_children )
		{
			RemoveEntity ( child );
		}
		// remove as child from parent if exists
		if ( entity_data.m_parent != NullEntity && childIndex != NullEntity )
		{
			m_entity_data[ entity_data.m_parent ].m_children.erase ( m_entity_data[ entity_data.m_parent ].m_children.begin () + childIndex );
		}
		// remove entity
		m_entity_data[ entity ] = EntityData ();
	}

	void EnttXSol::SerializeScriptComponents ( Entity entity , int imguiUniqueID ,
		void( *Header )( std::string const& name ) ,
		SerializeFunction<bool> SerializeBool ,
		SerializeFunction<int> SerializeInt ,
		SerializeFunction<float> SerializeFloat ,
		SerializeFunction<std::string> SerializeString )
	{
		auto entt_id = GetEntity ( entity );
		int i { imguiUniqueID };
		for ( auto const& script : m_scripts )
		{
			for ( auto const& component : script.second.m_components )
			{
				auto& storage = m_registry.storage<sol::table> ( entt::hashed_string ( component.first.c_str () ) );
				if ( storage.contains ( entt_id ) )
				{
					if ( !component.second.m_serialize_attributes.empty () )
					{
						Header ( component.first );
					}
					auto& table = storage.get ( entt_id );
					for ( auto const& attribute : component.second.m_serialize_attributes )
					{
						auto name = std::get<0> ( attribute );
						auto type = std::get<1> ( attribute );
						switch ( type )
						{
						case AttributeTypes::BOOL:
						{
							bool val = table.get<bool> ( name );
							SerializeBool ( val , i , name );
							table.set ( name , val );
							break;
						}
						case AttributeTypes::INT:
						{
							int val = table.get<int> ( name );
							SerializeInt ( val , i , name );
							table.set ( name , val );
							break;
						}
						case AttributeTypes::FLOAT:
						{
							float val = table.get<float> ( name );
							SerializeFloat ( val , i , name );
							table.set ( name , val );
							break;
						}
						case AttributeTypes::STRING:
						{
							std::string val = table.get<std::string> ( name );
							SerializeString ( val , i , name );
							table.set ( name , val );
							break;
						}
						}
						++i;
					}
				}
			}
		}
	}

	void EnttXSol::AttachEngineComponent ( Entity entity , uint32_t componentID )
	{
		T_Manip::RunOnType ( EngineComponents::Components () , componentID , AttachEngineComponentFunctor () , this , entity );
	}

	std::vector<std::optional<entt::entity>> const& EnttXSol::GetEntities ()const
	{
		return m_entities;
	}

	std::vector<EnttXSol::EntityData> const& EnttXSol::GetEntityData ()const
	{
		return m_entity_data;
	}

	std::unordered_map<std::string , EnttXSol::Script> const& EnttXSol::GetScripts () const
	{
		return m_scripts;
	}

	void EnttXSol::SerializeState ( EngineResources& engineResources , std::string const& filePath )
	{
		rapidjson::Document document;
		document.SetObject ();

		// serialize all root prefabs
		rapidjson::Value state_prefabs;
		state_prefabs.SetObject ();
		for ( auto const& prefab : m_prefabs )
		{
			if ( std::get<1> ( std::get<1> ( prefab )[ 0 ] ).m_parent == NullEntity )
			{
				RapidJSON::JSONifyToValue ( state_prefabs , document , std::get<0> ( prefab ) , "test" );
			}
		}
		RapidJSON::JSONify ( document , "State Prefabs" , state_prefabs );

		// loop through all entities
		for ( auto i = 0; i < m_entities.size (); ++i )
		{
			if ( m_entities[ i ].has_value () )
			{
				rapidjson::Value entity_value;
				entity_value.SetObject ();

				// serialize prefabs attached to this entity
				rapidjson::Value prefabs;
				prefabs.SetObject ();
				for ( auto const& prefab_child : m_entity_data[ i ].m_prefab_children )
				{
					RapidJSON::JSONifyToValue ( prefabs , document , std::get<0> ( m_prefabs[ prefab_child ] ) , "test" );
				}
				RapidJSON::JSONifyToValue ( entity_value , document , "Prefabs" , prefabs );

				// loop through all possible script components seeing if its in this entity
				rapidjson::Value script_components;
				script_components.SetObject ();
				for ( auto const& script : m_scripts )
				{
					for ( auto const& component : script.second.m_components )
					{
						// check if the entity has this component
						auto&& storage = GetStorage<sol::table> ( component.first );
						if ( storage.contains ( m_entities[ i ].value () ) )
						{
							rapidjson::Value script_component;
							script_component.SetObject ();

							// serialize component attributes
							auto& table = storage.get ( m_entities[ i ].value () );
							for ( auto const& attribute : component.second.m_serialize_attributes )
							{
								auto name = std::get<0> ( attribute );
								auto type = std::get<1> ( attribute );

								switch ( type )
								{
								case ( AttributeTypes::BOOL ):
									RapidJSON::JSONifyToValue ( script_component , document , name , table.get<bool> ( name ) );
									break;
								case ( AttributeTypes::INT ):
									RapidJSON::JSONifyToValue ( script_component , document , name , table.get<int> ( name ) );
									break;
								case ( AttributeTypes::FLOAT ):
									RapidJSON::JSONifyToValue ( script_component , document , name , table.get<float> ( name ) );
									break;
								case ( AttributeTypes::STRING ):
									RapidJSON::JSONifyToValue ( script_component , document , name , table.get<std::string> ( name ) );
									break;
								}
							}

							// component.first is now the name of the component in the script
							RapidJSON::JSONifyToValue ( script_components , document , component.first , script_component );
						}
					}
				}
				RapidJSON::JSONifyToValue ( entity_value , document , "Script Components" , script_components );

				// loop through all possible engine components seeing if its in this entity
				rapidjson::Value engine_components;
				engine_components.SetObject ();
				for ( auto j = 0; j < EngineComponents::m_component_names.size (); ++j )
				{
					T_Manip::RunOnType ( EngineComponents::Components () , j ,
						SerializeEngineComponent<EngineResources> () , std::ref ( m_registry ) , std::ref ( document ) , std::ref ( engine_components ) ,
						m_entities[ i ].value () , EngineComponents::m_component_names[ j ] , engineResources );
				}
				RapidJSON::JSONifyToValue ( entity_value , document , "Engine Components" , engine_components );

				// count how many empty spaces before parent
				int parent_id = -1;
				if ( m_entity_data[ i ].m_parent != NullEntity )
				{
					parent_id = m_entity_data[ i ].m_parent - std::count_if ( m_entities.begin () , m_entities.begin () + m_entity_data[ i ].m_parent ,
						[]( auto val )
						{
							return !val.has_value ();
						}
					);
				}
				RapidJSON::JSONifyToValue ( entity_value , document , "Parent" , parent_id );

				RapidJSON::JSONify ( document , m_entity_data[ i ].m_name , entity_value );
			}
		}

		WriteJSON ( document , filePath );
	}

	void EnttXSol::DeserializeState ( EngineResources& engineResources , std::string const& filePath )
	{
		rapidjson::Document document;

		ReadJSON ( document , filePath );

		for ( auto& member : document.GetObj () )
		{
			if ( std::string ( member.name.GetString () ) == "State Prefabs" )
			{
				for ( auto& prefab : member.value.GetObj () )
				{
					LoadPrefab ( engineResources , prefab.name.GetString () );
				}
			}
			else
			{
				auto e = LoadEntity ( member.name.GetString () , member.value[ "Parent" ].GetInt () );

				// iterate prefabs
				for ( auto& prefabs : member.value[ "Prefabs" ].GetObj () )
				{
					LoadPrefab ( engineResources , prefabs.name.GetString () , e );
				}

				// iterate script components
				for ( auto& script_component : member.value[ "Script Components" ].GetObj () )
				{
					std::string component_name = script_component.name.GetString ();
					AttachComponent ( e , component_name );

					// update values
					auto&& storage = GetStorage<sol::table> ( component_name );
					auto& component = storage.get ( GetEntity ( e ) );

					bool script_component_found { false };
					for ( auto const& script : m_scripts )
					{
						if ( script.second.m_components.find ( component_name ) != script.second.m_components.end () )
						{
							// attribute here refers to the component's attribute description
							for ( auto const& attribute : script.second.m_components.at ( component_name ).m_serialize_attributes )
							{
								auto name = std::get<0> ( attribute );
								auto type = std::get<1> ( attribute );

								if ( script_component.value.FindMember ( name.c_str () ) != script_component.value.MemberEnd () )
								{
									switch ( type )
									{
									case ( AttributeTypes::BOOL ):
										component.set ( name , script_component.value[ name.c_str () ].GetBool () );
										break;
									case ( AttributeTypes::INT ):
										component.set ( name , script_component.value[ name.c_str () ].GetInt () );
										break;
									case ( AttributeTypes::FLOAT ):
										component.set ( name , script_component.value[ name.c_str () ].GetFloat () );
										break;
									case ( AttributeTypes::STRING ):
										component.set ( name , script_component.value[ name.c_str () ].GetString () );
										break;
									}
								}
								else
								{
									std::cerr << "EnttXSol::DeserializeState - Expected value not in serialized file. Maybe outdated file? ["
										<< name << "] in File [" << filePath << "]" << std::endl;
								}
							}
							script_component_found = true;
						}
					}
					if ( !script_component_found )
					{
						std::cerr << "EnttXSol::DeserializeState - Previously serialized script component not found. Maybe redefined? ["
							<< component_name << "] in File [" << filePath << "]" << std::endl;
					}
				}

				// iterate engine components
				for ( auto& engine_component : member.value[ "Engine Components" ].GetObj () )
				{
					auto it = std::find ( EngineComponents::m_component_names.begin () , EngineComponents::m_component_names.end () , engine_component.name.GetString () );
					if ( it != EngineComponents::m_component_names.end () )
					{
						auto i = it - EngineComponents::m_component_names.begin ();
						T_Manip::RunOnType ( EngineComponents::Components () , i ,
							DeserializeEngineComponent<EngineResources> () , std::ref ( m_registry ) , m_entities[ e ].value () , std::ref ( engine_component.value ) , engineResources );
					}
				}
			}
		}
	}

	void EnttXSol::SerializeEntity ( EngineResources& engineResources , rapidjson::Document& document , Entity entity , int parent , int& count )
	{
		if ( m_entities[ entity ].has_value () )
		{
			rapidjson::Value entity_value;
			entity_value.SetObject ();

			// loop through all possible script components seeing if its in this entity
			rapidjson::Value script_components;
			script_components.SetObject ();
			for ( auto const& script : m_scripts )
			{
				for ( auto const& component : script.second.m_components )
				{
					// check if the entity has this component
					auto&& storage = GetStorage<sol::table> ( component.first );
					if ( storage.contains ( m_entities[ entity ].value () ) )
					{
						rapidjson::Value script_component;
						script_component.SetObject ();

						// serialize component attributes
						auto& table = storage.get ( m_entities[ entity ].value () );
						for ( auto const& attribute : component.second.m_serialize_attributes )
						{
							auto name = std::get<0> ( attribute );
							auto type = std::get<1> ( attribute );

							switch ( type )
							{
							case ( AttributeTypes::BOOL ):
								RapidJSON::JSONifyToValue ( script_component , document , name , table.get<bool> ( name ) );
								break;
							case ( AttributeTypes::INT ):
								RapidJSON::JSONifyToValue ( script_component , document , name , table.get<int> ( name ) );
								break;
							case ( AttributeTypes::FLOAT ):
								RapidJSON::JSONifyToValue ( script_component , document , name , table.get<float> ( name ) );
								break;
							case ( AttributeTypes::STRING ):
								RapidJSON::JSONifyToValue ( script_component , document , name , table.get<std::string> ( name ) );
								break;
							}
						}

						// component.first is now the name of the component in the script
						RapidJSON::JSONifyToValue ( script_components , document , component.first , script_component );
					}
				}
			}
			RapidJSON::JSONifyToValue ( entity_value , document , "Script Components" , script_components );

			// loop through all possible engine components seeing if its in this entity
			rapidjson::Value engine_components;
			engine_components.SetObject ();
			for ( auto j = 0; j < EngineComponents::m_component_names.size (); ++j )
			{
				T_Manip::RunOnType ( EngineComponents::Components () , j ,
					SerializeEngineComponent<EngineResources> () , std::ref ( m_registry ) , std::ref ( document ) , std::ref ( engine_components ) ,
					m_entities[ entity ].value () , EngineComponents::m_component_names[ j ] , engineResources );
			}
			RapidJSON::JSONifyToValue ( entity_value , document , "Engine Components" , engine_components );

			RapidJSON::JSONifyToValue ( entity_value , document , "Parent" , parent );

			RapidJSON::JSONify ( document , m_entity_data[ entity ].m_name , entity_value );

			// serialize children too
			parent = count + 1;
			for ( auto const& child : m_entity_data[ entity ].m_children )
			{
				SerializeEntity ( engineResources , document , child , parent , ++count );
			}
		}
	}

	void EnttXSol::SavePrefab ( EngineResources& engineResources , Entity root , std::string const& filePath )
	{
		rapidjson::Document document;
		document.SetObject ();

		int count { -1 };
		SerializeEntity ( engineResources , document , root , NullEntity , count );

		WriteJSON ( document , filePath );
	}

	void EnttXSol::LoadPrefab ( EngineResources& engineResources , std::string const& fileName , Entity parent )
	{
		rapidjson::Document document;

		ReadJSON ( document , std::string ( "Assets/GameAssets/Prefabs/" ) + fileName + ".json" );

		Prefab prefab;
		std::get<0> ( prefab ) = fileName;
		auto& prefab_data = std::get<1> ( prefab );

		for ( auto& member : document.GetObj () )
		{
			//auto e = LoadEntity ( member.name.GetString () , member.value[ "Parent" ].GetInt () );
			entt::entity entity = m_registry.create ();
			EntityData entity_data;
			entity_data.m_name = member.name.GetString ();
			entity_data.m_parent = static_cast< Entity >( member.value[ "Parent" ].GetInt () );

			if ( entity_data.m_parent != NullEntity )
			{
				std::get<1> ( prefab_data[ entity_data.m_parent ] ).m_children.push_back ( prefab_data.size () );
			}

			// iterate script components
			for ( auto& script_component : member.value[ "Script Components" ].GetObj () )
			{
				std::string component_name = script_component.name.GetString ();
				AttachComponent ( entity , component_name );

				// update values
				auto&& storage = GetStorage<sol::table> ( component_name );
				auto& component = storage.get ( entity );

				bool script_component_found { false };
				for ( auto const& script : m_scripts )
				{
					if ( script.second.m_components.find ( component_name ) != script.second.m_components.end () )
					{
						// attribute here refers to the component's attribute description
						for ( auto const& attribute : script.second.m_components.at ( component_name ).m_serialize_attributes )
						{
							auto name = std::get<0> ( attribute );
							auto type = std::get<1> ( attribute );

							if ( script_component.value.FindMember ( name.c_str () ) != script_component.value.MemberEnd () )
							{
								switch ( type )
								{
								case ( AttributeTypes::BOOL ):
									component.set ( name , script_component.value[ name.c_str () ].GetBool () );
									break;
								case ( AttributeTypes::INT ):
									component.set ( name , script_component.value[ name.c_str () ].GetInt () );
									break;
								case ( AttributeTypes::FLOAT ):
									component.set ( name , script_component.value[ name.c_str () ].GetFloat () );
									break;
								case ( AttributeTypes::STRING ):
									component.set ( name , script_component.value[ name.c_str () ].GetString () );
									break;
								}
							}
							else
							{
								std::cerr << "EnttXSol::DeserializeState - Expected value not in serialized file. Maybe outdated file? ["
									<< name << "] in prefab file [" << fileName << "]" << std::endl;
							}
						}
						script_component_found = true;
					}
				}
				if ( !script_component_found )
				{
					std::cerr << "EnttXSol::DeserializeState - Previously serialized script component not found. Maybe redefined? ["
						<< component_name << "] in prefab file [" << fileName << "]" << std::endl;
				}
			}

			// iterate engine components
			for ( auto& engine_component : member.value[ "Engine Components" ].GetObj () )
			{
				auto it = std::find ( EngineComponents::m_component_names.begin () , EngineComponents::m_component_names.end () , engine_component.name.GetString () );
				if ( it != EngineComponents::m_component_names.end () )
				{
					auto i = it - EngineComponents::m_component_names.begin ();
					T_Manip::RunOnType ( EngineComponents::Components () , i ,
						DeserializeEngineComponent<EngineResources> () , std::ref ( m_registry ) , entity , std::ref ( engine_component.value ) , engineResources );
				}
			}

			// add to prefab container
			prefab_data.push_back ( { entity, entity_data } );
		}

		if ( prefab_data.size () > 0 )
		{
			if ( parent != NullEntity )
			{
				std::get<1> ( prefab_data[ 0 ] ).m_parent = parent;
				m_entity_data[ parent ].m_prefab_children.push_back ( m_prefabs.size () );
			}

			m_prefabs.push_back ( prefab );
		}
	}

	void EnttXSol::RemovePrefab ( uint32_t id )
	{
		
	}

	std::vector<EnttXSol::Prefab> const& EnttXSol::GetPrefabs ()
	{
		return m_prefabs;
	}

	void EnttXSol::LoadScript ( std::string const& scriptFile )
	{
		m_lua.script_file ( scriptFile );

		auto last_slash = scriptFile.find_last_of ( '/' ) + 1;
		auto last_dot = scriptFile.find_last_of ( '.' );
		auto script_name = scriptFile.substr ( last_slash , last_dot - last_slash );
		m_scripts.insert ( { script_name, {} } );
		auto& script = m_scripts.at ( script_name );

		// process script
		// read all components
		std::ifstream file { scriptFile };
		if ( file )
		{
			std::string line;
			while ( std::getline ( file , line ) )
			{
				// next line contains a component
				if ( line.find ( m_identifier_component ) != std::string::npos )
				{
					std::getline ( file , line );
					std::string component_name;
					std::stringstream ss;
					ss.str ( line );
					ss >> component_name >> component_name;
					auto component = script.m_components.insert ( { component_name.substr ( 0 , component_name.find_first_of ( '(' ) ), {} } );

					// loop till end of component
					while ( line.find ( "end" ) != 0 )
					{
						// check if any components need serializing/gui
						// NOTE: Assumes next line should be the attribute to serialize
						// serialize bool
						if ( line.find ( m_identifier_serialize_input_bool ) != std::string::npos )
						{
							std::getline ( file , line );
							auto attribute_name = line.substr ( 0 , line.find_first_of ( '=' ) );
							attribute_name.erase ( std::remove ( attribute_name.begin () , attribute_name.end () , ' ' ) , attribute_name.end () );
							component.first->second.m_serialize_attributes.push_back ( { attribute_name, AttributeTypes::BOOL } );
						}
						// serialize int
						else if ( line.find ( m_identifier_serialize_input_int ) != std::string::npos )
						{
							std::getline ( file , line );
							auto attribute_name = line.substr ( 0 , line.find_first_of ( '=' ) );
							attribute_name.erase ( std::remove ( attribute_name.begin () , attribute_name.end () , ' ' ) , attribute_name.end () );
							component.first->second.m_serialize_attributes.push_back ( { attribute_name, AttributeTypes::INT } );
						}
						// serialize float
						else if ( line.find ( m_identifier_serialize_input_float ) != std::string::npos )
						{
							std::getline ( file , line );
							auto attribute_name = line.substr ( 0 , line.find_first_of ( '=' ) );
							attribute_name.erase ( std::remove ( attribute_name.begin () , attribute_name.end () , ' ' ) , attribute_name.end () );
							component.first->second.m_serialize_attributes.push_back ( { attribute_name, AttributeTypes::FLOAT } );
						}
						// serialize string
						else if ( line.find ( m_identifier_serialize_input_string ) != std::string::npos )
						{
							std::getline ( file , line );
							auto attribute_name = line.substr ( 0 , line.find_first_of ( '=' ) );
							attribute_name.erase ( std::remove ( attribute_name.begin () , attribute_name.end () , ' ' ) , attribute_name.end () );
							component.first->second.m_serialize_attributes.push_back ( { attribute_name, AttributeTypes::STRING } );
						}
						std::getline ( file , line );
					}
				}
				// if next line contains a system
				if ( line.find ( m_identifier_system ) != std::string::npos )
				{
					std::getline ( file , line );
					std::string system_name;
					std::stringstream ss;
					ss.str ( line );
					ss >> system_name >> system_name;
					system_name = system_name.substr ( 0 , system_name.find_first_of ( '(' ) );
					script.m_systems.insert ( { system_name, {} } );
					auto& system = script.m_systems.at ( system_name );

					std::string component_name;
					while ( line.find ( "end" ) != 0 )
					{
						// do stuff with function lines that are not commented out
						if ( line.find ( "--" ) == std::string::npos )
						{
							// find GetScriptComponent lines
							if ( line.find ( m_identifier_GetScriptComponent ) != std::string::npos )
							{
								auto first = line.find_first_of ( '\"' ) + 1;
								component_name = line.substr ( first , line.find_last_of ( '\"' ) - first );
								if ( std::find ( system.m_used_script_components.begin () , system.m_used_script_components.end () , component_name ) == system.m_used_script_components.end () )
								{
									system.m_used_script_components.push_back ( component_name );
								}
							}
							// find Get... engine component lines
							// identified with a "Get" and no "", more robust test might be neededs
							std::string get_key { "Get" };
							if ( line.find ( get_key ) != std::string::npos &&
								line.find ( "\"" ) == std::string::npos )
							{
								auto get = line.find ( get_key );
								auto engine_component_name = line.substr ( get , line.size () - get );
								engine_component_name = engine_component_name.substr ( get_key.length () , engine_component_name.find_first_of ( "(" ) - get_key.length () );
								if ( std::find ( system.m_used_engine_components.begin () , system.m_used_engine_components.end () , engine_component_name ) == system.m_used_engine_components.end () )
								{
									system.m_used_engine_components.push_back ( engine_component_name );
								}
							}
						}

						// get next line
						std::getline ( file , line );
					}
				}
			}
		}
		else
		{
			std::cerr << "EnttXSol::AddScript - Failed to read script." << std::endl;
		}

		// check if there is a matching component made for this system in the script
		for ( auto& system : script.m_systems )
		{
			if ( auto dash_position = system.first.find_first_of ( '_' ) )
			{
				auto const name_to_match = system.first.substr ( dash_position , system.first.length () - dash_position );
				for ( auto const& component : script.m_components )
				{
					if ( component.first.find ( name_to_match ) != std::string::npos &&
						std::find ( system.second.m_used_script_components.begin () , system.second.m_used_script_components.end () , component.first ) == system.second.m_used_script_components.end () )
					{
						// add matching component as system required component
						system.second.m_used_script_components.push_back ( component.first );
					}
				}
			}
		}

		// load all systems found in script as a sol::function
		for ( auto const& system : script.m_systems )
		{
			m_sol_functions.insert ( { system.first, m_lua[ system.first ] } );
		}
	}

	void EnttXSol::LoadSystem ( std::string const& name )
	{
		m_sol_functions.insert ( { name, m_lua[ name ] } );
	}

	void EnttXSol::AttachComponent ( Entity id , std::string const& name )
	{
		sol::function component = m_lua[ name ] ();
		auto& storage = GetStorage<sol::table> ( name );
		if ( !storage.contains ( this->operator[]( id ) ) )
		{
			storage.emplace ( this->operator[]( id ) ) = component ();
		}
	}

	void EnttXSol::AttachComponent ( entt::entity id , std::string const& name )
	{
		sol::function component = m_lua[ name ] ();
		auto& storage = GetStorage<sol::table> ( name );
		if ( !storage.contains ( id ) )
		{
			storage.emplace ( id ) = component ();
		}
	}

	entt::runtime_view EnttXSol::GetView ( std::vector<std::string> const& scriptComponents , std::vector<std::string> const& engineComponents )
	{
		entt::runtime_view view {};
		for ( auto const& component : scriptComponents )
		{
			view.iterate ( GetStorage<sol::table> ( component ) );
		}
		for ( auto const& component : engineComponents )
		{
			auto it = std::find ( EngineComponents::m_component_names.begin () , EngineComponents::m_component_names.end () , component );
			if ( it != EngineComponents::m_component_names.end () )
			{
				auto i = it - EngineComponents::m_component_names.begin ();
				T_Manip::RunOnType ( EngineComponents::Components () , i ,
					AppendEngineComponentToView () , std::ref ( m_registry ) , std::ref ( view ) );
			}
		}
		return view;
	}
}