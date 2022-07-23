#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <assert.h>

namespace god
{
	using ResourceID = uint32_t;

	template <typename T>
	struct ResourceManager
	{
		ResourceManager ();
		ResourceID Insert ( std::string const& name , T const& resource );
		T& Get ( std::string const& name );
		T& Get ( ResourceID id );

		std::string GetName ( ResourceID id ) const;
		ResourceID	GetID ( std::string const& name ) const;

		std::vector<T> const& GetResources () const;
		std::unordered_map<std::string , ResourceID> const& GetIDs () const;

		bool Has ( std::string const& name ) const;

	private:
		std::vector<T> m_resources;
		std::unordered_map<std::string , ResourceID> m_resource_ids;
	};

	template<typename T>
	ResourceManager<T>::ResourceManager ()
	{
	}

	template <typename T>
	ResourceID ResourceManager<T>::Insert ( std::string const& name , T const& resource )
	{
		// assert that resource has not been added before
		assert ( m_resource_ids.find ( name ) == m_resource_ids.end () );
		ResourceID new_id = static_cast< ResourceID >( m_resources.size () );
		m_resource_ids.emplace ( name , new_id );
		m_resources.push_back ( resource );
		return new_id;
	}

	template<typename T>
	T& ResourceManager<T>::Get ( std::string const& name )
	{
		assert ( m_resource_ids.find ( name ) != m_resource_ids.end () );
		return Get ( m_resource_ids[ name ] );
	}

	template<typename T>
	T& ResourceManager<T>::Get ( ResourceID id )
	{
		assert ( id < static_cast< ResourceID >( m_resources.size () ) );
		return m_resources[ id ];
	}

	template<typename T>
	std::string ResourceManager<T>::GetName ( ResourceID id ) const
	{
		for ( auto const& resource : m_resource_ids )
		{
			if ( resource.second == id )
			{
				return resource.first;
			}
		}
		return "Not Found!";
	}

	template<typename T>
	ResourceID ResourceManager<T>::GetID ( std::string const& name ) const
	{
		assert ( m_resource_ids.find ( name ) != m_resource_ids.end () );
		return m_resource_ids.at ( name );
	}

	template<typename T>
	std::vector<T> const& ResourceManager<T>::GetResources () const
	{
		return m_resources;
	}

	template<typename T>
	std::unordered_map<std::string , ResourceID> const& ResourceManager<T>::GetIDs () const
	{
		return m_resource_ids;
	}

	template<typename T>
	bool ResourceManager<T>::Has ( std::string const& name ) const
	{
		return m_resource_ids.find ( name ) != m_resource_ids.end ();
	}
}