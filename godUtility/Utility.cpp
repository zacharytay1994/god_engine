#include "pch.h"
#include "godUtility.h"
#include "Utility.h"

namespace god
{
	AssimpModel LoadModel ( char const* modelPath )
	{
		return AssimpModel ( modelPath );
	}
}