#pragma once

#include <godUtility/Structures.h>
#include <godUtility/Utility.h>
#include "../Window/GLFWWindow.h"

namespace god
{
	/*!
	 * @brief
	 * : A collection of reference wrappers of type.
	 * @tparam ...TYPES
	 * : The types that the are contained within.
	*/
	template <typename...TYPES>
	using EditorResources = god::ReferenceWrapperCollection<TYPES...>;

	using EditorResourcesDef = EditorResources<
		GLFWWindow ,
		Asset3DManager 
	>;
}