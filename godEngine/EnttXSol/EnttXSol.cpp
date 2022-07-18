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
				GetView ( system.second.m_used_script_components ).each ( m_sol_functions[ system.first ] );
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

	EnttXSol::Entity EnttXSol::CreateEntity ( std::string const& name )
	{
		if ( m_free_ids.empty () )
		{
			m_entities.emplace_back ( m_registry.create () );
			auto id = static_cast< Entity >( m_entities.size () ) - 1;
			if ( !name.empty () )
			{
				if ( m_entity_names.size () <= id )
				{
					m_entity_names.resize ( static_cast< size_t >( id ) + 1 );
				}
				m_entity_names[ id ] = name;
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
				if ( m_entity_names.size () <= id )
				{
					m_entity_names.resize ( static_cast< size_t >( id ) + 1 );
				}
				m_entity_names[ id ] = name;
			}
			return id;
		}
	}

	void EnttXSol::RemoveEntity ( Entity entity )
	{
		assert ( entity < m_entities.size () && m_entities[ entity ].has_value () );
		m_registry.destroy ( GetEntity ( entity ) );
		m_entities[ entity ].reset ();
		m_free_ids.push ( entity );
		if ( entity < m_entity_names.size () )
		{
			m_entity_names[ entity ].clear ();
		}
	}

	void EnttXSol::SerializeScriptComponents ( Entity entity , int imguiUniqueID ,
		void( *Header )( std::string const& name ) ,
		SerializeFunction<bool> SerializeBool ,
		SerializeFunction<int> SerializeInt ,
		SerializeFunction<float> SerializeFloat ,
		SerializeFunction<std::string> SerializeString )
	{
		auto entt_id = GetEntity ( entity );
		for ( auto const& script : m_scripts )
		{
			int i { imguiUniqueID };
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

	//void EnttXSol::AttachScript ( Entity entity , std::string const& script )
	//{
	//	assert ( entity < m_entities.size () && m_entities[ entity ].has_value () );
	//	if ( m_scripts.find ( script ) != m_scripts.end () )
	//	{
	//		// for each system in the script
	//		for ( auto const& system : m_scripts.at ( script ).m_systems )
	//		{
	//			// add all script components used by this system to the entity
	//			for ( auto const& component : system.second.m_used_script_components )
	//			{
	//				AttachComponent ( entity , component );
	//			}
	//			// add all engine components used by this system to the entity
	//			for ( auto const& component : system.second.m_used_engine_components )
	//			{
	//				
	//			}
	//		}
	//	}
	//	else
	//	{
	//		std::cerr << "EnttXSol::AttachScript - Script not found " << script << std::endl;
	//	}
	//}

	std::vector<std::optional<entt::entity>> const& EnttXSol::GetEntities ()const
	{
		return m_entities;
	}

	std::vector<std::string> const& EnttXSol::GetEntityNames ()const
	{
		return m_entity_names;
	}

	std::unordered_map<std::string , EnttXSol::Script> const& EnttXSol::GetScripts () const
	{
		return m_scripts;
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

	entt::runtime_view EnttXSol::GetView ( std::vector<std::string> const& components )
	{
		entt::runtime_view view {};
		for ( auto component : components )
		{
			view.iterate ( GetStorage<sol::table> ( component ) );
		}
		return view;
	}
}