#include "pch.h"
#include "godUtility.h"
#include "Utility.h"

namespace god
{
	Asset3D LoadAsset3D ( char const* assetPath , bool custom )
	{
		return Asset3D ( assetPath , custom );
	}
}