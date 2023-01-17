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

	glm::vec3 ViewportToWorldRay ( glm::vec2 const& position , uint32_t viewportWidth , uint32_t viewportHeight , glm::mat4 const& perspectiveMatrix , glm::mat4 const& viewMatrix )
	{
		glm::mat4 inv = glm::inverse ( perspectiveMatrix * viewMatrix );
		float halfwidth = static_cast< float >( viewportWidth ) / 2.0f;
		float halfheight = static_cast< float >( viewportHeight ) / 2.0f;
		glm::vec4 n = glm::vec4 (
			( position.x - halfwidth ) / halfwidth ,
			-( position.y - halfheight ) / halfheight ,
			-1.0f ,
			1.0f
		);
		glm::vec4 f = n;
		f.z = 1.0f;
		glm::vec4 near_result = inv * n;
		glm::vec4 far_result = inv * f;
		near_result /= near_result.w;
		far_result /= far_result.w;
		return glm::normalize ( far_result - near_result );
	}

	bool IntersectLineSegmentPlane ( glm::vec3 const& a , glm::vec3 const& b , glm::vec3 const& n , float d , glm::vec3& p )
	{
		glm::vec3 ab = b - a;
		float t = ( d - glm::dot ( n , a ) ) / glm::dot ( n , ab );
		if ( t >= 0.0f && t <= 1.0f )
		{
			p = ( a + t * ab );
			return true;
		}
		return false;
	}

	float LengthSq ( glm::vec3 const& v )
	{
		return v.x * v.x + v.y * v.y + v.z * v.z;
	}

	float HorizontalFaceCameraDegrees ( glm::vec3 const& cameraPosition , glm::vec3 const& targetPosition )
	{
		glm::vec3 targetToCamera = cameraPosition - targetPosition;
		targetToCamera.y = 0;
		glm::vec3 norm_position = glm::normalize ( targetToCamera );
		if ( targetToCamera.x < 0.0f )
		{
			return 180.0f + glm::degrees ( acos ( -norm_position.z ) );
		}
		return glm::degrees ( acos ( norm_position.z ) );
	}

	glm::vec2 RotateVector ( glm::vec2 const& v , float degree )
	{
		degree = glm::radians ( degree );
		return { v.x * cos ( degree ) - v.y * sin ( degree ), v.x * sin ( degree ) + v.y * cos ( degree ) };
	}

	float DegreeBetweenVec2 ( glm::vec2 const& v1 , glm::vec2 const& v2 )
	{
		auto v1_ = glm::normalize ( v1 );
		auto v2_ = glm::normalize ( v2 );
		float degree = glm::degrees ( atan2 ( glm::dot ( v1_ , v2_ ) , ( v1_.x * v2_.y - v1_.y * v2_.x ) ) );
		if ( degree < 0.0f )
		{
			return 360.0f + degree;
		}
		return degree;
	}

	std::tuple<bool , float> RayIntersectAABB ( glm::vec3 const& rayOrigin , glm::vec3 const& rayDirection , glm::vec3 const& aabbMin , glm::vec3 const& aabbMax )
	{
		float tmin { 0.0f };
		float tmax = std::numeric_limits<float>::max ();

		for ( int i = 0; i < 3; ++i )
		{
			float invD = 1.0f / rayDirection[ i ];
			float t0 = ( aabbMin[ i ] - rayOrigin[ i ] ) * invD;
			float t1 = ( aabbMax[ i ] - rayOrigin[ i ] ) * invD;

			if ( invD < 0.0f )
			{
				std::swap ( t0 , t1 );
			}

			tmin = std::max ( tmin , t0 );
			tmax = std::min ( tmax , t1 );

			if ( tmax < tmin )
			{
				return { false,std::numeric_limits<float>::max () };
			}
		}

		return { true,tmin };
	}
}