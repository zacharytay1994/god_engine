#include <iostream>

#include <godEngine/godEngine.h>

// Memory leak detection
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

int main ()
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	_CrtSetReportMode ( _CRT_WARN , _CRTDBG_MODE_DEBUG );
#endif

	std::cout << "Game entry point!" << std::endl;

	god::godEngine engine;

	engine.Update ();
}