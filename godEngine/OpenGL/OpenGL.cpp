#include "../pch.h"
#include "OpenGL.h"
#include "../godUtility/Scene.h"
#include <godUtility/Math.h>
#include "../Window/DeltaTimer.h"

#include "Internal/OGLDebug.h"

#pragma comment(lib, "opengl32.lib")

#include <iostream>
#include <assert.h>
#include <algorithm>

#pragma warning(disable : 4201)
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#pragma warning(default : 4201)

namespace god
{
	OpenGL::Lines OpenGL::m_lines{};

	OpenGL::OpenGL( HWND windowHandle, int width, int height )
		:
		m_screen_width{ width },
		m_screen_height{ height },
		m_window_device_context{ GetDC( windowHandle ) }
	{
		// Load opengl functions with glad
		if ( !gladLoadGL() )
		{
			std::cerr << "Failed to initialize GLAD" << std::endl;
			return;
		}

		ResizeViewport( width, height );

		// OpenGl debug features
		int flags;
		// Features of contexts can be detected via context flags
		glGetIntegerv( GL_CONTEXT_FLAGS, &flags );
		// Check if the context is a debug context
		if ( flags & GL_CONTEXT_FLAG_DEBUG_BIT )
		{
			OGLDebug::EnableOpenGLDebugging();
			std::cout << "Debug Mode" << std::endl;
		}

		// Create flat shader
		m_flat_shader.InitializeFromFile(
			"Assets/EngineAssets/OpenGLShaders/flatcolour.vs",
			"Assets/EngineAssets/OpenGLShaders/flatcolour.fs" );

		// Create textured shader
		m_textured_shader.InitializeFromFile(
			"Assets/EngineAssets/OpenGLShaders/texturemaps.vs",
			"Assets/EngineAssets/OpenGLShaders/texturemaps.fs" );

		m_single_colour_outline_shader.InitializeFromFile(
			"Assets/EngineAssets/OpenGLShaders/single_colour_outline.vs",
			"Assets/EngineAssets/OpenGLShaders/single_colour_outline.fs" );

		// Create cubemap shader
		m_cubemap_shader.InitializeFromFile(
			"Assets/EngineAssets/OpenGLShaders/skybox.vs",
			"Assets/EngineAssets/OpenGLShaders/skybox.fs" );

		// Create depthmap shader
		m_depthmap_shader.InitializeFromFile(
			"Assets/EngineAssets/OpenGLShaders/depthmap.vs",
			"Assets/EngineAssets/OpenGLShaders/depthmap.fs" );

		// Create 2D shader
		m_2D_shader.InitializeFromFile(
			"Assets/EngineAssets/OpenGLShaders/texturemaps2D.vs",
			"Assets/EngineAssets/OpenGLShaders/texturemaps2D.fs" );

		// Calculation for shadow map
		m_light_space_matrix =
			glm::ortho( -10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 20.0f ) *
			glm::lookAt(
			glm::vec3( 10.0f, 10.0f, 10.0f ),
			glm::vec3( 0.0f, 0.0f, 0.0f ),
			glm::vec3( 0.0f, 1.0f, 0.0f ) );

		// opengl settings
		// glEnable( GL_CULL_FACE );
		// Enables the Depth Buffer
		glEnable( GL_DEPTH_TEST );
		// Enables the Stencil Buffer
		glEnable( GL_STENCIL_TEST );
		// Sets rules for outcomes of stecil tests
		glStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );

		// glEnable ( GL_DEPTH_TEST );
		SetLineWidth( m_default_line_size );

