#pragma once

#include "godEditor.h"

#include "ImGui/imgui.h"

#include <Windows.h>

namespace god
{
	struct Editor
	{
		GODEDITOR_API Editor ( HWND windowHandle );
		GODEDITOR_API ~Editor ();

		void GODEDITOR_API Update ();
	};
}