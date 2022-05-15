#pragma once

#include "../godUtility.h"
#include "Model3D.h"

#include <string>
#include <vector>

namespace god
{
	struct Asset3D
	{
		Model3D m_model;

		GODUTILITY_API Asset3D () = default;
		GODUTILITY_API Asset3D ( std::string const& modelRaw );

		void GODUTILITY_API Serialize ();
		void GODUTILITY_API Deserialize ();

	private:
		std::string m_extension { ".god3d" };

	};
}