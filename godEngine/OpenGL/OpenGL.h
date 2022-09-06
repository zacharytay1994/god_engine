#pragma once

#include "Internal/OGLShader.h"
#include "Internal/OGLPrimitives.h"
#include "Internal/OGLTexture.h"
#include "../godUtility/Utility.h"

#include <vector>
#include <queue>
#include <unordered_map>
#include <string>

#include "glad/glad.h"

#pragma warning(disable: 4201)
#include <glm/glm/glm.hpp>
#pragma warning(default: 4201)

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
		void  BuildOGLModels ( Asset3DManager const& asset3DManager );
		void  UpdateOGLModel ( ResourceID id , Asset3DManager const& asset3DManager );
		void  RenderScene ( Scene const& scene ,
			glm::mat4 const& projection , glm::mat4 const& view , glm::vec3 const& camera_position , OGLTextureManager& textures );

		void  ResizeViewport ( int width , int height ) const;

		// temp line rendering
		OGLShader m_line_shader;
		OGLMesh m_lines_mesh;
		static constexpr float m_default_line_size { 2.0f };
		using Lines = std::unordered_map<float , std::vector<std::tuple<glm::vec3 , glm::vec3 , glm::vec4 , float>>>;
		static Lines m_lines;
		static void DrawLine ( glm::vec3 const& a , glm::vec3 const& b , glm::vec4 const& c = { 1,1,1,1 } , float size = m_default_line_size );
		void RenderLines ( glm::mat4 const& projection , glm::mat4 const& view , glm::vec3 const& camera_position );
		void SetLineWidth ( float size );

	private:
		int m_pixel_format { 0 };
		HDC m_window_device_context { nullptr };
		HGLRC m_opengl_rendering_context { nullptr };

		// shaders
		OGLShader m_flat_shader;
		OGLShader m_textured_shader;

		// models - vector of vector of OGLMesh because 1 model can consist of more than 1 mesh
		std::vector<std::vector<OGLMesh>> m_models;
		std::unordered_map<std::string , OGLModelID> m_model_ids;

		// assimp bridge
		OGLMesh BuildOGLMeshFromAssimpMesh ( Mesh3D const& mesh3D ) const;
		void BuildOGLMeshesFromAssimpMeshes ( std::vector<OGLMesh>& oglMeshes , std::vector<Mesh3D> const& meshes3D ) const;
	};
}