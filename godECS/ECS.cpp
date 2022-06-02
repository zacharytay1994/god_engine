#include "pch.h"
#include "ECS.h"

#include <assert.h>

namespace god
{
	ECS::ECS ()
	{
	}

	EntityCollection const& ECS::GetEntities () const
	{
		return m_entities;
	}

	uint32_t ECS::AddEntity ( std::string const& name )
	{
		// create a new entity in entt registry
		return m_entities.PushBack ( { m_registry.create () , name } );
	}

	entt::entity ECS::GetEntity ( uint32_t id )
	{
		return entt::entity ();
	}

	void ECS::RemoveEntity ( uint32_t id )
	{
		assert ( m_entities.Valid ( id ) && "\ngodECS/Entities.cpp - EntityManager::RemoveEntity - Invalid id." );
		// remove entity from registry
		m_registry.destroy ( m_entities[ id ].value ().m_entt_id );
		// remove entity from manager
		m_entities.Remove ( id );
	}
}