		// Temp line rendering code
		constexpr int MAX_POINTS{ 10'000 };
		char const line_vs[] = {
			"#version 430 core\n"
			"layout ( location = 0 ) in vec3 aPos;\n"
			"layout ( location = 4 ) in vec4 aColour;\n"
			"out vec4 vColour;\n"
			"uniform mat4 uProjection;\n"
			"uniform mat4 uView;\n"
			"void main ()\n"
			"{\n"
			"gl_Position = uProjection * uView * vec4(aPos, 1.0);\n"
			"vColour = aColour;\n"
			"}\n" };

		char const line_fs[] = {
			"#version 430 core\n"
			"in vec4 vColour;\n"
			"out vec4 fFragColor;\n"
			"void main ()\n"
			"{\n"
			"fFragColor = vColour;\n"
			"}\n" };

		m_line_shader.InitializeFromCode( line_vs, line_fs );
		m_lines_mesh.m_vertices.resize( MAX_POINTS );
		m_lines_mesh.m_indices.resize( MAX_POINTS );
		m_lines_mesh.Initialize();

		// Creating cube map
		m_cubemap.Initialize();
		std::string skybox_model_path = "Assets/GameAssets/2DAssets/Raw/Skybox/";
		// All the faces of the cubemap (make sure they are in this exact order)
		std::string facesCubemap[6] =
		{
			skybox_model_path + "right.jpg",
			skybox_model_path + "left.jpg",
			skybox_model_path + "top.jpg",
			skybox_model_path + "bottom.jpg",
			skybox_model_path + "front.jpg",
			skybox_model_path + "back.jpg" };
		m_cubemap.CubeTexture( facesCubemap );

		// Creating shadow map
		m_shadowmap.Initialize( 2048, 2048 );

		// m_square
		m_square_mesh.m_vertices = {
			{{ 1.0f,  1.0f, 0.0f }, { 1.0f, 1.0f }},  // top right
			{{ 1.0f, -1.0f, 0.0f }, { 1.0f, 0.0f }},  // bottom right
			{{-1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f }},  // bottom left
			{{-1.0f,  1.0f, 0.0f }, { 0.0f, 1.0f }}	  // top left 
		};
		m_square_mesh.m_indices = {
			0, 1, 3,   // first triangle
			1, 2, 3    // second triangle
		};
		m_square_mesh.Initialize();

		// load hdr shader
		m_hdr_shader.InitializeFromFile(
			"Assets/EngineAssets/OpenGLShaders/tonemap.vs",
			"Assets/EngineAssets/OpenGLShaders/tonemap.fs"
		);

		// blur renderpasses
		m_blur_pingpong_1.Initialize( width, height );
		m_blur_pingpong_2.Initialize( width, height );

		// blur shader
		m_blur_shader.InitializeFromFile(
			"Assets/EngineAssets/OpenGLShaders/blur.vs",
			"Assets/EngineAssets/OpenGLShaders/blur.fs"
		);

		// blend shader
		m_blend_shader.InitializeFromFile(
			"Assets/EngineAssets/OpenGLShaders/blend.vs",
			"Assets/EngineAssets/OpenGLShaders/blend.fs"
		);

		m_causticmap_textures = OGLTexture( "Assets/EngineAssets/Textures/CausticMap.png" );

		//----------------------------------------
		// Tested on file format : .dae / .md5mesh / .fbx
		//std::string animated_path = "Assets/GameAssets/3DAssets/Animation/Vampire/dancing_vampire.dae";
		//std::string animated_path = "Assets/GameAssets/3DAssets/Animation/Bob_lamp/bob_lamp_update_export.md5mesh";
		//std::string animated_path = "Assets/GameAssets/3DAssets/Animation/skeleton.fbx";
		//std::string animated_path = "Assets/GameAssets/3DAssets/Animation/JonathanKwekSample/Bipedal01_TestAnim_ImportReexportMax.fbx";

		std::string animated_path = "Assets/GameAssets/3DAssets/Animation/Shark/Correctshark002.dae";
		m_animation_model = Animation3D::Model( animated_path );
		m_animation_dance = Animation3D::Animation( animated_path, &m_animation_model );
		m_animator = Animation3D::Animator( &m_animation_dance );

		m_animation_shader.InitializeFromFile(
			"Assets/EngineAssets/OpenGLShaders/anim_model.vs",
			"Assets/EngineAssets/OpenGLShaders/texturemaps.fs"
		);

		//----------------------------------------

		glCheckError();
		std::cout << "OpenGL constructed." << std::endl;
	}

	OpenGL::~OpenGL()
	{
		// wglDeleteContext ( m_opengl_rendering_context );
	}

	void OpenGL::ClearColour() const
	{
		glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
	}

	void OpenGL::BuildOGLModels( Asset3DManager const& asset3DManager )
	{
		// Clear current mesh list
		m_models.clear();

		// Copy meshes
		for ( auto const& asset : asset3DManager.GetResources() )
		{
			m_models.emplace_back();
			BuildOGLMeshesFromAssimpMeshes( m_models.back(), std::get<1>( asset ).m_model.m_meshes );
		}

		// Copy mesh ids
		m_model_ids = asset3DManager.GetIDs();
	}

	void OpenGL::UpdateOGLModel( ResourceID id, Asset3DManager const& asset3DManager )
	{
		m_models[id].clear();
		BuildOGLMeshesFromAssimpMeshes( m_models[id], std::get<1>( asset3DManager.Get( id ) ).m_model.m_meshes );
		// Copy mesh ids
		m_model_ids = asset3DManager.GetIDs();
	}

