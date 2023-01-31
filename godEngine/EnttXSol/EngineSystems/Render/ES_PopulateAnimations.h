#pragma once

#include "../../EngineComponents/EC_All.h"
#include "../../EnttXSol.h"

#include <godUtility/Math.h>

namespace god
{
	void PopulateAnimations ( EnttXSol& entt , EngineResources& engineResources , std::tuple<EntityData& , Transform& , SkeleAnim3D& , ActiveComponent&> components )
	{
		( entt );
		( components );

		auto& [entity_data , transform , skele_anim , active] = components;

		// initialize animation
		if ( !skele_anim.m_initialized )
		{
			auto& opengl = engineResources.Get<OpenGL> ().get ();
			auto [animation , animator] = opengl.AddAnimationInstance ( skele_anim.m_animation );
			skele_anim.m_animation_id = animation;
			skele_anim.m_animator_id = animator;
			skele_anim.m_initialized = true;
		}
		else
		{
			// update animator on opengl side
			auto& opengl = engineResources.Get<OpenGL> ().get ();
			if ( opengl.m_animations.find ( skele_anim.m_animation ) != opengl.m_animations.end () )
			{
				auto& animator = opengl.m_animations[ skele_anim.m_animation ].m_animators[ skele_anim.m_animator_id ];
				if ( skele_anim.m_sub_animations.find ( skele_anim.m_current_sub_animation ) != skele_anim.m_sub_animations.end () )
				{
					auto& [start , end] = skele_anim.m_sub_animations[ skele_anim.m_current_sub_animation ];
					animator.m_startTime = start;
					animator.m_endTime = end;
				}
			}
		}

		// return if not valid
		if ( !entt.m_entities.Valid ( entity_data.m_id ) )
		{
			return;
		}

		Scene& scene = engineResources.Get<Scene> ().get ();

		bool changed { false };
		glm::mat4 parentTransform { 1.0f };
		if ( entt.m_entities[ entity_data.m_id ].m_parent_id != EnttXSol::Entities::Null )
		{
			//changed = entt.m_entities[ entity_data.m_id ].m_parent_changed;
			parentTransform = entt.m_entities[ entity_data.m_id ].m_parent_transform;
			changed = transform.m_parent_transform != parentTransform;
		}

		bool world_changed { false };
		if ( changed )
		{
			transform.m_parent_transform = parentTransform;
			world_changed = true;
		}
		if ( transform.m_changed )
		{
			glm::mat4 model_transform = BuildModelMatrixRotDegrees ( transform.m_position , transform.m_rotation , transform.m_scale );
			transform.m_local_transform = model_transform;
			world_changed = true;
		}
		if ( world_changed )
		{
			transform.m_world_transform = transform.m_parent_transform * transform.m_local_transform;
		}

		// add to scene
		if ( skele_anim.m_animation_id != static_cast< uint32_t >( -1 ) && skele_anim.m_visible )
		{
			scene.AddAnimation ( { skele_anim.m_animation_id ,
				skele_anim.m_diffuse_id , skele_anim.m_specular_id , static_cast< float >( skele_anim.m_animator_id ) , skele_anim.m_emissive } , transform.m_world_transform );
		}

		// populate scene with children
		for ( auto const& child : entt.m_entities[ entity_data.m_id ].m_children )
		{
			//entt.m_entities[ child ].m_parent_changed = world_changed;
			entt.m_entities[ child ].m_parent_transform = transform.m_world_transform;
		}
	}
}