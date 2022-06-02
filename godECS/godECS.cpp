// godECS.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "godECS.h"


// This is an example of an exported variable
GODECS_API int ngodECS=0;

// This is an example of an exported function.
GODECS_API int fngodECS(void)
{
    return 0;
}

// This is the constructor of a class that has been exported.
CgodECS::CgodECS()
{
    return;
}
