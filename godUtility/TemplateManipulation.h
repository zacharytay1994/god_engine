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
		template <typename RETURN , typename...ARGS>
		using TYPE_FUNCTION = RETURN ( * )( ARGS... );

		template < size_t I = 0 , typename...T >
		typename std::enable_if< I == sizeof...( T ) , void >::type
			GetType ( std::tuple< T... > tuple , size_t index )
		{
			( tuple );
			( index );
			std::cerr << "god::T_Manip::GetType - Index out of bound of tuple" << std::endl;
		}

		template < size_t I = 0 , typename...T >
		typename std::enable_if < I < sizeof...( T ) , void >::type
			GetType ( std::tuple< T... > tuple , size_t index )
		{
			if ( index == I )
			{
				using TYPE = decltype( std::get<I> ( tuple ) );

				//... do some logic
				std::cout << typeid( std::remove_reference_t<TYPE> ).name () << std::endl;

				return;
			}
			GetType<I + 1> ( tuple , index );
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

			template <size_t I>
			auto& Get ()
			{
				return std::get<I> ( m_data );
			}

			template <typename TYPE>
			constexpr bool Has () const
			{
				return HasType < WRAP<TYPE> , decltype( m_data ) >::value;
			}

		private:
			DataType m_data;
		};

		// template stuff
		template < typename...T >
		using TYPE_PACK = std::tuple< T... >;

		template < size_t I = 0 , typename...T , typename Function , typename...FArgs >
		typename std::enable_if< I == sizeof...( T ) , void >::type
			RunOnType ( std::tuple< T... > tuple , size_t index , Function function , FArgs...functionArgs )
		{
			( tuple );
			( index );
			std::cerr << "god::T_Manip::GetType - Index out of bound of tuple" << std::endl;
		}

		template < size_t I = 0 , typename...T , typename Function , typename...FArgs >
		typename std::enable_if < I < sizeof...( T ) , void >::type
			RunOnType ( std::tuple< T... > tuple , size_t index , Function function , FArgs...functionArgs )
		{
			if ( index == I )
			{
				using TYPE = decltype( std::get<I> ( tuple ) );

				//... do some logic
				function.operator() < std::remove_reference_t<TYPE> > ( functionArgs... );

				return;
			}
			RunOnType<I + 1> ( tuple , index , function , functionArgs... );
		}
	}
}