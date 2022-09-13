#include "../../pch.h"
#include "ShadowMap.h"

#include <glad/glad.h>
#pragma comment (lib, "opengl32.lib")

namespace god
{
	void OGLShadowMap::Initialize( unsigned int shadowMapWidth, unsigned int shadowMapHeight )
	{
		m_shadowmap_height = shadowMapHeight;
		m_shadowmap_width = shadowMapWidth;

		// Framebuffer for Shadow Map
		glGenFramebuffers( 1, &m_shadowmap_fbo );

		// Texture for Shadow Map FBO
		glGenTextures( 1, &m_shadowmap_texture );
		glBindTexture( GL_TEXTURE_2D, m_shadowmap_texture );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );
		// Prevents darkness outside the frustrum
		float clampColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv( GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor );

		glBindFramebuffer( GL_FRAMEBUFFER, m_shadowmap_fbo );
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowmap_texture, 0 );

		// Needed since we don't touch the color buffer
		glDrawBuffer( GL_NONE );
		glReadBuffer( GL_NONE );
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	}
	void OGLShadowMap::ShadowMapEnableDepth()
	{
		// Depth testing needed for Shadow Map
		glEnable( GL_DEPTH_TEST );

		// Preparations for the Shadow Map
		glViewport( 0, 0, m_shadowmap_width, m_shadowmap_height );
		glBindFramebuffer( GL_FRAMEBUFFER, m_shadowmap_fbo );
		glClear( GL_DEPTH_BUFFER_BIT );
	}
	void OGLShadowMap::Bind( uint32_t location )
	{
		// Bind the Shadow Map
		glActiveTexture( GL_TEXTURE0 + location );
		glBindTexture( GL_TEXTURE_2D, m_shadowmap_texture );
	}
	void OGLShadowMap::ShadowMapDisableDepth()
	{}
}