#pragma once

#include "godUtility.h"
#include "Internal/Miscellaneous.h"

#include <tuple>
#include <unordered_map>
#include <vector>
#include <algorithm>

namespace god
{
	template <typename T>
	struct Grid3D
	{
		struct Cell
		{
			std::vector<T> m_values;
		};

		// size, x, y, z
		using Coordinate = std::tuple<int32_t , int32_t , int32_t>;
		struct CoordinateHash
		{
			size_t operator()( Coordinate const& coordinate ) const;
		};
		// a GridLayer is a grid corresponding to a specific cell size
		using GridLayer = std::unordered_map<Coordinate , Cell , CoordinateHash>;
		using Grid = std::unordered_map<uint32_t , GridLayer>;

		void Insert ( float granularity , Coordinate const& coord , T const& value );
		void Erase ( float granularity , Coordinate const& coord );
		bool EraseValue ( float granularity , Coordinate const& coord , T const& value );
		std::vector<T> const& Get ( float granularity , Coordinate const& coord ) const;
		std::vector<T>& Get ( float granularity , Coordinate const& coord );
		void ChangeCell ( T const& value , float granularity , Coordinate const& from , Coordinate const& to );

		template <typename FN , typename...ARGS>
		void RunOver ( float granularity , Coordinate const& coord , FN fn , ARGS...args );

	private:
		Grid m_grid;

		// used to normalize the granularity of a grid accurate to 4dp, to counter the effects of floating point error
		uint32_t NormGran ( float granularity );
	};

	template<typename T>
	inline size_t Grid3D<T>::CoordinateHash::operator()( Coordinate const& coordinate ) const
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
	inline void Grid3D<T>::ChangeCell ( T const& value , float granularity , Coordinate const& from , Coordinate const& to )
	{
		if ( EraseValue ( granularity , from , value ) )
		{
			Insert ( granularity , to , value );
		}
	}

	template<typename T>
	inline uint32_t Grid3D<T>::NormGran ( float granularity )
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

	using EntityGrid = std::unordered_map<uint32_t , Grid3D<uint32_t>>;
}