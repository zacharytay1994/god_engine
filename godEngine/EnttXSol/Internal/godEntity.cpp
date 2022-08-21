#include "../../pch.h"
#include "godEntity.h"

namespace god
{
	Entity_::Entity_ ( entt::registry& registry , std::string const& name , ContainerID parent , Type type )
		:
		m_type { type } ,
		m_name { name } ,
		m_parent_id { parent }
	{
		m_id = registry.create ();
		if ( parent == Null )
		{
			m_root = true;
		}
	}

	void Entity_::Destroy ( entt::registry& registry )
	{
		registry.destroy ( m_id );
	}
}