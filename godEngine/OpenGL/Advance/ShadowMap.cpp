#include "../../pch.h"
#include "ShadowMap.h"

#include <glad/glad.h>
#pragma comment (lib, "opengl32.lib")

namespace god
{
	void OGLShadowMap::Initialize( unsigned int shadowMapWidth,
								   unsigned int shadowMapHeight )
	{
		m_shadow_width = shadowMapWidth;
		m_shadow_height = shadowMapHeight;

		glGenFramebuffers( 1, &m_depthmap_fbo );

		glGenTextures( 1, &m_depthmap );
		glBindTexture( GL_TEXTURE_2D, m_depthmap );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
					  m_shadow_width, m_shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv( GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor );

		glBindFramebuffer( GL_FRAMEBUFFER, m_depthmap_fbo );
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthmap, 0 );
		glDrawBuffer( GL_NONE );
		glReadBuffer( GL_NONE );
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );

		std::cout << "Depthmap Created" << std::endl;
	}

	void OGLShadowMap::EnableDepthMap()
	{
		glCullFace( GL_FRONT );
		glViewport( 0, 0, m_shadow_width, m_shadow_height );
		glBindFramebuffer( GL_FRAMEBUFFER, m_depthmap_fbo );
		glClear( GL_DEPTH_BUFFER_BIT );
	}

	void OGLShadowMap::DisableDepthMap()
	{
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		glUseProgram( 0 );
		glCullFace( GL_BACK );
	}

	void OGLShadowMap::Bind( uint32_t location )
	{
		glActiveTexture( GL_TEXTURE0 + location );
		glBindTexture( GL_TEXTURE_2D, m_depthmap );
	}
}