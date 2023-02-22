#pragma once

#include "../EngineComponents.h"

#include <vector>
#include <queue>
#include <tuple>
#include <unordered_map>
#include <algorithm>

#include <godUtility/Math.h>

namespace god
{
	namespace _350
	{
		using Cell = std::tuple <int32_t , int32_t , int32_t>;
		struct CellHash
		{
			template <class T1 , class T2 , class T3>
			size_t operator()( std::tuple<T1 , T2 , T3> const& t ) const
			{
				return std::get<0> ( t ) ^ std::get<1> ( t ) ^ std::get<2> ( t );
			}
		};

		struct Entity
		{
			enum class Type
			{
				Tile = 0 ,
				Enemy ,
				Playable
			} m_parent_type = Type::Tile;
			Cell m_cell { 0,0,0 };
			uint32_t m_entity_id;
			bool m_moused_over { false };
			float m_tmin { 0.0f };
			bool m_to_destroy { false };

			// movement
			bool m_moved { false };
			glm::vec3 m_lerp_to { 0,0,0 };

			void LerpPositionTo ( glm::vec3& position , float lerpSpeed , float dt )
			{
				position.x = std::lerp ( position.x , m_lerp_to.x , lerpSpeed * dt );
				position.y = std::lerp ( position.y , m_lerp_to.y , lerpSpeed * dt );
				position.z = std::lerp ( position.z , m_lerp_to.z , lerpSpeed * dt );

				if ( glm::length2 ( m_lerp_to - position ) < 0.1f )
				{
					m_moved = true;
				}
			}

			void UpdateLerpTo ( float lerpSpeed , float dt )
			{
				auto const& [x , y , z] = m_cell;
				m_lerp_to.x = std::lerp ( m_lerp_to.x , 2 * x , lerpSpeed * dt );
				m_lerp_to.y = std::lerp ( m_lerp_to.y , 2 * y , lerpSpeed * dt );
				m_lerp_to.z = std::lerp ( m_lerp_to.z , 2 * z , lerpSpeed * dt );
			}

			void UpdateCombatLerpTo ( float lerpSpeed , float dt , glm::vec3 combatOffset )
			{
				auto const& [x , y , z] = m_cell;
				m_lerp_to.x = std::lerp ( m_lerp_to.x , 2 * x + combatOffset.x , lerpSpeed * dt );
				m_lerp_to.y = std::lerp ( m_lerp_to.y , 2 * y + combatOffset.y , lerpSpeed * dt );
				m_lerp_to.z = std::lerp ( m_lerp_to.z , 2 * z + combatOffset.z , lerpSpeed * dt );
			}
		};

		struct Tile : public Entity
		{
			enum class Type
			{
				Normal = 0 ,
				Rock
			} m_type = Type::Normal;
			Tile ( Type type ) : m_type { type } {}
		};

		enum class Orientation
		{
			North = 0 ,
			East ,
			South ,
			West
		};

		struct Playable;
		struct Enemy : public Entity
		{
			enum class Type
			{
				Pawn = 0
			} m_type = Type::Pawn;
			Orientation m_orientation = Orientation::North;

			uint32_t m_destination { static_cast< uint32_t >( -1 ) };

			Enemy ( Type type , Orientation orientation = Orientation::North )
				:
				m_type { type } ,
				m_orientation { orientation }
			{}

			template <typename ENTT , typename ENGINE_RESOURCES , typename GRID>
			void Update ( ENTT& entt , ENGINE_RESOURCES& engineResources ,
				GRID& grid ,
				Enemy*& enemy ,
				Playable*& playable ,
				bool& combatAttacking )
			{
				switch ( m_type )
				{
				case ( Enemy::Type::Pawn ):
				{
					// move to previous decided
					if ( m_destination != static_cast< uint32_t >( -1 ) )
					{
						/*grid.MoveEntityOnTopOf ( entt , engineResources ,
							*static_cast< Entity* >( this ) , *static_cast< Entity* >( &grid.m_tiles[ m_destination ] ) );*/
						grid.LerpEntityOnTopOf ( entt , engineResources ,
							*static_cast< Entity* >( this ) , *static_cast< Entity* >( &grid.m_tiles[ m_destination ] ) );

						// if position is triton
						Tile& tile = grid.m_tiles[ m_destination ];
						auto& [x , y , z] = tile.m_cell;
						Entity* triton = grid.GetTriton ( x , y + 1 , z );
						if ( triton )
						{
							triton->m_to_destroy = true;
							// check if tile walked on has playable, if so eat it
							playable = static_cast< Playable* >( triton );
							enemy = this;
							combatAttacking = false;
						}

						// walk sound
						entt.QueueInstancePrefab ( "SFX_FootStep01" , 0.f , 0.f , 0.f );
					}
					// try searching for destination
					for ( int i = 0; i < 2; ++i )
					{
						auto [x , y , z] = m_cell;
						switch ( m_orientation )
						{
						case ( Orientation::North ):
						{
							++z;
							break;
						}
						case ( Orientation::East ):
						{
							++x;
							break;
						}
						case ( Orientation::South ):
						{
							--z;
							break;
						}
						case ( Orientation::West ):
						{
							--x;
							break;
						}
						}

						m_destination = static_cast< uint32_t >( -1 );
						// bottom step
						Tile* bottom = grid.GetTile ( x , y - 2 , z );
						if ( bottom != nullptr && bottom->m_type == Tile::Type::Normal && !grid.HasTile ( x , y - 1 , z ) )
						{
							m_destination = grid.m_map[ {x , y - 2 , z} ];
						}
						// same step
						else if ( ( bottom = grid.GetTile ( x , y - 1 , z ) ) != nullptr && bottom->m_type == Tile::Type::Normal && !grid.HasTile ( x , y , z ) )
						{
							m_destination = grid.m_map[ {x , y - 1 , z} ];
						}
						// top step
						else if ( ( bottom = grid.GetTile ( x , y , z ) ) != nullptr && bottom->m_type == Tile::Type::Normal && !grid.HasTile ( x , y + 1 , z ) )
						{
							m_destination = grid.m_map[ {x , y , z} ];
						}

						// if no destination and havent rotate
						if ( m_destination == static_cast< uint32_t >( -1 ) && i == 0 )
						{
							m_orientation = static_cast< Orientation >( ( static_cast< int >( m_orientation ) + 2 ) % 4 );
							Transform* transform = entt.GetEngineComponent<Transform> ( m_entity_id );
							if ( transform )
							{
								transform->m_rotation.y += 180.0f;
							}
						}
					}
				}
				}
			}
		};

