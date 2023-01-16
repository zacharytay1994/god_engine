#pragma once

#include "../EngineComponents.h"

#include <vector>
#include <queue>
#include <tuple>
#include <unordered_map>

namespace god
{
	namespace _350
	{
		using Cell = std::tuple <uint32_t , uint32_t , uint32_t>;
		struct CellHash
		{
			template <class T1 , class T2 , class T3>
			size_t operator()( std::tuple<T1 , T2 , T3> const& t ) const
			{
				return std::get<0> ( t ) ^ std::get<1> ( t ) ^ std::get<2> ( t );
			}
		};

		struct Tile
		{
			enum class Type
			{
				Normal = 0
			} m_type = Type::Normal;
			Cell m_cell { 0,0,0 };
		};

		struct Entity
		{
			enum class Type
			{
				Player = 0 ,
				Pawn
			} m_type = Type::Pawn;
			enum class Orientation
			{
				North = 0 ,
				South ,
				East ,
				West
			} m_orientation = Orientation::North;
			Cell m_cell { 0,0,0 };
		};

		struct Grid
		{
			bool m_initialized { false };
			std::vector<Tile> m_tiles;
			std::vector<Entity> m_entities;

			template <typename ENTT , typename ENGINE_RESOURCES>
			void AddTile ( ENTT& entt , ENGINE_RESOURCES& engineResources , EntityData& level , Transform& level_transform ,
				Tile const& tile )
			{
				m_tiles.push_back ( tile );
				m_map[ tile.m_cell ] = m_tiles.size () - 1;

				// instance tile prefab
				auto e = entt.InstancePrefab ( engineResources , m_tile_names[ static_cast< int >( tile.m_type ) ] , level.m_id );
				Transform* transform = entt.GetEngineComponent<Transform> ( e );
				if ( transform )
				{
					auto& [x , y , z] = tile.m_cell;
					transform->m_position = { 2 * x,2 * y,2 * z };
				}
			}

			template <typename ENTT , typename ENGINE_RESOURCES>
			void AddTile ( ENTT& entt , ENGINE_RESOURCES& engineResources , EntityData& level , Transform& level_transform ,
				uint32_t x , uint32_t y , uint32_t z , Tile::Type type = Tile::Type::Normal )
			{
				AddTile ( entt , engineResources , level , level_transform ,
					{ type, std::make_tuple ( x,y,z ) } );
			}

			template <typename ENTT , typename ENGINE_RESOURCES>
			void AddEntity ( ENTT& entt , ENGINE_RESOURCES& engineResources , EntityData& level , Transform& level_transform ,
				Entity const& entity )
			{
				m_entities.push_back ( entity );

				// instance entity prefab
				auto e = entt.InstancePrefab ( engineResources , m_entity_names[ static_cast< int >( entity.m_type ) ] , level.m_id );
				Transform* transform = entt.GetEngineComponent<Transform> ( e );
				if ( transform )
				{
					auto& [x , y , z] = entity.m_cell;
					transform->m_position = { 2 * x,2 * y,2 * z };
					transform->m_rotation.y = static_cast< int >( entity.m_orientation ) * 90.0f;
				}
			}

			template <typename ENTT , typename ENGINE_RESOURCES>
			void AddEntity ( ENTT& entt , ENGINE_RESOURCES& engineResources , EntityData& level , Transform& level_transform ,
				uint32_t x , uint32_t y , uint32_t z , Entity::Type type = Entity::Type::Player )
			{
				AddEntity ( entt , engineResources , level , level_transform ,
					{ type, std::make_tuple ( x,y,z ) } );
			}

			bool HasTile ( uint32_t x , uint32_t y , uint32_t z )
			{
				return m_map.find ( std::make_tuple ( x , y , z ) ) != m_map.end ();
			}

			template <typename ENTT , typename ENGINE_RESOURCES>
			void Initialize ( ENTT& entt , ENGINE_RESOURCES& engineResources , EntityData& level , Transform& level_transform , std::string const& level_layout, std::string const& entity_layout )
			{
				// read level layout into m_nonset_tiles
				std::stringstream ss_level { level_layout };
				int type , x , y , z;
				while ( ss_level >> type >> x >> y >> z )
				{
					Tile::Type default_type { Tile::Type::Normal };
					// check the type
					switch ( static_cast< Tile::Type >( type ) )
					{
					case ( Tile::Type::Normal ):
					{
						default_type = Tile::Type::Normal;
						break;
					}
					}

					m_nonset_tiles.push ( { default_type, std::make_tuple ( x,y,z ) } );
				}

				// read entity layout into m_nonset_entities
				std::stringstream ss_entities { entity_layout };
				char orientation;
				while ( ss_entities >> type >> orientation >> x >> y >> z )
				{
					Entity::Type default_type { Entity::Type::Player };
					Entity::Orientation default_orientation { Entity::Orientation::North };
					switch ( static_cast< Entity::Type >( type ) )
					{
					case ( Entity::Type::Player ):
					{
						default_type = Entity::Type::Player;
						break;
					}
					}
					switch ( orientation )
					{
					case ( 'N' ):
					{
						default_orientation = Entity::Orientation::North;
					}
					case ( 'E' ):
					{
						default_orientation = Entity::Orientation::East;
					}
					case ( 'S' ):
					{
						default_orientation = Entity::Orientation::South;
					}
					case ( 'W' ):
					{
						default_orientation = Entity::Orientation::West;
					}
					}

					m_nonset_entities.push ( { default_type, default_orientation, std::make_tuple ( x,y,z ) } );
				}
			}

