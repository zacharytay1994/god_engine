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

		while ( !window.WindowShouldClose () )
		{
			// start frame also resized the viewport 
			opengl.StartFrame ( window.Resized () , window.GetWindowWidth () , window.GetWindowHeight () );

			// ...

			window.PollEvents ();
			opengl.EndFrame ();
		}
	}
}