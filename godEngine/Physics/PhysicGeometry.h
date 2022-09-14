#pragma once


#include "PhysicUtils.h"

namespace god
{
	enum class ColliderType
	{
		Box, 
		Sphere, 
		Capsule, 
		ConvexMesh, 
		TriangleMesh
	};

	class ColliderGeometry
	{
		protected:
			ColliderGeometry(ColliderType type)
				: m_type(type), m_material(nullptr) {}

		public:
			virtual ~ColliderGeometry() {}

			void Release() { m_material->release(); }

	protected:
		ColliderType m_type;

		physx::PxMaterial* m_material;
	};
}
