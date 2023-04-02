#pragma once

#include "Internal/OGLShader.h"
#include "Internal/OGLPrimitives.h"
#include "Internal/OGLTexture.h"
#include "Internal/OGLRenderPass.h"

#include "Advance/CubeMap.h"
#include "Advance/ShadowMap.h"

#include "Font/FontRenderer.h"

#include "../godUtility/Utility.h"
#include <godUtility/Internal/RecycleVector.h>

#include "Animation/AnimationModel3D.h"
#include "Animation/Animator.h"

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

	struct AnimationWrap
	{
		Animation3D::Model m_model;
		Animation3D::Animation m_animation;
		std::vector<std::vector<glm::mat4>> m_cached_node_transforms;
		RecycleVector<Animation3D::Animator> m_animators;

		void Initialize ( std::string const& path );
		uint32_t AddInstance ();
		void PreCalculateBoneTransform ( uint32_t frame , float currentTime , const Animation3D::AssimpNodeData* node ,
			glm::mat4 parentTransform );
	};

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
		void RenderGUI ( Scene& scene ,
			glm::mat4 const& projection ,
			OGLTextureManager& textures );
		void RenderTransparent ( Scene& scene ,
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
		void UpdateAnimations ( float dt );
		void FirstPassRenderToDepthmap ( Scene const& scene , glm::mat4 const& projection , glm::mat4 const& view , glm::vec3 const& camera_position , OGLTextureManager& textures );

		// gaussian blur
		OGLRenderPass<1>& BlurTexture ( unsigned int texture );
		void RenderBlendTextures ( unsigned int texture1 , unsigned int texture2 );
		void RenderTexture ( unsigned int texture );

		std::vector<std::vector<OGLMesh>> getMesh () { return m_models; }

		// animation interface
		std::tuple<uint32_t , uint32_t> AddAnimationInstance ( std::string const& name );

		// blur 
		OGLRenderPass<1> m_blur_pingpong_1;
		OGLRenderPass<1> m_blur_pingpong_2;

		std::unordered_map<std::string , AnimationWrap> m_animations;

	private:
		int m_screen_width { 0 } , m_screen_height { 0 };

		int m_pixel_format { 0 };
		HDC m_window_device_context { nullptr };
		HGLRC m_opengl_rendering_context { nullptr };

		// shaders
		OGLShader m_flat_shader;
		OGLShader m_textured_shader;
		OGLShader m_textured_discard_shader;
		OGLShader m_textured_discard_shader_no_lighting;
		OGLShader m_single_colour_outline_shader;
		OGLShader m_cubemap_shader;
		OGLShader m_depthmap_shader;
		OGLShader m_anim_depthmap_shader;
		OGLShader m_hdr_shader;
		OGLShader m_blur_shader;
		OGLShader m_blend_shader;
		OGLShader m_2D_shader;
		OGLShader m_font_shader;

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

		std::vector<std::string> m_animation_names;
		void LoadAllAnimations ();
		void RenderAnimations ( Scene& scene ,
			glm::mat4 const& projection ,
			glm::mat4 const& view ,
			glm::vec3 const& camera_position ,
			OGLTextureManager& textures ,
			glm::vec3 const& camera_front );

		OGLShader m_animation_shader;
		Animation3D::Model m_animation_model;
		Animation3D::Animation m_animation_dance;
		Animation3D::Animator m_animator;

		//Temperory hard coded 
		/* May god forgive my sin !*/
		// To change the speed go to constructor OpenGL() , line 200
		float shark_temp_x {  };
		bool shark_start { false };

		// font
		FontRenderer m_font_renderer;

		glm::vec3 m_fog_color { 0.0,0.05,0.5 };
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