#pragma once

#include "godUtility.h"
#include "Internal/Miscellaneous.h"
#include "Math.h"

#include <tuple>
#include <unordered_map>
#include <vector>
#include <list>
#include <algorithm>
#include <queue>

namespace god
{
	using Coordinate = std::tuple<int32_t , int32_t , int32_t>;
	struct CoordinateHash
	{
		size_t operator()( Coordinate const& coordinate ) const;
	};
	template <typename T>
	struct Grid3D
	{
		struct Cell
		{
			std::vector<T> m_values;
		};

		// size, x, y, z
		// a GridLayer is a grid corresponding to a specific cell size
		using GridLayer = std::unordered_map<Coordinate , Cell , CoordinateHash>;
		using Grid = std::unordered_map<uint32_t , GridLayer>;

		void Insert ( float granularity , Coordinate const& coord , T const& value );
		void Erase ( float granularity , Coordinate const& coord );
		bool EraseValue ( float granularity , Coordinate const& coord , T const& value );
		std::vector<T> const& Get ( float granularity , Coordinate const& coord ) const;
		std::vector<T>& Get ( float granularity , Coordinate const& coord );
		bool Has ( float granularity , Coordinate const& coord ) const;
		void ChangeCell ( T const& value , float granularity , Coordinate const& from , Coordinate const& to );

		template <typename FN , typename...ARGS>
		void RunOver ( float granularity , Coordinate const& coord , FN fn , ARGS...args );

		// pathfinding
		std::vector<Coordinate> GetPathAStar ( float granularity , Coordinate const& c1 , Coordinate const& c2 );

		bool RayCastToGrid ( Coordinate& coordinate , glm::vec3 const& origin , glm::vec3 const& scale , float granularity , glm::vec3 const& start , glm::vec3 const& end , int depth = 100 );

	private:
		Grid m_grid;

		// used to normalize the granularity of a grid accurate to 4dp, to counter the effects of floating point error
		uint32_t NormGran ( float granularity ) const;
	};

	inline size_t CoordinateHash::operator()( Coordinate const& coordinate ) const
	{
		size_t seed { 0 };
		HashCombine ( seed , std::get<0> ( coordinate ) );
		HashCombine ( seed , std::get<1> ( coordinate ) );
		HashCombine ( seed , std::get<2> ( coordinate ) );
		return seed;
	}

	template<typename T>
	inline void Grid3D<T>::Insert ( float granularity , Coordinate const& coord , T const& value )
	{
		m_grid[ NormGran ( granularity ) ][ coord ].m_values.push_back ( value );
	}

	template<typename T>
	inline bool Grid3D<T>::EraseValue ( float granularity , Coordinate const& coord , T const& value )
	{
		auto& first_container = Get ( granularity , coord );
		auto it = std::find ( first_container.begin () , first_container.end () , value );
		if ( it != first_container.end () )
		{
			first_container.erase ( it );
			return true;
		}
		return false;
	}

	template<typename T>
	inline void Grid3D<T>::Erase ( float granularity , Coordinate const& coord )
	{
		m_grid[ NormGran ( granularity ) ][ coord ].m_values.clear ();
	}

	template<typename T>
	inline std::vector<T> const& Grid3D<T>::Get ( float granularity , Coordinate const& coord ) const
	{
		return m_grid.at ( NormGran ( granularity ) ).at ( coord ).m_values;
	}

	template<typename T>
	inline std::vector<T>& Grid3D<T>::Get ( float granularity , Coordinate const& coord )
	{
		return m_grid[ NormGran ( granularity ) ][ coord ].m_values;
	}

	template<typename T>
	inline bool Grid3D<T>::Has ( float granularity , Coordinate const& coord ) const
	{
		uint32_t gran = NormGran ( granularity );
		if ( m_grid.find ( gran ) != m_grid.end () )
		{
			auto const& gran_grid = m_grid.at ( gran );
			if ( gran_grid.find ( coord ) != gran_grid.end () && !gran_grid.at ( coord ).m_values.empty () )
			{
				return true;
			}
		}
		return false;
	}

