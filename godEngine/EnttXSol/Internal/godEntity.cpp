#include "../../pch.h"
#include "godEntity.h"

namespace god
{
	entity::entity ( entt::registry& registry , std::string const& name , ContainerID parent )
		:
		m_name { name } ,
		m_parent_id { parent }
	{
		m_id = registry.create ();
	}
}