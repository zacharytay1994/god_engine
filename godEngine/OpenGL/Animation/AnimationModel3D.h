#pragma once

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#ifdef _DEBUG
#pragma comment (lib, "assimp-vc142-mtd.lib")
#else
#pragma comment (lib, "assimp-vc142-mt.lib")
#endif

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../Internal/OGLShader.h"

#include "AnimationMesh.h"
#include "AnimationModel3D.h"
#include "AssimpGLMHelpers.h"
#include "AnimationBoneInfo.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>


namespace god
{
	namespace Animation3D
	{
		using namespace std;

		class Model
		{
		public:
			// model data 
			vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
			vector<Mesh>    meshes;
			string directory;
			bool gammaCorrection;

			Model() = default;

			// constructor, expects a filepath to a 3D model.
			Model( string const& path, bool gamma = false );

			// draws the model, and thus all its meshes
			void Draw( OGLShader& shader );

			std::map<string, BoneInfo>& GetBoneInfoMap();

			int& GetBoneCount();

		private:

			std::map<string, BoneInfo> m_BoneInfoMap;
			int m_BoneCounter = 0;

			// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
			void loadModel( string const& path );

			// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
			void processNode( aiNode* node, const aiScene* scene );

			void SetVertexBoneDataToDefault( Vertex& vertex );

			Mesh processMesh( aiMesh* mesh, const aiScene* scene );

			void SetVertexBoneData( Vertex& vertex, int boneID, float weight );

			void ExtractBoneWeightForVertices( std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene );

			unsigned int TextureFromFile( const char* path, const string& directory, bool gamma = false );

			// checks all material textures of a given type and loads the textures if they're not loaded yet.
			// the required info is returned as a Texture struct.
			vector<Texture> loadMaterialTextures( aiMaterial* mat, aiTextureType type, string typeName );
		};
	}
}