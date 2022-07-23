#pragma once

#include <godUtility/Structures.h>
#include <godUtility/Utility.h>
#include "../Window/GLFWWindow.h"
#include "../OpenGL/OpenGL.h"

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
		Asset3DManager
	>;
}