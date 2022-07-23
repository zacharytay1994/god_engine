#include "pch.h"
#include "godUtility.h"
#include "Utility.h"

#include "FileIO.h"
#include "Internal/RapidJSONWrapper.h"

// to bypass windows.h GetObject() macro definition
#ifdef _MSC_VER
#undef GetObject
#endif

namespace god
{
	Asset3D LoadAsset3D ( std::string const& assetPath , bool custom )
	{
		return Asset3D ( assetPath , custom );
	}

	void InsertAsset3DFromPath ( std::string const& assetName , std::string const& assetFolderPath , Asset3DManager& manager )
	{
		manager.Insert ( assetName , LoadAsset3D ( assetFolderPath + assetName , true ) );
	}

	void InsertAllAsset3DsFromConfig ( std::string const& configPath , std::string const& assetFolderPath , Asset3DManager& manager )
	{
		rapidjson::Document m_document_models;
		m_document_models.SetObject ();
		god::ReadJSON ( m_document_models , configPath );
		if ( m_document_models.IsObject () )
		{
			for ( auto& model : m_document_models.GetObject () )
			{
				if ( !manager.Has ( model.name.GetString () ) )
				{
					manager.Insert ( model.name.GetString () , LoadAsset3D ( assetFolderPath + model.name.GetString () , true ) );
				}
			}
		}
	}
}