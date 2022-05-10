#pragma once

#include "TemplateManipulation.h"

#include <functional>

namespace god
{
	template <typename...TYPES>
	using ReferenceWrapperCollection = T_Manip::TypeCollection<std::reference_wrapper , TYPES...>;
}