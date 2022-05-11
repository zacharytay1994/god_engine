#pragma once

#include "Internal/Shader.h"
#include "Internal/Primitives.h"
#include "../godUtility/Utility.h"

#include <vector>
#include <queue>
#include <unordered_map>
#include <string>

#include "glad/glad.h"
#include <glm/glm/glm.hpp>

namespace god
{
	using OGLModelID = uint32_t;

	struct Scene;
	struct AssimpMesh;
	struct OpenGL
	{
		 OpenGL ( HWND windowHandle , int width , int height );
		 ~OpenGL ();

		void  ClearColour () const;

		// render scene functions
		// assimp bridge
		void  BuildOGLModels ( AssimpModelManager const& modelManager );
		void  RenderScene ( Scene const& scene ,
			glm::mat4 const& projection , glm::mat4 const& view , glm::vec3 const& camera_position );

		void  ResizeViewport ( int width , int height ) const;

	private:
		int m_pixel_format { 0 };
		HDC m_window_device_context { nullptr };
		HGLRC m_opengl_rendering_context { nullptr };

		// shaders
		OGLShader m_flat_shader;

		// models - vector of vector of OGLMesh because 1 model can consist of more than 1 mesh
		std::vector<std::vector<OGLMesh>> m_models;
		std::unordered_map<std::string , OGLModelID> m_model_ids;

		// assimp bridge
		OGLMesh BuildOGLMeshFromAssimpMesh ( AssimpMesh const& assimpMesh ) const;
		void BuildOGLMeshesFromAssimpMeshes ( std::vector<OGLMesh>& oglMeshes , std::vector<AssimpMesh> const& assimpMeshes ) const;
	};
}