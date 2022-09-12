#pragma once
#include <string>

namespace god
{
	struct OGLCubeMap
	{
		void Initialize( unsigned int width, unsigned int height );
		void CubeTexture( std::string facesCubemap[6] );
		void CubeMapEnableDepth();
		void CubeMapDisableDepth();
		void Free();
		void Bind();
		void Draw();
		void UnBind();
		unsigned int GetTexture();
		void UpdateWidth( unsigned int width, unsigned int height );

	private:
		static constexpr uint32_t GL_UNINITIALIZED = static_cast< uint32_t >( -1 );

		unsigned int m_skybox_vao{ GL_UNINITIALIZED };
		unsigned int m_skybox_vbo{ GL_UNINITIALIZED };
		unsigned int m_skybox_ebo{ GL_UNINITIALIZED };
		unsigned int m_cubemap_texture{ GL_UNINITIALIZED };
	};
}