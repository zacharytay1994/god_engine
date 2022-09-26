#pragma once

#include "../../EngineComponents/EC_All.h"
#include "../../EnttXSol.h"

namespace god
{

	void ShapeUpdate(EnttXSol& entt, EngineResources& engineResources, std::tuple< EntityData&, Transform&, PhysicsMaterial&, PhysicsShape& > component)
	{
		UNREFERENCED_PARAMETER(entt);

		Transform& transform = std::get<1>(component);
		PhysicsMaterial& phymaterial = std::get<2>(component);
		PhysicsShape& physhape = std::get<3>(component);

		physx::PxPhysics* mPhysics = engineResources.Get<PhysicsSystem>().get().GetPhysics();
		//std::map < std::string, physx::PxMaterial*> & materialcont = engineResources.Get<PhysicsSystem>().get().GetMaterialContainer();

		//Update Physics Material Properties
		if (phymaterial.updatematerial)
		{
			if (phymaterial.p_material == nullptr)
			{
				phymaterial.p_material = mPhysics->createMaterial(phymaterial.StaticFriction, phymaterial.DynamicFriction, phymaterial.Restitution);
			}
			else
			{
				phymaterial.p_material->setDynamicFriction(phymaterial.DynamicFriction);
				phymaterial.p_material->setStaticFriction(phymaterial.StaticFriction);
				phymaterial.p_material->setRestitution(phymaterial.Restitution);
			}
			std::cout << " Material Updated\n";
			phymaterial.updatematerial = false;

		}

		//Update Physics Material Properties
		if (physhape.updatePhysicsShape)
		{
			if (physhape.p_shape == nullptr)
			{
				//exclusive shape (can be modified)
				physhape.p_shape = mPhysics->createShape(physx::PxBoxGeometry(physhape.extents.x, physhape.extents.y, physhape.extents.z), *phymaterial.p_material, true);
			}
			else
			{
				physhape.p_shape->setGeometry(physx::PxBoxGeometry(physhape.extents.x, physhape.extents.y, physhape.extents.z));
				physhape.p_shape->setMaterials(&phymaterial.p_material, 1);
			}
			std::cout << " Shape Updated\n";
			physhape.updatePhysicsShape = false;
		}

		if (physhape.locktoscale)
		{
			physhape.extents = transform.m_scale;
			physhape.p_shape->setGeometry(physx::PxBoxGeometry(physhape.extents.x, physhape.extents.y, physhape.extents.z));
		}
	}




}