	void OpenGL::RenderScene( Scene& scene,
							  glm::mat4 const& projection,
							  glm::mat4 const& view,
							  glm::vec3 const& camera_position,
							  OGLTextureManager& textures,
							  glm::vec3 const& camera_front )
	{
		// Update animation transformation 
		m_animator.UpdateAnimation( DeltaTimer::m_dt ); //--

		glViewport( 0, 0, m_screen_width, m_screen_height );
		ClearColour();

		// Draw the normal model
		m_textured_shader.Use();

		// Reset point light uniforms
		OGLShader::SetUniform( m_textured_shader.GetShaderID(), "uNumPointLight", 0 );

		// Reset directional light uniforms
		OGLShader::SetUniform( m_textured_shader.GetShaderID(), "uNumDirectionalLight", 0 );

		for ( auto const& data : scene.m_instanced_render_data )
		{
			// Make it so the stencil test always passes
			//glStencilFunc( GL_ALWAYS, 1, 0xFF );
			//// Enable modifying of the stencil buffer
			//glStencilMask( 0xFF );

			// Draw the normal model
			m_textured_shader.Use();

			// projection matrix
			OGLShader::SetUniform( m_textured_shader.GetShaderID(), "uProjection", projection );

			// view matrix
			OGLShader::SetUniform( m_textured_shader.GetShaderID(), "uView", view );

			// set uniforms for fragment shader
			// set view position
			OGLShader::SetUniform( m_textured_shader.GetShaderID(), "uViewPosition", camera_position );

			// set material
			OGLShader::SetUniform( m_textured_shader.GetShaderID(), "uMaterial.diffuse_map", 0 );
			std::get<1>( textures.Get( data.first.m_diffuse_id ) ).Bind( 0 );
			OGLShader::SetUniform( m_textured_shader.GetShaderID(), "uMaterial.specular_map", 1 );
			std::get<1>( textures.Get( data.first.m_specular_id ) ).Bind( 1 );
			OGLShader::SetUniform( m_textured_shader.GetShaderID(), "uMaterial.shininess", data.first.m_shininess );

			// set reflection
			OGLShader::SetUniform( m_textured_shader.GetShaderID(), "uSkybox", 2 );
			m_cubemap.Bind( 2 );

			// set shadowmap
			OGLShader::SetUniform( m_textured_shader.GetShaderID(), "uShadowMap", 3 );
			m_shadowmap.Bind( 3 );

			// bind caustic map textures
			OGLShader::SetUniform( m_textured_shader.GetShaderID(), "uCausticMap", 8 );
			m_causticmap_textures.Bind( 8 );

			// pass in delta time into shader 
			OGLShader::SetUniform( m_textured_shader.GetShaderID(), "uDT", DeltaTimer::m_acc_dt );

			// render point lights
			std::sort( scene.m_point_light_data.begin(), scene.m_point_light_data.end(),
					   [&camera_position] ( Scene::PointLightData const& pld1, Scene::PointLightData const& pld2 )
					   {
						   return LengthSq( pld1.m_position - camera_position ) < LengthSq( pld2.m_position - camera_position );
					   }
			);
			int num_point_light = scene.m_point_light_data.size() > m_max_point_lights ? m_max_point_lights : scene.m_point_light_data.size();
			OGLShader::SetUniform( m_textured_shader.GetShaderID(), "uNumPointLight", num_point_light );
			for ( auto i = 0; i < num_point_light; ++i )
			{
				auto const& light = scene.m_point_light_data[i];

				// max 5 point lights as defined in the shader
				OGLShader::SetUniform( m_textured_shader.GetShaderID(), ( "uPointLight[" + std::to_string( i ) + "].position" ).c_str(), light.m_position );
				OGLShader::SetUniform( m_textured_shader.GetShaderID(), ( "uPointLight[" + std::to_string( i ) + "].colour" ).c_str(), light.m_colour );
				OGLShader::SetUniform( m_textured_shader.GetShaderID(), ( "uPointLight[" + std::to_string( i ) + "].ambient" ).c_str(), light.m_ambient );
				OGLShader::SetUniform( m_textured_shader.GetShaderID(), ( "uPointLight[" + std::to_string( i ) + "].diffuse" ).c_str(), light.m_diffuse );
				OGLShader::SetUniform( m_textured_shader.GetShaderID(), ( "uPointLight[" + std::to_string( i ) + "].specular" ).c_str(), light.m_specular );
			}

			// render directional lights
			std::sort( scene.m_directional_light_data.begin(), scene.m_directional_light_data.end(),
					   [&camera_position] ( Scene::DirectionalLightData const& dld1, Scene::DirectionalLightData const& dld2 )
					   {
						   return LengthSq( dld1.m_position - camera_position ) < LengthSq( dld2.m_position - camera_position );
					   }
			);
			int num_directional_light = scene.m_directional_light_data.size() > m_max_directional_lights ? m_max_directional_lights : scene.m_directional_light_data.size();
			OGLShader::SetUniform( m_textured_shader.GetShaderID(), "uNumDirectionalLight", num_directional_light );
			for ( auto i = 0; i < num_directional_light; ++i )
			{
				auto const& light = scene.m_directional_light_data[i];
				OGLShader::SetUniform( m_textured_shader.GetShaderID(), ( "uDirectionalLight[" + std::to_string( i ) + "].position" ).c_str(), light.m_position );
				OGLShader::SetUniform( m_textured_shader.GetShaderID(), ( "uDirectionalLight[" + std::to_string( i ) + "].direction" ).c_str(), -glm::normalize( light.m_position ) );
				OGLShader::SetUniform( m_textured_shader.GetShaderID(), ( "uDirectionalLight[" + std::to_string( i ) + "].colour" ).c_str(), light.m_colour );
				OGLShader::SetUniform( m_textured_shader.GetShaderID(), ( "uDirectionalLight[" + std::to_string( i ) + "].ambient" ).c_str(), light.m_ambient );
				OGLShader::SetUniform( m_textured_shader.GetShaderID(), ( "uDirectionalLight[" + std::to_string( i ) + "].diffuse" ).c_str(), light.m_diffuse );
				OGLShader::SetUniform( m_textured_shader.GetShaderID(), ( "uDirectionalLight[" + std::to_string( i ) + "].specular" ).c_str(), light.m_specular );

				m_light_space_matrix =
					glm::ortho( -20.0f, 20.0f, -20.0f, 20.0f, 1.0f, 20.0f ) *
					glm::lookAt(
					glm::vec3( light.m_position ),
					glm::vec3( 0.0f, 0.0f, 0.0f ),
					glm::vec3( 0.0f, 1.0f, 0.0f ) );

				OGLShader::SetUniform( m_textured_shader.GetShaderID(), "uLightSpaceMatrix", m_light_space_matrix );
			}

			// Set Fog
			OGLShader::SetUniform( m_textured_shader.GetShaderID(), "uFogParams.color", { 0.45f,0.65f,0.90f } );
			OGLShader::SetUniform( m_textured_shader.GetShaderID(), "uFogParams.linearStart", 10.0f );
			OGLShader::SetUniform( m_textured_shader.GetShaderID(), "uFogParams.linearEnd", 100.0f );
			OGLShader::SetUniform( m_textured_shader.GetShaderID(), "uFogParams.density", 0.03f );
			OGLShader::SetUniform( m_textured_shader.GetShaderID(), "uFogParams.equation", 0 );
			OGLShader::SetUniform( m_textured_shader.GetShaderID(), "uFogParams.isEnabled", true );

			// Set Tint
			OGLShader::SetUniform( m_textured_shader.GetShaderID(), "uTint", glm::vec4( 0.0f ) );

			OGLShader::SetUniform( m_textured_shader.GetShaderID(), "uEmissive", data.first.m_emissive );

			// draw model
			for ( auto& mesh : m_models[data.first.m_model_id] )
			{
				mesh.SetTransformData( data.second );
				mesh.DrawInstanced( GL_TRIANGLES );
			}

			// Make it so only the pixels without the value 1 pass the test
			//glStencilFunc( GL_NOTEQUAL, 1, 0xFF );
			//// Disable modifying of the stencil buffer
			//glStencilMask( 0x00 );
			//// Disable the depth buffer
			//glDisable( GL_DEPTH_TEST );

			//m_single_colour_outline_shader.Use();
			//OGLShader::SetUniform( m_single_colour_outline_shader.GetShaderID(), "uProjection", projection );
			//OGLShader::SetUniform( m_single_colour_outline_shader.GetShaderID(), "uView", view );
			//OGLShader::SetUniform( m_single_colour_outline_shader.GetShaderID(), "uOutlining", 0.01f );

			//for ( auto& mesh : m_models[data.first.m_model_id] )
			//{
			//	mesh.SetTransformData( data.second );
			//	mesh.DrawInstanced( GL_TRIANGLES );
			//}

			//// Enable modifying of the stencil buffer
			//glStencilMask( 0xFF );
			//// Clear stencil buffer
			//glStencilFunc( GL_ALWAYS, 0, 0xFF );
			//// Enable the depth buffer
			//glEnable( GL_DEPTH_TEST );

		}

		//[SPACE]-----------------------------------------------------------------------------------

		//[Animation - START]-----------------------------------------------------------------------
		
		// Draw the normal model
		m_animation_shader.Use();

		// reset point light uniforms
		OGLShader::SetUniform( m_animation_shader.GetShaderID(), "uNumPointLight", 0 );

		// reset directional light uniforms
		OGLShader::SetUniform( m_animation_shader.GetShaderID(), "uNumDirectionalLight", 0 );

		auto transforms = m_animator.GetFinalBoneMatrices();
		for ( int i = 0; i < transforms.size(); ++i )
		{
			std::string temp = "bone_transforms[" + std::to_string( i ) + "]";
			OGLShader::SetUniform( m_animation_shader.GetShaderID(), temp.c_str(), transforms[i] );
		}

		// projection matrix
		OGLShader::SetUniform( m_animation_shader.GetShaderID(), "uProjection", projection );

		// view matrix
		OGLShader::SetUniform( m_animation_shader.GetShaderID(), "uView", view );

		// set uniforms for fragment shader
		// set view position
		OGLShader::SetUniform( m_animation_shader.GetShaderID(), "uViewPosition", camera_position );

		// set material
		OGLShader::SetUniform( m_animation_shader.GetShaderID(), "uMaterial.diffuse_map", 0 );
		std::get<1>( textures.Get( textures.GetID( "White" ) ) ).Bind( 0 );
		OGLShader::SetUniform( m_animation_shader.GetShaderID(), "uMaterial.specular_map", 1 );
		std::get<1>( textures.Get( textures.GetID( "White" ) ) ).Bind( 1 );
		OGLShader::SetUniform( m_animation_shader.GetShaderID(), "uMaterial.shininess", 32.0f );

		// set reflection
		OGLShader::SetUniform( m_animation_shader.GetShaderID(), "uSkybox", 2 );
		m_cubemap.Bind( 2 );

		// set shadowmap
		OGLShader::SetUniform( m_animation_shader.GetShaderID(), "uShadowMap", 3 );
		m_shadowmap.Bind( 3 );

		// bind caustic map textures
		OGLShader::SetUniform( m_animation_shader.GetShaderID(), "uCausticMap", 8 );
		//m_causticmap_textures.Bind( 8 );
		std::get<1>( textures.Get( textures.GetID( "None" ) ) ).Bind( 8 );

		// pass in delta time into shader 
		OGLShader::SetUniform( m_animation_shader.GetShaderID(), "uDT", DeltaTimer::m_acc_dt );

		// render point lights
		std::sort( scene.m_point_light_data.begin(), scene.m_point_light_data.end(),
				   [&camera_position] ( Scene::PointLightData const& pld1, Scene::PointLightData const& pld2 )
				   {
					   return LengthSq( pld1.m_position - camera_position ) < LengthSq( pld2.m_position - camera_position );
				   }
		);
		int num_point_light = scene.m_point_light_data.size() > m_max_point_lights ? m_max_point_lights : scene.m_point_light_data.size();
		OGLShader::SetUniform( m_animation_shader.GetShaderID(), "uNumPointLight", num_point_light );
		for ( auto i = 0; i < num_point_light; ++i )
		{
			auto const& light = scene.m_point_light_data[i];

			// max 5 point lights as defined in the shader
			OGLShader::SetUniform( m_animation_shader.GetShaderID(), ( "uPointLight[" + std::to_string( i ) + "].position" ).c_str(), light.m_position );
			OGLShader::SetUniform( m_animation_shader.GetShaderID(), ( "uPointLight[" + std::to_string( i ) + "].colour" ).c_str(), light.m_colour );
			OGLShader::SetUniform( m_animation_shader.GetShaderID(), ( "uPointLight[" + std::to_string( i ) + "].ambient" ).c_str(), light.m_ambient );
			OGLShader::SetUniform( m_animation_shader.GetShaderID(), ( "uPointLight[" + std::to_string( i ) + "].diffuse" ).c_str(), light.m_diffuse );
			OGLShader::SetUniform( m_animation_shader.GetShaderID(), ( "uPointLight[" + std::to_string( i ) + "].specular" ).c_str(), light.m_specular );
		}

		// render directional lights
		std::sort( scene.m_directional_light_data.begin(), scene.m_directional_light_data.end(),
				   [&camera_position] ( Scene::DirectionalLightData const& dld1, Scene::DirectionalLightData const& dld2 )
				   {
					   return LengthSq( dld1.m_position - camera_position ) < LengthSq( dld2.m_position - camera_position );
				   }
		);
		int num_directional_light = scene.m_directional_light_data.size() > m_max_directional_lights ? m_max_directional_lights : scene.m_directional_light_data.size();
		OGLShader::SetUniform( m_animation_shader.GetShaderID(), "uNumDirectionalLight", num_directional_light );
		for ( auto i = 0; i < num_directional_light; ++i )
		{
			auto const& light = scene.m_directional_light_data[i];
			OGLShader::SetUniform( m_animation_shader.GetShaderID(), ( "uDirectionalLight[" + std::to_string( i ) + "].position" ).c_str(), light.m_position );
			OGLShader::SetUniform( m_animation_shader.GetShaderID(), ( "uDirectionalLight[" + std::to_string( i ) + "].direction" ).c_str(), -glm::normalize( light.m_position ) );
			OGLShader::SetUniform( m_animation_shader.GetShaderID(), ( "uDirectionalLight[" + std::to_string( i ) + "].colour" ).c_str(), light.m_colour );
			OGLShader::SetUniform( m_animation_shader.GetShaderID(), ( "uDirectionalLight[" + std::to_string( i ) + "].ambient" ).c_str(), light.m_ambient );
			OGLShader::SetUniform( m_animation_shader.GetShaderID(), ( "uDirectionalLight[" + std::to_string( i ) + "].diffuse" ).c_str(), light.m_diffuse );
			OGLShader::SetUniform( m_animation_shader.GetShaderID(), ( "uDirectionalLight[" + std::to_string( i ) + "].specular" ).c_str(), light.m_specular );

			m_light_space_matrix =
				glm::ortho( -20.0f, 20.0f, -20.0f, 20.0f, 1.0f, 20.0f ) *
				glm::lookAt(
				glm::vec3( light.m_position ),
				glm::vec3( 0.0f, 0.0f, 0.0f ),
				glm::vec3( 0.0f, 1.0f, 0.0f ) );

			OGLShader::SetUniform( m_animation_shader.GetShaderID(), "uLightSpaceMatrix", m_light_space_matrix );
		}

		// Set Fog
		OGLShader::SetUniform( m_animation_shader.GetShaderID(), "uFogParams.color", { 0.45f,0.65f,0.90f } );
		OGLShader::SetUniform( m_animation_shader.GetShaderID(), "uFogParams.linearStart", 10.0f );
		OGLShader::SetUniform( m_animation_shader.GetShaderID(), "uFogParams.linearEnd", 100.0f );
		OGLShader::SetUniform( m_animation_shader.GetShaderID(), "uFogParams.density", 0.03f );
		OGLShader::SetUniform( m_animation_shader.GetShaderID(), "uFogParams.equation", 0 );
		OGLShader::SetUniform( m_animation_shader.GetShaderID(), "uFogParams.isEnabled", false );

		// Set Tint
		OGLShader::SetUniform( m_animation_shader.GetShaderID(), "uIsTint", false );
		//OGLShader::SetUniform( m_animation_shader.GetShaderID(), "uTint", glm::vec4( 0.8f, 0.8f, 0.8f, 1.0f ) );

		// draw model
		glm::mat4 model = glm::mat4( 1.0f );

		model = glm::translate( model, glm::vec3( 1.0f, 1.0f, 5.0f ) ); // translate it down so it's at the center of the scene
		model = glm::rotate( model, glm::radians( 90.0f ), glm::vec3( 0.0f, 1.0f, 0.0f ) );// rotate at y-axis
		model = glm::scale( model, glm::vec3( 1.0f ) );	// it's a bit too big for our scene, so scale it down
		OGLShader::SetUniform( m_animation_shader.GetShaderID(), "uModel", model );

		OGLShader::SetUniform( m_animation_shader.GetShaderID(), "uEmissive", 100.0f );
		m_animation_model.Draw( m_animation_shader );

	


		//[Animation - END]-------------------------------------------------------------------------

		//[SPACE]-----------------------------------------------------------------------------------

		// Draw skybox as last
		m_cubemap.CubeMapEnableDepth();
		m_cubemap_shader.Use();
		OGLShader::SetUniform( m_cubemap_shader.GetShaderID(), "view", view );
		OGLShader::SetUniform( m_cubemap_shader.GetShaderID(), "projection", projection );
		OGLShader::SetUniform( m_cubemap_shader.GetShaderID(), "camera", camera_position );
		m_cubemap.Bind();
		m_cubemap.Draw();
		m_cubemap.UnBind();
		m_cubemap.CubeMapDisableDepth();

		// Unuse the bound shader
		OGLShader::UnUse();
	}

