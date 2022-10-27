#pragma once

#include "Internal/OGLShader.h"
#include "Internal/OGLPrimitives.h"
#include "Internal/OGLTexture.h"
#include "Internal/OGLRenderPass.h"

#include "Advance/CubeMap.h"
#include "Advance/ShadowMap.h"

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
		void  RenderScene ( Scene& scene ,
			glm::mat4 const& projection ,
			glm::mat4 const& view ,
			glm::vec3 const& camera_position ,
			OGLTextureManager& textures ,
			glm::vec3 const& camera_front );

		void  ResizeViewport ( int width , int height );

		template<size_t N>
		void RenderTonemap ( OGLRenderPass<N> const& hdrBuffer );

		// temp line rendering
		OGLShader m_line_shader;
		OGLMesh m_lines_mesh;
		static constexpr float m_default_line_size { 2.0f };
		using Lines = std::unordered_map<float , std::vector<std::tuple<glm::vec3 , glm::vec3 , glm::vec4 , float>>>;
		static Lines m_lines;
		static void DrawLine ( glm::vec3 const& a , glm::vec3 const& b , glm::vec4 const& c = { 1,1,1,1 } , float size = m_default_line_size );
		void RenderLines ( glm::mat4 const& projection , glm::mat4 const& view );
		void SetLineWidth ( float size );

		// shadow stuff
		void FirstPassRenderToDepthmap ( Scene const& scene , glm::mat4 const& projection , glm::mat4 const& view , glm::vec3 const& camera_position , OGLTextureManager& textures );

		// gaussian blur
		OGLRenderPass<1>& BlurTexture ( unsigned int texture );
		void RenderBlendTextures ( unsigned int texture1 , unsigned int texture2 );

		std::vector<std::vector<OGLMesh>> getMesh() { return m_models; }

		// blur 
		OGLRenderPass<1> m_blur_pingpong_1;
		OGLRenderPass<1> m_blur_pingpong_2;

	private:
		int m_screen_width { 0 } , m_screen_height { 0 };

		int m_pixel_format { 0 };
		HDC m_window_device_context { nullptr };
		HGLRC m_opengl_rendering_context { nullptr };

		// shaders
		OGLShader m_flat_shader;
		OGLShader m_textured_shader;
		OGLShader m_single_colour_outline_shader;
		OGLShader m_cubemap_shader;
		OGLShader m_depthmap_shader;
		OGLShader m_hdr_shader;
		OGLShader m_blur_shader;
		OGLShader m_blend_shader;

		// hdr
		OGLMesh m_square_mesh;

		OGLTexture m_causticmap_textures;

		int m_max_point_lights { 5 };
		int m_max_directional_lights { 5 };

		// models - vector of vector of OGLMesh because 1 model can consist of more than 1 mesh
		std::vector<std::vector<OGLMesh>> m_models;
		std::unordered_map<std::string , OGLModelID> m_model_ids;

		// assimp bridge
		OGLMesh BuildOGLMeshFromAssimpMesh ( Mesh3D const& mesh3D ) const;
		void BuildOGLMeshesFromAssimpMeshes ( std::vector<OGLMesh>& oglMeshes , std::vector<Mesh3D> const& meshes3D ) const;

		OGLCubeMap m_cubemap;

		// shadow stuff
		glm::mat4 m_light_space_matrix;
		OGLShadowMap m_shadowmap;
	};

	template<size_t N>
	inline void OpenGL::RenderTonemap ( OGLRenderPass<N> const& hdrBuffer )
	{
		m_hdr_shader.Use ();

		OGLShader::SetUniform ( m_hdr_shader.GetShaderID () , "uHdrBuffer" , 0 );
		glActiveTexture ( GL_TEXTURE0 );
		glBindTexture ( GL_TEXTURE_2D , hdrBuffer.GetTexture () );

		m_square_mesh.Draw ( GL_TRIANGLES );

		OGLShader::UnUse ();
	}
}