#pragma once

#include <tuple>
#include <type_traits>
#include <iostream>

namespace god
{
	namespace T_Manip
	{
		template < typename...T >
		using TYPE_PACK = std::tuple< T... >;

		/*!
		 * @brief
		 * : Access type in a type pack, i.e. tuple of packs, by index at runtime.
		 * @tparam ...T
		 * @param tuple
		 * : TYPE_PACK to process.
		 * @param index
		 * : Index of the type to retrieve.
		 * @return
		*/
		template <typename...ARGS>
		using TYPE_FUNCTION = void ( * )( ARGS... );

		//template < size_t I = 0 , typename...T , typename...U >
		//typename std::enable_if< I == sizeof...( T ) , void >::type
		//	GetType ( std::tuple< T... > tuple , size_t index , void( *function )( U... ) , U...functionArgs )
		//{
		//	( tuple );
		//	( index );
		//	std::cerr << "god::T_Manip::GetType - Index out of bound of tuple" << std::endl;
		//}

		//template < size_t I = 0 , typename...T , typename...U >
		//typename std::enable_if < I < sizeof...( T ) , void >::type
		//	GetType ( std::tuple< T... > tuple , size_t index , void( *function )( U... ) , U...functionArgs )
		//{
		//	if ( index == I )
		//	{
		//		using TYPE = decltype( std::get<I> ( tuple ) );

		//		//... do some logic
		//		function ( functionArgs... );

		//		return;
		//	}
		//	GetType<I + 1> ( tuple , index , function , functionArgs... );
		//}

		template < size_t I = 0 , typename...T , typename Function , typename...FArgs >
		typename std::enable_if< I == sizeof...( T ) , void >::type
			GetType ( std::tuple< T... > tuple , size_t index , Function function , FArgs...functionArgs )
		{
			( tuple );
			( index );
			std::cerr << "god::T_Manip::GetType - Index out of bound of tuple" << std::endl;
		}

		template < size_t I = 0 , typename...T , typename Function , typename...FArgs >
		typename std::enable_if < I < sizeof...( T ) , void >::type
			GetType ( std::tuple< T... > tuple , size_t index , Function function , FArgs...functionArgs )
		{
			if ( index == I )
			{
				using TYPE = decltype( std::get<I> ( tuple ) );

				//... do some logic
				function.operator() < std::remove_reference_t<TYPE> > ( functionArgs... );

				return;
			}
			GetType<I + 1> ( tuple , index , function, functionArgs... );
		}

		/*
			CHECK IF A TUPLE HAS A TYPE
		*/
		template <typename T , typename Tuple>
		struct HasType;

		template <typename T , typename... Us>
		struct HasType<T , std::tuple<Us...>> : std::disjunction<std::is_same<T , Us>...> {};

		/*
			TYPE COLLECTIONS
		*/
		template <template<typename T> class WRAP , typename...TYPES>
		struct TypeCollection
		{
			using DataType = std::tuple<WRAP<TYPES>...>;
			TypeCollection ( WRAP<TYPES>...types )
				:
				m_data { std::make_tuple ( types... ) }
			{

			}

			template <typename TYPE>
			WRAP<TYPE>& Get ()
			{
				return std::get<WRAP<TYPE>> ( m_data );
			}

			template <typename TYPE>
			constexpr bool Has () const
			{
				return HasType < WRAP<TYPE> , decltype( m_data ) >::value;
			}

		private:
			DataType m_data;
		};
	}
}