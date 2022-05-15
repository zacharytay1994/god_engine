#pragma once

#include "../godUtility.h"

#include <glm/glm/glm.hpp>

#include <vector>
#include <string>

struct aiNode;
struct aiScene;

namespace god
{
	struct Vertex3D
	{
		glm::vec3 m_position;
		glm::vec2 m_uv;
		glm::vec3 m_tangent;
		glm::vec3 m_normal;
		glm::vec4 m_colour;
	};

	struct Material
	{
		struct Texture
		{
			unsigned int	m_id {};
			std::string		m_type;
			std::string		m_path;
		};

		std::vector<Texture> m_diffuse_maps;
		std::vector<Texture> m_specular_maps;
	};

	struct Mesh3D
	{
		std::vector<Vertex3D>	m_vertices;
		std::vector<uint32_t>	m_indices;
	};

	struct Model3D
	{
		std::vector<Mesh3D> m_meshes;

		GODUTILITY_API Model3D() = default;
		GODUTILITY_API Model3D ( std::string const& modelFile );
	private:
		std::vector<char> ReadFile ( std::string const& filename );
		void ProcessNode ( aiNode* node , const aiScene* scene , std::vector<Mesh3D>& meshes );
	};
}