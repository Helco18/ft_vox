#include "OpenGLEngine.hpp"

void OpenGLEngine::_updateUniformBuffer()
{
	UniformBuffer * ubo;
	glm::vec3 camPos;
	glm::vec3 forward;
	glm::vec3 up;

	glBindBuffer(GL_UNIFORM_BUFFER, _ubo);
	ubo = (UniformBuffer *)glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
	camPos = _camera->getPosition();
	forward = _camera->computeForward();
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	ubo->view = glm::lookAt(camPos, camPos + forward, up);

	ubo->proj = glm::perspective(glm::radians(_camera->getFOV()),
		(float)_camera->getWidth() / (float)_camera->getHeight(),
		0.01f, 1500.0f);
	glUnmapBuffer(GL_UNIFORM_BUFFER);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
