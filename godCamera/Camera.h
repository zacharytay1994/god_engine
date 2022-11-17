#pragma once

#include "godCamera.h"
#include <glm/glm/glm.hpp>

namespace god
{
	struct Camera
	{
		// position
		glm::vec3 m_position { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_look_at { 0.0f, 0.0f, -1.0f };
		glm::vec3 m_up { 0.0f, 1.0f, 0.0f };

		float m_yaw { 0.0f };
		float m_pitch { 0.0f };

		// perspective
		float m_field_of_view { 60.0f };
		float m_aspect_ratio { 1.0f };
		float m_near_plane { 0.1f };
		float m_far_plane { 1'000'000.0f };

		GODCAMERA_API Camera ();
		void GODCAMERA_API UpdateAspectRatio ( int width , int height );

		glm::mat4 GODCAMERA_API GetPerpectiveProjectionMatrix ();
		glm::mat4 GODCAMERA_API GetOrthographicProjectionMatrix ( float width , float height );
		glm::mat4 GODCAMERA_API GetCameraViewMatrix ();
		glm::mat4 GODCAMERA_API GetCameraViewFaceCamera ();

		// camera modes
		// free camera
		float m_free_camera_speed { 1.0f };
		float m_free_camera_sensitivity { 0.3f };
		float m_free_camera_scroll_sensitivity { 1000.0f };
		float m_free_camera_zoom { 1.0f };
		float m_free_camera_zoom_sensitivity { 0.9f };
		void GODCAMERA_API FreeCamera (
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
			bool zoomOut
		);

		void GODCAMERA_API SceneCamera (
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
			bool zoomOut );

	private:
		float m_last_mouse_x { 0.0f };
		float m_last_mouse_y { 0.0f };
	};
}