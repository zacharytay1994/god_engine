// 2_Engine.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "2_Engine.h"


// This is an example of an exported variable
MY2ENGINE_API int nMy2Engine=0;

// This is an example of an exported function.
MY2ENGINE_API int fnMy2Engine(void)
{
    return 0;
}

// This is the constructor of a class that has been exported.
CMy2Engine::CMy2Engine()
{
    return;
}