	void OpenGL::RenderGUI( Scene& scene, glm::mat4 const& projection, OGLTextureManager& textures )
	{
		// enable blending
		glEnable ( GL_BLEND );
		glBlendFunc ( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );

		m_2D_shader.Use ();

		OGLShader::SetUniform( m_2D_shader.GetShaderID(), "uProjection", projection );

		/*std::vector<std::tuple< Scene::InstancedRenderData , glm::mat4>> m_sort_container;
		for ( auto const& data : scene.m_2D_instanced_render_data )
		{
			m_sort_container.emplace_back ( data.first , data.second.begin () );
		}
		std::sort ( m_sort_container.begin () , m_sort_container.end () ,
			[]( auto const& lhs , auto const& rhs )
			{
				auto& [lhs_first , lhs_second] = lhs;
				auto& [rhs_first , rhs_second] = rhs;
				return lhs_second[ 3 ].z < rhs_second[ 3 ].z;
			}
		);*/

		for ( auto const& data : scene.m_2D_instanced_render_data )
		{
			OGLShader::SetUniform( m_2D_shader.GetShaderID(), "uMaterial.diffuse_map", 0 );
			std::get<1>( textures.Get( data.first.m_diffuse_id ) ).Bind( 0 );

			for ( auto& mesh : m_models[data.first.m_model_id] )
			{
				mesh.SetTransformData( data.second );
				mesh.DrawInstanced( GL_TRIANGLES );
			}
		}
		/*for ( auto const& key_pair : m_sort_container )
		{
			auto& [key , t] = key_pair;
			auto& value = scene.m_2D_instanced_render_data[ key ];

			OGLShader::SetUniform ( m_2D_shader.GetShaderID () , "uMaterial.diffuse_map" , 0 );
			std::get<1> ( textures.Get ( key.m_diffuse_id ) ).Bind ( 0 );

			for ( auto& mesh : m_models[ key.m_model_id ] )
			{
				mesh.SetTransformData ( value );
				mesh.DrawInstanced ( GL_TRIANGLES );
			}
		}*/

		m_2D_shader.UnUse ();
		glDisable ( GL_BLEND );
	}