			template <typename ENTT , typename ENGINE_RESOURCES>
			void Update ( float dt , ENTT& entt , ENGINE_RESOURCES& engineResources , EntityData& level , Transform& level_transform )
			{
				if ( m_nonset_tiles.size () > 0 )
				{
					if ( m_add_interval_timer < m_add_interval )
					{
						m_add_interval_timer += dt;
					}
					else
					{
						// add a tile
						auto& tile = m_nonset_tiles.front ();
						AddTile ( entt , engineResources , level , level_transform , tile );
						m_nonset_tiles.pop ();

						m_add_interval_timer = 0.0f;
					}
				}
				else if ( m_nonset_entities.size () > 0 )
				{
					if ( m_add_interval_timer < m_add_interval )
					{
						m_add_interval_timer += dt;
					}
					else
					{
						// add an entity
						auto& entity = m_nonset_entities.front ();
						AddEntity ( entt , engineResources , level , level_transform , entity );
						m_nonset_entities.pop ();

						m_add_interval_timer = 0.0f;
					}
				}
				else
				{
					m_initialized = true;
				}
			}

		private:
			// should only be used to query the actual grid cells container
			std::unordered_map<Cell , uint32_t , CellHash> m_map;

			// fancy add 
			std::queue<Tile> m_nonset_tiles;
			std::queue<Entity> m_nonset_entities;
			float m_add_interval { 0.1f };
			float m_add_interval_timer { m_add_interval };

			// names
			std::vector<std::string> m_tile_names { "350NormalTile" };
			std::vector<std::string> m_entity_names { "350Player" };
		};
	}

	/* ENGINE COMPONENTS */
	struct _350Level
	{
		_350::Grid m_grid;
		std::string m_level_layout { "" };
		std::string m_entity_layout { "" };

		bool operator==( _350Level const& rhs )
		{
			( rhs );
			return true;
		}
	};

	template <>
	inline void NewLuaType<_350Level> ( sol::state& luaState , std::string const& name )
	{
		/*RegisterLuaType<_350Level> ( luaState , name ,
			"i" , &_350Level::i ,
			"f" , &_350Level::f ,
			"s" , &_350Level::s );*/
	}
	template<>
	inline void ComponentInspector::operator() < _350Level > ( entt::entity entity , entt::registry& registry , int& imguiUniqueID , EngineResources& editorResources )
	{
		RegisterInspector<_350Level , EngineResources> ( entity , registry , imguiUniqueID , editorResources ,
			[]( _350Level& component , EngineResources& resources )
			{
				( resources );
				ImGui::Separator ();
				ImGui::Text ( "GAM350 Level" );
				ImGui::Separator ();

				ImGui::InputText ( "Level" , &component.m_level_layout );
				ImGui::InputText ( "Entities" , &component.m_entity_layout );
			} );
	}

	template<>
	inline void JSONify<_350Level> ( EngineResources& engineResources , rapidjson::Document& document , rapidjson::Value& value , _350Level& component )
	{
		( engineResources );
		// serialize
		/*RapidJSON::JSONifyToValue ( value , document , "i" , component.i );
		RapidJSON::JSONifyToValue ( value , document , "f" , component.f );
		RapidJSON::JSONifyToValue ( value , document , "s" , component.s );*/
		RapidJSON::JSONifyToValue ( value , document , "level_layout" , component.m_level_layout );
		RapidJSON::JSONifyToValue ( value , document , "entity_layout" , component.m_entity_layout );
	}

	template<>
	inline void DeJSONify<_350Level> ( EngineResources& engineResources , _350Level& component , rapidjson::Value& jsonObj )
	{
		( engineResources );
		// deserialize
		/*AssignIfExist ( jsonObj , component.i , "i" );
		AssignIfExist ( jsonObj , component.f , "f" );
		AssignIfExist ( jsonObj , component.s , "s" );*/
		AssignIfExist ( jsonObj , component.m_level_layout , "level_layout" );
		AssignIfExist ( jsonObj , component.m_entity_layout , "entity_layout" );
	}
}