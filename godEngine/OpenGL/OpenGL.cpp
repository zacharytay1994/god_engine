#include "../pch.h"
#include "OpenGL.h"
#include "../godUtility/Scene.h"
#include <godUtility/Math.h>
#include "../Window/DeltaTimer.h"
#include "../Window/SystemTimer.h"

#include "Internal/OGLDebug.h"
#include <godUtility/Internal/FolderHelper.h>

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
	OpenGL::Lines OpenGL::m_lines {};

	OpenGL::OpenGL ( HWND windowHandle , int width , int height )
		:
		m_screen_width { width } ,
		m_screen_height { height } ,
		m_window_device_context { GetDC ( windowHandle ) }
	{
		// Load opengl functions with glad
		if ( !gladLoadGL () )
		{
			std::cerr << "Failed to initialize GLAD" << std::endl;
			return;
		}

		ResizeViewport ( width , height );

		// OpenGl debug features
		int flags;
		// Features of contexts can be detected via context flags
		glGetIntegerv ( GL_CONTEXT_FLAGS , &flags );
		// Check if the context is a debug context
		if ( flags & GL_CONTEXT_FLAG_DEBUG_BIT )
		{
			OGLDebug::EnableOpenGLDebugging ();
			std::cout << "Debug Mode" << std::endl;
		}

		// Create flat shader
		m_flat_shader.InitializeFromFile (
			"Assets/EngineAssets/OpenGLShaders/flatcolour.vs" ,
			"Assets/EngineAssets/OpenGLShaders/flatcolour.fs" );

		// Create textured shader
		m_textured_shader.InitializeFromFile (
			"Assets/EngineAssets/OpenGLShaders/texturemaps.vs" ,
			"Assets/EngineAssets/OpenGLShaders/texturemaps.fs" );

		// Create textured shader with discard
		m_textured_discard_shader.InitializeFromFile (
			"Assets/EngineAssets/OpenGLShaders/texturemaps.vs" ,
			"Assets/EngineAssets/OpenGLShaders/texturemapsWithDiscard.fs" );

		m_textured_discard_shader_no_lighting.InitializeFromFile (
			"Assets/EngineAssets/OpenGLShaders/texturemaps.vs" ,
			"Assets/EngineAssets/OpenGLShaders/texturemapsWithDiscardNoLighting.fs" );

		m_single_colour_outline_shader.InitializeFromFile (
			"Assets/EngineAssets/OpenGLShaders/single_colour_outline.vs" ,
			"Assets/EngineAssets/OpenGLShaders/single_colour_outline.fs" );

		// Create cubemap shader
		m_cubemap_shader.InitializeFromFile (
			"Assets/EngineAssets/OpenGLShaders/skybox.vs" ,
			"Assets/EngineAssets/OpenGLShaders/skybox.fs" );

		// Create depthmap shader
		m_depthmap_shader.InitializeFromFile (
			"Assets/EngineAssets/OpenGLShaders/depthmap.vs" ,
			"Assets/EngineAssets/OpenGLShaders/depthmap.fs" );

		m_anim_depthmap_shader.InitializeFromFile (
			"Assets/EngineAssets/OpenGLShaders/anim_depthmap.vs" ,
			"Assets/EngineAssets/OpenGLShaders/depthmap.fs" );

		// Create 2D shader
		m_2D_shader.InitializeFromFile (
			"Assets/EngineAssets/OpenGLShaders/texturemaps2D.vs" ,
			"Assets/EngineAssets/OpenGLShaders/texturemaps2D.fs" );

		// Create font shader
		m_font_shader.InitializeFromFile (
			"Assets/EngineAssets/OpenGLShaders/font.vs" ,
			"Assets/EngineAssets/OpenGLShaders/font.fs" );

		// Calculation for shadow map
		m_light_space_matrix =
			glm::ortho ( -10.0f , 10.0f , -10.0f , 10.0f , 1.0f , 20.0f ) *
			glm::lookAt (
				glm::vec3 ( 10.0f , 10.0f , 10.0f ) ,
				glm::vec3 ( 0.0f , 0.0f , 0.0f ) ,
				glm::vec3 ( 0.0f , 1.0f , 0.0f ) );

		// opengl settings
		glEnable ( GL_CULL_FACE );
		// Enables the Depth Buffer
		glEnable ( GL_DEPTH_TEST );
		// Enables the Stencil Buffer
		glEnable ( GL_STENCIL_TEST );
		// Sets rules for outcomes of stecil tests
		glStencilOp ( GL_KEEP , GL_KEEP , GL_REPLACE );

		// glEnable ( GL_DEPTH_TEST );
		SetLineWidth ( m_default_line_size );

		// Temp line rendering code
		constexpr int MAX_POINTS { 10'000 };
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

		m_line_shader.InitializeFromCode ( line_vs , line_fs );
		m_lines_mesh.m_vertices.resize ( MAX_POINTS );
		m_lines_mesh.m_indices.resize ( MAX_POINTS );
		m_lines_mesh.Initialize ();

		// Creating cube map
		m_cubemap.Initialize ();
		std::string skybox_model_path = "Assets/GameAssets/2DAssets/Raw/Skybox/";
		// All the faces of the cubemap (make sure they are in this exact order)
		std::string facesCubemap[ 6 ] =
		{
			skybox_model_path + "right.jpg",
			skybox_model_path + "left.jpg",
			skybox_model_path + "top.jpg",
			skybox_model_path + "bottom.jpg",
			skybox_model_path + "front.jpg",
			skybox_model_path + "back.jpg" };
		m_cubemap.CubeTexture ( facesCubemap );

		// Creating shadow map
		m_shadowmap.Initialize ( 2048 , 2048 );

		// m_square
		m_square_mesh.m_vertices = {
			{{ 1.0f,  1.0f, 0.0f }, { 1.0f, 1.0f }},  // top right
			{{ 1.0f, -1.0f, 0.0f }, { 1.0f, 0.0f }},  // bottom right
			{{-1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f }},  // bottom left
			{{-1.0f,  1.0f, 0.0f }, { 0.0f, 1.0f }}	  // top left 
		};
		m_square_mesh.m_indices = {
			3, 1, 0,   // first triangle
			3, 2, 1    // second triangle
		};
		m_square_mesh.Initialize ();

		// load hdr shader
		m_hdr_shader.InitializeFromFile (
			"Assets/EngineAssets/OpenGLShaders/tonemap.vs" ,
			"Assets/EngineAssets/OpenGLShaders/tonemap.fs"
		);

		// blur renderpasses
		m_blur_pingpong_1.Initialize ( width , height );
		m_blur_pingpong_2.Initialize ( width , height );

		// blur shader
		m_blur_shader.InitializeFromFile (
			"Assets/EngineAssets/OpenGLShaders/blur.vs" ,
			"Assets/EngineAssets/OpenGLShaders/blur.fs"
		);

		// blend shader
		m_blend_shader.InitializeFromFile (
			"Assets/EngineAssets/OpenGLShaders/blend.vs" ,
			"Assets/EngineAssets/OpenGLShaders/blend.fs"
		);

		m_causticmap_textures = OGLTexture ( "Assets/EngineAssets/Textures/CausticMap.png" );

		//----------------------------------------
		// Tested on file format : .dae / .md5mesh / .fbx
		//std::string animated_path = "Assets/GameAssets/3DAssets/Animation/Vampire/dancing_vampire.dae";
		//std::string animated_path = "Assets/GameAssets/3DAssets/Animation/Bob_lamp/bob_lamp_update_export.md5mesh";
		//std::string animated_path = "Assets/GameAssets/3DAssets/Animation/skeleton.fbx";
		//std::string animated_path = "Assets/GameAssets/3DAssets/Animation/JonathanKwekSample/Bipedal01_TestAnim_ImportReexportMax.fbx";

		std::string animated_path = "Assets/GameAssets/3DAssets/Animation/Shark/Correctshark002.dae";
		m_animation_model = Animation3D::Model ( animated_path );
		m_animation_dance = Animation3D::Animation ( animated_path , &m_animation_model );
		m_animator = Animation3D::Animator ( &m_animation_dance );

		m_animation_shader.InitializeFromFile (
			"Assets/EngineAssets/OpenGLShaders/anim_model.vs" ,
			"Assets/EngineAssets/OpenGLShaders/texturemaps.fs"
		);

		LoadAllAnimations ();

		//----------------------------------------

		// initialize fonts
		m_font_renderer.Initialize ();

		glCheckError ();
		std::cout << "OpenGL constructed." << std::endl;
	}

	OpenGL::~OpenGL ()
	{
		// wglDeleteContext ( m_opengl_rendering_context );
	}

	void OpenGL::ClearColour () const
	{
		glClearColor ( 0.0f , 0.0f , 0.0f , 1.0f );
		glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
	}

	void OpenGL::BuildOGLModels ( Asset3DManager const& asset3DManager )
	{
		// Clear current mesh list
		m_models.clear ();

		// Copy meshes
		for ( auto const& asset : asset3DManager.GetResources () )
		{
			m_models.emplace_back ();
			BuildOGLMeshesFromAssimpMeshes ( m_models.back () , std::get<1> ( asset ).m_model.m_meshes );
		}

		// Copy mesh ids
		m_model_ids = asset3DManager.GetIDs ();
	}

	void OpenGL::UpdateOGLModel ( ResourceID id , Asset3DManager const& asset3DManager )
	{
		m_models[ id ].clear ();
		BuildOGLMeshesFromAssimpMeshes ( m_models[ id ] , std::get<1> ( asset3DManager.Get ( id ) ).m_model.m_meshes );
		// Copy mesh ids
		m_model_ids = asset3DManager.GetIDs ();
	}

	void OpenGL::RenderScene ( Scene& scene ,
		glm::mat4 const& projection ,
		glm::mat4 const& view ,
		glm::vec3 const& camera_position ,
		OGLTextureManager& textures ,
		glm::vec3 const& camera_front )
	{
		( camera_front );
		// Update animation transformation 
		//m_animator.UpdateAnimation ( DeltaTimer::m_dt ); //--

		glViewport ( 0 , 0 , m_screen_width , m_screen_height );
		ClearColour ();

		// Draw the normal model
		m_textured_shader.Use ();

		// Reset point light uniforms
		OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uNumPointLight" , 0 );

		// Reset directional light uniforms
		OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uNumDirectionalLight" , 0 );

		// projection matrix
		OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uProjection" , projection );

		// view matrix
		OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uView" , view );

		// set view position
		OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uViewPosition" , camera_position );

		for ( auto const& data : scene.m_instanced_render_data )
		{
			if ( data.first.m_outlined )
			{
				// Make it so the stencil test always passes
				glStencilFunc ( GL_ALWAYS , 1 , 0xFF );
				// Enable modifying of the stencil buffer
				glStencilMask ( 0xFF );
			}

			// Draw the normal model
			m_textured_shader.Use ();

			OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uSpritesheetData" , data.first.m_spritesheet_data );

			// set material
			OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uMaterial.diffuse_map" , 0 );
			std::get<1> ( textures.Get ( data.first.m_diffuse_id ) ).Bind ( 0 );
			OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uMaterial.specular_map" , 1 );
			std::get<1> ( textures.Get ( data.first.m_specular_id ) ).Bind ( 1 );
			OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uMaterial.shininess" , data.first.m_shininess );

			// set reflection
			OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uSkybox" , 2 );
			m_cubemap.Bind ( 2 );

			// set shadowmap
			OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uShadowMap" , 3 );
			m_shadowmap.Bind ( 3 );

			// bind caustic map textures
			OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uCausticMap" , 8 );
			m_causticmap_textures.Bind ( 8 );

			// pass in delta time into shader 
			OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uDT" , DeltaTimer::m_acc_dt );

			// render point lights
			std::sort ( scene.m_point_light_data.begin () , scene.m_point_light_data.end () ,
				[&camera_position]( Scene::PointLightData const& pld1 , Scene::PointLightData const& pld2 )
				{
					return LengthSq ( pld1.m_position - camera_position ) < LengthSq ( pld2.m_position - camera_position );
				}
			);
			int num_point_light = static_cast< int >( scene.m_point_light_data.size () > m_max_point_lights ? m_max_point_lights : scene.m_point_light_data.size () );
			OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uNumPointLight" , num_point_light );
			for ( auto i = 0; i < num_point_light; ++i )
			{
				auto const& light = scene.m_point_light_data[ i ];

				// max 5 point lights as defined in the shader
				OGLShader::SetUniform ( m_textured_shader.GetShaderID () , ( "uPointLight[" + std::to_string ( i ) + "].position" ).c_str () , light.m_position );
				OGLShader::SetUniform ( m_textured_shader.GetShaderID () , ( "uPointLight[" + std::to_string ( i ) + "].colour" ).c_str () , light.m_colour );
				OGLShader::SetUniform ( m_textured_shader.GetShaderID () , ( "uPointLight[" + std::to_string ( i ) + "].ambient" ).c_str () , light.m_ambient );
				OGLShader::SetUniform ( m_textured_shader.GetShaderID () , ( "uPointLight[" + std::to_string ( i ) + "].diffuse" ).c_str () , light.m_diffuse );
				OGLShader::SetUniform ( m_textured_shader.GetShaderID () , ( "uPointLight[" + std::to_string ( i ) + "].specular" ).c_str () , light.m_specular );
			}

			// render directional lights
			std::sort ( scene.m_directional_light_data.begin () , scene.m_directional_light_data.end () ,
				[&camera_position]( Scene::DirectionalLightData const& dld1 , Scene::DirectionalLightData const& dld2 )
				{
					return LengthSq ( dld1.m_position - camera_position ) < LengthSq ( dld2.m_position - camera_position );
				}
			);
			int num_directional_light = static_cast< int >( scene.m_directional_light_data.size () > m_max_directional_lights ? m_max_directional_lights : scene.m_directional_light_data.size () );
			OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uNumDirectionalLight" , num_directional_light );
			for ( auto i = 0; i < num_directional_light; ++i )
			{
				auto const& light = scene.m_directional_light_data[ i ];
				OGLShader::SetUniform ( m_textured_shader.GetShaderID () , ( "uDirectionalLight[" + std::to_string ( i ) + "].position" ).c_str () , light.m_position );
				OGLShader::SetUniform ( m_textured_shader.GetShaderID () , ( "uDirectionalLight[" + std::to_string ( i ) + "].direction" ).c_str () , -glm::normalize ( light.m_position ) );
				OGLShader::SetUniform ( m_textured_shader.GetShaderID () , ( "uDirectionalLight[" + std::to_string ( i ) + "].colour" ).c_str () , light.m_colour );
				OGLShader::SetUniform ( m_textured_shader.GetShaderID () , ( "uDirectionalLight[" + std::to_string ( i ) + "].ambient" ).c_str () , light.m_ambient );
				OGLShader::SetUniform ( m_textured_shader.GetShaderID () , ( "uDirectionalLight[" + std::to_string ( i ) + "].diffuse" ).c_str () , light.m_diffuse );
				OGLShader::SetUniform ( m_textured_shader.GetShaderID () , ( "uDirectionalLight[" + std::to_string ( i ) + "].specular" ).c_str () , light.m_specular );

				m_light_space_matrix =
					glm::ortho ( -20.0f , 20.0f , -20.0f , 20.0f , 1.0f , 20.0f ) *
					glm::lookAt (
						glm::vec3 ( light.m_position ) ,
						glm::vec3 ( 0.0f , 0.0f , 0.0f ) ,
						glm::vec3 ( 0.0f , 1.0f , 0.0f ) );

				OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uLightSpaceMatrix" , m_light_space_matrix );
			}

			// Set Fog
			OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uFogParams.color" , { 0.45f,0.65f,0.90f } );
			OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uFogParams.linearStart" , 150.0f );
			OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uFogParams.linearEnd" , 300.0f );
			OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uFogParams.density" , 0.03f );
			OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uFogParams.equation" , 0 );
			OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uFogParams.isEnabled" , true );

			// Set Tint
			OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uTint" , data.first.m_tint );

			OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uEmissive" , data.first.m_emissive );

			// draw model
			for ( auto& mesh : m_models[ data.first.m_model_id ] )
			{
				mesh.SetTransformData ( data.second );
				mesh.DrawInstanced ( GL_TRIANGLES );
			}

			// Make it so only the pixels without the value 1 pass the test
			if ( data.first.m_outlined )
			{
				glStencilFunc ( GL_NOTEQUAL , 1 , 0xFF );
				// Disable modifying of the stencil buffer
				glStencilMask ( 0x00 );
				// Disable the depth buffer
				//glDisable( GL_DEPTH_TEST );

				m_single_colour_outline_shader.Use ();
				OGLShader::SetUniform ( m_single_colour_outline_shader.GetShaderID () , "uProjection" , projection );
				OGLShader::SetUniform ( m_single_colour_outline_shader.GetShaderID () , "uView" , view );
				OGLShader::SetUniform ( m_single_colour_outline_shader.GetShaderID () , "uOutlining" , 0.05f );

				for ( auto& mesh : m_models[ data.first.m_model_id ] )
				{
					mesh.SetTransformData ( data.second );
					mesh.DrawInstanced ( GL_TRIANGLES );
				}

				// Enable modifying of the stencil buffer
				glStencilMask ( 0xFF );
				// Clear stencil buffer
				glStencilFunc ( GL_ALWAYS , 1 , 0xFF );
				// Enable the depth buffer
				//glEnable( GL_DEPTH_TEST );
			}
		}

		// [Draw Billboard Sprites]
		// billboard sprites are not depth tested and not backface culled

		// view matrix
		//OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uView" , viewNoRot );

		// Draw skybox as last
		glCullFace ( GL_CW );
		m_cubemap.CubeMapEnableDepth ();
		m_cubemap_shader.Use ();
		OGLShader::SetUniform ( m_cubemap_shader.GetShaderID () , "view" , view );
		OGLShader::SetUniform ( m_cubemap_shader.GetShaderID () , "projection" , projection );
		OGLShader::SetUniform ( m_cubemap_shader.GetShaderID () , "camera" , camera_position );
		m_cubemap.Bind ();
		m_cubemap.Draw ();
		m_cubemap.UnBind ();
		m_cubemap.CubeMapDisableDepth ();
		glCullFace ( GL_CCW );

		// Render all transparent objects after cubemap
		RenderTransparent ( scene , projection , view , camera_position , textures , camera_front );

		//[SPACE]-----------------------------------------------------------------------------------

		//[Animation - START]-----------------------------------------------------------------------

		RenderAnimations ( scene , projection , view , camera_position , textures , camera_front );
		// Draw the normal model
		//m_animation_shader.Use ();

		//// reset point light uniforms
		//OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uNumPointLight" , 0 );

		//// reset directional light uniforms
		//OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uNumDirectionalLight" , 0 );

		//auto transforms = m_animator.GetFinalBoneMatrices ();
		//for ( int i = 0; i < transforms.size (); ++i )
		//{
		//	std::string temp = "bone_transforms[" + std::to_string ( i ) + "]";
		//	OGLShader::SetUniform ( m_animation_shader.GetShaderID () , temp.c_str () , transforms[ i ] );
		//}

		//// projection matrix
		//OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uProjection" , projection );

		//// view matrix
		//OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uView" , view );

		//// set uniforms for fragment shader
		//// set view position
		//OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uViewPosition" , camera_position );

		//// set material
		//OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uMaterial.diffuse_map" , 0 );
		//std::get<1> ( textures.Get ( textures.GetID ( "White" ) ) ).Bind ( 0 );
		//OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uMaterial.specular_map" , 1 );
		//std::get<1> ( textures.Get ( textures.GetID ( "White" ) ) ).Bind ( 1 );
		//OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uMaterial.shininess" , 32.0f );

		//// set reflection
		//OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uSkybox" , 2 );
		//m_cubemap.Bind ( 2 );

		//// set shadowmap
		//OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uShadowMap" , 3 );
		//m_shadowmap.Bind ( 3 );

		//// bind caustic map textures
		//OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uCausticMap" , 8 );
		////m_causticmap_textures.Bind( 8 );
		//std::get<1> ( textures.Get ( textures.GetID ( "None" ) ) ).Bind ( 8 );

		//// pass in delta time into shader 
		//OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uDT" , DeltaTimer::m_acc_dt );

		//// render point lights
		//std::sort ( scene.m_point_light_data.begin () , scene.m_point_light_data.end () ,
		//	[&camera_position]( Scene::PointLightData const& pld1 , Scene::PointLightData const& pld2 )
		//	{
		//		return LengthSq ( pld1.m_position - camera_position ) < LengthSq ( pld2.m_position - camera_position );
		//	}
		//);
		//int num_point_light = static_cast< int >( scene.m_point_light_data.size () > m_max_point_lights ? m_max_point_lights : scene.m_point_light_data.size () );
		//OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uNumPointLight" , num_point_light );
		//for ( auto i = 0; i < num_point_light; ++i )
		//{
		//	auto const& light = scene.m_point_light_data[ i ];

		//	// max 5 point lights as defined in the shader
		//	OGLShader::SetUniform ( m_animation_shader.GetShaderID () , ( "uPointLight[" + std::to_string ( i ) + "].position" ).c_str () , light.m_position );
		//	OGLShader::SetUniform ( m_animation_shader.GetShaderID () , ( "uPointLight[" + std::to_string ( i ) + "].colour" ).c_str () , light.m_colour );
		//	OGLShader::SetUniform ( m_animation_shader.GetShaderID () , ( "uPointLight[" + std::to_string ( i ) + "].ambient" ).c_str () , light.m_ambient );
		//	OGLShader::SetUniform ( m_animation_shader.GetShaderID () , ( "uPointLight[" + std::to_string ( i ) + "].diffuse" ).c_str () , light.m_diffuse );
		//	OGLShader::SetUniform ( m_animation_shader.GetShaderID () , ( "uPointLight[" + std::to_string ( i ) + "].specular" ).c_str () , light.m_specular );
		//}

		//// render directional lights
		//std::sort ( scene.m_directional_light_data.begin () , scene.m_directional_light_data.end () ,
		//	[&camera_position]( Scene::DirectionalLightData const& dld1 , Scene::DirectionalLightData const& dld2 )
		//	{
		//		return LengthSq ( dld1.m_position - camera_position ) < LengthSq ( dld2.m_position - camera_position );
		//	}
		//);
		//int num_directional_light = static_cast< int >( scene.m_directional_light_data.size () > m_max_directional_lights ? m_max_directional_lights : scene.m_directional_light_data.size () );
		//OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uNumDirectionalLight" , num_directional_light );
		//for ( auto i = 0; i < num_directional_light; ++i )
		//{
		//	auto const& light = scene.m_directional_light_data[ i ];
		//	OGLShader::SetUniform ( m_animation_shader.GetShaderID () , ( "uDirectionalLight[" + std::to_string ( i ) + "].position" ).c_str () , light.m_position );
		//	OGLShader::SetUniform ( m_animation_shader.GetShaderID () , ( "uDirectionalLight[" + std::to_string ( i ) + "].direction" ).c_str () , -glm::normalize ( light.m_position ) );
		//	OGLShader::SetUniform ( m_animation_shader.GetShaderID () , ( "uDirectionalLight[" + std::to_string ( i ) + "].colour" ).c_str () , light.m_colour );
		//	OGLShader::SetUniform ( m_animation_shader.GetShaderID () , ( "uDirectionalLight[" + std::to_string ( i ) + "].ambient" ).c_str () , light.m_ambient );
		//	OGLShader::SetUniform ( m_animation_shader.GetShaderID () , ( "uDirectionalLight[" + std::to_string ( i ) + "].diffuse" ).c_str () , light.m_diffuse );
		//	OGLShader::SetUniform ( m_animation_shader.GetShaderID () , ( "uDirectionalLight[" + std::to_string ( i ) + "].specular" ).c_str () , light.m_specular );

		//	m_light_space_matrix =
		//		glm::ortho ( -20.0f , 20.0f , -20.0f , 20.0f , 1.0f , 20.0f ) *
		//		glm::lookAt (
		//			glm::vec3 ( light.m_position ) ,
		//			glm::vec3 ( 0.0f , 0.0f , 0.0f ) ,
		//			glm::vec3 ( 0.0f , 1.0f , 0.0f ) );

		//	OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uLightSpaceMatrix" , m_light_space_matrix );
		//}

		//// Set Fog
		//OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uFogParams.color" , { 0.45f,0.65f,0.90f } );
		//OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uFogParams.linearStart" , 10.0f );
		//OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uFogParams.linearEnd" , 100.0f );
		//OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uFogParams.density" , 0.03f );
		//OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uFogParams.equation" , 0 );
		//OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uFogParams.isEnabled" , false );

		//// Set Tint
		//OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uIsTint" , false );
		////OGLShader::SetUniform( m_animation_shader.GetShaderID(), "uTint", glm::vec4( 0.8f, 0.8f, 0.8f, 1.0f ) );

		//// draw model
		//glm::mat4 model = glm::mat4 ( 1.0f );

		//model = glm::translate ( model , glm::vec3 ( 50.0f , 1.0f , 5.0f ) ); // translate it down so it's at the center of the scene
		//model = glm::rotate ( model , glm::radians ( 90.0f ) , glm::vec3 ( 0.0f , 1.0f , 0.0f ) );// rotate at y-axis
		//model = glm::scale ( model , glm::vec3 ( 1.0f ) );	// it's a bit too big for our scene, so scale it down
		//OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uModel" , model );

		//OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uEmissive" , 100.0f );
		//m_animation_model.Draw ( m_animation_shader );




		//[Animation - END]-------------------------------------------------------------------------

		//[SPACE]-----------------------------------------------------------------------------------

		//// Draw skybox as last
		//glCullFace ( GL_CW );
		//m_cubemap.CubeMapEnableDepth ();
		//m_cubemap_shader.Use ();
		//OGLShader::SetUniform ( m_cubemap_shader.GetShaderID () , "view" , view );
		//OGLShader::SetUniform ( m_cubemap_shader.GetShaderID () , "projection" , projection );
		//OGLShader::SetUniform ( m_cubemap_shader.GetShaderID () , "camera" , camera_position );
		//m_cubemap.Bind ();
		//m_cubemap.Draw ();
		//m_cubemap.UnBind ();
		//m_cubemap.CubeMapDisableDepth ();
		//glCullFace ( GL_CCW );

		// Unuse the bound shader
		OGLShader::UnUse ();
	}

	void OpenGL::RenderGUI ( Scene& scene , glm::mat4 const& projection , OGLTextureManager& textures )
	{
		// enable blending
		glEnable ( GL_BLEND );
		glBlendFunc ( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );
		glDisable ( GL_CULL_FACE );

		m_2D_shader.Use ();

		OGLShader::SetUniform ( m_2D_shader.GetShaderID () , "uProjection" , projection );

		SystemTimer::StartTimeSegment ( "Sorting GUI" );
		using RenderDataReference = std::tuple<Scene::InstancedRenderData , std::reference_wrapper<std::vector<glm::mat4>>>;
		std::vector<RenderDataReference> sorted_render_data;
		sorted_render_data.reserve ( scene.m_2D_instanced_render_data.size () );
		for ( auto& data : scene.m_2D_instanced_render_data )
		{
			if ( !data.second.empty () )
			{
				sorted_render_data.push_back ( { data.first, data.second } );
			}
		}
		std::sort ( sorted_render_data.begin () , sorted_render_data.end () , []( auto const& lhs , auto const& rhs )
			{
				return std::get<1> ( lhs ).get ()[ 0 ][ 3 ].z < std::get<1> ( rhs ).get ()[ 0 ][ 3 ].z;
			} );
		SystemTimer::EndTimeSegment ( "Sorting GUI" );

		for ( auto const& data : sorted_render_data )
		{
			auto& [first , second] = data;

			OGLShader::SetUniform ( m_textured_discard_shader.GetShaderID () , "uSpritesheetData" , first.m_spritesheet_data );

			OGLShader::SetUniform ( m_2D_shader.GetShaderID () , "uMaterial.diffuse_map" , 0 );
			OGLShader::SetUniform ( m_2D_shader.GetShaderID () , "uAlpha" , first.m_shininess );
			std::get<1> ( textures.Get ( first.m_diffuse_id ) ).Bind ( 0 );

			for ( auto& mesh : m_models[ first.m_model_id ] )
			{
				mesh.SetTransformData ( second.get () );
				mesh.DrawInstanced ( GL_TRIANGLES );
			}
		}

		m_font_shader.Use ();

		OGLShader::SetUniform ( m_2D_shader.GetShaderID () , "uProjection" , projection );

		for ( auto const& data : scene.m_characters )
		{
			OGLShader::SetUniform ( m_font_shader.GetShaderID () , "uMaterial.diffuse_map" , 0 );
			glActiveTexture ( GL_TEXTURE0 + 0 );
			glBindTexture ( GL_TEXTURE_2D , data.first.m_diffuse_id );

			for ( auto& mesh : m_models[ data.first.m_model_id ] )
			{
				mesh.SetTransformData ( data.second );
				mesh.DrawInstanced ( GL_TRIANGLES );
			}
		}

		OGLShader::UnUse ();
		glDisable ( GL_BLEND );
		glEnable ( GL_CULL_FACE );
	}

	void OpenGL::RenderTransparent ( Scene& scene , glm::mat4 const& projection , glm::mat4 const& view , glm::vec3 const& camera_position , OGLTextureManager& textures , glm::vec3 const& camera_front )
	{
		( camera_front );
		// Draw the normal model
		m_textured_discard_shader.Use ();

		// Reset point light uniforms
		OGLShader::SetUniform ( m_textured_discard_shader.GetShaderID () , "uNumPointLight" , 0 );

		// Reset directional light uniforms
		OGLShader::SetUniform ( m_textured_discard_shader.GetShaderID () , "uNumDirectionalLight" , 0 );

		// projection matrix
		OGLShader::SetUniform ( m_textured_discard_shader.GetShaderID () , "uProjection" , projection );

		// view matrix
		OGLShader::SetUniform ( m_textured_discard_shader.GetShaderID () , "uView" , view );

		// set view position
		OGLShader::SetUniform ( m_textured_discard_shader.GetShaderID () , "uViewPosition" , camera_position );

		glEnable ( GL_BLEND );
		glBlendFunc ( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );
		glDisable ( GL_CULL_FACE );

		// billboard with lighting
		for ( auto const& data : scene.m_billboard_sprites )
		{
			// Draw the normal model
			m_textured_discard_shader.Use ();

			OGLShader::SetUniform ( m_textured_discard_shader.GetShaderID () , "uSpritesheetData" , data.first.m_spritesheet_data );

			// set material
			OGLShader::SetUniform ( m_textured_discard_shader.GetShaderID () , "uMaterial.diffuse_map" , 0 );
			std::get<1> ( textures.Get ( data.first.m_diffuse_id ) ).Bind ( 0 );
			OGLShader::SetUniform ( m_textured_discard_shader.GetShaderID () , "uMaterial.specular_map" , 1 );
			std::get<1> ( textures.Get ( data.first.m_specular_id ) ).Bind ( 1 );
			OGLShader::SetUniform ( m_textured_discard_shader.GetShaderID () , "uMaterial.shininess" , data.first.m_shininess );

			// set reflection
			OGLShader::SetUniform ( m_textured_discard_shader.GetShaderID () , "uSkybox" , 2 );
			m_cubemap.Bind ( 2 );

			// set shadowmap
			OGLShader::SetUniform ( m_textured_discard_shader.GetShaderID () , "uShadowMap" , 3 );
			m_shadowmap.Bind ( 3 );

			// bind caustic map textures
			OGLShader::SetUniform ( m_textured_discard_shader.GetShaderID () , "uCausticMap" , 8 );
			m_causticmap_textures.Bind ( 8 );

			// pass in delta time into shader 
			OGLShader::SetUniform ( m_textured_discard_shader.GetShaderID () , "uDT" , DeltaTimer::m_acc_dt );

			// render point lights
			std::sort ( scene.m_point_light_data.begin () , scene.m_point_light_data.end () ,
				[&camera_position]( Scene::PointLightData const& pld1 , Scene::PointLightData const& pld2 )
				{
					return LengthSq ( pld1.m_position - camera_position ) < LengthSq ( pld2.m_position - camera_position );
				}
			);
			int num_point_light = static_cast< int >( scene.m_point_light_data.size () > m_max_point_lights ? m_max_point_lights : scene.m_point_light_data.size () );
			OGLShader::SetUniform ( m_textured_discard_shader.GetShaderID () , "uNumPointLight" , num_point_light );
			for ( auto i = 0; i < num_point_light; ++i )
			{
				auto const& light = scene.m_point_light_data[ i ];

				// max 5 point lights as defined in the shader
				OGLShader::SetUniform ( m_textured_discard_shader.GetShaderID () , ( "uPointLight[" + std::to_string ( i ) + "].position" ).c_str () , light.m_position );
				OGLShader::SetUniform ( m_textured_discard_shader.GetShaderID () , ( "uPointLight[" + std::to_string ( i ) + "].colour" ).c_str () , light.m_colour );
				OGLShader::SetUniform ( m_textured_discard_shader.GetShaderID () , ( "uPointLight[" + std::to_string ( i ) + "].ambient" ).c_str () , light.m_ambient );
				OGLShader::SetUniform ( m_textured_discard_shader.GetShaderID () , ( "uPointLight[" + std::to_string ( i ) + "].diffuse" ).c_str () , light.m_diffuse );
				OGLShader::SetUniform ( m_textured_discard_shader.GetShaderID () , ( "uPointLight[" + std::to_string ( i ) + "].specular" ).c_str () , light.m_specular );
			}

			// render directional lights
			std::sort ( scene.m_directional_light_data.begin () , scene.m_directional_light_data.end () ,
				[&camera_position]( Scene::DirectionalLightData const& dld1 , Scene::DirectionalLightData const& dld2 )
				{
					return LengthSq ( dld1.m_position - camera_position ) < LengthSq ( dld2.m_position - camera_position );
				}
			);
			int num_directional_light = static_cast< int >( scene.m_directional_light_data.size () > m_max_directional_lights ? m_max_directional_lights : scene.m_directional_light_data.size () );
			OGLShader::SetUniform ( m_textured_discard_shader.GetShaderID () , "uNumDirectionalLight" , num_directional_light );
			for ( auto i = 0; i < num_directional_light; ++i )
			{
				auto const& light = scene.m_directional_light_data[ i ];
				OGLShader::SetUniform ( m_textured_discard_shader.GetShaderID () , ( "uDirectionalLight[" + std::to_string ( i ) + "].position" ).c_str () , light.m_position );
				OGLShader::SetUniform ( m_textured_discard_shader.GetShaderID () , ( "uDirectionalLight[" + std::to_string ( i ) + "].direction" ).c_str () , -glm::normalize ( light.m_position ) );
				OGLShader::SetUniform ( m_textured_discard_shader.GetShaderID () , ( "uDirectionalLight[" + std::to_string ( i ) + "].colour" ).c_str () , light.m_colour );
				OGLShader::SetUniform ( m_textured_discard_shader.GetShaderID () , ( "uDirectionalLight[" + std::to_string ( i ) + "].ambient" ).c_str () , light.m_ambient );
				OGLShader::SetUniform ( m_textured_discard_shader.GetShaderID () , ( "uDirectionalLight[" + std::to_string ( i ) + "].diffuse" ).c_str () , light.m_diffuse );
				OGLShader::SetUniform ( m_textured_discard_shader.GetShaderID () , ( "uDirectionalLight[" + std::to_string ( i ) + "].specular" ).c_str () , light.m_specular );

				m_light_space_matrix =
					glm::ortho ( -20.0f , 20.0f , -20.0f , 20.0f , 1.0f , 20.0f ) *
					glm::lookAt (
						glm::vec3 ( light.m_position ) ,
						glm::vec3 ( 0.0f , 0.0f , 0.0f ) ,
						glm::vec3 ( 0.0f , 1.0f , 0.0f ) );

				OGLShader::SetUniform ( m_textured_discard_shader.GetShaderID () , "uLightSpaceMatrix" , m_light_space_matrix );
			}

			// Set Fog
			OGLShader::SetUniform ( m_textured_discard_shader.GetShaderID () , "uFogParams.color" , { 0.45f,0.65f,0.90f } );
			OGLShader::SetUniform ( m_textured_discard_shader.GetShaderID () , "uFogParams.linearStart" , 10.0f );
			OGLShader::SetUniform ( m_textured_discard_shader.GetShaderID () , "uFogParams.linearEnd" , 100.0f );
			OGLShader::SetUniform ( m_textured_discard_shader.GetShaderID () , "uFogParams.density" , 0.03f );
			OGLShader::SetUniform ( m_textured_discard_shader.GetShaderID () , "uFogParams.equation" , 0 );
			OGLShader::SetUniform ( m_textured_discard_shader.GetShaderID () , "uFogParams.isEnabled" , true );

			// Set Tint
			OGLShader::SetUniform ( m_textured_discard_shader.GetShaderID () , "uTint" , glm::vec4 ( 1.0f ) );

			OGLShader::SetUniform ( m_textured_discard_shader.GetShaderID () , "uEmissive" , data.first.m_emissive );

			// draw model
			for ( auto& mesh : m_models[ data.first.m_model_id ] )
			{
				mesh.SetTransformData ( data.second );
				mesh.DrawInstanced ( GL_TRIANGLES );
			}
		}

		// billboard sprites with no lighting
		m_textured_discard_shader_no_lighting.Use ();

		// projection matrix
		OGLShader::SetUniform ( m_textured_discard_shader.GetShaderID () , "uProjection" , projection );

		// view matrix
		OGLShader::SetUniform ( m_textured_discard_shader.GetShaderID () , "uView" , view );

		for ( auto const& data : scene.m_billboard_sprites_no_lighting )
		{
			// Draw the normal model
			//m_textured_discard_shader_no_lighting.Use ();
			OGLShader::SetUniform ( m_textured_discard_shader_no_lighting.GetShaderID () , "uSpritesheetData" , data.first.m_spritesheet_data );

			// set material
			OGLShader::SetUniform ( m_textured_discard_shader_no_lighting.GetShaderID () , "uMaterial.diffuse_map" , 0 );
			std::get<1> ( textures.Get ( data.first.m_diffuse_id ) ).Bind ( 0 );
			OGLShader::SetUniform ( m_textured_discard_shader_no_lighting.GetShaderID () , "uMaterial.specular_map" , 1 );
			std::get<1> ( textures.Get ( data.first.m_specular_id ) ).Bind ( 1 );
			OGLShader::SetUniform ( m_textured_discard_shader_no_lighting.GetShaderID () , "uMaterial.shininess" , data.first.m_shininess );

			// Set Tint
			OGLShader::SetUniform ( m_textured_discard_shader_no_lighting.GetShaderID () , "uTint" , data.first.m_tint );

			OGLShader::SetUniform ( m_textured_discard_shader_no_lighting.GetShaderID () , "uEmissive" , data.first.m_emissive );

			// draw model
			for ( auto& mesh : m_models[ data.first.m_model_id ] )
			{
				mesh.SetTransformData ( data.second );
				mesh.DrawInstanced ( GL_TRIANGLES );
			}
		}
		glDisable ( GL_BLEND );
		glEnable ( GL_CULL_FACE );
	}

	void OpenGL::ResizeViewport ( int width , int height )
	{
		m_screen_width = width;
		m_screen_height = height;
		glViewport ( 0 , 0 , width , height );

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

	void OpenGL::DrawLine ( glm::vec3 const& a , glm::vec3 const& b , glm::vec4 const& c , float size )
	{
		m_lines[ size ].push_back ( { a, b, c, size } );
	}

	void OpenGL::RenderLines ( glm::mat4 const& projection , glm::mat4 const& view )
	{
		m_line_shader.Use ();

		m_line_shader.SetUniform ( m_line_shader.GetShaderID () , "uProjection" , projection );
		m_line_shader.SetUniform ( m_line_shader.GetShaderID () , "uView" , view );

		for ( auto const& line_group : m_lines )
		{
			m_lines_mesh.m_indices.clear ();
			m_lines_mesh.m_vertices.clear ();

			for ( auto const& line : line_group.second )
			{
				OGLMesh::OGLVertex start , end;
				start.m_position = std::get<0> ( line );
				end.m_position = std::get<1> ( line );
				start.m_colour = std::get<2> ( line );
				end.m_colour = std::get<2> ( line );

				m_lines_mesh.m_indices.push_back ( static_cast< Index3D >( m_lines_mesh.m_vertices.size () ) );
				m_lines_mesh.m_vertices.push_back ( start );
				m_lines_mesh.m_indices.push_back ( static_cast< Index3D >( m_lines_mesh.m_vertices.size () ) );
				m_lines_mesh.m_vertices.push_back ( end );
			}

			m_lines_mesh.SetData ();
			SetLineWidth ( line_group.first );
			m_lines_mesh.Draw ( GL_LINES );
		}

		OGLShader::UnUse ();
		m_lines.clear ();
	}

	void OpenGL::SetLineWidth ( float size )
	{
		glLineWidth ( size );
	}

	OGLMesh OpenGL::BuildOGLMeshFromAssimpMesh ( Mesh3D const& mesh3d ) const
	{
		OGLMesh mesh;

		// copy vertices
		mesh.m_vertices.resize ( mesh3d.m_vertices.size () );
		for ( auto i = 0; i < mesh3d.m_vertices.size (); ++i )
		{
			mesh.m_vertices[ i ].m_position = mesh3d.m_vertices[ i ].m_position;
			mesh.m_vertices[ i ].m_uv = mesh3d.m_vertices[ i ].m_uv;
			mesh.m_vertices[ i ].m_tangent = mesh3d.m_vertices[ i ].m_tangent;
			mesh.m_vertices[ i ].m_normal = mesh3d.m_vertices[ i ].m_normal;
			mesh.m_vertices[ i ].m_colour = mesh3d.m_vertices[ i ].m_colour;
		}

		// copy indices
		mesh.m_indices = mesh3d.m_indices;

		mesh.Initialize ();

		return mesh;
	}

	void OpenGL::BuildOGLMeshesFromAssimpMeshes ( std::vector<OGLMesh>& oglMeshes , std::vector<Mesh3D> const& meshes3D ) const
	{
		for ( auto& mesh : meshes3D )
		{
			oglMeshes.emplace_back ( BuildOGLMeshFromAssimpMesh ( mesh ) );
		}
	}

	void OpenGL::LoadAllAnimations ()
	{
		auto files = FolderHelper::GetFiles ( "Assets/GameAssets/3DAssets/Animation/" );
		for ( auto const& file : files )
		{
			if ( file.find ( ".fbx" ) != std::string::npos ||
				file.find ( ".dae" ) != std::string::npos )
			{
				std::string trimmed_name;
				size_t start_trim { std::string::npos };
				if ( ( start_trim = file.find_last_of ( '\\' ) ) != std::string::npos )
				{
					trimmed_name = file.substr ( start_trim + 1 , file.find_last_of ( '.' ) - start_trim - 1 );
				}
				else if ( ( start_trim = file.find_last_of ( '/' ) ) != std::string::npos )
				{
					trimmed_name = file.substr ( start_trim + 1 , file.find_last_of ( '.' ) - start_trim - 1 );
				}
				m_animations.insert ( { trimmed_name , {} } );
				// initialize with animation data
				m_animations[ trimmed_name ].Initialize ( file );
				m_animation_names.push_back ( trimmed_name );
			}
		}
	}

	void OpenGL::RenderAnimations ( Scene& scene , glm::mat4 const& projection , glm::mat4 const& view , glm::vec3 const& camera_position , OGLTextureManager& textures , glm::vec3 const& camera_front )
	{
		// update all animations
		//SystemTimer::StartTimeSegment ( "UpdateAnimations" );
		//for ( auto& animation : m_animations )
		//{
		//	std::vector<uint32_t> to_remove;
		//	for ( auto i = 0; i < animation.second.m_animators.Size (); ++i )
		//	{
		//		if ( animation.second.m_animators.Valid ( i ) )
		//		{
		//			if ( animation.second.m_animators[ i ].m_alive )
		//			{
		//				animation.second.m_animators[ i ].UpdateAnimation ( DeltaTimer::m_dt , animation.second.m_cached_node_transforms );
		//				animation.second.m_animators[ i ].m_alive = false;
		//			}
		//			else
		//			{
		//				// remove animator
		//				to_remove.push_back ( i );
		//			}
		//		}
		//	}
		//	for ( auto const& i : to_remove )
		//	{
		//		animation.second.m_animators.Erase ( i );
		//	}
		//}
		//SystemTimer::EndTimeSegment ( "UpdateAnimations" );

		m_animation_shader.Use ();

		// reset point light uniforms
		OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uNumPointLight" , 0 );

		// reset directional light uniforms
		OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uNumDirectionalLight" , 0 );

		// projection matrix
		OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uProjection" , projection );

		// view matrix
		OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uView" , view );

		// set uniforms for fragment shader
		// set view position
		OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uViewPosition" , camera_position );

		// set reflection
		OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uSkybox" , 2 );
		m_cubemap.Bind ( 2 );

		// set shadowmap
		OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uShadowMap" , 3 );
		m_shadowmap.Bind ( 3 );

		// bind caustic map textures
		OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uCausticMap" , 8 );
		//m_causticmap_textures.Bind( 8 );
		std::get<1> ( textures.Get ( textures.GetID ( "None" ) ) ).Bind ( 8 );

		// pass in delta time into shader 
		OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uDT" , DeltaTimer::m_acc_dt );

		// render point lights
		std::sort ( scene.m_point_light_data.begin () , scene.m_point_light_data.end () ,
			[&camera_position]( Scene::PointLightData const& pld1 , Scene::PointLightData const& pld2 )
			{
				return LengthSq ( pld1.m_position - camera_position ) < LengthSq ( pld2.m_position - camera_position );
			}
		);
		int num_point_light = static_cast< int >( scene.m_point_light_data.size () > m_max_point_lights ? m_max_point_lights : scene.m_point_light_data.size () );
		OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uNumPointLight" , num_point_light );
		for ( auto i = 0; i < num_point_light; ++i )
		{
			auto const& light = scene.m_point_light_data[ i ];

			// max 5 point lights as defined in the shader
			OGLShader::SetUniform ( m_animation_shader.GetShaderID () , ( "uPointLight[" + std::to_string ( i ) + "].position" ).c_str () , light.m_position );
			OGLShader::SetUniform ( m_animation_shader.GetShaderID () , ( "uPointLight[" + std::to_string ( i ) + "].colour" ).c_str () , light.m_colour );
			OGLShader::SetUniform ( m_animation_shader.GetShaderID () , ( "uPointLight[" + std::to_string ( i ) + "].ambient" ).c_str () , light.m_ambient );
			OGLShader::SetUniform ( m_animation_shader.GetShaderID () , ( "uPointLight[" + std::to_string ( i ) + "].diffuse" ).c_str () , light.m_diffuse );
			OGLShader::SetUniform ( m_animation_shader.GetShaderID () , ( "uPointLight[" + std::to_string ( i ) + "].specular" ).c_str () , light.m_specular );
		}

		// render directional lights
		std::sort ( scene.m_directional_light_data.begin () , scene.m_directional_light_data.end () ,
			[&camera_position]( Scene::DirectionalLightData const& dld1 , Scene::DirectionalLightData const& dld2 )
			{
				return LengthSq ( dld1.m_position - camera_position ) < LengthSq ( dld2.m_position - camera_position );
			}
		);
		int num_directional_light = static_cast< int >( scene.m_directional_light_data.size () > m_max_directional_lights ? m_max_directional_lights : scene.m_directional_light_data.size () );
		OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uNumDirectionalLight" , num_directional_light );
		for ( auto i = 0; i < num_directional_light; ++i )
		{
			auto const& light = scene.m_directional_light_data[ i ];
			OGLShader::SetUniform ( m_animation_shader.GetShaderID () , ( "uDirectionalLight[" + std::to_string ( i ) + "].position" ).c_str () , light.m_position );
			OGLShader::SetUniform ( m_animation_shader.GetShaderID () , ( "uDirectionalLight[" + std::to_string ( i ) + "].direction" ).c_str () , -glm::normalize ( light.m_position ) );
			OGLShader::SetUniform ( m_animation_shader.GetShaderID () , ( "uDirectionalLight[" + std::to_string ( i ) + "].colour" ).c_str () , light.m_colour );
			OGLShader::SetUniform ( m_animation_shader.GetShaderID () , ( "uDirectionalLight[" + std::to_string ( i ) + "].ambient" ).c_str () , light.m_ambient );
			OGLShader::SetUniform ( m_animation_shader.GetShaderID () , ( "uDirectionalLight[" + std::to_string ( i ) + "].diffuse" ).c_str () , light.m_diffuse );
			OGLShader::SetUniform ( m_animation_shader.GetShaderID () , ( "uDirectionalLight[" + std::to_string ( i ) + "].specular" ).c_str () , light.m_specular );

			m_light_space_matrix =
				glm::ortho ( -20.0f , 20.0f , -20.0f , 20.0f , 1.0f , 20.0f ) *
				glm::lookAt (
					glm::vec3 ( light.m_position ) ,
					glm::vec3 ( 0.0f , 0.0f , 0.0f ) ,
					glm::vec3 ( 0.0f , 1.0f , 0.0f ) );

			OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uLightSpaceMatrix" , m_light_space_matrix );
		}

		// Set Fog
		OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uFogParams.color" , { 0.45f,0.65f,0.90f } );
		OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uFogParams.linearStart" , 10.0f );
		OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uFogParams.linearEnd" , 100.0f );
		OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uFogParams.density" , 0.03f );
		OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uFogParams.equation" , 0 );
		OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uFogParams.isEnabled" , false );

		// Set Tint
		OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uIsTint" , false );
		OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uTint" , glm::vec4 ( 1.0f ) );

		for ( auto const& data : scene.m_animation_data )
		{
			auto& [render_data , transform] = data;
			auto& animator = m_animations[ m_animation_names[ render_data.m_model_id ] ].m_animators[ static_cast< uint32_t >( render_data.m_shininess ) ];

			auto transforms = animator.GetFinalBoneMatrices ();
			for ( int i = 0; i < transforms.size (); ++i )
			{
				std::string temp = "bone_transforms[" + std::to_string ( i ) + "]";
				OGLShader::SetUniform ( m_animation_shader.GetShaderID () , temp.c_str () , transforms[ i ] );
			}

			// set material
			/*OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uMaterial.diffuse_map" , 0 );
			std::get<1> ( textures.Get ( textures.GetID ( "White" ) ) ).Bind ( 0 );
			OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uMaterial.specular_map" , 1 );
			std::get<1> ( textures.Get ( textures.GetID ( "White" ) ) ).Bind ( 1 );*/
			OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uMaterial.diffuse_map" , 0 );
			std::get<1> ( textures.Get ( render_data.m_diffuse_id ) ).Bind ( 0 );
			OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uMaterial.specular_map" , 1 );
			std::get<1> ( textures.Get ( render_data.m_specular_id ) ).Bind ( 1 );
			OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uMaterial.shininess" , 32.0f );

			// draw model
			OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uModel" , transform );

			OGLShader::SetUniform ( m_animation_shader.GetShaderID () , "uEmissive" , render_data.m_emissive );

			m_animations[ m_animation_names[ render_data.m_model_id ] ].m_model.Draw ( m_animation_shader );
		}
	}

	void OpenGL::UpdateAnimations ( float dt )
	{
		for ( auto& animation : m_animations )
		{
			std::vector<uint32_t> to_remove;
			for ( auto i = 0; i < animation.second.m_animators.Size (); ++i )
			{
				if ( animation.second.m_animators.Valid ( i ) )
				{
					if ( animation.second.m_animators[ i ].m_alive )
					{
						animation.second.m_animators[ i ].UpdateAnimation ( dt , animation.second.m_cached_node_transforms );
						animation.second.m_animators[ i ].m_alive = false;
					}
					else
					{
						// remove animator
						to_remove.push_back ( i );
					}
				}
			}
			for ( auto const& i : to_remove )
			{
				animation.second.m_animators.Erase ( i );
			}
		}
	}

	void OpenGL::FirstPassRenderToDepthmap ( Scene const& scene , glm::mat4 const& projection , glm::mat4 const& view , glm::vec3 const& camera_position , OGLTextureManager& textures )
	{
		( textures );
		( camera_position );
		( view );
		( projection );

		//glCheckError();
		m_shadowmap.EnableDepthMap ();

		for ( auto const& data : scene.m_instanced_render_data )
		{
			// Draw the normal model
			m_depthmap_shader.Use ();

			OGLShader::SetUniform (
				m_depthmap_shader.GetShaderID () ,
				"uLightSpaceMatrix" ,
				m_light_space_matrix );

			// Draw model
			for ( auto& mesh : m_models[ data.first.m_model_id ] )
			{
				mesh.SetTransformData ( data.second );
				mesh.DrawInstanced ( GL_TRIANGLES );
			}
		}

		m_anim_depthmap_shader.Use ();

		OGLShader::SetUniform (
			m_anim_depthmap_shader.GetShaderID () ,
			"uLightSpaceMatrix" ,
			m_light_space_matrix );

		for ( auto const& data : scene.m_animation_data )
		{
			auto& [render_data , transform] = data;
			auto& animator = m_animations[ m_animation_names[ render_data.m_model_id ] ].m_animators[ static_cast< uint32_t >( render_data.m_shininess ) ];

			auto transforms = animator.GetFinalBoneMatrices ();
			for ( int i = 0; i < transforms.size (); ++i )
			{
				std::string temp = "bone_transforms[" + std::to_string ( i ) + "]";
				OGLShader::SetUniform ( m_anim_depthmap_shader.GetShaderID () , temp.c_str () , transforms[ i ] );
			}

			// draw model
			OGLShader::SetUniform ( m_anim_depthmap_shader.GetShaderID () , "uModel" , transform );

			m_animations[ m_animation_names[ render_data.m_model_id ] ].m_model.Draw ( m_anim_depthmap_shader );
		}
		m_anim_depthmap_shader.UnUse ();

		m_shadowmap.DisableDepthMap ();

	}

	OGLRenderPass<1>& OpenGL::BlurTexture ( unsigned int texture )
	{
		bool horizontal = true , first_iteration = true;
		unsigned int amount = 5;
		m_blur_shader.Use ();
		for ( unsigned int i = 0; i < amount; i++ )
		{
			if ( horizontal )
			{
				m_blur_pingpong_1.Bind ();
				OGLShader::SetUniform ( m_blur_shader.GetShaderID () , "uHorizontal" , horizontal );
				if ( first_iteration )
				{
					glActiveTexture ( GL_TEXTURE0 );
					glBindTexture ( GL_TEXTURE_2D , texture );
				}
				else
				{
					glActiveTexture ( GL_TEXTURE0 );
					glBindTexture ( GL_TEXTURE_2D , m_blur_pingpong_2.GetTexture () );
				}
			}
			else
			{
				m_blur_pingpong_2.Bind ();
				OGLShader::SetUniform ( m_blur_shader.GetShaderID () , "uHorizontal" , horizontal );
				if ( first_iteration )
				{
					glActiveTexture ( GL_TEXTURE0 );
					glBindTexture ( GL_TEXTURE_2D , texture );
				}
				else
				{
					glActiveTexture ( GL_TEXTURE0 );
					glBindTexture ( GL_TEXTURE_2D , m_blur_pingpong_1.GetTexture () );
				}
			}

			m_square_mesh.Draw ( GL_TRIANGLES );

			horizontal = !horizontal;
			if ( first_iteration )
				first_iteration = false;
		}
		OGLShader::UnUse ();
		glBindFramebuffer ( GL_FRAMEBUFFER , 0 );

		return m_blur_pingpong_1;
	}

	void OpenGL::RenderBlendTextures ( unsigned int texture1 , unsigned int texture2 )
	{
		m_blend_shader.Use ();

		OGLShader::SetUniform ( m_blend_shader.GetShaderID () , "uScene" , 0 );
		glActiveTexture ( GL_TEXTURE0 );
		glBindTexture ( GL_TEXTURE_2D , texture1 );
		OGLShader::SetUniform ( m_blend_shader.GetShaderID () , "uBloomBlur" , 1 );
		glActiveTexture ( GL_TEXTURE0 + 1 );
		glBindTexture ( GL_TEXTURE_2D , texture2 );

		m_square_mesh.Draw ( GL_TRIANGLES );

		OGLShader::UnUse ();
	}

	void OpenGL::RenderTexture ( unsigned int texture )
	{
		m_2D_shader.Use ();

		// vertex shader
		OGLShader::SetUniform ( m_2D_shader.GetShaderID () , "uProjection" , glm::mat4 ( 1.0f ) );

		// fragment shader
		OGLShader::SetUniform ( m_2D_shader.GetShaderID () , "diffuse_map" , 0 );
		glActiveTexture ( GL_TEXTURE0 );
		glBindTexture ( GL_TEXTURE_2D , texture );

		OGLShader::SetUniform ( m_2D_shader.GetShaderID () , "uAlpha" , 1.0f );

		m_square_mesh.SetTransformData ( { glm::mat4 ( 1.0f ) } );
		m_square_mesh.DrawInstanced ( GL_TRIANGLES );

		//m_square_mesh.Draw ( GL_TRIANGLES );

		m_2D_shader.UnUse ();
	}

	std::tuple<uint32_t , uint32_t> OpenGL::AddAnimationInstance ( std::string const& name )
	{
		if ( m_animations.find ( name ) != m_animations.end () )
		{
			auto it = std::find_if ( m_animation_names.begin () , m_animation_names.end () , [&name]( auto const& val ) { return val == name; } );
			return { it - m_animation_names.begin (), m_animations[ name ].AddInstance () };
		}
		return { -1,-1 };
	}

	void AnimationWrap::Initialize ( std::string const& path )
	{
		std::cout << "Trying to load animation from " << path << std::endl;
		m_model = Animation3D::Model { path };
		m_animation = Animation3D::Animation { path, &m_model };

		// cache all node transforms at fix timestep of 60 fps
		float current_time { 0.0f };
		uint32_t frame { 0 };
		while ( current_time < m_animation.GetDuration () )
		{
			PreCalculateBoneTransform ( frame , current_time , &m_animation.GetRootNode () , glm::mat4 ( 1.0f ) );
			current_time += m_animation.GetTicksPerSecond () * 1.0f / 60.0f;
			++frame;
		}

		m_cached_node_transforms;
		int test = 1;
	}

	uint32_t AnimationWrap::AddInstance ()
	{
		return m_animators.Push ( { &m_animation } );
	}

	void AnimationWrap::PreCalculateBoneTransform ( uint32_t frame , float currentTime , const Animation3D::AssimpNodeData* node , glm::mat4 parentTransform )
	{
		std::string nodeName = node->name;
		glm::mat4 nodeTransform = node->transformation;

		glm::mat4 globalTransformation = glm::mat4 ( 1.0f );

		Animation3D::Bone* Bone = m_animation.FindBone ( nodeName );

		if ( Bone )
		{
			Bone->Update ( currentTime );
			nodeTransform = Bone->GetLocalTransform ();
		}

		globalTransformation = parentTransform * nodeTransform;

		auto boneInfoMap = m_animation.GetBoneIDMap ();
		if ( boneInfoMap.find ( nodeName ) != boneInfoMap.end () )
		{
			int index = boneInfoMap[ nodeName ].id;
			glm::mat4 offset = boneInfoMap[ nodeName ].offset;
			//uint32_t frame_index = static_cast< uint32_t >( currentTime * 60.0f );
			if ( index >= m_cached_node_transforms.size () )
			{
				m_cached_node_transforms.resize ( index + 1 );
			}
			if ( frame >= m_cached_node_transforms[ index ].size () )
			{
				m_cached_node_transforms[ index ].resize ( frame + 1 );
			}

			m_cached_node_transforms[ index ][ frame ] = globalTransformation * offset;
			//m_FinalBoneMatrices[ index ] = ( globalTransformation * glm::mat4( 10.0f ) ) *offset; //sus
			//m_cached_node_transforms[ index ][ frame ] = ( globalTransformation * glm::mat4 ( 10.0f ) ) * offset;
		}

		for ( int i = 0; i < node->childrenCount; i++ )
			PreCalculateBoneTransform ( frame , currentTime , &node->children[ i ] , globalTransformation );
	}
}