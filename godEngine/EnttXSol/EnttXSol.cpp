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

	void EnttXSol::Clear ()
	{
		m_registry.clear ();
		m_entities.Clear ();
	}

	void EnttXSol::SetupBindings ()
	{
		RegisterLuaType<glm::vec3> ( "vec3" ,
			"x" , &glm::vec3::x ,
			"y" , &glm::vec3::y ,
			"z" , &glm::vec3::z );
	}

	void EnttXSol::BindEngineSystemUpdate ( void( *update )( EnttXSol& ) )
	{
		m_engine_update = update;
	}

	EnttXSol::Entities::ID EnttXSol::CreateEntity ( std::string const& name , Entities::ID parent )
	{
		auto new_entity = m_entities.Push ( { m_registry, name, parent } );
		if ( parent != Entities::Null )
		{
			m_entities[ parent ].m_children.push_back ( new_entity );
		}
		return new_entity;
	}

	void EnttXSol::RemoveEntity ( Entities::ID entity )
	{
		// detach parent if any from this entity
		auto parent = m_entities[ entity ].m_parent_id;
		if ( parent != Entities::Null )
		{
			auto it = std::find ( m_entities[ parent ].m_children.begin () , m_entities[ parent ].m_children.end () , entity );
			m_entities[ parent ].m_children.erase ( it );
		}
		// remove all branches
		RecursiveRemoveEntity ( entity );
	}

	void EnttXSol::SerializeScriptComponents ( Entities::ID entity , int imguiUniqueID ,
		void( *Header )( std::string const& name ) ,
		SerializeFunction<bool> SerializeBool ,
		SerializeFunction<int> SerializeInt ,
		SerializeFunction<float> SerializeFloat ,
		SerializeFunction<std::string> SerializeString )
	{
		auto entt_id = m_entities[ entity ].m_id;
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

	void EnttXSol::AttachEngineComponent ( Entities::ID entity , uint32_t componentID )
	{
		T_Manip::RunOnType ( EngineComponents::Components () , componentID , AttachEngineComponentFunctor () , this , entity );
	}

	std::unordered_map<std::string , EnttXSol::Script> const& EnttXSol::GetScripts () const
	{
		return m_scripts;
	}

	void EnttXSol::SerializeStateV2 ( EngineResources& engineResources , std::string const& fileName )
	{
		rapidjson::Document document { rapidjson::kObjectType };

		// serialize only root nodes
		for ( uint32_t entity = 0; entity < m_entities.Size (); ++entity )
		{
			if ( m_entities.Valid ( entity ) && m_entities[ entity ].m_parent_id == Entities::Null )
			{
				rapidjson::Value value { rapidjson::kObjectType };
				SerializeStateV2Recurse ( engineResources , entity , document , value );
				RapidJSON::JSONify ( document , m_entities[ entity ].m_name , value );
			}
		}

		WriteJSON ( document , std::string ( "Assets/GameAssets/Scenes/" ) + fileName + ".json" );
	}

	void EnttXSol::SerializeStateV2Recurse ( EngineResources& engineResources , Entities::ID entity , rapidjson::Document& document , rapidjson::Value& value )
	{
		if ( m_entities[ entity ].m_type == Entity_::Type::Default )
		{
			RapidJSON::JSONifyToValue ( value , document , "Type" , "Default" );

			// loop through all possible script components seeing if its in this entity
			rapidjson::Value script_components;
			script_components.SetObject ();
			for ( auto const& script : m_scripts )
			{
				for ( auto const& component : script.second.m_components )
				{
					// check if the entity has this component
					auto&& storage = GetStorage<sol::table> ( component.first );
					if ( storage.contains ( m_entities[ entity ].m_id ) )
					{
						rapidjson::Value script_component;
						script_component.SetObject ();

						// serialize component attributes
						auto& table = storage.get ( m_entities[ entity ].m_id );
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
			RapidJSON::JSONifyToValue ( value , document , "Script Components" , script_components );

			// loop through all possible engine components seeing if its in this entity
			rapidjson::Value engine_components;
			engine_components.SetObject ();
			for ( auto j = 0; j < EngineComponents::m_component_names.size (); ++j )
			{
				T_Manip::RunOnType ( EngineComponents::Components () , j ,
					SerializeEngineComponent<EngineResources> () , std::ref ( m_registry ) , std::ref ( document ) , std::ref ( engine_components ) ,
					m_entities[ entity ].m_id , EngineComponents::m_component_names[ j ] , engineResources );
			}
			RapidJSON::JSONifyToValue ( value , document , "Engine Components" , engine_components );

			// Serialize Children
			rapidjson::Value children_value { rapidjson::kObjectType };
			for ( auto const& child : m_entities[ entity ].m_children )
			{
				rapidjson::Value child_value { rapidjson::kObjectType };
				SerializeStateV2Recurse ( engineResources , child , document , child_value );

				RapidJSON::JSONifyToValue ( children_value , document , m_entities[ child ].m_name , child_value );
			}

			RapidJSON::JSONifyToValue ( value , document , "Children" , children_value );
		}
		// or if it is a prefab, serialize it without processing its children
		// i.e. children of prefabs will not be serialize, as they fall under the prefab jurisdiction,
		// alter the prefab instead if wish to add children to it.
		else if ( m_entities[ entity ].m_type == Entity_::Type::Prefab )
		{
			RapidJSON::JSONifyToValue ( value , document , "Type" , "Prefab" );
		}
	}

	void EnttXSol::DeserializeStateV2 ( EngineResources& engineResources , std::string const& fileName )
	{
		rapidjson::Document document { rapidjson::kObjectType };

		ReadJSON ( document , std::string ( "Assets/GameAssets/Scenes/" ) + fileName + ".json" );

		for ( auto& member : document.GetObj () )
		{
			DeserializeStateV2Recurse ( engineResources , member.value , member.name.GetString () , Entities::Null );
		}
	}

	void EnttXSol::DeserializeStateV2Recurse ( EngineResources& engineResources , rapidjson::Value& value , std::string const& name , Entities::ID parent )
	{
		if ( std::string ( value[ "Type" ].GetString () ) == "Prefab" )
		{
			LoadPrefabV2 ( engineResources , name , parent );
		}
		else if ( std::string ( value[ "Type" ].GetString () ) == "Default" )
		{
			auto entity = CreateEntity ( name , parent );

			// iterate script components
			for ( auto& script_component : value[ "Script Components" ].GetObj () )
			{
				std::string component_name = script_component.name.GetString ();
				AttachComponent ( entity , component_name );

				// update values
				auto&& storage = GetStorage<sol::table> ( component_name );
				auto& component = storage.get ( m_entities[ entity ].m_id );

				bool script_component_found { false };
				for ( auto const& script : m_scripts )
				{
					if ( script.second.m_components.find ( component_name ) != script.second.m_components.end () )
					{
						// attribute here refers to the component's attribute description
						for ( auto const& attribute : script.second.m_components.at ( component_name ).m_serialize_attributes )
						{
							auto attribute_name = std::get<0> ( attribute );
							auto attribute_type = std::get<1> ( attribute );

							if ( script_component.value.FindMember ( attribute_name.c_str () ) != script_component.value.MemberEnd () )
							{
								switch ( attribute_type )
								{
								case ( AttributeTypes::BOOL ):
									component.set ( attribute_name , script_component.value[ attribute_name.c_str () ].GetBool () );
									break;
								case ( AttributeTypes::INT ):
									component.set ( attribute_name , script_component.value[ attribute_name.c_str () ].GetInt () );
									break;
								case ( AttributeTypes::FLOAT ):
									component.set ( attribute_name , script_component.value[ attribute_name.c_str () ].GetFloat () );
									break;
								case ( AttributeTypes::STRING ):
									component.set ( attribute_name , script_component.value[ attribute_name.c_str () ].GetString () );
									break;
								}
							}
							else
							{
								std::cerr << "EnttXSol::DeserializeState - Expected value not in serialized file. Maybe outdated file? ["
									<< attribute_name << "] in File [" << "oops" << "]" << std::endl;
							}
						}
						script_component_found = true;
					}
				}
				if ( !script_component_found )
				{
					std::cerr << "EnttXSol::DeserializeState - Previously serialized script component not found. Maybe redefined? ["
						<< component_name << "] in File [" << "oops" << "]" << std::endl;
				}
			}

			// iterate engine components
			for ( auto& engine_component : value[ "Engine Components" ].GetObj () )
			{
				auto it = std::find ( EngineComponents::m_component_names.begin () , EngineComponents::m_component_names.end () , engine_component.name.GetString () );
				if ( it != EngineComponents::m_component_names.end () )
				{
					auto i = it - EngineComponents::m_component_names.begin ();
					T_Manip::RunOnType ( EngineComponents::Components () , i ,
						DeserializeEngineComponent<EngineResources> () , std::ref ( m_registry ) , m_entities[ entity ].m_id , std::ref ( engine_component.value ) , engineResources );
				}
			}

			/*
				DESERIALIZE CHILDREN
			*/
			for ( auto& child : value[ "Children" ].GetObj () )
			{
				DeserializeStateV2Recurse ( engineResources , child.value , child.name.GetString () , entity );
			}
		}
	}

	void EnttXSol::SavePrefabV2 ( EngineResources& engineResources , Entities::ID root , std::string const& fileName )
	{
		rapidjson::Document document { rapidjson::kObjectType };
		rapidjson::Value value { rapidjson::kObjectType };

		SavePrefabV2Recurse ( engineResources , root , document , value , true );

		RapidJSON::JSONify ( document , fileName , value );

		WriteJSON ( document , std::string ( "Assets/GameAssets/Prefabs/" ) + fileName + ".json" );
	}

	void EnttXSol::SavePrefabV2Recurse ( EngineResources& engineResources , Entities::ID entity , rapidjson::Document& document , rapidjson::Value& value , bool root )
	{
		if ( m_entities.Valid ( entity ) )
		{
			if ( m_entities[ entity ].m_type == Entity_::Type::Default || root )
			{
				/*!
					SERIALIZE SELF
				*/
				RapidJSON::JSONifyToValue ( value , document , "Type" , "Default" );

				// loop through all possible script components seeing if its in this entity
				rapidjson::Value script_component_value { rapidjson::kObjectType };
				script_component_value.SetObject ();
				for ( auto const& script : m_scripts )
				{
					for ( auto const& component : script.second.m_components )
					{
						// check if the entity has this component
						auto&& storage = GetStorage<sol::table> ( component.first );
						if ( storage.contains ( m_entities[ entity ].m_id ) )
						{
							rapidjson::Value script_component;
							script_component.SetObject ();

							// serialize component attributes
							auto& table = storage.get ( m_entities[ entity ].m_id );
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
							RapidJSON::JSONifyToValue ( script_component_value , document , component.first , script_component );
						}
					}
				}
				RapidJSON::JSONifyToValue ( value , document , "Script Components" , script_component_value );

				// loop through all possible engine components seeing if its in this entity
				rapidjson::Value engine_component_value { rapidjson::kObjectType };
				for ( auto j = 0; j < EngineComponents::m_component_names.size (); ++j )
				{
					T_Manip::RunOnType ( EngineComponents::Components () , j ,
						SerializeEngineComponent<EngineResources> () , std::ref ( m_registry ) , std::ref ( document ) , std::ref ( engine_component_value ) ,
						m_entities[ entity ].m_id , EngineComponents::m_component_names[ j ] , engineResources );
				}
				RapidJSON::JSONifyToValue ( value , document , "Engine Components" , engine_component_value );

				/*
					SERIALIZE CHILDREN
				*/
				rapidjson::Value children_value { rapidjson::kObjectType };
				for ( auto const& child : m_entities[ entity ].m_children )
				{
					rapidjson::Value child_value { rapidjson::kObjectType };
					SavePrefabV2Recurse ( engineResources , child , document , child_value );

					RapidJSON::JSONifyToValue ( children_value , document , m_entities[ child ].m_name , child_value );
				}
				RapidJSON::JSONifyToValue ( value , document , "Children" , children_value );
			}
			else if ( m_entities[ entity ].m_type == Entity_::Type::Prefab )
			{
				RapidJSON::JSONifyToValue ( value , document , "Type" , "Prefab" );
			}
		}
	}

	void EnttXSol::LoadPrefabV2 ( EngineResources& engineResources , std::string const& fileName , Entities::ID parent )
	{
		rapidjson::Document document;
		ReadJSON ( document , std::string ( "Assets/GameAssets/Prefabs/" ) + fileName + ".json" );

		LoadPrefabV2Recurse ( engineResources , document.MemberBegin ()->value , document.MemberBegin ()->name.GetString () , parent , true );
	}

	void EnttXSol::LoadPrefabV2Recurse ( EngineResources& engineResources , rapidjson::Value& value , std::string const& name , Entities::ID parent , bool root )
	{
		if ( std::string ( value[ "Type" ].GetString () ) == "Prefab" )
		{
			LoadPrefabV2 ( engineResources , name , parent );
		}
		else if ( std::string ( value[ "Type" ].GetString () ) == "Default" )
		{
			/*
				DESERIALIZE SELF
			*/
			// get name
			auto entity = CreateEntity ( name , parent );
			// the type prefab should only be assigned to the root node of a prefab
			if ( root )
			{
				m_entities[ entity ].m_type = Entity_::Type::Prefab;
			}

			// iterate script components
			for ( auto& script_component : value[ "Script Components" ].GetObj () )
			{
				std::string component_name = script_component.name.GetString ();
				AttachComponent ( entity , component_name );

				// update values
				auto&& storage = GetStorage<sol::table> ( component_name );
				auto& component = storage.get ( m_entities[ entity ].m_id );

				bool script_component_found { false };
				for ( auto const& script : m_scripts )
				{
					if ( script.second.m_components.find ( component_name ) != script.second.m_components.end () )
					{
						// attribute here refers to the component's attribute description
						for ( auto const& attribute : script.second.m_components.at ( component_name ).m_serialize_attributes )
						{
							auto attribute_name = std::get<0> ( attribute );
							auto attribute_type = std::get<1> ( attribute );

							if ( script_component.value.FindMember ( attribute_name.c_str () ) != script_component.value.MemberEnd () )
							{
								switch ( attribute_type )
								{
								case ( AttributeTypes::BOOL ):
									component.set ( attribute_name , script_component.value[ attribute_name.c_str () ].GetBool () );
									break;
								case ( AttributeTypes::INT ):
									component.set ( attribute_name , script_component.value[ attribute_name.c_str () ].GetInt () );
									break;
								case ( AttributeTypes::FLOAT ):
									component.set ( attribute_name , script_component.value[ attribute_name.c_str () ].GetFloat () );
									break;
								case ( AttributeTypes::STRING ):
									component.set ( attribute_name , script_component.value[ attribute_name.c_str () ].GetString () );
									break;
								}
							}
							else
							{
								std::cerr << "EnttXSol::DeserializeState - Expected value not in serialized file. Maybe outdated file? ["
									<< attribute_name << "] in prefab file [" << "oops" << "]" << std::endl;
							}
						}
						script_component_found = true;
					}
				}
				if ( !script_component_found )
				{
					std::cerr << "EnttXSol::DeserializeState - Previously serialized script component not found. Maybe redefined? ["
						<< component_name << "] in prefab file [" << "oops" << "]" << std::endl;
				}
			}

			// iterate engine components
			for ( auto& engine_component : value[ "Engine Components" ].GetObj () )
			{
				auto it = std::find ( EngineComponents::m_component_names.begin () , EngineComponents::m_component_names.end () , engine_component.name.GetString () );
				if ( it != EngineComponents::m_component_names.end () )
				{
					auto i = it - EngineComponents::m_component_names.begin ();
					T_Manip::RunOnType ( EngineComponents::Components () , i ,
						DeserializeEngineComponent<EngineResources> () , std::ref ( m_registry ) , m_entities[ entity ].m_id , std::ref ( engine_component.value ) , engineResources );
				}
			}

			/*
				DESERIALIZE CHILDREN
			*/
			for ( auto& children : value[ "Children" ].GetObj () )
			{
				LoadPrefabV2Recurse ( engineResources , children.value , children.name.GetString () , entity );
			}
		}
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

	void EnttXSol::AttachComponent ( Entities::ID id , std::string const& name )
	{
		sol::function component = m_lua[ name ] ();
		auto& storage = GetStorage<sol::table> ( name );
		if ( !storage.contains ( m_entities[ id ].m_id ) )
		{
			storage.emplace ( m_entities[ id ].m_id ) = component ();
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

	void EnttXSol::RecursiveRemoveEntity ( Entities::ID entity )
	{
		while ( !m_entities[ entity ].m_children.empty () )
		{
			RemoveEntity ( m_entities[ entity ].m_children.front () );
		}
		m_entities[ entity ].Destroy ( m_registry );
		m_entities.Erase ( entity );
	}
}