	template<typename T>
	inline void Grid3D<T>::ChangeCell ( T const& value , float granularity , Coordinate const& from , Coordinate const& to )
	{
		if ( EraseValue ( granularity , from , value ) )
		{
			Insert ( granularity , to , value );
		}
	}

	struct AStarNode
	{
		//Coordinate m_coordinate;
		int m_g { 1'000'000 } , m_h { 0 } , m_f { 0 };
		bool m_open { false } , m_close { false };
		Coordinate m_parent {};

		int GetF () const
		{
			return m_g + m_h;
		}
	};

	/*struct AStarNodeCompare
	{
		bool operator()( AStarNode const& node1 , AStarNode const& node2 )
		{
			return node1.m_f < node2.m_f;
		}
	};*/

	using AStarGrid = std::unordered_map<Coordinate , AStarNode , CoordinateHash>;

	template<typename T>
	inline std::vector<Coordinate> Grid3D<T>::GetPathAStar ( float granularity , Coordinate const& c1 , Coordinate const& c2 )
	{
		// max iterations if no solution found
		int32_t max_iteration = 1000;
		int32_t iteration { 0 };

		// A* algorithm
		GridLayer grid = m_grid[ NormGran ( granularity ) ];

		// check if target is occupied
		if ( grid.find ( c2 ) != grid.end () )
		{
			if ( grid[ c2 ].m_values.size () > 0 )
			{
				return std::vector<Coordinate> ();
			}
		}

		AStarGrid astar_grid;

		// initialize the open list
		astar_grid.insert ( { c1, AStarNode () } );
		astar_grid[ c1 ].m_g = 0;
		astar_grid[ c1 ].m_open = true;

		// loop open list
		bool search { true };
		while ( search )
		{
			if ( ++iteration > max_iteration )
			{
				std::cout << "Pathfinding too far, exceeded iterations." << std::endl;
				break;
			}

			search = false;

			// pop lowest f value node off the open and add to close list
			Coordinate lowest_f_coordinate;
			int lowest_f = 1'000'000;
			for ( auto const& node : astar_grid )
			{
				if ( node.second.m_open )
				{
					search = true;
					if ( node.second.GetF () < lowest_f )
					{
						lowest_f = node.second.GetF ();
						lowest_f_coordinate = node.first;
					}
				}
			}

			if ( search == false )
			{
				break;
			}

			AStarNode& current_node = astar_grid[ lowest_f_coordinate ];
			current_node.m_open = false;
			current_node.m_close = true;

			// if current node == end node we have reached
			if ( lowest_f_coordinate == c2 )
			{
				Coordinate current_coordinate = lowest_f_coordinate;
				std::vector<Coordinate> out;

				while ( current_coordinate != c1 )
				{
					out.push_back ( astar_grid[ current_coordinate ].m_parent );
					current_coordinate = astar_grid[ current_coordinate ].m_parent;
				}

				std::reverse ( out.begin () , out.end () );

				out.push_back ( c2 );

				return out;
			}

			// loop through neighbours, assuming no diagonal movement
			Coordinate neighbours[ 4 ]
			{
				{ std::get<0> ( lowest_f_coordinate ) - 1 , std::get<1> ( lowest_f_coordinate ), std::get<2> ( lowest_f_coordinate ) },
				{ std::get<0> ( lowest_f_coordinate ) + 1 , std::get<1> ( lowest_f_coordinate ), std::get<2> ( lowest_f_coordinate ) },
				{ std::get<0> ( lowest_f_coordinate ) , std::get<1> ( lowest_f_coordinate ), std::get<2> ( lowest_f_coordinate ) + 1 },
				{ std::get<0> ( lowest_f_coordinate ) , std::get<1> ( lowest_f_coordinate ), std::get<2> ( lowest_f_coordinate ) - 1 }
			};
			astar_grid.insert ( { neighbours[ 0 ] , AStarNode () } );
			astar_grid.insert ( { neighbours[ 1 ] , AStarNode () } );
			astar_grid.insert ( { neighbours[ 2 ] , AStarNode () } );
			astar_grid.insert ( { neighbours[ 3 ] , AStarNode () } );

			// for each neighbour
			for ( auto i = 0; i < 4; ++i )
			{
				Coordinate& neighbour_coordinate = neighbours[ i ];
				// if in close or not traversable, for now treat any occupied cell as untraversable
				if ( astar_grid[ neighbour_coordinate ].m_close || ( grid.find ( neighbour_coordinate ) != grid.end () && grid[ neighbour_coordinate ].m_values.size () > 0 ) )
				{
					continue;
				}

				int new_g_cost = astar_grid[ lowest_f_coordinate ].m_g + 1;

				// if not in open or new g cost lower than old
				auto& neighbour_node = astar_grid[ neighbour_coordinate ];
				if ( new_g_cost < neighbour_node.m_g || !neighbour_node.m_open )
				{
					neighbour_node.m_g = new_g_cost;
					neighbour_node.m_h = abs ( std::get<0> ( neighbour_coordinate ) - std::get<0> ( c2 ) ) +
						abs ( std::get<1> ( neighbour_coordinate ) - std::get<1> ( c2 ) );
					neighbour_node.m_parent = lowest_f_coordinate;

					if ( !neighbour_node.m_open )
					{
						astar_grid[ neighbour_coordinate ].m_open = true;
					}
				}
			}
		}

		return std::vector<Coordinate> ();
	}

