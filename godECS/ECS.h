#pragma once
#include "godECS.h"
#include "entt.hpp"

#include <godUtility/Structures.h>

#include <functional>
#include <string>

namespace god
{
	struct Entity
	{
		entt::entity m_entt_id;
		std::string m_name { "" };
	};
	using EntityCollection = DefaultCollection<Entity>;

	template <typename...Components>
	using System = void( * )( Components&... );

	struct ECS
	{
		GODECS_API ECS ();

		EntityCollection GODECS_API const& GetEntities () const;

		/*
			ENTITY
		*/

		/*!
		 * @brief
		 * : Add an entity to the EntityManager
		 * @return
		 * : Manager id of the entity, note: not the entt::entity id
		*/
		uint32_t GODECS_API AddEntity ( std::string const& name = "" );

		/*!
		 * @brief
		 * : Get the entt entity
		 * @return
		 * : entt::entity
		*/
		entt::entity GODECS_API GetEntity ( uint32_t id );

		/*!
		 * @brief
		 * : Remove the entity from the manager
		 * @param id
		 * : Manager id of the entity
		*/
		void GODECS_API RemoveEntity ( uint32_t id );

		/*
			COMPONENT
		*/
		template <typename Component>
		void AddComponent ( uint32_t id , Component const& component = Component () );

		template <typename Component>
		Component& GetComponent ( uint32_t id );

		/*
			SYSTEM
		*/
		template <typename...Components>
		void RunSystem ( System<Components...> system );

	private:
		entt::registry m_registry;
		EntityCollection m_entities;
	};

	template<typename Component>
	inline void ECS::AddComponent ( uint32_t id , Component const& component )
	{
		assert ( m_entities.Valid ( id ) );
		m_registry.emplace<Component> ( m_entities[ id ].value ().m_entt_id ) = component;
	}

	template<typename Component>
	inline Component& ECS::GetComponent ( uint32_t id )
	{
		assert ( m_entities.Valid ( id ) );
		return m_registry.get<Component> ( m_entities[ id ].value ().m_entt_id );
	}

	template<typename ...Components>
	inline void ECS::RunSystem ( System<Components...> system )
	{
		auto view = m_registry.view<Components...> ();
		view.each ( system );
	}
}