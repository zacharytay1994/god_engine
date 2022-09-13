#pragma once

#include "PxPhysicsAPI.h"
#include "PhysicScene.h"
#include "PhysicSettings.h"
#include "PhysicActor.h"

namespace god
{
	class PhysicsSystem
	{
	public:
		static void Init();
		static void Shutdown();

		static PhysicScene& getScene() { return m_scene; }

		static void CreateScene();
		static void DestroyScene();

		static void CreateActors(Scene& scene);
		static PhysicActor& CreateActor(Entity_ entity);

		static PhysicSettings& getSettings() { return m_settings; }

	private:
		static PhysicScene m_scene;
		static PhysicSettings m_settings;
	};
	/*PxDefaultAllocator		gAllocator;
	PxDefaultErrorCallback	gErrorCallback;

	PxFoundation* gFoundation = NULL;
	PxPhysics* gPhysics = NULL;

	PxDefaultCpuDispatcher* gDispatcher = NULL;
	PxScene* gScene = NULL;

	PxMaterial* gMaterial = NULL;

	PxPvd* gPvd = NULL;

	PxReal stackZ = 10.0f;*/
}

