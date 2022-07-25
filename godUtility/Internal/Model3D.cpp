#include "Model3D.h"

#ifdef _DEBUG
#pragma comment (lib, "assimp-vc142-mtd.lib")
#else
#pragma comment (lib, "assimp-vc142-mt.lib")
#endif

#include <fstream>
#include <stdexcept>
#include <unordered_map>
#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace god
{
	Vertex3D Vertex3D::operator+( Vertex3D const& rhs )
	{
		return {
			m_position + rhs.m_position,
			m_uv + rhs.m_uv,
			m_tangent + rhs.m_tangent,
			m_normal + rhs.m_normal,
			m_colour + rhs.m_colour
		};
	}

	Mesh3D ProcessMesh ( aiMesh* mesh , aiScene const* scene );
	std::vector<Material::Texture> LoadMaterialTextures ( aiMaterial* mat , aiTextureType type , std::string typeName );

	Model3D::Model3D ( std::string const& modelFile, bool flipUVs )
	{
		Assimp::Importer importer;

		importer.SetPropertyBool ( AI_CONFIG_PP_PTV_NORMALIZE , true );
		auto process_flags = aiProcess_Triangulate | aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace;
		if ( flipUVs )
		{
			process_flags |= aiProcess_FlipUVs;
		}

		// read into scene
		const aiScene* scene = importer.ReadFile ( modelFile.c_str () , process_flags );
		if ( !scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode )
		{
			throw std::runtime_error ( importer.GetErrorString () );
		}

		ProcessNode ( scene->mRootNode , scene , m_meshes );
	}

	bool Model3D::LoadFromFile ( std::string const& modelFile , bool flipUVs )
	{
		Assimp::Importer importer;

		importer.SetPropertyBool ( AI_CONFIG_PP_PTV_NORMALIZE , true );
		auto process_flags = aiProcess_Triangulate | aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace;
		if ( flipUVs )
		{
			process_flags |= aiProcess_FlipUVs;
		}

		// read into scene
		const aiScene* scene = importer.ReadFile ( modelFile.c_str () , process_flags );
		if ( !scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode )
		{
			std::cerr << "Asset3D:: - " << modelFile << " is unsupported." << std::endl;
			std::cerr << importer.GetErrorString () << std::endl;
			return false;
		}

		ProcessNode ( scene->mRootNode , scene , m_meshes );
		return true;
	}

	std::vector<char> Model3D::ReadFile ( std::string const& filename )
	{
		std::ifstream file ( filename , std::ios::ate | std::ios::binary ); //start from end of file

		if ( !file.is_open () )
		{
			throw std::runtime_error ( "Failed to open file!" );
		}

		size_t fileSize = ( size_t ) file.tellg ();
		std::vector<char> buffer ( fileSize );

		file.seekg ( 0 );
		file.read ( buffer.data () , fileSize );

		file.close ();
		return buffer;
	}

	void Model3D::ProcessNode ( aiNode* node , const aiScene* scene , std::vector<Mesh3D>& meshes )
	{
		// process all the node's meshes (if any)
		for ( unsigned int i = 0; i < node->mNumMeshes; i++ )
		{
			aiMesh* mesh = scene->mMeshes[ node->mMeshes[ i ] ];
			meshes.push_back ( ProcessMesh ( mesh , scene ) );
		}
		// then do the same for each of its children
		for ( unsigned int i = 0; i < node->mNumChildren; i++ )
		{
			ProcessNode ( node->mChildren[ i ] , scene , meshes );
		}
	}

	Mesh3D ProcessMesh ( aiMesh* mesh , aiScene const* scene )
	{
		std::vector<Vertex3D> vertices;
		std::vector<unsigned int> indices;

		for ( unsigned int i = 0; i < mesh->mNumVertices; i++ )
		{
			Vertex3D vertex;

			glm::vec3 vector;
			vector.x = mesh->mVertices[ i ].x;
			vector.y = mesh->mVertices[ i ].y;
			vector.z = mesh->mVertices[ i ].z;
			vertex.m_position = vector;

			if ( mesh->mNormals )
			{
				vertex.m_normal.x = mesh->mNormals[ i ].x;
				vertex.m_normal.y = mesh->mNormals[ i ].y;
				vertex.m_normal.z = mesh->mNormals[ i ].z;
			}

			if ( mesh->mTangents )
			{
				vertex.m_tangent.x = mesh->mTangents[ i ].x;
				vertex.m_tangent.y = mesh->mTangents[ i ].y;
				vertex.m_tangent.z = mesh->mTangents[ i ].z;
			}

			if ( mesh->mColors[ 0 ] )
			{
				vertex.m_colour.r = mesh->mColors[ i ]->r;
				vertex.m_colour.g = mesh->mColors[ i ]->g;
				vertex.m_colour.b = mesh->mColors[ i ]->b;
				vertex.m_colour.a = mesh->mColors[ i ]->a;
			}
			else
				vertex.m_colour = glm::vec4 ( 1.0f );

			if ( mesh->mTextureCoords[ 0 ] ) // does the mesh contain texture coordinates?
			{
				glm::vec2 vec;
				vec.x = mesh->mTextureCoords[ 0 ][ i ].x;
				vec.y = mesh->mTextureCoords[ 0 ][ i ].y;
				vertex.m_uv = vec;
			}
			else
				vertex.m_uv = glm::vec2 ( 0.0f , 0.0f );

			if ( mesh->mMaterialIndex >= 0 )
			{
				Material material;
				aiMaterial* ai_material = scene->mMaterials[ mesh->mMaterialIndex ];

				//unused for now
				material.m_diffuse_maps = LoadMaterialTextures ( ai_material , aiTextureType_DIFFUSE , "texture_diffuse" );
				material.m_specular_maps = LoadMaterialTextures ( ai_material , aiTextureType_SPECULAR , "texture_specular" );
			}

			vertices.push_back ( vertex );
		}

		for ( unsigned int j = 0; j < mesh->mNumFaces; j++ )
		{
			aiFace face = mesh->mFaces[ j ];
			for ( unsigned int k = 0; k < face.mNumIndices; k++ )
			{
				indices.push_back ( face.mIndices[ k ] );
			}

		}

		// if no normals, generate some
		if ( !mesh->mNormals )
		{
			for ( int i = 0; i < indices.size (); i += 3 )
			{
				auto& v1 = vertices[ indices[ i ] ];
				auto& v2 = vertices[ indices[ i + 1 ] ];
				auto& v3 = vertices[ indices[ i + 2 ] ];

				auto edge1 = v1.m_position - v2.m_position;
				auto edge2 = v3.m_position - v2.m_position;

				auto normal = glm::cross ( edge2 , edge1 );
				v1.m_normal = normal;
				v2.m_normal = normal;
				v3.m_normal = normal;
			}
		}

		return { vertices, indices };
	}

	std::vector<Material::Texture> LoadMaterialTextures ( aiMaterial* mat , aiTextureType type , std::string typeName )
	{
		std::unordered_map<std::string , Material::Texture> textureMap;
		std::vector<Material::Texture> textures;

		for ( unsigned int i = 0; i < mat->GetTextureCount ( type ); i++ )
		{
			aiString str;
			mat->GetTexture ( type , i , &str );
			Material::Texture texture;

			if ( textureMap.count ( str.C_Str () ) )
				continue;

			// no load texture with assimp, load with dds instead
			texture.m_id = 0;
			texture.m_type = typeName;
			texture.m_path = str.C_Str ();
			textureMap.insert ( { texture.m_path, texture } );
		}

		for ( auto& entry : textureMap )
		{
			auto& texture = entry.second;
			textures.push_back ( texture );
		}

		return textures;
	}
}