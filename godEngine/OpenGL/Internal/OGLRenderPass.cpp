#include "../../pch.h"
#include "OGLRenderPass.h"

#include <glad/glad.h>
#pragma comment (lib, "opengl32.lib")

#include <iostream>

namespace god
{
	OGLRenderPass::OGLRenderPass ( unsigned int width , unsigned int height )
	{
		// create framebuffer object
		glGenFramebuffers ( 1 , &m_framebuffer );
		glBindFramebuffer ( GL_FRAMEBUFFER , m_framebuffer );

		// create texture
		glGenTextures ( 1 , &m_texture_colour_buffer );
		glBindTexture ( GL_TEXTURE_2D , m_texture_colour_buffer );
		glTexImage2D ( GL_TEXTURE_2D , 0 , GL_RGB , width , height , 0 , GL_RGB , GL_UNSIGNED_BYTE , NULL );
		glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_LINEAR );
		glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_LINEAR );
		glBindTexture ( GL_TEXTURE_2D , 0 );

		// attach to currently bound framebuffer object
		glFramebufferTexture2D ( GL_FRAMEBUFFER , GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D , m_texture_colour_buffer , 0 );

		// create renderbuffer object
		glGenRenderbuffers ( 1 , &m_renderbuffer_object );
		glBindRenderbuffer ( GL_RENDERBUFFER , m_renderbuffer_object );
		glRenderbufferStorage ( GL_RENDERBUFFER , GL_DEPTH24_STENCIL8 , width , height );
		glBindRenderbuffer ( GL_RENDERBUFFER , 0 );

		// attach to currently bound framebuffer object
		glFramebufferRenderbuffer ( GL_FRAMEBUFFER , GL_DEPTH_STENCIL_ATTACHMENT , GL_RENDERBUFFER , m_renderbuffer_object );

		// check if success
		if ( glCheckFramebufferStatus ( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
			std::cerr << "RenderPass::OGLRenderPass - Framebuffer is not complete!" << std::endl;
		glBindFramebuffer ( GL_FRAMEBUFFER , 0 );
	}

	void OGLRenderPass::Free ()
	{
		glDeleteFramebuffers ( 1 , &m_framebuffer );
		glDeleteTextures ( 1 , &m_texture_colour_buffer );
		glDeleteRenderbuffers ( 1 , &m_renderbuffer_object );
	}

	void OGLRenderPass::Bind ()
	{
		glBindFramebuffer ( GL_FRAMEBUFFER , m_framebuffer );
		glClearColor ( 0.1f , 0.1f , 0.1f , 1.0f );
		glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glEnable ( GL_DEPTH_TEST );
	}

	void OGLRenderPass::UnBind ()
	{
		glBindFramebuffer ( GL_FRAMEBUFFER , 0 );
	}

	unsigned int OGLRenderPass::GetTexture ()
	{
		return m_texture_colour_buffer;
	}

	void OGLRenderPass::UpdateWidth ( unsigned int width , unsigned int height )
	{
		Free ();
		*this = OGLRenderPass ( width , height );
	}
}