	template<typename T>
	inline bool Grid3D<T>::RayCastToGrid ( Coordinate& coordinate , glm::vec3 const& origin , glm::vec3 const& scale , float granularity , glm::vec3 const& start , glm::vec3 const& end , int depth )
	{
		// start plane
		auto difference = start - origin;
		glm::vec3 stride = scale * 2.0f * granularity;
		int num_layers = static_cast< int >( difference.y / stride.y );
		float start_layer = origin.y + num_layers * stride.y;
		glm::vec3 intersect;
		int cell_x , cell_y , cell_z;
		bool found { false };
		for ( auto i = 0; i < depth && !found; ++i , start_layer -= stride.y )
		{
			if ( IntersectLineSegmentPlane ( start , end , { 0,1,0 } , start_layer + ( stride.y / 2.0f ) , intersect ) )
			{
				intersect -= origin;
				cell_x = static_cast< int >( std::floor ( intersect.x / stride.x ) );
				cell_y = num_layers - i;
				cell_z = static_cast< int >( std::floor ( intersect.z / stride.z ) );

				if ( Has ( granularity , { cell_x, cell_y, cell_z } ) )
				{
					coordinate = { cell_x, cell_y, cell_z };
					return true;
				}
			}
		}
		return false;
	}

	template<typename T>
	inline uint32_t Grid3D<T>::NormGran ( float granularity ) const
	{
		// cell granularity accurate to 4 decimal place
		return static_cast< uint32_t >( granularity * 1000 );
	}

	template<typename T>
	template<typename FN , typename...ARGS>
	inline void Grid3D<T>::RunOver ( float granularity , Coordinate const& coord , FN fn , ARGS...args )
	{
		for ( auto const& val : Get ( granularity , coord ) )
		{
			fn ( val , args... );
		}
	}

	//using EntityGrid = std::unordered_map<uint32_t , Grid3D<uint32_t>>;

	struct Grid3DCell
	{
		uint32_t m_entity_id;
		bool m_solid;

		Grid3DCell ( uint32_t entityID , bool solid = true )
			:
			m_entity_id ( entityID ) ,
			m_solid ( solid )
		{

		}

		operator uint32_t() const { return m_entity_id; }
	};
	using EntityGrid = std::unordered_map<uint32_t , Grid3D<Grid3DCell>>;
}