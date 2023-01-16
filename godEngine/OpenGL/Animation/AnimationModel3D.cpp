#include "AnimationModel3D.h"


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#include <assert.h>

namespace god
{
	namespace Animation3D
	{
		Model::Model( string const& path, bool gamma )
			:
			gammaCorrection( gamma )
		{
			loadModel( path );
		}

		// draws the model, and thus all its meshes
		void Model::Draw( OGLShader& shader )
		{
			for ( unsigned int i = 0; i < meshes.size(); i++ )
				meshes[i].Draw( shader );
		}

		std::map<string, BoneInfo>& Model::GetBoneInfoMap()
		{
			return m_BoneInfoMap;
		}

		int& Model::GetBoneCount()
		{
			return m_BoneCounter;
		}

		// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
		void Model::loadModel( string const& path )
		{
			// read file via ASSIMP
			Assimp::Importer importer;
			//const aiScene* scene = importer.ReadFile( path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace );

			const aiScene* scene = importer.ReadFile( path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs ); //--

			// check for errors
			if ( !scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode ) // if is Not Zero
			{
				cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
				return;
			}
			// retrieve the directory path of the filepath
			directory = path.substr( 0, path.find_last_of( '/' ) );

			// process ASSIMP's root node recursively
			processNode( scene->mRootNode, scene );
		}

		// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
		void Model::processNode( aiNode* node, const aiScene* scene )
		{
			// process each mesh located at the current node
			for ( unsigned int i = 0; i < node->mNumMeshes; i++ )
			{
				// the node object only contains indices to index the actual objects in the scene. 
				// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				meshes.push_back( processMesh( mesh, scene ) );
			}
			// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
			for ( unsigned int i = 0; i < node->mNumChildren; i++ )
			{
				processNode( node->mChildren[i], scene );
			}

		}

		void Model::SetVertexBoneDataToDefault( Vertex& vertex )
		{
			for ( int i = 0; i < MAX_BONE_INFLUENCE; i++ )
			{
				vertex.m_BoneIDs[i] = -1;
				vertex.m_Weights[i] = 0.0f;
			}
		}


		Mesh Model::processMesh( aiMesh* mesh, const aiScene* scene )
		{
			vector<Vertex> vertices;
			vector<unsigned int> indices;
			vector<Texture> textures;

			for ( unsigned int i = 0; i < mesh->mNumVertices; i++ )
			{
				Vertex vertex;
				SetVertexBoneDataToDefault( vertex );
				vertex.Position = AssimpGLMHelpers::GetGLMVec( mesh->mVertices[i] );
				if ( mesh->mNormals )
				{
					vertex.Normal = AssimpGLMHelpers::GetGLMVec( mesh->mNormals[i] );
				}

				if ( mesh->mTextureCoords[0] )
				{
					glm::vec2 vec;
					vec.x = mesh->mTextureCoords[0][i].x;
					vec.y = mesh->mTextureCoords[0][i].y;
					vertex.TexCoords = vec;
				}
				else
					vertex.TexCoords = glm::vec2( 0.0f, 0.0f );

				vertices.push_back( vertex );
			}

			for ( unsigned int i = 0; i < mesh->mNumFaces; i++ )
			{
				aiFace face = mesh->mFaces[i];
				for ( unsigned int j = 0; j < face.mNumIndices; j++ )
					indices.push_back( face.mIndices[j] );
			}

			// if no normals, generate some
			for ( int i = 0; i < indices.size(); i += 3 )
			{
				auto& v1 = vertices[indices[i]];
				auto& v2 = vertices[indices[i + 1]];
				auto& v3 = vertices[indices[i + 2]];

				auto edge1 = v1.Position - v2.Position;
				auto edge2 = v3.Position - v2.Position;

				auto normal = glm::normalize( glm::cross( edge2, edge1 ) );
				v1.Normal = normal;
				v2.Normal = normal;
				v3.Normal = normal;
			}

			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			vector<Texture> diffuseMaps = loadMaterialTextures( material, aiTextureType_DIFFUSE, "texture_diffuse" );
			textures.insert( textures.end(), diffuseMaps.begin(), diffuseMaps.end() );
			vector<Texture> specularMaps = loadMaterialTextures( material, aiTextureType_SPECULAR, "texture_specular" );
			textures.insert( textures.end(), specularMaps.begin(), specularMaps.end() );
			std::vector<Texture> normalMaps = loadMaterialTextures( material, aiTextureType_HEIGHT, "texture_normal" );
			textures.insert( textures.end(), normalMaps.begin(), normalMaps.end() );
			std::vector<Texture> heightMaps = loadMaterialTextures( material, aiTextureType_AMBIENT, "texture_height" );
			textures.insert( textures.end(), heightMaps.begin(), heightMaps.end() );

			ExtractBoneWeightForVertices( vertices, mesh, scene );

			return Mesh( vertices, indices, textures );
		}

