#pragma once

#include "../../EngineComponents/EC_All.h"
#include "../../EnttXSol.h"
#include "../../../OpenGL/OpenGL.h"


namespace god
{


	/**
	 *	Only Draws the respective debug shapes
	 * 
	 * \param entt
	 * \param engineResources
	 * \param component
	 */
	void DebugDynamic(EnttXSol& entt, EngineResources& engineResources, std::tuple< EntityData&, Transform&, RigidDynamic& > component)
	{
		( entt );
		PhysicsSystem& psystem = engineResources.Get<PhysicsSystem>().get();
		RigidDynamic& rigiddynamic = std::get<2>(component);
		//EntityData& edata = std::get<0>(component);

		if (!psystem.debugdraw)
			return;
		Transform& transform = std::get<1>(component);
		//std::cout << "DebugDynamic\n";


		switch (rigiddynamic.Shapeid)
		{
		case PhysicsTypes::Cube:
			glm::vec3 scale = glm::vec3{ 1.f,1.f,1.f };
			glm::vec3 scale2 = glm::vec3{ 2.f,2.f,2.f };

			glm::vec3 min{ glm::vec3{0.f,0.f,0.f} - scale };
			glm::vec3 max{ glm::vec3{0.f,0.f,0.f} + scale };

			glm::vec3 pt[6];
			pt[0] = min;
			pt[1] = min;
			pt[2] = min;

			pt[0].x = min.x + scale2.x;
			pt[1].z = min.z + scale2.z;
			pt[2].x = pt[0].x;
			pt[2].z = pt[1].z;

			pt[3] = max;
			pt[4] = max;
			pt[5] = max;

			pt[3].x = max.x - scale2.x;
			pt[4].z = max.z - scale2.z;
			pt[5] = pt[3];
			pt[5].z -= scale2.z;

			//parent * local if got parent
			for (auto& vert : pt)
			{
				vert = transform.m_local_transform * glm::vec4{ vert,1.f };
			}
			min = transform.m_local_transform * glm::vec4{ min,1.f };
			max = transform.m_local_transform * glm::vec4{ max,1.f };

			OpenGL::DrawLine(min, pt[0]);
			OpenGL::DrawLine(min, pt[1]);
			OpenGL::DrawLine(pt[2], pt[0]);
			OpenGL::DrawLine(pt[2], pt[1]);
			OpenGL::DrawLine(max, pt[3]);

			OpenGL::DrawLine(max, pt[4]);

			OpenGL::DrawLine(pt[3], pt[5]);
			OpenGL::DrawLine(pt[4], pt[5]);

			OpenGL::DrawLine(pt[1], pt[3]);
			OpenGL::DrawLine(pt[2], max);
			OpenGL::DrawLine(pt[4], pt[0]);
			OpenGL::DrawLine(min, pt[5]);
			break;
		case PhysicsTypes::Sphere:
		{
			float radius = rigiddynamic.extents.x;
			glm::vec3 center = transform.m_local_transform * glm::vec4(transform.m_position, 1.0);

			// Instead of drawing individual lines, we will draw a circle
			// around the sphere with a certain number of vertices
			const int NUM_VERTICES = 20;
			glm::vec3 last_point = center + radius * glm::vec3(cos(0), sin(0), 0);
			for (int i = 1; i <= NUM_VERTICES; i++)
			{
				float theta = glm::radians((float)(i * 360) / NUM_VERTICES);
				glm::vec3 next_point = center + radius * glm::vec3(cos(theta), sin(theta), 0);
				OpenGL::DrawLine(last_point, next_point);
				last_point = next_point;
			}
			break;
		}



		case PhysicsTypes::Capsule:



			break;
		}



	}

	void DebugStatic(EnttXSol& entt, EngineResources& engineResources, std::tuple< EntityData&, Transform&, RigidStatic& > component)
	{
		( entt );
		PhysicsSystem& psystem = engineResources.Get<PhysicsSystem>().get();
		if (!psystem.debugdraw)
			return;

		Transform& transform = std::get<1>(component);
	
		glm::vec3 scale = glm::vec3{ 1.f,1.f,1.f };
		glm::vec3 scale2 = glm::vec3{ 2.f,2.f,2.f };

		glm::vec3 min{ glm::vec3{0.f,0.f,0.f} - scale };
		glm::vec3 max{ glm::vec3{0.f,0.f,0.f} + scale };

		glm::vec3 pt[6];
		pt[0] = min;
		pt[1] = min;
		pt[2] = min;

		pt[0].x = min.x + scale2.x;
		pt[1].z = min.z + scale2.z;
		pt[2].x = pt[0].x;
		pt[2].z = pt[1].z;

		pt[3] = max;
		pt[4] = max;
		pt[5] = max;

		pt[3].x = max.x - scale2.x;
		pt[4].z = max.z - scale2.z;
		pt[5] = pt[3];
		pt[5].z -= scale2.z;

		//parent * local if got parent
		for (auto& vert : pt)
		{
			vert = transform.m_local_transform * glm::vec4{ vert,1.f };
		}
		min = transform.m_local_transform * glm::vec4{ min,1.f };
		max = transform.m_local_transform * glm::vec4{ max,1.f };

		OpenGL::DrawLine(min, pt[0]);
		OpenGL::DrawLine(min, pt[1]);
		OpenGL::DrawLine(pt[2], pt[0]);
		OpenGL::DrawLine(pt[2], pt[1]);
		OpenGL::DrawLine(max, pt[3]);

		OpenGL::DrawLine(max, pt[4]);

		OpenGL::DrawLine(pt[3], pt[5]);
		OpenGL::DrawLine(pt[4], pt[5]);

		OpenGL::DrawLine(pt[1], pt[3]);
		OpenGL::DrawLine(pt[2], max);
		OpenGL::DrawLine(pt[4], pt[0]);
		OpenGL::DrawLine(min, pt[5]);

	}

}