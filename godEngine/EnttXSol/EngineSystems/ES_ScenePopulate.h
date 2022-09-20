#pragma once

#include "../EngineComponents/EC_All.h"
#include "../EnttXSol.h"

namespace god
{
	void ScenePopulateStepOne ( EnttXSol& entt , EngineResources& engineResources , std::tuple<Transform&> components )
	{
		( entt );
		( components );
		( engineResources );
		// some code here ...
		//std::cout << "Start Frame" << std::endl;
		auto& transform = std::get<0> ( components );
		glm::mat4 model_transform = BuildModelMatrixRotDegrees ( transform.m_position , transform.m_rotation , transform.m_scale );
		transform.m_local_transform = model_transform;
	}

	void ScenePopulateStepTwo ( EnttXSol& entt , EngineResources& engineResources , std::tuple<EntityData& , Transform& , Renderable3D&> components )
	{
		( entt );
		( components );
		( engineResources );
		// some code here ...
		//std::cout << "Start Frame" << std::endl;
		auto& entity_data = std::get<0> ( components );
		auto& transform = std::get<1> ( components );
		auto& renderable = std::get<2> ( components );
		if ( entity_data.m_parent_id != -1 )
		{
			Transform* parent_transform = entt.GetEngineComponent<Transform> ( entity_data.m_parent_id );
			if ( parent_transform )
			{
				transform.m_parent_transform = parent_transform->m_parent_transform * parent_transform->m_local_transform;
			}
		}

		if ( renderable.m_model_id != -1 )
		{
			auto& scene = engineResources.Get<Scene> ().get ();
			auto& object = scene.GetSceneObject ( scene.AddSceneObject ( renderable.m_model_id , transform.m_parent_transform * transform.m_local_transform ) );
			object.m_diffuse_id = renderable.m_diffuse_id;
			object.m_specular_id = renderable.m_specular_id;
			object.m_shininess = renderable.m_shininess;
		}
	}
}