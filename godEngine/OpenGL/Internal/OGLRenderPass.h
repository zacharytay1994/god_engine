#pragma once

#include <glad/glad.h>

namespace god
{
	struct OGLRenderPass
	{
		OGLRenderPass ( unsigned int width , unsigned int height ,
						GLenum internalformat = GL_RGB, GLenum format = GL_RGB, GLenum type = GL_UNSIGNED_BYTE );

		void Free ();
		void Bind ();
		void UnBind ();
		unsigned int GetTexture () const;
		void UpdateWidth ( unsigned int width , unsigned int height );

	private:
		static constexpr uint32_t GL_UNINITIALIZED = static_cast< uint32_t >( -1 );

		unsigned int m_framebuffer { GL_UNINITIALIZED };
		unsigned int m_texture_colour_buffer { GL_UNINITIALIZED };
		unsigned int m_renderbuffer_object { GL_UNINITIALIZED };
	};
}