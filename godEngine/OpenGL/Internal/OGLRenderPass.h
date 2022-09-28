#pragma once

#include <glad/glad.h>

namespace god
{
	template <size_t N = 1>
	struct OGLRenderPass
	{
		OGLRenderPass () = default;
		OGLRenderPass ( unsigned int width , unsigned int height ,
			GLenum internalformat = GL_RGB , GLenum format = GL_RGB , GLenum type = GL_UNSIGNED_BYTE );

		void Initialize ( unsigned int width , unsigned int height ,
			GLenum internalformat = GL_RGB , GLenum format = GL_RGB , GLenum type = GL_UNSIGNED_BYTE );

		void Free ();
		void Bind ();
		void UnBind ();
		unsigned int GetTexture ( uint32_t i = 0 ) const;
		void UpdateWidth ( unsigned int width , unsigned int height );

	private:
		static constexpr uint32_t GL_UNINITIALIZED = static_cast< uint32_t >( -1 );

		unsigned int m_framebuffer { GL_UNINITIALIZED };
		unsigned int m_texture_colour_buffer[ N ] { GL_UNINITIALIZED };
		unsigned int m_renderbuffer_object { GL_UNINITIALIZED };

		GLenum m_internal_format;
		GLenum m_format;
		GLenum m_type;
	};

	template<size_t N>
	inline OGLRenderPass<N>::OGLRenderPass ( unsigned int width , unsigned int height , GLenum internalformat , GLenum format , GLenum type )
		:
		m_internal_format { internalformat } ,
		m_format { format } ,
		m_type { type }
	{
		// create framebuffer object
		glGenFramebuffers ( 1 , &m_framebuffer );
		glBindFramebuffer ( GL_FRAMEBUFFER , m_framebuffer );

		// create texture
		glGenTextures ( N , m_texture_colour_buffer );
		for ( auto i = 0; i < N; ++i )
		{
			glBindTexture ( GL_TEXTURE_2D , m_texture_colour_buffer[ i ] );
			glTexImage2D ( GL_TEXTURE_2D , 0 , internalformat , width , height , 0 , format , type , NULL );
			glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_LINEAR );
			glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_LINEAR );
			glBindTexture ( GL_TEXTURE_2D , 0 );

			// attach to currently bound framebuffer object
			glFramebufferTexture2D ( GL_FRAMEBUFFER , GL_COLOR_ATTACHMENT0 + i , GL_TEXTURE_2D , m_texture_colour_buffer[ i ] , 0 );
		}

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

		if ( N == 2 )
		{
			unsigned int attachments[ 2 ] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
			glDrawBuffers ( 2 , attachments );
		}

		glBindFramebuffer ( GL_FRAMEBUFFER , 0 );
	}

	template<size_t N>
	inline void OGLRenderPass<N>::Initialize ( unsigned int width , unsigned int height , GLenum internalformat , GLenum format , GLenum type )
	{
		m_internal_format = internalformat;
		m_format = format;
		m_type = type;

		// create framebuffer object
		glGenFramebuffers ( 1 , &m_framebuffer );
		glBindFramebuffer ( GL_FRAMEBUFFER , m_framebuffer );

		// create texture
		glGenTextures ( N , m_texture_colour_buffer );
		for ( auto i = 0; i < N; ++i )
		{
			glBindTexture ( GL_TEXTURE_2D , m_texture_colour_buffer[ i ] );
			glTexImage2D ( GL_TEXTURE_2D , 0 , internalformat , width , height , 0 , format , type , NULL );
			glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_LINEAR );
			glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_LINEAR );
			glBindTexture ( GL_TEXTURE_2D , 0 );

			// attach to currently bound framebuffer object
			glFramebufferTexture2D ( GL_FRAMEBUFFER , GL_COLOR_ATTACHMENT0 + i , GL_TEXTURE_2D , m_texture_colour_buffer[ i ] , 0 );
		}

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

		if ( N == 2 )
		{
			unsigned int attachments[ 2 ] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
			glDrawBuffers ( 2 , attachments );
		}

		glBindFramebuffer ( GL_FRAMEBUFFER , 0 );
	}

	template<size_t N>
	inline void OGLRenderPass<N>::Free ()
	{
		glDeleteFramebuffers ( 1 , &m_framebuffer );
		glDeleteTextures ( N , m_texture_colour_buffer );
		glDeleteRenderbuffers ( 1 , &m_renderbuffer_object );
	}

	template<size_t N>
	inline void OGLRenderPass<N>::Bind ()
	{
		glBindFramebuffer ( GL_FRAMEBUFFER , m_framebuffer );
		glClearColor ( 0.1f , 0.1f , 0.1f , 1.0f );
		glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glEnable ( GL_DEPTH_TEST );
	}

	template<size_t N>
	inline void OGLRenderPass<N>::UnBind ()
	{
		glBindFramebuffer ( GL_FRAMEBUFFER , 0 );
	}

	template<size_t N>
	inline unsigned int OGLRenderPass<N>::GetTexture ( uint32_t i ) const
	{
		return m_texture_colour_buffer[ i ];
	}

	template<size_t N>
	inline void OGLRenderPass<N>::UpdateWidth ( unsigned int width , unsigned int height )
	{
		Free ();
		*this = OGLRenderPass<N> ( width , height , m_internal_format , m_format , m_type );
	}
}