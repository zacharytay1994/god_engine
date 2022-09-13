#pragma once
#include <string>

namespace god
{
	struct OGLShadowMap
	{
		void Initialize( unsigned int shadowMapWidth = 2048 , unsigned int shadowMapHeight = 2048 );
		void ShadowMapEnableDepth();
		void Bind( uint32_t location );
		void Draw();
		void UnBind();
		void ShadowMapDisableDepth();
		unsigned int GetTexture();

	private:
		static constexpr uint32_t GL_UNINITIALIZED = static_cast< uint32_t >( -1 );

		unsigned int m_shadowmap_fbo     { GL_UNINITIALIZED };
		unsigned int m_shadowmap_width   { GL_UNINITIALIZED };
		unsigned int m_shadowmap_height  { GL_UNINITIALIZED };
		unsigned int m_shadowmap_texture { GL_UNINITIALIZED };
	};
}