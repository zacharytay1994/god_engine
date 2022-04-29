// godEditor.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "godEditor.h"


// This is an example of an exported variable
GODEDITOR_API int ngodEditor=0;

// This is an example of an exported function.
GODEDITOR_API int fngodEditor(void)
{
    return 0;
}

// This is the constructor of a class that has been exported.
CgodEditor::CgodEditor()
{
    return;
}
