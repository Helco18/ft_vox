#include "OpenGLEngine.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>

void OpenGLEngine::_updateUniformBuffer()
{
	// UniformBuffer ubo;
	// glm::vec3 camPos;
	// glm::vec3 forward;
	// glm::vec3 up;

	// glBindBuffer(GL_UNIFORM_BUFFER, _ubo);
	// camPos = _camera->getPosition();
	// forward = _camera->computeForward();
	// up = glm::vec3(0.0f, 1.0f, 0.0f);
	// // ubo.view = glm::lookAt(camPos, camPos + forward, up);
	// ubo.view = _camera->getView();

	// ubo.proj = glm::perspective(glm::radians(_camera->getFOV()),
	// 	static_cast<float>(_camera->getWidth()) / static_cast<float>(_camera->getHeight()),
	// 	0.01f, 1500.0f);
	// glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(UniformBuffer), &ubo);
	// glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
