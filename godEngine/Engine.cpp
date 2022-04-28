#include "pch.h"
#include "Engine.h"

#include <godWindow/Window.h>
#include <godOpenGL/OpenGL.h>

namespace god
{
	GODENGINE_API godEngine::godEngine ()
	{
		std::cout << "godEngine constructed." << std::endl;
	}

	void godEngine::Update ()
	{
		// create window
		god::Window window ( 300 , 300 );
		god::OpenGL opengl ( window.GetWindowHandle () , window.GetWindowWidth () , window.GetWindowHeight () );

		// setup scene
		OGLEntityID c1 = opengl.AddCube ( { 0.0f,0.0f,-10.0f } );
		OGLEntityID c2 = opengl.AddCube ( { 0.5f,0.5f,-4.0f } );

		while ( !window.WindowShouldClose () )
		{
			// start frame also resizes the viewport if window size changes
			opengl.FrameBegin ( window.Resized () , window.GetWindowWidth () , window.GetWindowHeight () );

			// ...
			OGLRenderData& cube1 = opengl.GetCube ( c1 );
			OGLRenderData& cube2 = opengl.GetCube ( c2 );

			cube1.m_rotation.x += 0.0002f;
			cube1.m_rotation.y += 0.0002f;
			cube2.m_rotation.x += 0.0002f;
			cube2.m_rotation.y += 0.0002f;

			opengl.FrameRender ();

			window.PollEvents ();
			opengl.FrameEnd ();
		}
	}
}