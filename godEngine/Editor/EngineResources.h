#pragma once

#include <godUtility/Structures.h>
#include <godUtility/Utility.h>
#include <godUtility/Grid3D.h>
#include <godUtility/Scene.h>
#include <godCamera/Camera.h>
#include "../Window/GLFWWindow.h"
#include "../OpenGL/OpenGL.h"
#include "../OpenGL/Internal/OGLTexture.h"
#include "../OpenGL/Font/Font.h"
#include "../Audio/Internal/SoundManager.h"
#include "../Physics/godPhysics.h"
#include "../Physics/PhysicsController.h"

#define __PHYSICS

namespace god
{

	/*!
	 * @brief
	 * : A collection of reference wrappers of type.
	 * @tparam ...TYPES
	 * : The types that the are contained within.
	*/
	template <typename...TYPES>
	using Resources = god::ReferenceWrapperCollection<TYPES...>;

	using EngineResources = Resources<
		GLFWWindow ,
		OpenGL ,
		Camera ,
		Asset3DManager ,
		OGLTextureManager ,
		EntityGrid ,
		SoundManager ,
		PhysicsSystem ,
		PX::PhysicsController ,
		Scene ,
		Fonts
	>;
}
