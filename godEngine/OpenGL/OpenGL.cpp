#include "../pch.h"
#include "OpenGL.h"
#include "../godUtility/Scene.h"

#pragma comment (lib, "opengl32.lib")

#include <iostream>
#include <assert.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

namespace god
{
	OpenGL::OpenGL ( HWND windowHandle , int width , int height )
		:
		m_window_device_context { GetDC ( windowHandle ) }
	{
		// load opengl functions with glad
		if ( !gladLoadGL () )
		{
			std::cerr << "Failed to initialize GLAD" << std::endl;
			return;
		}

		ResizeViewport ( width , height );

		// create flat shader
		m_flat_shader.InitializeFromFile (
			"Assets/EngineAssets/OpenGLShaders/flatcolour.vs" ,
			"Assets/EngineAssets/OpenGLShaders/flatcolour.fs" );

		// create textured shader
		m_textured_shader.InitializeFromFile (
			"Assets/EngineAssets/OpenGLShaders/texturemaps.vs" ,
			"Assets/EngineAssets/OpenGLShaders/texturemaps.fs"
		);

		// opengl settings
		glEnable ( GL_CULL_FACE );
		glEnable ( GL_DEPTH_TEST );

		std::cout << "OpenGL constructed." << std::endl;
	}

	OpenGL::~OpenGL ()
	{
		//wglDeleteContext ( m_opengl_rendering_context );
	}

	void OpenGL::ClearColour () const
	{
		glClearColor ( 0.2f , 0.2f , 0.2f , 1.0f );
		glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	}

	void OpenGL::BuildOGLModels ( Asset3DManager const& asset3DManager )
	{
		// clear current mesh list
		m_models.clear ();
		
		// copy meshes
		for ( auto const& asset : asset3DManager.GetResources() )
		{
			m_models.emplace_back ();
			BuildOGLMeshesFromAssimpMeshes ( m_models.back () , asset.m_model.m_meshes );
		}

		// copy mesh ids
		m_model_ids = asset3DManager.GetIDs ();
	}

	void OpenGL::UpdateOGLModel ( ResourceID id , Asset3DManager const& asset3DManager )
	{
		m_models[ id ].clear ();
		BuildOGLMeshesFromAssimpMeshes ( m_models[ id ] , asset3DManager.Get ( id ).m_model.m_meshes );
		// copy mesh ids
		m_model_ids = asset3DManager.GetIDs ();
	}

	void OpenGL::RenderScene ( Scene const& scene ,
		glm::mat4 const& projection , glm::mat4 const& view , glm::vec3 const& camera_position , OGLTextureManager& textures )
	{
		//m_flat_shader.Use ();
		m_textured_shader.Use ();

		// projection matrix
		OGLShader::SetUniform ( m_flat_shader.GetShaderID () , "uProjection" , projection );

		// view matrix
		OGLShader::SetUniform ( m_flat_shader.GetShaderID () , "uView" , view );

		for ( auto const& data : scene.m_render_data )
		{
			if ( data.Active() )
			{
				//// set uniforms for vertex shader
				//// model matrix
				//glm::mat4 model_matrix = glm::mat4 ( 1.0f );
				//model_matrix = glm::translate ( model_matrix , data.m_position );
				//model_matrix = glm::rotate ( model_matrix , data.m_rotation.x , glm::vec3 ( 1.0f , 0.0f , 0.0f ) );
				//model_matrix = glm::rotate ( model_matrix , data.m_rotation.y , glm::vec3 ( 0.0f , 1.0f , 0.0f ) );
				//model_matrix = glm::rotate ( model_matrix , data.m_rotation.z , glm::vec3 ( 0.0f , 0.0f , 1.0f ) );
				//model_matrix = glm::scale ( model_matrix , data.m_scale );
				//OGLShader::SetUniform ( m_flat_shader.GetShaderID () , "uModel" , model_matrix );

				//// set uniforms for fragment shader
				//// set view position
				//OGLShader::SetUniform ( m_flat_shader.GetShaderID () , "uViewPosition" , camera_position );

				//// set material
				//OGLMaterial material;
				//OGLShader::SetUniform ( m_flat_shader.GetShaderID () , "uMaterial.ambient" , material.m_ambient );
				//OGLShader::SetUniform ( m_flat_shader.GetShaderID () , "uMaterial.diffuse" , material.m_diffuse );
				//OGLShader::SetUniform ( m_flat_shader.GetShaderID () , "uMaterial.specular" , material.m_specular );
				//OGLShader::SetUniform ( m_flat_shader.GetShaderID () , "uMaterial.shininess" , material.m_shininess );

				//// set light
				//OGLLight light;
				//OGLShader::SetUniform ( m_flat_shader.GetShaderID () , "uLight.position" , light.m_position );
				//OGLShader::SetUniform ( m_flat_shader.GetShaderID () , "uLight.colour" , light.m_colour );
				//OGLShader::SetUniform ( m_flat_shader.GetShaderID () , "uLight.ambient" , light.m_ambient );
				//OGLShader::SetUniform ( m_flat_shader.GetShaderID () , "uLight.diffuse" , light.m_diffuse );
				//OGLShader::SetUniform ( m_flat_shader.GetShaderID () , "uLight.specular" , light.m_specular );

				// set uniforms for vertex shader
				// model matrix
				glm::mat4 model_matrix = glm::mat4 ( 1.0f );
				model_matrix = glm::translate ( model_matrix , data.m_position );
				model_matrix = glm::rotate ( model_matrix , data.m_rotation.x , glm::vec3 ( 1.0f , 0.0f , 0.0f ) );
				model_matrix = glm::rotate ( model_matrix , data.m_rotation.y , glm::vec3 ( 0.0f , 1.0f , 0.0f ) );
				model_matrix = glm::rotate ( model_matrix , data.m_rotation.z , glm::vec3 ( 0.0f , 0.0f , 1.0f ) );
				model_matrix = glm::scale ( model_matrix , data.m_scale );
				OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uModel" , model_matrix );

				// set uniforms for fragment shader
				// set view position
				OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uViewPosition" , camera_position );

				// set material
				OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uMaterial.diffuse_map" , 0 );
				textures.Get ( data.m_diffuse_id ).Bind ( 0 );
				OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uMaterial.specular_map" , 1 );
				textures.Get ( data.m_specular_id ).Bind ( 1 );
				OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uMaterial.shininess" , data.m_shininess );

				// set light
				OGLLight light;
				OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uLight.position" , light.m_position );
				OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uLight.colour" , light.m_colour );
				OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uLight.ambient" , light.m_ambient );
				OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uLight.diffuse" , light.m_diffuse );
				OGLShader::SetUniform ( m_textured_shader.GetShaderID () , "uLight.specular" , light.m_specular );

				// draw model
				for ( auto const& mesh : m_models[ data.m_model_id ] )
				{
					mesh.Draw ( GL_TRIANGLES );
				}
			}
		}

		// unuse the bound shader
		OGLShader::UnUse ();
	}

	void OpenGL::ResizeViewport ( int width , int height ) const
	{
		glViewport ( 0 , 0 , width , height );
		std::cout << "OpenGL viewport resized x:" << width << " y: " << height << std::endl;
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
}