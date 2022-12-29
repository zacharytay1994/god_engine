#include "../pch.h"
#include "PhysicsController.h"

namespace god
{
	namespace PX
	{
		PhysicsController::PhysicsController ()
		{
		}

		PhysicsController::~PhysicsController ()
		{
			m_controller_manager->release ();
		}

		void PhysicsController::CreateControllerManager ( physx::PxScene* scene )
		{
			m_controller_manager = PxCreateControllerManager ( *scene );
			std::cout << "Physics Controller - Controller manager created." << std::endl;
		}

		void PhysicsController::FlagFreeControllers ()
		{
			m_free_controllers = true;
		}

		void PhysicsController::FreeControllers ()
		{
			/*if ( m_free_controllers )
			{
				m_controller_manager->purgeControllers ();
				m_free_controllers = false;
			}*/
			m_controller_manager->purgeControllers ();
		}

		physx::PxController* PhysicsController::CreateController ( ControllerShape shape , physx::PxMaterial* material ,
			physx::PxExtendedVec3 const& position , float height , float radius )
		{
			assert ( m_controller_manager != nullptr );

			switch ( shape )
			{
			case god::PX::PhysicsController::ControllerShape::BOX:
			{
				physx::PxBoxControllerDesc description;
				return m_controller_manager->createController ( description );
				break;
			}
			case god::PX::PhysicsController::ControllerShape::CAPSULE:
			{
				physx::PxCapsuleControllerDesc description;
				description.material = material;
				description.radius = radius;
				description.height = height;
				description.position = position;

				assert ( description.isValid () );

				return m_controller_manager->createController ( description );
				break;
			}
			}
			return nullptr;
		}
	}
}