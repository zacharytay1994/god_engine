#pragma once

#include <vector>
#include <queue>
#include <optional>

namespace god
{
	template <typename T>
	struct RecycleVector
	{
		using ID = uint32_t;
		using IDQueue = std::priority_queue<ID , std::vector<ID> , std::greater<ID>>;
		static constexpr uint32_t Null = static_cast< uint32_t >( -1 );

		T& operator[]( ID id );
		bool Valid ( ID id );
		uint32_t Size ();
		uint32_t FreeIDSize ();

		ID Push ( T const& value );
		bool Erase ( ID id );

		void Clear ();

	private:
		std::vector<std::optional<T>> m_container;
		IDQueue m_free_ids;
	};

	template<typename T>
	inline T& RecycleVector<T>::operator[]( ID id )
	{
		return m_container[ id ].value ();
	}

	template<typename T>
	inline bool RecycleVector<T>::Valid ( ID id )
	{
		return id < m_container.size () && m_container[ id ].has_value ();
	}

	template<typename T>
	inline uint32_t RecycleVector<T>::Size ()
	{
		return static_cast< uint32_t >( m_container.size () );
	}

	template<typename T>
	inline uint32_t RecycleVector<T>::FreeIDSize ()
	{
		return m_free_ids.size ();
	}

	template<typename T>
	inline RecycleVector<T>::ID RecycleVector<T>::Push ( T const& value )
	{
		// if free ids available
		if ( m_free_ids.empty () )
		{
			m_container.push_back ( value );
			return static_cast< uint32_t >( m_container.size () ) - 1;
		}

		// if provide the free id nearest to 0
		ID top = m_free_ids.top ();
		m_free_ids.pop ();
		m_container[ top ].reset ();
		m_container[ top ].emplace ( value );
		return top;
	}

	template<typename T>
	inline bool RecycleVector<T>::Erase ( ID id )
	{
		if ( id < m_container.size () )
		{
			if ( m_container[ id ].has_value () )
			{
				m_container[ id ].reset ();
				return true;
			}
			return false;
		}
		return false;
	}

	template<typename T>
	inline void RecycleVector<T>::Clear ()
	{
		m_container.clear ();
		m_free_ids = IDQueue ();
	}
}