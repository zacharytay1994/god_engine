#include "pch.h"
#include "Engine.h"

#include <godOpenGL/OpenGL.h>
#include <godWindow/GLFWWindow.h>
#include <godCamera/Camera.h>

namespace god
{
	GODENGINE_API godEngine::godEngine ()
	{
		std::cout << "godEngine constructed." << std::endl;
	}

	void godEngine::Update ()
	{
		// create window
		god::GLFWWindow window ( 800 , 450 );
		god::OpenGL opengl ( window.GetWindowHandle () , window.GetWindowWidth () , window.GetWindowHeight () );

		// setup camera
		god::Camera camera;
		camera.UpdateAspectRatio ( window.GetWindowWidth () , window.GetWindowHeight () );

		// setup scene
		OGLEntityID c1 = opengl.AddCube ( { 0.0f,0.0f,-10.0f } );
		OGLEntityID c2 = opengl.AddCube ( { 0.5f,0.5f,-4.0f } );

		while ( !window.WindowShouldClose () )
		{
			window.PollEvents ();
			window.SwapWindowBuffers ();

			// window resize changes
			if ( window.Resized () )
			{
				opengl.ResizeViewport ( window.GetWindowWidth () , window.GetWindowHeight () );
				camera.UpdateAspectRatio ( window.GetWindowWidth () , window.GetWindowHeight () );
			}

			opengl.ClearColour ();

			// ...
			OGLRenderData& cube1 = opengl.GetCube ( c1 );
			OGLRenderData& cube2 = opengl.GetCube ( c2 );

			cube1.m_rotation.x += 0.0002f;
			cube1.m_rotation.y += 0.0002f;
			cube2.m_rotation.x += 0.0002f;
			cube2.m_rotation.y += 0.0002f;

			opengl.FrameRender (
				camera.GetPerpectiveProjectionMatrix () ,
				camera.GetCameraViewMatrix () ,
				camera.m_position
			);

			// test camera movement
			camera.FreeCamera ( 0.0002f ,
				true ,
				window.KeyDown ( GLFW_KEY_W ) ,
				window.KeyDown ( GLFW_KEY_S ) ,
				window.KeyDown ( GLFW_KEY_A ) ,
				window.KeyDown ( GLFW_KEY_D ) ,
				window.KeyDown ( GLFW_KEY_SPACE ) ,
				window.KeyDown ( GLFW_KEY_LEFT_SHIFT ) ,
				window.MouseRDown () ,
				static_cast< float >( window.MouseX () ) ,
				static_cast< float >( window.MouseY () ) ,
				window.MouseRDown () ,
				window.MouseScrollUp () ,
				window.MouseScrollDown () ,
				window.KeyDown ( GLFW_KEY_LEFT_CONTROL ) ,
				window.MouseScrollUp () ,
				window.MouseScrollDown () );
		}
	}
}