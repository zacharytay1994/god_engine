#include "pch.h"
#include "Camera.h"

#include <algorithm>
#include <glm/glm/gtc/matrix_transform.hpp>

namespace god
{
	Camera::Camera ()
	{
	}

	void Camera::UpdateAspectRatio ( int width , int height )
	{
		m_aspect_ratio = static_cast< float >( width ) / static_cast< float >( height );
	}

	glm::mat4 Camera::GetPerpectiveProjectionMatrix ()
	{
		return glm::perspective ( glm::radians ( m_field_of_view * m_free_camera_zoom ) , m_aspect_ratio , m_near_plane , m_far_plane );
	}

	glm::mat4 Camera::GetCameraViewMatrix ()
	{
		return glm::lookAt ( m_position , m_position + m_look_at , m_up );
	}

	void Camera::FreeCamera (
		float dt ,
		bool unlockMovement ,
		bool forward ,
		bool backward ,
		bool left ,
		bool right ,
		bool up ,
		bool down ,
		bool unlockRotation ,
		float mouseX ,
		float mouseY ,
		bool unlockLookatMovement ,
		bool lookatForward ,
		bool lookatBackward ,
		bool unlockZoom ,
		bool zoomIn ,
		bool zoomOut )
	{
		glm::vec3 right_direction = glm::normalize ( glm::cross ( m_look_at , m_up ) );
		glm::vec3 forward_direction = glm::normalize ( glm::cross ( m_up , right_direction ) );

		if ( unlockMovement )
		{
			if ( forward )
			{
				m_position += forward_direction * m_free_camera_speed * dt;
			}
			if ( backward )
			{
				m_position -= forward_direction * m_free_camera_speed * dt;
			}
			if ( left )
			{
				m_position -= right_direction * m_free_camera_speed * dt;
			}
			if ( right )
			{
				m_position += right_direction * m_free_camera_speed * dt;
			}
			if ( up )
			{
				m_position.y += m_free_camera_speed * dt;
			}
			if ( down )
			{
				m_position.y -= m_free_camera_speed * dt;
			}
		}
		if ( unlockLookatMovement )
		{
			if ( lookatForward )
			{
				m_position += m_look_at * m_free_camera_scroll_sensitivity * m_free_camera_speed * dt;;
			}
			if ( lookatBackward )
			{
				m_position -= m_look_at * m_free_camera_scroll_sensitivity * m_free_camera_speed * dt;
			}
		}
		if ( unlockZoom )
		{
			if ( zoomIn )
			{
				m_free_camera_zoom = std::max ( 0.0f , m_free_camera_zoom * m_free_camera_zoom_sensitivity );
			}
			if ( zoomOut )
			{
				m_free_camera_zoom = std::min ( 1.0f , m_free_camera_zoom / m_free_camera_zoom_sensitivity );
			}
		}
		if ( unlockRotation )
		{
			if ( m_last_mouse_x > 0.0f )
			{
				float x_offset = ( mouseX - m_last_mouse_x ) * m_free_camera_sensitivity;;
				m_last_mouse_x = mouseX;
				m_yaw += x_offset;
			}
			else
			{
				m_last_mouse_x = mouseX;
			}

			if ( m_last_mouse_y > 0.0f )
			{
				float y_offset = ( mouseY - m_last_mouse_y ) * m_free_camera_sensitivity;
				m_last_mouse_y = mouseY;
				m_pitch -= y_offset;

				m_pitch = std::clamp ( m_pitch , -89.0f , 89.0f );
			}
			else
			{
				m_last_mouse_y = mouseY;
			}
		}
		else
		{
			m_last_mouse_x = 0.0f;
			m_last_mouse_y = 0.0f;
		}
		glm::vec3 direction;
		direction.x = cos ( glm::radians ( -90 + m_yaw ) ) * cos ( glm::radians ( m_pitch ) );
		direction.y = sin ( glm::radians ( m_pitch ) );
		direction.z = sin ( glm::radians ( -90 + m_yaw ) ) * cos ( glm::radians ( m_pitch ) );
		m_look_at = glm::normalize ( direction );
	}
}