	void OpenGL::ResizeViewport( int width, int height )
	{
		m_screen_width = width;
		m_screen_height = height;
		glViewport( 0, 0, width, height );

		std::cout << "OpenGL viewport resized x:" << width << " y: " << height << std::endl;
	}

	/*void OpenGL::RenderTonemap( OGLRenderPass const& framebuffer )
	{
		m_hdr_shader.Use();

		OGLShader::SetUniform( m_hdr_shader.GetShaderID(), "uHdrBuffer", 0 );
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, framebuffer.GetTexture() );

		m_square_mesh.Draw( GL_TRIANGLES );

		OGLShader::UnUse();
	}*/

	void OpenGL::DrawLine( glm::vec3 const& a, glm::vec3 const& b, glm::vec4 const& c, float size )
	{
		m_lines[size].push_back( { a, b, c, size } );
	}

	void OpenGL::RenderLines( glm::mat4 const& projection, glm::mat4 const& view )
	{
		m_line_shader.Use();

		m_line_shader.SetUniform( m_line_shader.GetShaderID(), "uProjection", projection );
		m_line_shader.SetUniform( m_line_shader.GetShaderID(), "uView", view );

		for ( auto const& line_group : m_lines )
		{
			m_lines_mesh.m_indices.clear();
			m_lines_mesh.m_vertices.clear();

			for ( auto const& line : line_group.second )
			{
				OGLMesh::OGLVertex start, end;
				start.m_position = std::get<0>( line );
				end.m_position = std::get<1>( line );
				start.m_colour = std::get<2>( line );
				end.m_colour = std::get<2>( line );

				m_lines_mesh.m_indices.push_back( static_cast< Index3D >( m_lines_mesh.m_vertices.size() ) );
				m_lines_mesh.m_vertices.push_back( start );
				m_lines_mesh.m_indices.push_back( static_cast< Index3D >( m_lines_mesh.m_vertices.size() ) );
				m_lines_mesh.m_vertices.push_back( end );
			}

			m_lines_mesh.SetData();
			SetLineWidth( line_group.first );
			m_lines_mesh.Draw( GL_LINES );
		}

		OGLShader::UnUse();
		m_lines.clear();
	}

