#include "../pch.h"
#include "OpenGL.h"
#include "../godUtility/Scene.h"


#pragma comment(lib, "opengl32.lib")

#include <iostream>
#include <assert.h>

#pragma warning(disable : 4201)
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#pragma warning(default : 4201)

namespace god
{
	OpenGL::Lines OpenGL::m_lines{};

	OpenGL::OpenGL(HWND windowHandle, int width, int height)
		: 
		m_window_device_context{GetDC(windowHandle)}
	{
		// load opengl functions with glad
		if (!gladLoadGL())
		{
			std::cerr << "Failed to initialize GLAD" << std::endl;
			return;
		}

		ResizeViewport(width, height);

		// create flat shader
		m_flat_shader.InitializeFromFile(
			"Assets/EngineAssets/OpenGLShaders/flatcolour.vs",
			"Assets/EngineAssets/OpenGLShaders/flatcolour.fs");

		// create textured shader
		m_textured_shader.InitializeFromFile(
			"Assets/EngineAssets/OpenGLShaders/texturemaps.vs",
			"Assets/EngineAssets/OpenGLShaders/texturemaps.fs");

		m_single_colour_outline_shader.InitializeFromFile(
			"Assets/EngineAssets/OpenGLShaders/single_colour_outline.vs",
			"Assets/EngineAssets/OpenGLShaders/single_colour_outline.fs");

		m_cubemap_shader.InitializeFromFile(
			"Assets/EngineAssets/OpenGLShaders/skybox.vs",
			"Assets/EngineAssets/OpenGLShaders/skybox.fs" );

		
		// opengl settings
		glEnable(GL_CULL_FACE);
		// Enables the Depth Buffer
		glEnable(GL_DEPTH_TEST);
		// Enables the Stencil Buffer
		glEnable(GL_STENCIL_TEST);
		// Sets rules for outcomes of stecil tests
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

		// glEnable ( GL_DEPTH_TEST );
		SetLineWidth(m_default_line_size);

		// temp line rendering code
		// line shader
		constexpr int MAX_POINTS{1000};
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
			"}\n"};

		char const line_fs[] = {
			"#version 430 core\n"
			"in vec4 vColour;\n"
			"out vec4 fFragColor;\n"
			"void main ()\n"
			"{\n"
			"fFragColor = vColour;\n"
			"}\n"};

		m_line_shader.InitializeFromCode(line_vs, line_fs);
		m_lines_mesh.m_vertices.resize(MAX_POINTS);
		m_lines_mesh.m_indices.resize(MAX_POINTS);
		m_lines_mesh.Initialize();


		// creating cube map
		m_cubemap.Initialize( width, height );
		std::string skybox_model_path = "Assets/GameAssets/2DAssets/Raw/Skybox/";
		// All the faces of the cubemap (make sure they are in this exact order)
		std::string facesCubemap[6] =
		{
			skybox_model_path + "right.jpg",
			skybox_model_path + "left.jpg",
			skybox_model_path + "top.jpg",
			skybox_model_path + "bottom.jpg",
			skybox_model_path + "front.jpg",
			skybox_model_path + "back.jpg"
		};
		m_cubemap.CubeTexture( facesCubemap );


