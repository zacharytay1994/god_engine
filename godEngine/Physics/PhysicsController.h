#pragma once

#include "PxPhysicsAPI.h"

namespace god
{
	namespace PX
	{
		struct PhysicsController
		{
			PhysicsController ();
			~PhysicsController ();
			void CreateControllerManager ( physx::PxScene* scene );

			enum class ControllerShape
			{
				BOX ,
				CAPSULE
			};
			physx::PxController* CreateController ( ControllerShape shape , physx::PxMaterial* material ,
				physx::PxExtendedVec3 const& position = { 0,0,0 } , float height = 0.5f , float radius = 0.25f );

		private:
			physx::PxControllerManager* m_controller_manager { nullptr };
		};
	}
}