#pragma once

#include "godUtility.h"

#pragma warning(disable: 4201)
#include <glm/glm/glm.hpp>
#pragma warning(default: 4201)

namespace god
{
	GODUTILITY_API glm::mat4 BuildModelMatrixRotDegrees ( glm::vec3 const& position , glm::vec3 const& rotation , glm::vec3 const& scale );

	GODUTILITY_API glm::vec3 ViewportToWorldRay ( glm::vec2 const& position , uint32_t viewportWidth , uint32_t viewportHeight ,
		glm::mat4 const& perspectiveMatrix , glm::mat4 const& viewMatrix );

	GODUTILITY_API bool IntersectLineSegmentPlane ( glm::vec3 const& a , glm::vec3 const& b , glm::vec3 const& n , float d , glm::vec3& p );
}