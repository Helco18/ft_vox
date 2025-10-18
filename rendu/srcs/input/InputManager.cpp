#include "InputManager.hpp"

void InputManager::interceptInputs(GLFWwindow * window, int key, int, int action, int)
{
	if (action != GLFW_PRESS)
		return;

	if (key == GLFW_KEY_ESCAPE)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
		return;
	}
	if (key == GLFW_KEY_F11)
		toggleFullscreen(window);

	VulkanEngine * engine = reinterpret_cast<VulkanEngine *>(glfwGetWindowUserPointer(window));
	Camera * camera = engine->getCamera();
	glm::vec3 forward;
	forward.x = cosf(glm::radians(camera->getPitch())) * cosf(glm::radians(camera->getYaw())); // X component based on pitch
	forward.z = cosf(glm::radians(camera->getPitch())) * sinf(glm::radians(camera->getYaw())); // Z component based on pitch
	forward.y = 0.0f; // No vertical movement based on pitch, set Y to 0

	// Normalize the forward vector to maintain consistent speed
	forward = glm::normalize(forward);

	// Calculate the right vector (strafe) using yaw, no pitch influence
	const glm::vec3 right = glm::normalize(glm::cross(forward, camera->getAltitude())); // Right direction is based on forward and altitude (up vector)

	if (key == GLFW_KEY_W)
		camera->setPosition(camera->getPosition() + CAMERA_SPEED * forward);
	if (key == GLFW_KEY_A)
		camera->setPosition(camera->getPosition() + CAMERA_SPEED * -right);
	if (key == GLFW_KEY_S)
		camera->setPosition(camera->getPosition() + CAMERA_SPEED * -forward);
	if (key == GLFW_KEY_D)
		camera->setPosition(camera->getPosition() + CAMERA_SPEED * right);
	if (key == GLFW_KEY_SPACE)
		camera->setPosition(camera->getPosition() + CAMERA_SPEED * camera->getAltitude());
	if (key == GLFW_KEY_LEFT_SHIFT)
		camera->setPosition(camera->getPosition() + CAMERA_SPEED * -camera->getAltitude());
}
