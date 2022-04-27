// godOpenGL.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "godOpenGL.h"


// This is an example of an exported variable
GODOPENGL_API int ngodOpenGL=0;

// This is an example of an exported function.
GODOPENGL_API int fngodOpenGL(void)
{
    return 0;
}

// This is the constructor of a class that has been exported.
CgodOpenGL::CgodOpenGL()
{
    return;
}
