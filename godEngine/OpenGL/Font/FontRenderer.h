#pragma once

#include "../Internal/OGLShader.h"

namespace god
{
	struct FontRenderer
	{
		FontRenderer () = default;
		void Initialize ();
	private:
		OGLShader m_font_shader;
	};
}