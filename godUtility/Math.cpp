#include "pch.h"
#include "Math.h"

#pragma warning(disable: 4201)
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#pragma warning(default: 4201)

namespace god
{
	glm::mat4 BuildModelMatrixRotDegrees ( glm::vec3 const& position , glm::vec3 const& rotation , glm::vec3 const& scale )
	{
		glm::mat4 model_transform = glm::mat4 ( 1.0f );
		model_transform = glm::translate ( model_transform , position );
		model_transform = glm::rotate ( model_transform , glm::radians ( rotation.z ) , glm::vec3 ( 0.0f , 0.0f , 1.0f ) );
		model_transform = glm::rotate ( model_transform , glm::radians ( rotation.y ) , glm::vec3 ( 0.0f , 1.0f , 0.0f ) );
		model_transform = glm::rotate ( model_transform , glm::radians ( rotation.x ) , glm::vec3 ( 1.0f , 0.0f , 0.0f ) );
		model_transform = glm::scale ( model_transform , scale );

		return model_transform;
	}
}