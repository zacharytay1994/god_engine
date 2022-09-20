#pragma once
#include <string>

namespace god
{
	struct OGLCubeMap
	{
		void Initialize();
		void CubeTexture( std::string facesCubemap[6] );
		void CubeMapEnableDepth();
		void Bind();
		void Bind( uint32_t location );
		void Draw();
		void UnBind();
		void CubeMapDisableDepth();
		unsigned int GetTexture();

	private:

		static constexpr uint32_t GL_UNINITIALIZED = static_cast< uint32_t >( -1 );

		unsigned int m_skybox_vao{ GL_UNINITIALIZED };
		unsigned int m_skybox_vbo{ GL_UNINITIALIZED };
		unsigned int m_skybox_ebo{ GL_UNINITIALIZED };
		unsigned int m_cubemap_texture{ GL_UNINITIALIZED };
	};
}