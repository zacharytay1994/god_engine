#pragma once
#include <string>

namespace god
{
	struct OGLShadowMap
	{
		void Initialize( unsigned int shadowMapWidth = 2048,
						 unsigned int shadowMapHeight = 2048 );
		void EnableDepthMap();
		void DisableDepthMap();
		void Bind( uint32_t location );
	private:
		static constexpr uint32_t GL_UNINITIALIZED = static_cast< uint32_t >( -1 );

		unsigned int m_depthmap_fbo{ GL_UNINITIALIZED };
		unsigned int m_depthmap{ GL_UNINITIALIZED };
		unsigned int m_shadow_width{ GL_UNINITIALIZED };
		unsigned int m_shadow_height{ GL_UNINITIALIZED };
	};
}