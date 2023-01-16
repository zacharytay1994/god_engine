#include "pch.h"
#include "Camera.h"

#include <algorithm>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <math.h>

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

	glm::mat4 Camera::GetOrthographicProjectionMatrix ( float width , float height )
	{
		return glm::ortho ( 0.0f , width , 0.0f , height , 0.01f , 1000.0f );
	}

	glm::mat4 Camera::GetCameraViewMatrix ()
	{
		if ( m_free_camera_active )
		{
			return glm::lookAt ( m_position , m_position + m_look_at , m_up );
		}
		return glm::lookAt ( m_position , m_look_at , m_up );
	}

	float Camera::GetCameraFaceAngle ()
	{
		glm::mat4 view { GetCameraViewMatrix () };
		// camera rotation relative to origin
		glm::vec3 norm_position = glm::normalize ( glm::vec3 ( m_position.x , 0.0f , m_position.z ) );
		if ( m_position.x < 0.0f )
		{
			return 180.0f + glm::degrees ( acos ( -norm_position.z ) );
		}
		return glm::degrees ( acos ( norm_position.z ) );
	}

	void Camera::SetPosition ( glm::vec3 const& position )
	{
		m_position = position;
		SetNextPosition ( position );
	}

	void Camera::SetLookAt ( glm::vec3 const& lookAt )
	{
		m_look_at = lookAt;
		SetNextLookAt ( lookAt );
	}

	void Camera::SetNextPosition ( glm::vec3 const& position )
	{
		m_next_position = position;
	}

	void Camera::SetNextLookAt ( glm::vec3 const& lookat )
	{
		m_next_look_at = lookat;
	}

	void Camera::InterpolateCameraState ( float dt )
	{
		if ( m_free_camera_active )
		{
			return;
		}

		// interpolate distance
		glm::vec3 curr_to_next_position = m_next_position - m_position;
		float distance = glm::length ( curr_to_next_position );
		if ( distance > m_epsilon )
		{
			m_position.x = std::lerp ( m_position.x , m_next_position.x , dt * m_camera_move_speed );
			m_position.y = std::lerp ( m_position.y , m_next_position.y , dt * m_camera_move_speed );
			m_position.z = std::lerp ( m_position.z , m_next_position.z , dt * m_camera_move_speed );
		}

		// interpolate look at
		glm::vec3 curr_to_next_lookat = m_next_look_at - m_look_at;
		float pan_distance = glm::length ( curr_to_next_lookat );
		if ( pan_distance > m_epsilon )
		{
			m_look_at.x = std::lerp ( m_look_at.x , m_next_look_at.x , dt * m_camera_pan_speed );
			m_look_at.y = std::lerp ( m_look_at.y , m_next_look_at.y , dt * m_camera_pan_speed );
			m_look_at.z = std::lerp ( m_look_at.z , m_next_look_at.z , dt * m_camera_pan_speed );
		}
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
		if ( !m_free_camera_active )
		{
			return;
		}

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

	void Camera::SceneCamera (
		bool unlockAll ,
		float cameraMovementSpeed ,
		float sensitivity ,
		glm::vec3& positionOffset ,
		bool unlockMovement ,
		bool unlockOrientation ,
		float mouseX ,
		float mouseY ,
		float& zoomDistance ,
		bool zoomIn ,
		bool zoomOut )
	{
		if ( unlockAll )
		{
			if ( m_last_mouse_x > 0.0f )
			{
				float x_offset = ( mouseX - m_last_mouse_x ) * m_free_camera_sensitivity;;
				m_last_mouse_x = mouseX;
				if ( unlockMovement )
				{
					glm::vec3 direction = glm::normalize ( glm::cross ( m_look_at , { 0,1,0 } ) );
					positionOffset -= direction * cameraMovementSpeed * x_offset;
					//m_position.x -= cameraMovementSpeed * x_offset;
				}
				if ( unlockOrientation )
				{
					m_yaw += x_offset;
				}
			}
			else
			{
				m_last_mouse_x = mouseX;
			}

			if ( m_last_mouse_y > 0.0f )
			{
				float y_offset = ( mouseY - m_last_mouse_y ) * m_free_camera_sensitivity;
				m_last_mouse_y = mouseY;
				if ( unlockMovement )
				{
					glm::vec3 direction = glm::normalize ( glm::vec3 ( m_look_at.x , 0 , m_look_at.z ) );
					if ( m_position.y < 0.0f )
					{
						positionOffset -= direction * cameraMovementSpeed * y_offset;
					}
					else
					{
						positionOffset += direction * cameraMovementSpeed * y_offset;
					}
					//m_position.z -= cameraMovementSpeed * y_offset;
				}
				if ( unlockOrientation )
				{
					m_pitch -= y_offset;
					m_pitch = std::clamp ( m_pitch , -89.0f , 89.0f );
				}
			}
			else
			{
				m_last_mouse_y = mouseY;
			}

			if ( zoomIn )
			{
				zoomDistance *= sensitivity;
			}
			if ( zoomOut )
			{
				zoomDistance *= 1.0f + ( 1.0f - sensitivity );
			}
		}

		glm::vec3 direction;
		direction.x = cos ( glm::radians ( -90 + m_yaw ) ) * cos ( glm::radians ( m_pitch ) );
		direction.y = sin ( glm::radians ( m_pitch ) );
		direction.z = sin ( glm::radians ( -90 + m_yaw ) ) * cos ( glm::radians ( m_pitch ) );
		m_look_at = glm::normalize ( direction );
		direction *= zoomDistance;
		m_position = -direction + positionOffset;
	}
}