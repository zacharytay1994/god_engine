#pragma once

#pragma warning(disable: 4201)
#include <glm/glm/glm.hpp>
#pragma warning(default: 4201)

#include <cstdint>

namespace god
{
	struct OGLShader
	{
		OGLShader ();

		uint32_t InitializeFromFile ( char const* vsPath , char const* fsPath );
		uint32_t InitializeFromCode ( char const* vsCode , char const* fsCode );

		void Use ();
		static void UnUse ();

		uint32_t GetShaderID ();

		static void SetUniform ( uint32_t shaderProgram , char const* name , bool val );
		static void SetUniform ( uint32_t shaderProgram , char const* name , int val );
		static void SetUniform ( uint32_t shaderProgram , char const* name , float val );
		static void SetUniform ( uint32_t shaderProgram , char const* name , float x , float y );
		static void SetUniform ( uint32_t shaderProgram , char const* name , float x , float y , float z );
		static void SetUniform ( uint32_t shaderProgram , char const* name , float x , float y , float z , float w );
		static void SetUniform ( uint32_t shaderProgram , char const* name , glm::vec2 const& val );
		static void SetUniform ( uint32_t shaderProgram , char const* name , glm::vec3 const& val );
		static void SetUniform ( uint32_t shaderProgram , char const* name , glm::vec4 const& val );
		static void SetUniform ( uint32_t shaderProgram , char const* name , glm::mat3 const& val );
		static void SetUniform ( uint32_t shaderProgram , char const* name , glm::mat4 const& val );

		static constexpr uint32_t GL_UNINITIALIZED = static_cast< uint32_t >( -1 );
	private:

		uint32_t m_shader_program { GL_UNINITIALIZED };
	};

	struct OGLMaterial
	{
		glm::vec4	m_ambient { 0.25f, 0.20725f, 0.20725f , 0.922f };
		glm::vec4	m_diffuse { 1.0f, 0.829f, 0.829f , 0.922f };
		glm::vec4	m_specular { 0.296648f, 0.296648f, 0.296648f , 0.922f };
		float		m_shininess { 11.264f };
	};

	struct OGLLight
	{
		glm::vec3 m_position { 0.0f,0.0f,0.0f };
		glm::vec3 m_colour { 1.0f,1.0f,1.0f };
		glm::vec3 m_ambient { 0.2f, 0.2f, 0.2f };
		glm::vec3 m_diffuse { 0.8f, 0.8f, 0.8f };
		glm::vec3 m_specular { 1.0f, 1.0f, 1.0f };
	};
}