		std::cout << "OpenGL constructed." << std::endl;
	}

	OpenGL::~OpenGL()
	{
		// wglDeleteContext ( m_opengl_rendering_context );
	}

	void OpenGL::ClearColour() const
	{
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void OpenGL::BuildOGLModels(Asset3DManager const &asset3DManager)
	{
		// clear current mesh list
		m_models.clear();

		// copy meshes
		for (auto const &asset : asset3DManager.GetResources())
		{
			m_models.emplace_back();
			BuildOGLMeshesFromAssimpMeshes(m_models.back(), std::get<1>(asset).m_model.m_meshes);
		}

		// copy mesh ids
		m_model_ids = asset3DManager.GetIDs();
	}

	void OpenGL::UpdateOGLModel(ResourceID id, Asset3DManager const &asset3DManager)
	{
		m_models[id].clear();
		BuildOGLMeshesFromAssimpMeshes(m_models[id], std::get<1>(asset3DManager.Get(id)).m_model.m_meshes);
		// copy mesh ids
		m_model_ids = asset3DManager.GetIDs();
	}

	void OpenGL::RenderScene(Scene const &scene,
							 glm::mat4 const &projection, glm::mat4 const &view, glm::vec3 const &camera_position, OGLTextureManager &textures)
	{
		ClearColour();
		// use the shader
		// m_flat_shader.Use ();


		for (auto const &data : scene.m_render_data)
		{
			if (data.Active())
			{
				

				// Make it so the stencil test always passes
				glStencilFunc(GL_ALWAYS, 1, 0xFF);
				// Enable modifying of the stencil buffer
				glStencilMask(0xFF);

				// Draw the normal model
				m_textured_shader.Use();

				// projection matrix
				OGLShader::SetUniform(m_textured_shader.GetShaderID(), "uProjection", projection);

				// view matrix
				OGLShader::SetUniform(m_textured_shader.GetShaderID(), "uView", view);

				// set uniforms for vertex shader
				// set model transform
				OGLShader::SetUniform(m_textured_shader.GetShaderID(), "uModel", data.m_model_transform);

				// set uniforms for fragment shader
				// set view position
				OGLShader::SetUniform(m_textured_shader.GetShaderID(), "uViewPosition", camera_position);

				// set material
				OGLShader::SetUniform(m_textured_shader.GetShaderID(), "uMaterial.diffuse_map", 0);
				std::get<1>(textures.Get(data.m_diffuse_id)).Bind(0);
				OGLShader::SetUniform(m_textured_shader.GetShaderID(), "uMaterial.specular_map", 1);
				std::get<1>(textures.Get(data.m_specular_id)).Bind(1);
				OGLShader::SetUniform(m_textured_shader.GetShaderID(), "uMaterial.shininess", data.m_shininess);
				OGLShader::SetUniform( m_textured_shader.GetShaderID(), "uSkybox", 2 );
				glActiveTexture( GL_TEXTURE0 + 2);
				glBindTexture( GL_TEXTURE_CUBE_MAP, m_cubemap.GetTexture() );

				// set light
				OGLLight light;
				light.m_position = {0.0f, 100.0f, 0.0f};
				light.m_ambient = {0.8f, 0.8f, 0.8f};
				OGLShader::SetUniform(m_textured_shader.GetShaderID(), "uLight.position", light.m_position);
				OGLShader::SetUniform(m_textured_shader.GetShaderID(), "uLight.colour", light.m_colour);
				OGLShader::SetUniform(m_textured_shader.GetShaderID(), "uLight.ambient", light.m_ambient);
				OGLShader::SetUniform(m_textured_shader.GetShaderID(), "uLight.diffuse", light.m_diffuse);
				OGLShader::SetUniform(m_textured_shader.GetShaderID(), "uLight.specular", light.m_specular);

				// draw model
				for (auto const &mesh : m_models[data.m_model_id])
				{
					mesh.Draw(GL_TRIANGLES);
				}

				// Make it so only the pixels without the value 1 pass the test
				glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
				// Disable modifying of the stencil buffer
				glStencilMask(0x00);
				// Disable the depth buffer
				glDisable(GL_DEPTH_TEST);

				m_single_colour_outline_shader.Use();
				OGLShader::SetUniform(m_single_colour_outline_shader.GetShaderID(), "uProjection", projection);
				OGLShader::SetUniform(m_single_colour_outline_shader.GetShaderID(), "uView", view);
				OGLShader::SetUniform(m_single_colour_outline_shader.GetShaderID(), "uModel", data.m_model_transform);
				OGLShader::SetUniform(m_single_colour_outline_shader.GetShaderID(), "uOutlining", 0.01f);


				for (auto const &mesh : m_models[data.m_model_id])
				{
					mesh.Draw(GL_TRIANGLES);
				}

				// Enable modifying of the stencil buffer
				glStencilMask(0xFF);
				// Clear stencil buffer
				glStencilFunc(GL_ALWAYS, 0, 0xFF);
				// Enable the depth buffer
				glEnable(GL_DEPTH_TEST);
			}
		}


		// draw skybox as last
		m_cubemap.CubeMapEnableDepth();
		m_cubemap_shader.Use();
		OGLShader::SetUniform( m_cubemap_shader.GetShaderID(), "view", view );
		OGLShader::SetUniform( m_cubemap_shader.GetShaderID(), "projection", projection );
		OGLShader::SetUniform( m_cubemap_shader.GetShaderID(), "camera", camera_position );
		m_cubemap.Bind();
		m_cubemap.Draw();
		m_cubemap.UnBind();
		m_cubemap.CubeMapDisableDepth();


		// unuse the bound shader
		OGLShader::UnUse();
	}

	void OpenGL::ResizeViewport(int width, int height) const
	{
		glViewport(0, 0, width, height);
		std::cout << "OpenGL viewport resized x:" << width << " y: " << height << std::endl;
	}

	void OpenGL::DrawLine(glm::vec3 const &a, glm::vec3 const &b, glm::vec4 const &c, float size)
	{
		m_lines[size].push_back({a, b, c, size});
	}

	void OpenGL::RenderLines(glm::mat4 const &projection, glm::mat4 const &view)
	{
		m_line_shader.Use();

		m_line_shader.SetUniform(m_line_shader.GetShaderID(), "uProjection", projection);
		m_line_shader.SetUniform(m_line_shader.GetShaderID(), "uView", view);

		for (auto const &line_group : m_lines)
		{
			m_lines_mesh.m_indices.clear();
			m_lines_mesh.m_vertices.clear();

			for (auto const &line : line_group.second)
			{
				OGLMesh::OGLVertex start, end;
				start.m_position = std::get<0>(line);
				end.m_position = std::get<1>(line);
				start.m_colour = std::get<2>(line);
				end.m_colour = std::get<2>(line);

				m_lines_mesh.m_indices.push_back(static_cast<Index3D>(m_lines_mesh.m_vertices.size()));
				m_lines_mesh.m_vertices.push_back(start);
				m_lines_mesh.m_indices.push_back(static_cast<Index3D>(m_lines_mesh.m_vertices.size()));
				m_lines_mesh.m_vertices.push_back(end);
			}

			m_lines_mesh.SetData();
			SetLineWidth(line_group.first);
			m_lines_mesh.Draw(GL_LINES);
		}

		OGLShader::UnUse();
		m_lines.clear();
	}

	void OpenGL::SetLineWidth(float size)
	{
		glLineWidth(size);
	}

	OGLMesh OpenGL::BuildOGLMeshFromAssimpMesh(Mesh3D const &mesh3d) const
	{
		OGLMesh mesh;

		// copy vertices
		mesh.m_vertices.resize(mesh3d.m_vertices.size());
		for (auto i = 0; i < mesh3d.m_vertices.size(); ++i)
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

	void OpenGL::BuildOGLMeshesFromAssimpMeshes(std::vector<OGLMesh> &oglMeshes, std::vector<Mesh3D> const &meshes3D) const
	{
		for (auto &mesh : meshes3D)
		{
			oglMeshes.emplace_back(BuildOGLMeshFromAssimpMesh(mesh));
		}
	}
}