		struct Playable : public Entity
		{
			enum class Type
			{
				Triton = 0
			} m_type = Type::Triton;
			Orientation m_orientation = Orientation::North;
			Playable ( Type type , Orientation orientation = Orientation::North )
				:
				m_type { type } ,
				m_orientation { orientation }
			{}

			bool Walkable ( Cell const& cell )
			{
				switch ( m_type )
				{
				case( Playable::Type::Triton ):
				{
					// 1 tile in any direction
					auto const& [x , y , z] = cell;
					auto const& [sx , sy , sz] = m_cell;

					int32_t dx = x - sx , dy = y - sy , dz = z - sz;

					return !( dx > 1 || dx < -1 || dy > 1 || dy < -1 || dz > 1 || dz < -1 ) && cell != m_cell;
				}
				}
				return false;
			}
		};

		struct Grid
		{
			bool m_deserialized { false };
			bool m_initialized { false };
			bool m_ready { false };
			bool m_start { false };

			bool m_to_restart { false };
			bool m_won { false };
			bool m_playable_walkable { false };
			std::vector<Tile> m_tiles;
			std::vector<Enemy> m_enemies;
			std::vector<Playable> m_playables;

			bool m_combat_paused { false };
			bool m_combat_attacking { true };
			bool m_combat_done { false };
			Playable* m_combat_playable { nullptr };
			Enemy* m_combat_enemy { nullptr };
			glm::vec3 m_combat_area_offset { 10'000,10'000,10'000 };
			float m_combat_trigger_distance { 0.7f };
			float m_combat_prep_time { 1.0f };
			float m_combat_prep_time_counter { m_combat_prep_time };
			glm::vec3 m_pre_combat_camera_position { 0.0f };
			glm::vec3 m_pre_combat_camera_lookat { 0.0f };
			bool m_combat_start_animation { false };
			float m_combat_end_lag { 1.0f };
			float m_combat_end_lag_counter { m_combat_end_lag };

			// camera controls
			glm::vec3 m_center { 0.0f,0.0f,0.0f };
			float m_camera_move_speed { 8.0f };
			float m_circle_value { 0.0f };
			float m_camera_circle_speed { 1.4f };
			float m_camera_height { 4.0f };
			float m_camera_zoom_speed { 8.0f };
			float m_camera_zoom_distance { 18.0f };
			float m_min_camera_zoom { 1.0f };
			float m_max_camera_zoom { 20.0f };

			float m_click_circle_value { 0.0f };
			float m_click_position_x , m_click_position_y;
			float m_click_zoom_value { 0.0f };
			float m_click_height_value { 0.0f };

			friend struct Enemy;

			template <typename ENTT , typename ENGINE_RESOURCES>
			void MoveEntityOnTopOf ( ENTT& entt , ENGINE_RESOURCES& engineResources ,
				Entity& src , Entity& dst )
			{
				Transform* src_transform = entt.GetEngineComponent<Transform> ( src.m_entity_id );
				Transform* dst_transform = entt.GetEngineComponent<Transform> ( dst.m_entity_id );
				if ( src_transform && dst_transform )
				{
					glm::vec3 pos { dst_transform->m_position };
					pos.y += 2.0f;
					src_transform->m_position = pos;
					auto& [x , y , z] = dst.m_cell;
					src.m_cell = { x,y + 1,z };
				}
			}

			template <typename ENTT , typename ENGINE_RESOURCES>
			void LerpEntityOnTopOf ( ENTT& entt , ENGINE_RESOURCES& engineResources ,
				Entity& src , Entity& dst )
			{
				Transform* src_transform = entt.GetEngineComponent<Transform> ( src.m_entity_id );
				Transform* dst_transform = entt.GetEngineComponent<Transform> ( dst.m_entity_id );
				if ( src_transform && dst_transform )
				{
					// calculate midpoint
					glm::vec3 midpoint = src_transform->m_position + ( dst_transform->m_position - src_transform->m_position ) / 2.0f;
					// height offset is height diff of src and dst + k, offset is 0 if negative
					float height_max = dst_transform->m_position.y - src_transform->m_position.y;
					auto const& [sx , sy , sz] = src.m_cell;
					auto const& [dx , dy , dz] = dst.m_cell;
					int32_t diagonal_x = dx - sx + sx;
					int32_t diagonal_z = dz - sz + sz;
					float potential_heighest { 0.0f };
					for ( int i = 0; i < 10; ++i )
					{
						if ( HasTile ( diagonal_x , i , sz ) || HasTile ( sx , i , diagonal_z ) )
						{
							potential_heighest = 2 * ( i - std::get<1> ( src.m_cell ) );
							if ( potential_heighest > height_max )
							{
								height_max = potential_heighest;
							}
						}
					}

					midpoint.y = 2.0f * std::get<1> ( src.m_cell ) + std::max ( height_max , 0.0f ) * 3.0f + 5.0f;
					src.m_lerp_to = midpoint;
					auto& [x , y , z] = dst.m_cell;
					src.m_cell = { x,y + 1,z };
				}

				src.m_moved = false;
			}

			template <typename ENTT , typename ENGINE_RESOURCES>
			void AddTile ( ENTT& entt , ENGINE_RESOURCES& engineResources , EntityData& level , Transform& level_transform ,
				Tile const& tile )
			{
				m_tiles.push_back ( tile );
				m_map[ tile.m_cell ] = m_tiles.size () - 1;

				// instance tile prefab
				auto e = entt.InstancePrefab ( engineResources , m_tile_names[ static_cast< int >( tile.m_type ) ] , level.m_id );
				m_tiles.back ().m_entity_id = e;
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
				Tile tile { type };
				tile.m_cell = std::make_tuple ( x , y , z );
				tile.m_parent_type = Entity::Type::Tile;
				AddTile ( entt , engineResources , level , level_transform , tile );
			}

			template <typename ENTT , typename ENGINE_RESOURCES>
			void AddEnemy ( ENTT& entt , ENGINE_RESOURCES& engineResources , EntityData& level , Transform& level_transform ,
				Enemy const& entity )
			{
				m_enemies.push_back ( entity );

				// instance entity prefab
				auto e = entt.InstancePrefab ( engineResources , m_enemy_names[ static_cast< int >( entity.m_type ) ] , level.m_id );
				m_enemies.back ().m_entity_id = e;
				Transform* transform = entt.GetEngineComponent<Transform> ( e );
				if ( transform )
				{
					auto& [x , y , z] = entity.m_cell;
					transform->m_position = { 2 * x,2 * y,2 * z };
					transform->m_rotation.y = static_cast< int >( entity.m_orientation ) * 90.0f;
				}
			}

			template <typename ENTT , typename ENGINE_RESOURCES>
			void AddEnemy ( ENTT& entt , ENGINE_RESOURCES& engineResources , EntityData& level , Transform& level_transform ,
				uint32_t x , uint32_t y , uint32_t z , Enemy::Type type = Enemy::Type::Pawn )
			{
				Enemy enemy { type };
				enemy.m_cell = std::make_tuple ( x , y , z );
				enemy.m_parent_type = Entity::Type::Enemy;
				AddEnemy ( entt , engineResources , level , level_transform , enemy );
			}

			template <typename ENTT , typename ENGINE_RESOURCES>
			void AddPlayable ( ENTT& entt , ENGINE_RESOURCES& engineResources , EntityData& level , Transform& level_transform ,
				Playable const& playable )
			{
				m_playables.push_back ( playable );

				// instance entity prefab
				auto e = entt.InstancePrefab ( engineResources , m_player_names[ static_cast< int >( playable.m_type ) ] , level.m_id );
				Transform* child_transform = entt.GetEngineComponent<Transform> ( entt.m_entities[ e ].m_children[ 0 ] );
				m_playables.back ().m_entity_id = e;
				Transform* transform = entt.GetEngineComponent<Transform> ( e );
				if ( transform )
				{
					auto& [x , y , z] = playable.m_cell;
					transform->m_position = { 2 * x,2 * y,2 * z };
					transform->m_rotation.y = static_cast< int >( playable.m_orientation ) * 90.0f;
				}
			}

			template <typename ENTT , typename ENGINE_RESOURCES>
			void AddPlayable ( ENTT& entt , ENGINE_RESOURCES& engineResources , EntityData& level , Transform& level_transform ,
				uint32_t x , uint32_t y , uint32_t z , Playable::Type type = Playable::Type::Triton )
			{
				Playable playable { type };
				playable.m_cell = std::make_tuple ( x , y , z );
				playable.m_parent_type = Entity::Type::Playable;
				AddPlayable ( entt , engineResources , level , level_transform , playable );
			}

			bool HasTile ( int32_t x , int32_t y , int32_t z )
			{
				return m_map.find ( std::make_tuple ( x , y , z ) ) != m_map.end ();
			}

			Tile* GetTile ( int32_t x , int32_t y , int32_t z )
			{
				if ( HasTile ( x , y , z ) )
				{
					return &m_tiles[ m_map[ {x , y , z} ] ];
				}
				return nullptr;
			}

			bool HasEnemy ( int32_t x , int32_t y , int32_t z )
			{
				auto it = std::find_if ( m_enemies.begin () , m_enemies.end () , [&x , &y , &z]( Enemy const& enemy )
					{
						return enemy.m_cell == std::make_tuple ( x , y , z );
					} );
				return it != m_enemies.end ();
			}

			Entity* GetEnemy ( int32_t x , int32_t y , int32_t z )
			{
				auto it = std::find_if ( m_enemies.begin () , m_enemies.end () , [&x , &y , &z]( Enemy const& enemy )
					{
						return enemy.m_cell == std::make_tuple ( x , y , z );
					} );
				if ( it != m_enemies.end () )
				{
					return static_cast< Entity* >( &*it );
				}
				return nullptr;
			}

			bool HasTriton ( int32_t x , int32_t y , int32_t z )
			{
				auto it = std::find_if ( m_playables.begin () , m_playables.end () , [&x , &y , &z]( Playable const& playable )
					{
						return playable.m_cell == std::make_tuple ( x , y , z ) && playable.m_type == Playable::Type::Triton;
					} );
				return it != m_playables.end ();
			}

			Entity* GetTriton ( int32_t x , int32_t y , int32_t z )
			{
				auto it = std::find_if ( m_playables.begin () , m_playables.end () , [&x , &y , &z]( Playable const& playable )
					{
						return playable.m_cell == std::make_tuple ( x , y , z ) && playable.m_type == Playable::Type::Triton;
					} );
				if ( it != m_playables.end () )
				{
					return static_cast< Entity* >( &*it );
				}
				return nullptr;
			}

			template <typename ENTT , typename ENGINE_RESOURCES>
			void Initialize ( ENTT& entt , ENGINE_RESOURCES& engineResources , EntityData& level , Transform& level_transform ,
				std::string const& level_layout , std::string const& entity_layout , std::string const& playable_layout )
			{
				// read level layout into m_nonset_tiles
				std::stringstream ss_level { level_layout };
				int type , x , y , z;
				while ( ss_level >> type >> x >> y >> z )
				{
					Tile tile ( static_cast< Tile::Type >( type ) );
					tile.m_cell = std::make_tuple ( x , y , z );
					tile.m_parent_type = Entity::Type::Tile;
					m_nonset_tiles.push ( tile );
				}

				// read entity layout into m_nonset_enemies
				std::stringstream ss_entities { entity_layout };
				char orientation;
				while ( ss_entities >> type >> orientation >> x >> y >> z )
				{
					Enemy enemy ( static_cast< Enemy::Type >( type ) , orientation_map[ orientation ] );
					enemy.m_cell = std::make_tuple ( x , y , z );
					enemy.m_parent_type = Entity::Type::Enemy;
					enemy.m_lerp_to = { 2 * x , 2 * y , 2 * z };
					m_nonset_enemies.push ( enemy );
				}

				// read playables into m_nonset_playable
				std::stringstream ss_playable { playable_layout };
				while ( ss_playable >> type >> orientation >> x >> y >> z )
				{
					Playable playable ( static_cast< Playable::Type >( type ) , orientation_map[ orientation ] );
					playable.m_cell = std::make_tuple ( x , y , z );
					playable.m_parent_type = Entity::Type::Playable;
					playable.m_lerp_to = { 2 * x , 2 * y , 2 * z };
					m_nonset_playable.push ( playable );
				}

				m_deserialized = true;
			}

			template<typename ENGINE_RESOURCES>
			void UpdateCameraControls ( float dt , ENGINE_RESOURCES& engineResources , glm::vec3 pos )
			{
				GLFWWindow& window = engineResources.Get<GLFWWindow> ().get ();
				Camera& camera = engineResources.Get<Camera> ().get ();

				// spin camera 
				camera.m_camera_pan_speed = 3.0f;
				camera.SetNextLookAt ( pos );
				float c_x = std::sin ( m_circle_value );
				float c_z = std::cos ( m_circle_value );

				camera.m_camera_move_speed = 3.0f;
				camera.SetNextPosition ( pos +
					glm::vec3 ( c_x * m_camera_zoom_distance ,
						m_camera_height ,
						c_z * m_camera_zoom_distance ) );

				if ( window.MouseRPressed () )
				{
					m_click_circle_value = m_circle_value;
					m_click_position_x = window.ViewportMouseX ();

					m_click_zoom_value = m_camera_zoom_distance;
					m_click_height_value = m_camera_height;
					m_click_position_y = window.ViewportMouseY ();
				}
				if ( window.MouseRUp () )
				{

				}

				if ( window.MouseRDown () )
				{
					m_circle_value = m_click_circle_value + ( m_click_position_x - window.ViewportMouseX () ) * 0.002f;

					float z = m_click_zoom_value + ( m_click_position_y - window.ViewportMouseY () ) * 0.02f;

					if ( z > m_min_camera_zoom && z < m_max_camera_zoom )
					{
						m_camera_zoom_distance = z;
						m_camera_height = m_click_height_value - ( m_click_position_y - window.ViewportMouseY () ) * 0.02f;
					}
				}

				// move camera left
				if ( window.KeyDown ( GLFW_KEY_A ) )
				{
					m_circle_value -= dt * m_camera_circle_speed;
				}
				if ( window.KeyDown ( GLFW_KEY_D ) )
				{
					m_circle_value += dt * m_camera_circle_speed;
				}
				// zoom camera
				if ( window.KeyDown ( GLFW_KEY_W ) )
				{
					if ( m_camera_zoom_distance > m_min_camera_zoom )
					{
						m_camera_zoom_distance -= dt * m_camera_zoom_speed;
						m_camera_height += dt * m_camera_zoom_speed;
					}
				}
				if ( window.KeyDown ( GLFW_KEY_S ) )
				{
					if ( m_camera_zoom_distance < m_max_camera_zoom && m_camera_height < m_max_camera_zoom )
					{
						m_camera_zoom_distance += dt * m_camera_zoom_speed;
						m_camera_height -= dt * m_camera_zoom_speed;
					}
				}
			}

			template <typename ENTT , typename ENGINE_RESOURCES>
			void Update ( float dt , ENTT& entt , ENGINE_RESOURCES& engineResources , EntityData& level , Transform& level_transform )
			{
				GLFWWindow& r_window = engineResources.Get<GLFWWindow> ().get ();
				if ( r_window.KeyPressed ( GLFW_KEY_R ) && m_initialized )
				{
					m_to_restart = true;
				}

				// add tiles, to ready state
				if ( !m_ready && m_deserialized )
				{
					// set reset button inactive
					uint32_t reset_button = entt.GetEntity ( "ResetButton" );
					if ( reset_button != static_cast< uint32_t >( -1 ) )
					{
						entt.SetEntityActive ( reset_button , false );
					}

					// set skip button active
					uint32_t skip_button = entt.GetEntity ( "SkipButton" );
					if ( skip_button != static_cast< uint32_t >( -1 ) )
					{
						entt.SetEntityActive ( skip_button , true );

						GUIObject* skip_gui = entt.GetEngineComponent<GUIObject> ( skip_button );
						if ( skip_gui )
						{
							if ( skip_gui->m_released )
							{
								m_add_interval = 0.0f;
							}
						}
					}

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
					// add enemies
					else if ( m_nonset_enemies.size () > 0 )
					{
						if ( m_add_interval_timer < m_add_interval )
						{
							m_add_interval_timer += dt;
						}
						else
						{
							// add an entity
							auto& entity = m_nonset_enemies.front ();
							AddEnemy ( entt , engineResources , level , level_transform , entity );
							m_nonset_enemies.pop ();

							m_add_interval_timer = 0.0f;
						}
					}
					else
					{
						m_ready = true;
					}
				}

				if ( m_start && m_ready && !m_initialized )
				{
					// add playable
					if ( m_nonset_playable.size () > 0 )
					{
						if ( m_add_interval_timer < m_add_interval )
						{
							m_add_interval_timer += dt;
						}
						else
						{
							// add an entity
							auto& entity = m_nonset_playable.front ();
							AddPlayable ( entt , engineResources , level , level_transform , entity );
							m_nonset_playable.pop ();

							m_add_interval_timer = 0.0f;
						}
					}
					// add black combat box
					else
					{
						// add black box at combat offset position
						/*auto e = entt.InstancePrefab ( engineResources , "350BlackBox2" , level.m_id );
						Transform* transform = entt.GetEngineComponent<Transform> ( e );
						if ( transform )
						{
							transform->m_position = m_combat_area_offset;
						}*/

						// set reset button active
						uint32_t reset_button = entt.GetEntity ( "ResetButton" );
						if ( reset_button != static_cast< uint32_t >( -1 ) )
						{
							entt.SetEntityActive ( reset_button , true );
						}

						// set skip button inactive
						uint32_t skip_button = entt.GetEntity ( "SkipButton" );
						if ( skip_button != static_cast< uint32_t >( -1 ) )
						{
							entt.SetEntityActive ( skip_button , false );
						}

						m_initialized = true;
					}
				}

				// test raycast to all entities
				GLFWWindow& window = engineResources.Get<GLFWWindow> ().get ();
				Camera& camera = engineResources.Get<Camera> ().get ();
				glm::vec3 ray_dir = ViewportToWorldRay (
					{ window.ViewportMouseX (), window.ViewportMouseY () } ,
					window.GetWindowWidth () ,
					window.GetWindowHeight () ,
					camera.GetPerpectiveProjectionMatrix () ,
					camera.GetCameraViewMatrix () );
				float min = std::numeric_limits<float>::max ();
				m_selected_entity = nullptr;
				for ( auto& tile : m_tiles )
				{
					// only test normal tiles, i.e. not rocks
					if ( tile.m_type == Tile::Type::Normal )
					{
						Transform* transform = entt.GetEngineComponent<Transform> ( tile.m_entity_id );
						if ( transform )
						{
							glm::vec3 center = transform->m_parent_transform * glm::vec4 ( transform->m_position , 1.0f );
							auto [intersect , tmin] = RayIntersectAABB ( camera.m_position , ray_dir , center - glm::vec3 ( 1 , 1 , 1 ) , center + glm::vec3 ( 1 , 1 , 1 ) );
							tile.m_moused_over = intersect;
							tile.m_tmin = tmin;

							if ( tmin < min )
							{
								min = tmin;
								m_selected_entity = static_cast< Entity* >( &tile );
							}

							transform->m_scale = glm::vec3 ( 1.0f );

							EntityData* entity_data = entt.GetEngineComponent<EntityData> ( tile.m_entity_id );
							if ( entity_data && entt.m_entities[ entity_data->m_id ].m_children.size () > 0 )
							{
								Renderable3D* renderable = entt.GetEngineComponent<Renderable3D> ( entt.m_entities[ entity_data->m_id ].m_children[ 0 ] );
								if ( renderable )
								{
									renderable->m_outlined = false;
								}
							}
						}
					}
				}

				// update camera controls 
				if ( !m_combat_paused )
				{
					UpdateCameraControls ( dt , engineResources , level_transform.m_parent_transform * level_transform.m_local_transform * glm::vec4 ( m_center , 1.0f ) );
				}


				if ( m_initialized )
				{
					if ( !m_combat_paused )
					{
						if ( m_combat_prep_time_counter > 0.0f )
						{
							m_combat_prep_time_counter -= dt;
						}
						else
						{
							// update movement of playable
							for ( auto& playable : m_playables )
							{
								Transform* transform = entt.GetEngineComponent<Transform> ( playable.m_entity_id );
								if ( transform )
								{
									playable.LerpPositionTo ( transform->m_position , 4.0f , dt );
								}
								playable.UpdateLerpTo ( 3.0f , dt );
							}

							// update movement of enemies
							for ( auto& enemy : m_enemies )
							{
								Transform* transform = entt.GetEngineComponent<Transform> ( enemy.m_entity_id );
								if ( transform )
								{
									enemy.LerpPositionTo ( transform->m_position , 4.0f , dt );
								}
								enemy.UpdateLerpTo ( 3.0f , dt );
							}

							// if combat entities playable and enemy is !nullptr, prepare to do combat cutscene, and combat pause
							if ( m_combat_playable && m_combat_enemy )
							{
								Transform* combat_playable_transform = entt.GetEngineComponent<Transform> ( m_combat_playable->m_entity_id );
								Transform* combat_enemy_transform = entt.GetEngineComponent<Transform> ( m_combat_enemy->m_entity_id );

								if ( combat_playable_transform && combat_enemy_transform )
								{
									// if near enough
									if ( glm::length2 ( combat_playable_transform->m_position - combat_enemy_transform->m_position ) < m_combat_trigger_distance )
									{
										// set combat prep
										m_combat_prep_time_counter = m_combat_prep_time;
										// create combat transtion, i.e. white screen
										entt.InstancePrefab ( engineResources , "WhiteFade" );
										// activate combat pause 
										m_combat_paused = true;
										// move the 2 entities to the combat area
										combat_playable_transform->m_position += m_combat_area_offset;
										m_combat_playable->m_lerp_to += m_combat_area_offset;
										combat_enemy_transform->m_position += m_combat_area_offset;
										m_combat_enemy->m_lerp_to += m_combat_area_offset;
										// move the camera to the combat area
										Camera& camera = engineResources.Get<Camera> ().get ();
										// cache camera position
										m_pre_combat_camera_position = camera.m_position;
										m_pre_combat_camera_lookat = camera.m_look_at;
										glm::vec3 world_combat_area_offset { level_transform.m_parent_transform /** level_transform.m_local_transform*/ * glm::vec4 ( m_combat_area_offset,1.0f ) };
										camera.SetPosition ( camera.m_position + world_combat_area_offset );
										camera.SetLookAt ( camera.m_look_at + world_combat_area_offset );
										// set new camera pan
										glm::vec3 v1 = combat_enemy_transform->m_position - combat_playable_transform->m_position;
										glm::vec3 midpoint = combat_playable_transform->m_position + v1 / 2.0f + glm::vec3 ( 0 , 1 , 0 );
										camera.m_camera_move_speed = 3.0f;
										camera.SetNextLookAt ( level_transform.m_parent_transform * level_transform.m_local_transform * glm::vec4 ( midpoint , 1.0f ) );
										camera.SetNextPosition ( level_transform.m_parent_transform * level_transform.m_local_transform * glm::vec4 ( midpoint + glm::normalize ( glm::cross ( v1 , glm::vec3 ( 0 , 1 , 0 ) ) ) * 5.0f , 1.0f ) );

										// set reset button inactive
										uint32_t reset_button = entt.GetEntity ( "ResetButton" );
										if ( reset_button != static_cast< uint32_t >( -1 ) )
										{
											entt.SetEntityActive ( reset_button , false );
										}

										// set skip button active
										uint32_t skip_button = entt.GetEntity ( "SkipButton" );
										if ( skip_button != static_cast< uint32_t >( -1 ) )
										{
											entt.SetEntityActive ( skip_button , true );
										}

										// deactivate tutorial gui
										uint32_t tut_camera = entt.GetEntity ( "TutCamera" );
										uint32_t tut_interact = entt.GetEntity ( "TutInteract" );
										if ( tut_camera != static_cast< uint32_t >( -1 ) && tut_interact != static_cast< uint32_t >( -1 ) )
										{
											GUIObject* tut_cam_gui = entt.GetEngineComponent<GUIObject> ( tut_camera );
											GUIObject* tut_int_gui = entt.GetEngineComponent<GUIObject> ( tut_interact );
											if ( tut_cam_gui && tut_int_gui )
											{
												tut_cam_gui->m_active = false;
												tut_int_gui->m_active = false;
											}
										}
									}
								}
							}

							// process gameplay
							m_playable_walkable = false;
							switch ( m_turn_order )
							{
							case ( TurnOrder::Playable ):
							{
								// process playables
								if ( m_playable_i < m_playables.size () )
								{
									auto& playable = m_playables[ m_playable_i ];

									// highlight current playable todo

									// update playable
									switch ( playable.m_type )
									{
									case ( Playable::Type::Triton ):
									{
										if ( m_selected_entity )
										{
											if ( m_selected_entity->m_parent_type == Entity::Type::Tile )
											{
												switch ( static_cast< Tile* >( m_selected_entity )->m_type )
												{
												case ( Tile::Type::Normal ):
												{
													// check if tile has nothing on top
													auto [x , y , z] = m_selected_entity->m_cell;
													if ( !HasTile ( x , y + 1 , z ) )
													{
														// move on top of clicked tile
														GLFWWindow& window = engineResources.Get<GLFWWindow> ().get ();
														m_playable_walkable = playable.Walkable ( { x , y + 1 , z } );

														if ( m_playable_walkable )
														{
															if ( m_previous_selected_entity != m_selected_entity )
															{
																m_previous_selected_entity = m_selected_entity;
																entt.QueueInstancePrefab ( "SFX_Hover02" , 0.f , 0.f , 0.f );
															}
														}

														if ( window.MouseLPressed ( 2 ) )
														{
															if ( m_playable_walkable )
															{
																/*MoveEntityOnTopOf ( entt , engineResources ,
																	m_playables.front () , *m_selected_entity );*/
																LerpEntityOnTopOf ( entt , engineResources ,
																	m_playables.front () , *m_selected_entity );

																// check if tile walked on has enemy, if so eat it
																m_combat_playable = &playable;
																m_combat_enemy = static_cast< Enemy* >( GetEnemy ( x , y + 1 , z ) );
																m_combat_attacking = true;
																//Entity* enemy = GetEnemy ( x , y + 1 , z );
																if ( m_combat_enemy )
																{
																	m_combat_enemy->m_to_destroy = true;
																}

																++m_playable_i;

																entt.QueueInstancePrefab ( "SFX_ButtonPress" , 0.f , 0.f , 0.f );
																entt.QueueInstancePrefab ( "SFX_FootStep03" , 0.f , 0.f , 0.f );
															}
															else
															{
																entt.QueueInstancePrefab ( "SFX_Invalid" , 0.f , 0.f , 0.f );
															}
														}
													}
												}
												}
											}

											m_previous_selected_entity = m_selected_entity;
										}
										break;
									}
									}

									// rotate playable to face tile
									if ( m_selected_entity && m_turn_order == TurnOrder::Playable && m_playable_walkable )
									{
										Transform* tile_transform = entt.GetEngineComponent<Transform> ( m_selected_entity->m_entity_id );
										Transform* playable_transform = entt.GetEngineComponent<Transform> ( playable.m_entity_id );
										if ( tile_transform && playable_transform )
										{
											glm::vec3 dir = tile_transform->m_position - playable_transform->m_position;
											glm::vec2 dir2 = { dir.x, dir.z };
											float deg = DegreeBetweenVec2 ( { 1,0 } , glm::normalize ( dir2 ) );
											float lerp_to = playable_transform->m_rotation.y - deg;
											if ( lerp_to > 180.0f )
											{
												playable_transform->m_rotation.y = std::lerp ( playable_transform->m_rotation.y , deg - 180.0f , dt * 10.0f );
											}
											else
											{
												playable_transform->m_rotation.y = std::lerp ( playable_transform->m_rotation.y , deg , dt * 10.0f );
											}
										}
									}
								}
								else
								{
									bool all_moved { true };
									for ( auto& playable : m_playables )
									{
										if ( !playable.m_moved )
										{
											all_moved = false;
										}
									}
									if ( all_moved )
									{
										m_playable_i = 0;
										m_turn_order = TurnOrder::Enemy;
										m_turn_changed = true;
									}
								}
								break;
							}
							case ( TurnOrder::Enemy ):
							{
								// update enemies
								if ( !m_updated_enemies )
								{
									for ( auto& enemy : m_enemies )
									{
										enemy.Update ( entt , engineResources , *this , m_combat_enemy , m_combat_playable , m_combat_attacking );
									}
									m_updated_enemies = true;
								}
								bool all_moved { true };
								for ( auto& enemy : m_enemies )
								{
									if ( !enemy.m_moved )
									{
										all_moved = false;
									}
								}

								// check if all moved
								if ( all_moved )
								{
									m_updated_enemies = false;
									m_turn_order = TurnOrder::Playable;
									m_turn_changed = true;
								}
								break;
							}
							}
						}
					}
					// combat phase
					else
					{
						if ( m_combat_prep_time_counter > 0.0f )
						{
							m_combat_prep_time_counter -= dt;
						}
						else
						{
							// skip combat
							uint32_t skip_button = entt.GetEntity ( "SkipButton" );
							if ( skip_button != static_cast< uint32_t >( -1 ) )
							{
								GUIObject* skip_gui = entt.GetEngineComponent<GUIObject> ( skip_button );
								if ( skip_gui )
								{
									if ( skip_gui->m_released )
									{
										m_combat_done = true;
										entt.SetEntityActive ( skip_button , false );
									}
								}
							}

							if ( m_combat_attacking )
							{
								Transform* combat_playable_transform = entt.GetEngineComponent<Transform> ( m_combat_playable->m_entity_id );
								Transform* combat_enemy_transform = entt.GetEngineComponent<Transform> ( m_combat_enemy->m_entity_id );

								if ( combat_playable_transform && combat_enemy_transform )
								{
									// continue updating camera position as midpoint of both
									// set new camera pan
									Camera& camera = engineResources.Get<Camera> ().get ();
									glm::vec3 v1 = combat_enemy_transform->m_position - combat_playable_transform->m_position;
									glm::vec3 midpoint = combat_playable_transform->m_position + v1 / 2.0f + glm::vec3 ( 0 , 1 , 0 );
									camera.m_camera_move_speed = 3.0f;
									camera.SetNextLookAt ( level_transform.m_parent_transform * level_transform.m_local_transform * glm::vec4 ( midpoint , 1.0f ) );
									camera.SetNextPosition ( level_transform.m_parent_transform * level_transform.m_local_transform * glm::vec4 ( midpoint + glm::normalize ( glm::cross ( v1 , glm::vec3 ( 0 , 1 , 0 ) ) ) * 5.0f , 1.0f ) );

									// move enemy away from player, works no matter who is attacking
									glm::vec3 dir = combat_enemy_transform->m_position - combat_playable_transform->m_position;
									dir.y = 0;
									glm::vec3 dst = combat_playable_transform->m_position + glm::normalize ( dir ) * 1.0f;

									combat_enemy_transform->m_position.x = std::lerp ( combat_enemy_transform->m_position.x , dst.x , 5.0f * dt );
									combat_enemy_transform->m_position.y = std::lerp ( combat_enemy_transform->m_position.y , dst.y , 5.0f * dt );
									combat_enemy_transform->m_position.z = std::lerp ( combat_enemy_transform->m_position.z , dst.z , 5.0f * dt );

									// face each other
									glm::vec3 norm_dir = glm::normalize ( dir );
									float a1 = DegreeBetweenVec2 ( { 1,0 } , { norm_dir.x, norm_dir.z } );
									float a2 = DegreeBetweenVec2 ( { 1,0 } , { -norm_dir.x, -norm_dir.z } );
									combat_playable_transform->m_rotation.y = a1;
									combat_enemy_transform->m_rotation.y = a2;
								}

								// update movement of playable
								for ( auto& playable : m_playables )
								{
									Transform* transform = entt.GetEngineComponent<Transform> ( playable.m_entity_id );
									if ( transform )
									{
										playable.LerpPositionTo ( transform->m_position , 4.0f , dt );
									}
									playable.UpdateCombatLerpTo ( 3.0f , dt , m_combat_area_offset );
								}

								// process animations
								SkeleAnim3D* playable_anim = entt.GetEngineComponent<SkeleAnim3D> ( entt.m_entities[ entt.m_entities[ m_combat_playable->m_entity_id ].m_children[ 0 ] ].m_children[ 0 ] );
								SkeleAnim3D* enemy_anim = entt.GetEngineComponent<SkeleAnim3D> ( entt.m_entities[ entt.m_entities[ m_combat_enemy->m_entity_id ].m_children[ 0 ] ].m_children[ 0 ] );
								if ( !m_combat_start_animation && playable_anim )
								{
									playable_anim->PlayAnimation ( "AOE" , false );
									// will be moved to lua script 
									entt.QueueInstancePrefab ( "SFX_AnimGroundSmash" , 0.f , 0.f , 0.f );
									m_combat_start_animation = true;
								}
								if ( playable_anim->m_current_sub_animation == "AOE" && playable_anim->m_animation_played )
								{
									playable_anim->PlayAnimation ( "Idle" , true );
									if ( enemy_anim )
									{
										enemy_anim->PlayAnimation ( "Death" , false );
										// will be moved to lua script 
										entt.QueueInstancePrefab ( "SFX_EnemyDeath" , 0.f , 0.f , 0.f );
									}
								}
								if ( enemy_anim->m_current_sub_animation == "Death" && enemy_anim->m_animation_played )
								{
									m_combat_done = true;
								}
							}
							// enemy is the one attacking
							else
							{
								Transform* combat_playable_transform = entt.GetEngineComponent<Transform> ( m_combat_playable->m_entity_id );
								Transform* combat_enemy_transform = entt.GetEngineComponent<Transform> ( m_combat_enemy->m_entity_id );

								if ( combat_playable_transform && combat_enemy_transform )
								{
									// continue updating camera position as midpoint of both
									// set new camera pan
									Camera& camera = engineResources.Get<Camera> ().get ();
									glm::vec3 v1 = combat_enemy_transform->m_position - combat_playable_transform->m_position;
									glm::vec3 midpoint = combat_playable_transform->m_position + v1 / 2.0f + glm::vec3 ( 0 , 1 , 0 );
									camera.m_camera_move_speed = 3.0f;
									camera.SetNextLookAt ( level_transform.m_parent_transform * level_transform.m_local_transform * glm::vec4 ( midpoint , 1.0f ) );
									camera.SetNextPosition ( level_transform.m_parent_transform * level_transform.m_local_transform * glm::vec4 ( midpoint + glm::normalize ( glm::cross ( v1 , glm::vec3 ( 0 , 1 , 0 ) ) ) * 5.0f , 1.0f ) );

									// move player away from enemy
									glm::vec3 dir = combat_enemy_transform->m_position - combat_playable_transform->m_position;
									dir.y = 0;
									glm::vec3 dst = combat_enemy_transform->m_position - glm::normalize ( dir ) * 1.0f;

									combat_playable_transform->m_position.x = std::lerp ( combat_playable_transform->m_position.x , dst.x , 5.0f * dt );
									combat_playable_transform->m_position.y = std::lerp ( combat_playable_transform->m_position.y , dst.y , 5.0f * dt );
									combat_playable_transform->m_position.z = std::lerp ( combat_playable_transform->m_position.z , dst.z , 5.0f * dt );

									// face each other
									glm::vec3 norm_dir = glm::normalize ( dir );
									float a1 = DegreeBetweenVec2 ( { 1,0 } , { norm_dir.x, norm_dir.z } );
									float a2 = DegreeBetweenVec2 ( { 1,0 } , { -norm_dir.x, -norm_dir.z } );
									combat_playable_transform->m_rotation.y = a1;
									combat_enemy_transform->m_rotation.y = a2;
								}

								// update movement of enemies
								for ( auto& enemy : m_enemies )
								{
									Transform* transform = entt.GetEngineComponent<Transform> ( enemy.m_entity_id );
									if ( transform )
									{
										enemy.LerpPositionTo ( transform->m_position , 4.0f , dt );
									}
									enemy.UpdateCombatLerpTo ( 3.0f , dt , m_combat_area_offset );
								}

								SkeleAnim3D* playable_anim = entt.GetEngineComponent<SkeleAnim3D> ( entt.m_entities[ entt.m_entities[ m_combat_playable->m_entity_id ].m_children[ 0 ] ].m_children[ 0 ] );
								SkeleAnim3D* enemy_anim = entt.GetEngineComponent<SkeleAnim3D> ( entt.m_entities[ entt.m_entities[ m_combat_enemy->m_entity_id ].m_children[ 0 ] ].m_children[ 0 ] );
								if ( !m_combat_start_animation && enemy_anim )
								{
									enemy_anim->PlayAnimation ( "Headbutt" , false );
									// will be moved to lua script 
									entt.QueueInstancePrefab ( "SFX_EnemyCharge" , 0.f , 0.f , 0.f );
									m_combat_start_animation = true;
								}
								if ( enemy_anim->m_current_sub_animation == "Headbutt" && enemy_anim->m_animation_played )
								{
									enemy_anim->PlayAnimation ( "Idle" , true );
									if ( playable_anim )
									{
										playable_anim->PlayAnimation ( "Death" , false );
										// will be moved to lua script 
										entt.QueueInstancePrefab ( "SFX_PlayerDeath" , 0.f , 0.f , 0.f );
									}
								}
								if ( playable_anim->m_current_sub_animation == "Death" && playable_anim->m_animation_played )
								{
									m_combat_done = true;
								}

								GLFWWindow& window = engineResources.Get<GLFWWindow> ().get ();
								if ( window.KeyPressed ( GLFW_KEY_I ) )
								{
									m_combat_done = true;
								}
							}

							if ( m_combat_done )
							{
								if ( m_combat_end_lag_counter > 0.0f )
								{
									m_combat_end_lag_counter -= dt;
								}
								else
								{
									// set reset button active
									uint32_t reset_button = entt.GetEntity ( "ResetButton" );
									if ( reset_button != static_cast< uint32_t >( -1 ) )
									{
										entt.SetEntityActive ( reset_button , true );
									}

									m_combat_paused = false;
									m_combat_done = false;

									// reset combat variables
									m_combat_start_animation = false;
									m_combat_prep_time_counter = m_combat_prep_time;
									m_combat_end_lag_counter = m_combat_end_lag;

									// create combat transtion, i.e. white screen
									entt.InstancePrefab ( engineResources , "WhiteFade" );

									// reset remaining entity
									if ( m_combat_attacking )
									{
										Transform* transform = entt.GetEngineComponent<Transform> ( m_combat_playable->m_entity_id );
										SkeleAnim3D* playable_anim = entt.GetEngineComponent<SkeleAnim3D> ( entt.m_entities[ entt.m_entities[ m_combat_playable->m_entity_id ].m_children[ 0 ] ].m_children[ 0 ] );
										if ( transform )
										{
											transform->m_position -= m_combat_area_offset;
										}
										if ( playable_anim )
										{
											playable_anim->PlayAnimation ( "Idle" , true );
										}
										m_combat_playable->m_lerp_to -= m_combat_area_offset;
									}
									else
									{
										Transform* transform = entt.GetEngineComponent<Transform> ( m_combat_enemy->m_entity_id );
										SkeleAnim3D* enemy_anim = entt.GetEngineComponent<SkeleAnim3D> ( entt.m_entities[ entt.m_entities[ m_combat_enemy->m_entity_id ].m_children[ 0 ] ].m_children[ 0 ] );
										if ( transform )
										{
											transform->m_position -= m_combat_area_offset;
										}
										if ( enemy_anim )
										{
											enemy_anim->PlayAnimation ( "Idle" , true );
										}
										m_combat_playable->m_lerp_to -= m_combat_area_offset;
									}

									// reset camera
									Camera& camera = engineResources.Get<Camera> ().get ();
									camera.SetPosition ( m_pre_combat_camera_position );
									camera.SetLookAt ( m_pre_combat_camera_lookat );

									// set combat entities back
									m_combat_enemy = nullptr;
									m_combat_playable = nullptr;
								}
							}
						}
					}
				}
				else
				{
					// if not initialized, recalculate center
					if ( !m_tiles.empty () )
					{
						m_center = { 0.0f };
						for ( auto const& tile : m_tiles )
						{
							auto const& [x , y , z] = tile.m_cell;
							m_center += glm::vec3 ( 2.0f * x , 2.0f * y , 2.0f * z );
						}
						m_center /= m_tiles.size ();
					}

					// spin camera
					m_circle_value += dt * m_camera_circle_speed;
				}

				// enlarge selected entity
				// only ray cast on player turn
				if ( m_selected_entity && m_turn_order == TurnOrder::Playable && m_playable_walkable )
				{
					Transform* transform = entt.GetEngineComponent<Transform> ( m_selected_entity->m_entity_id );
					if ( transform )
					{
						transform->m_scale = glm::vec3 ( 1.1f );
					}
					EntityData* entity_data = entt.GetEngineComponent<EntityData> ( m_selected_entity->m_entity_id );
					if ( entity_data && entt.m_entities[ entity_data->m_id ].m_children.size () > 0 )
					{
						Renderable3D* renderable = entt.GetEngineComponent<Renderable3D> ( entt.m_entities[ entity_data->m_id ].m_children[ 0 ] );
						if ( renderable )
						{
							renderable->m_outlined = true;
						}
					}
				}

				// check on turn change
				if ( m_turn_changed )
				{
					// destroy all to be destroyed entities
					for ( auto e_it = m_enemies.begin (); e_it != m_enemies.end (); )
					{
						auto& enemy = *e_it;
						if ( enemy.m_to_destroy )
						{
							EntityData* entity_data = entt.GetEngineComponent<EntityData> ( enemy.m_entity_id );
							if ( entity_data )
							{
								entt.QueueDelete ( entity_data->m_id );
							}
							e_it = m_enemies.erase ( e_it );
						}
						else
						{
							++e_it;
						}
					}

					// destroy all to be destroyed playables
					for ( auto p_it = m_playables.begin (); p_it != m_playables.end (); )
					{
						auto& playable = *p_it;
						if ( playable.m_to_destroy )
						{
							EntityData* entity_data = entt.GetEngineComponent<EntityData> ( playable.m_entity_id );
							if ( entity_data )
							{
								entt.QueueDelete ( entity_data->m_id );
							}
							p_it = m_playables.erase ( p_it );
						}
						else
						{
							++p_it;
						}
					}

					// if theres no enemies left, reset the level
					if ( m_playables.empty () && m_initialized )
					{
						m_to_restart = true;
					}
					if ( m_enemies.empty () && m_initialized )
					{
						m_won = true;
					}

					m_turn_changed = false;
				}
			}

		private:
			// should only be used to query the actual grid cells container
			std::unordered_map<Cell , uint32_t , CellHash> m_map;

			// fancy add 
			std::queue<Tile> m_nonset_tiles;
			std::queue<Enemy> m_nonset_enemies;
			std::queue<Playable> m_nonset_playable;
			float m_add_interval { 0.1f };
			float m_add_interval_timer { m_add_interval };

			// names
			std::vector<std::string> m_tile_names { "350NormalTile", "350Rock" };
			std::vector<std::string> m_enemy_names { "350Pawn" };
			std::vector<std::string> m_player_names { "350Triton" };

			// orientation definition
			std::unordered_map<char , Orientation> orientation_map { {'N',Orientation::North}, {'E',Orientation::East}, {'S',Orientation::South}, {'W',Orientation::West} };

			// level gameplay
			enum class TurnOrder
			{
				Playable ,
				Enemy
			} m_turn_order = TurnOrder::Enemy;
			uint32_t m_playable_i { 0 };
			uint32_t m_enemy_i { 0 };
			Entity* m_selected_entity { nullptr };
			Entity* m_previous_selected_entity { nullptr };
			bool m_updated_enemies { false };
			bool m_turn_changed { false };

		};
	}

	/* ENGINE COMPONENTS */
	struct _350Level
	{
		bool m_focused { true };

		_350::Grid m_grid;
		std::string m_level_layout { "" };
		std::string m_enemy_layout { "" };
		std::string m_playable_layout { "" };

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
				ImGui::InputText ( "Enemies" , &component.m_enemy_layout );
				ImGui::InputText ( "Playable" , &component.m_playable_layout );
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
		RapidJSON::JSONifyToValue ( value , document , "entity_layout" , component.m_enemy_layout );
		RapidJSON::JSONifyToValue ( value , document , "player_layout" , component.m_playable_layout );
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
		AssignIfExist ( jsonObj , component.m_enemy_layout , "entity_layout" );
		AssignIfExist ( jsonObj , component.m_playable_layout , "player_layout" );
	}
}