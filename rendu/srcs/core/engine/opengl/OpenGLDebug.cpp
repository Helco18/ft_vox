#include "Logger.hpp"
#include "OpenGLEngine.hpp"
#include "utils.hpp"

void OpenGLEngine::_debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei, const GLchar *message, const void *)
{
	LogSeverity logSeverity;

	// Ignore-list
	if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
		return;
	logSeverity = (severity == GL_DEBUG_SEVERITY_NOTIFICATION) ? INFO :
			(severity == GL_DEBUG_SEVERITY_LOW || severity == GL_DEBUG_SEVERITY_LOW_AMD || severity == GL_DEBUG_SEVERITY_LOW_ARB) ? WARNING :
			(severity == GL_DEBUG_SEVERITY_MEDIUM || severity == GL_DEBUG_SEVERITY_MEDIUM_AMD || severity == GL_DEBUG_SEVERITY_MEDIUM_ARB) ? WARNING :
			FATAL;

	Logger::log(ENGINE_OPENGL, logSeverity, 
		"Source: " + toString(source) +
			" | Type: " + toString(type) +
			" | ID: " + toString(id) + "\n" + message);
}
