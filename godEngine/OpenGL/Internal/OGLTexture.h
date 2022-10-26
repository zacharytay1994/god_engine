#pragma once

#include <godUtility/Internal/ResourceManager.h>

#include <string>

namespace god
{
	struct OGLTexture
	{
		OGLTexture() = default;
		OGLTexture ( std::string const& texturePath, bool DDS = false );
		void Free ();

		void Bind ( uint32_t location ) const;
		void UnBind ( uint32_t location ) const;

	private:
		uint32_t m_texture_id;
	};

	using OGLTextureManager = ResourceManager<std::tuple<uint32_t , OGLTexture>>;

	void InsertEngineOGLTextures ( OGLTextureManager& manager );
	void InsertAllOGLTexturesFromConfig ( std::string const& configPath , std::string const& assetFolderPath , OGLTextureManager& manager );
}