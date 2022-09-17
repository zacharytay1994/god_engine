#pragma once

#ifdef _WIN32
#include <windows.h>
#else
#define APIENTRY
#endif

#include <glad/glad.h>

namespace OGLDebug
{
	GLenum glCheckError_( const char* function, int line );

#define glCheckError() OGLDebug::glCheckError_(__FILE__, __LINE__)

	void APIENTRY glDebugOutput( GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam );

	/**
	 * @brief
	 * Application-defined message callback function when an OpenGL error (or other interesting event) occurs within the driver
	 * The feature can even ensure that message callback functions are invoked on the same thread
	   and within the very same call stack as the GL call that triggered the GL error (or performance warning)
	*/
	void EnableOpenGLDebugging();
}

