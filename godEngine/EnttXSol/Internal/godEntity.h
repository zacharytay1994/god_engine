#pragma once

#include <entt.hpp>
#include <string>

namespace god
{
	struct entity
	{
		using EnttID = entt::entity;
		using ContainerID = uint32_t;
		static constexpr uint32_t Null = static_cast< uint32_t >( -1 );

		EnttID m_id { entt::null };
		ContainerID m_parent_id { Null };
		std::string m_name { "" };

		entity () = default;
		entity ( entt::registry& registry , std::string const& name , ContainerID parent );
	};
}