#pragma once

#include "TemplateManipulation.h"

#include <functional>
#include <vector>
#include <queue>
#include <optional>
#include <assert.h>

namespace god
{
	template <typename...TYPES>
	using ReferenceWrapperCollection = T_Manip::TypeCollection<std::reference_wrapper , TYPES...>;

	template <typename T>
	struct DefaultCollection : public std::vector<std::optional<T>>
	{
		uint32_t PushBack ( T const& value );
		void Remove ( uint32_t id );

		/*!
		 * @brief
		 * : Returns true if the id is valid, i.e. < size() and has_value()
		 * @param id
		 * : Id to check.
		 * @return
		 * : If id is valid.
		*/
		bool Valid ( uint32_t id );

	private:
		std::priority_queue<uint32_t , std::vector<uint32_t> , std::greater<uint32_t>> m_free_ids;
	};

	template<typename T>
	inline uint32_t DefaultCollection<T>::PushBack ( T const& value )
	{
		if ( m_free_ids.empty () )
		{
			this->push_back ( value );
			return this->size () - 1;
		}
		else
		{
			uint32_t free_id = m_free_ids.top ();
			this->operator[]( free_id ) = value;
			m_free_ids.pop ();
			return free_id;
		}
	}

	template<typename T>
	inline void DefaultCollection<T>::Remove ( uint32_t id )
	{
		assert ( Valid ( id ) && "\ngodUtility/Structures.h - DefaultCollection::Remove - Invalid id." );
		m_free_ids.push ( id );
		this->operator[]( id ).reset ();
	}

	template<typename T>
	inline bool DefaultCollection<T>::Valid ( uint32_t id )
	{
		return id < this->size () && this->operator[]( id ).has_value ();
	}
}