	void OpenGL::SetLineWidth( float size )
	{
		glLineWidth( size );
	}

	OGLMesh OpenGL::BuildOGLMeshFromAssimpMesh( Mesh3D const& mesh3d ) const
	{
		OGLMesh mesh;

		// copy vertices
		mesh.m_vertices.resize( mesh3d.m_vertices.size() );
		for ( auto i = 0; i < mesh3d.m_vertices.size(); ++i )
		{
			mesh.m_vertices[i].m_position = mesh3d.m_vertices[i].m_position;
			mesh.m_vertices[i].m_uv = mesh3d.m_vertices[i].m_uv;
			mesh.m_vertices[i].m_tangent = mesh3d.m_vertices[i].m_tangent;
			mesh.m_vertices[i].m_normal = mesh3d.m_vertices[i].m_normal;
			mesh.m_vertices[i].m_colour = mesh3d.m_vertices[i].m_colour;
		}

		// copy indices
		mesh.m_indices = mesh3d.m_indices;

		mesh.Initialize();

		return mesh;
	}

	void OpenGL::BuildOGLMeshesFromAssimpMeshes( std::vector<OGLMesh>& oglMeshes, std::vector<Mesh3D> const& meshes3D ) const
	{
		for ( auto& mesh : meshes3D )
		{
			oglMeshes.emplace_back( BuildOGLMeshFromAssimpMesh( mesh ) );
		}
	}

