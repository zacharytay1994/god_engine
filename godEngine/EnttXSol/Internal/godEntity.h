#pragma once

#include <entt.hpp>
#include <string>

namespace god
{
	struct Entity_
	{
		using EnttID = entt::entity;
		using ContainerID = uint32_t;
		static constexpr uint32_t Null = static_cast< uint32_t >( -1 );

		enum class Type
		{
			Default ,
			Prefab
		};
		Type m_type { Type::Default };

		EnttID m_id { entt::null };
		ContainerID m_parent_id { Null };
		std::string m_name { "" };
		std::vector<ContainerID> m_children;
		bool m_persist_in_scene { true };
		bool m_active { true };
		Entity_ () = default;
		Entity_ ( entt::registry& registry , std::string const& name , ContainerID parent , Type type = Type::Default );

		void Destroy ( entt::registry& registry );
	};
}