		void Model::SetVertexBoneData( Vertex& vertex, int boneID, float weight )
		{
			for ( int i = 0; i < MAX_BONE_INFLUENCE; ++i )
			{
				//if ( vertex.m_BoneIDs[i] < 0 )
				if ( vertex.m_Weights[i] == 0 )
				{
					vertex.m_Weights[i] = weight;
					vertex.m_BoneIDs[i] = boneID;
					break;
				}
			}
		}


		void Model::ExtractBoneWeightForVertices( std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene )
		{
			( scene );
			auto& boneInfoMap = m_BoneInfoMap;
			int& boneCount = m_BoneCounter;

			for ( unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex )
			{
				int boneID = -1;
				std::string boneName = mesh->mBones[boneIndex]->mName.data;
				if ( boneInfoMap.find( boneName ) == boneInfoMap.end() )
				{
					BoneInfo newBoneInfo;
					newBoneInfo.id = boneCount;
					newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat( mesh->mBones[boneIndex]->mOffsetMatrix );
					boneInfoMap[boneName] = newBoneInfo;
					boneID = boneCount;
					boneCount++;
				}
				else
				{
					boneID = boneInfoMap[boneName].id;
				}
				assert( boneID != -1 );
				auto weights = mesh->mBones[boneIndex]->mWeights;
				int numWeights = mesh->mBones[boneIndex]->mNumWeights;

				for ( int weightIndex = 0; weightIndex < numWeights; ++weightIndex )
				{
					int vertexId = weights[weightIndex].mVertexId;
					float weight = weights[weightIndex].mWeight;
					assert( vertexId <= vertices.size() );
					SetVertexBoneData( vertices[vertexId], boneID, weight );
				}
			}
		}

		unsigned int Model::TextureFromFile( const char* path, const string& _directory, bool gamma )
		{
			( gamma );
			string filename = string( path );
			filename = _directory + '/' + filename;

			unsigned int textureID;
			glGenTextures( 1, &textureID );

			int width, height, nrComponents;
			unsigned char* data = stbi_load( filename.c_str(), &width, &height, &nrComponents, 0 );
			if ( data )
			{
				GLenum format {};
				if ( nrComponents == 1 )
					format = GL_RED;
				else if ( nrComponents == 3 )
					format = GL_RGB;
				else if ( nrComponents == 4 )
					format = GL_RGBA;

				glBindTexture( GL_TEXTURE_2D, textureID );
				glTexImage2D( GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data );
				glGenerateMipmap( GL_TEXTURE_2D );

				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

				stbi_image_free( data );
			}
			else
			{
				std::cout << "Texture failed to load at path: " << path << std::endl;
				stbi_image_free( data );
			}

			return textureID;
		}

		// checks all material textures of a given type and loads the textures if they're not loaded yet.
		// the required info is returned as a Texture struct.
		vector<Texture> Model::loadMaterialTextures( aiMaterial* mat, aiTextureType type, string typeName )
		{
			vector<Texture> textures;
			for ( unsigned int i = 0; i < mat->GetTextureCount( type ); i++ )
			{
				aiString str;
				mat->GetTexture( type, i, &str );
				// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
				bool skip = false;
				for ( unsigned int j = 0; j < textures_loaded.size(); j++ )
				{
					if ( std::strcmp( textures_loaded[j].path.data(), str.C_Str() ) == 0 )
					{
						textures.push_back( textures_loaded[j] );
						skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
						break;
					}
				}
				if ( !skip )
				{   // if texture hasn't been loaded already, load it
					Texture texture;
					texture.id = TextureFromFile( str.C_Str(), this->directory );
					texture.type = typeName;
					texture.path = str.C_Str();
					textures.push_back( texture );
					textures_loaded.push_back( texture );  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
				}
			}
			return textures;
		}
	}
}