	void OpenGL::FirstPassRenderToDepthmap( Scene const& scene, glm::mat4 const& projection, glm::mat4 const& view, glm::vec3 const& camera_position, OGLTextureManager& textures )
	{
		//glCheckError();
		m_shadowmap.EnableDepthMap();
		for ( auto const& data : scene.m_instanced_render_data )
		{
			// Draw the normal model
			m_depthmap_shader.Use();

			OGLShader::SetUniform(
				m_depthmap_shader.GetShaderID(),
				"uLightSpaceMatrix",
				m_light_space_matrix );

			// Draw model
			for ( auto& mesh : m_models[data.first.m_model_id] )
			{
				mesh.SetTransformData( data.second );
				mesh.DrawInstanced( GL_TRIANGLES );
			}
		}
		m_shadowmap.DisableDepthMap();

	}

	OGLRenderPass<1>& OpenGL::BlurTexture( unsigned int texture )
	{
		bool horizontal = true, first_iteration = true;
		int amount = 5;
		m_blur_shader.Use();
		for ( unsigned int i = 0; i < amount; i++ )
		{
			if ( horizontal )
			{
				m_blur_pingpong_1.Bind();
				OGLShader::SetUniform( m_blur_shader.GetShaderID(), "uHorizontal", horizontal );
				if ( first_iteration )
				{
					glActiveTexture( GL_TEXTURE0 );
					glBindTexture( GL_TEXTURE_2D, texture );
				}
				else
				{
					glActiveTexture( GL_TEXTURE0 );
					glBindTexture( GL_TEXTURE_2D, m_blur_pingpong_2.GetTexture() );
				}
			}
			else
			{
				m_blur_pingpong_2.Bind();
				OGLShader::SetUniform( m_blur_shader.GetShaderID(), "uHorizontal", horizontal );
				if ( first_iteration )
				{
					glActiveTexture( GL_TEXTURE0 );
					glBindTexture( GL_TEXTURE_2D, texture );
				}
				else
				{
					glActiveTexture( GL_TEXTURE0 );
					glBindTexture( GL_TEXTURE_2D, m_blur_pingpong_1.GetTexture() );
				}
			}

			m_square_mesh.Draw( GL_TRIANGLES );

			horizontal = !horizontal;
			if ( first_iteration )
				first_iteration = false;
		}
		OGLShader::UnUse();
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );

		return m_blur_pingpong_1;
	}

	void OpenGL::RenderBlendTextures( unsigned int texture1, unsigned int texture2 )
	{
		m_blend_shader.Use();

		OGLShader::SetUniform( m_blend_shader.GetShaderID(), "uScene", 0 );
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, texture1 );
		OGLShader::SetUniform( m_blend_shader.GetShaderID(), "uBloomBlur", 1 );
		glActiveTexture( GL_TEXTURE0 + 1 );
		glBindTexture( GL_TEXTURE_2D, texture2 );

		m_square_mesh.Draw( GL_TRIANGLES );

		OGLShader::UnUse();
	}
}