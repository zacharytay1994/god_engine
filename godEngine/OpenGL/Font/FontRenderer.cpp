#include "../../pch.h"
#include "FontRenderer.h"

namespace god
{
	void FontRenderer::Initialize ()
	{
		m_font_shader.InitializeFromCode (
			"#version 330 core\n"
			"layout ( location = 0 ) in vec4 vertex; // <vec2 pos, vec2 tex>\n"
			"out vec2 TexCoords;\n"

			"uniform mat4 projection;\n"

			"void main ()\n"
			"{\n"
			"gl_Position = projection * vec4 ( vertex.xy , 0.0 , 1.0 );\n"
			"TexCoords = vertex.zw;\n"
			"}" ,

			"#version 330 core\n"
			"in vec2 TexCoords;\n"
			"out vec4 color;\n"

			"uniform sampler2D text;\n"
			"uniform vec3 textColor;\n"

			"void main ()\n"
			"{\n"
			"vec4 sampled = vec4 ( 1.0 , 1.0 , 1.0 , texture ( text , TexCoords ).r );\n"
			"color = vec4 ( textColor , 1.0 ) * sampled;\n"
			"}  "
		);

		std::cout << "FontRenderer - Initialized" << std::endl;
	}
}