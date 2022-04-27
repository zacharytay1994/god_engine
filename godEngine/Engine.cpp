#include "pch.h"
#include "Engine.h"

#include <godWindow/Window.h>

namespace god
{
	GODENGINE_API godEngine::godEngine ()
	{
		std::cout << "godEngine constructed." << std::endl;
	}

	void godEngine::Update ()
	{
		// create window
		god::Window window ( 100 , 100 );

		while ( !window.WindowShouldClose () )
		{
			window.PollEvents ();

			// e.g. input hold down left control + press and release j
			if ( window.KeyIsDown ( VK_LCONTROL ) && window.KeyUp ( 'J' ) )
			{
				std::cout << window.GetWindowWidth();
			}
		}
	}
}