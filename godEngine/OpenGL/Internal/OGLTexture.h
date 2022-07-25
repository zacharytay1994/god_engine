#pragma once

#include <godUtility/Internal/ResourceManager.h>

#include <string>

namespace god
{
	struct OGLTexture
	{
		OGLTexture ( std::string const& texturePath );
		void Free ();

		void Bind ( uint32_t location ) const;
		void UnBind () const;

	private:
		uint32_t m_texture_id;
	};

	using OGLTextureManager = ResourceManager<OGLTexture>;

	void InsertEngineOGLTextures ( OGLTextureManager& manager );
	void InsertAllOGLTexturesFromConfig ( std::string const& configPath , std::string const& assetFolderPath , OGLTextureManager& manager );
}