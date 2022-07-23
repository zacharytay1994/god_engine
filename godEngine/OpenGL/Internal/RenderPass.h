#pragma once

namespace god
{
	struct OGLRenderPass
	{
		OGLRenderPass ( unsigned int width , unsigned int height );

		void Free ();
		void Bind ();
		void UnBind ();
		unsigned int GetTexture ();
		void UpdateWidth ( unsigned int width , unsigned int height );

	private:
		static constexpr uint32_t GL_UNINITIALIZED = static_cast< uint32_t >( -1 );

		unsigned int m_framebuffer { GL_UNINITIALIZED };
		unsigned int m_texture_colour_buffer { GL_UNINITIALIZED };
		unsigned int m_renderbuffer_object { GL_UNINITIALIZED };
	};
}