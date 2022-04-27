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
		god::Window window ( 300 , 300 );

		while ( !window.WindowShouldClose () )
		{
			window.PollEvents ();

			// ...
		}
	}
}