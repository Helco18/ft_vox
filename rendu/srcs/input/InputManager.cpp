#include "InputManager.hpp"

void InputManager::interceptMouse(GLFWwindow * window, Camera * camera)
{
	int width, height;
	double mouseX, mouseY;

	width = camera->getWidth();
	height = camera->getHeight();
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT))
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwGetCursorPos(window, &mouseX, &mouseY);

		camera->updateOrientation(mouseX, mouseY);

		glfwSetCursorPos(window, (static_cast<float>(width) / 2), (static_cast<float>(height) / 2));
	}
	else
	{
		if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetCursorPos(window, (static_cast<float>(width) / 2), (static_cast<float>(height) / 2));
		}
	}
}

void InputManager::interceptMovements(GLFWwindow * window, Camera * camera, float deltaTime)
{
	float velocity;
	float speed;
	glm::vec3 forward;
	glm::vec3 pos;
	glm::vec3 up;
	glm::vec3 right;

	speed = CAMERA_SPEED;
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		speed *= 5.0f;

	velocity = speed * deltaTime;
	pos = camera->getPosition();
	up = camera->getAltitude();
	forward = camera->computeForward();
	right = glm::normalize(glm::cross(forward, up));
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		pos += forward * velocity;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		pos -= forward * velocity;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		pos -= right * velocity;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		pos += right * velocity;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		pos += up * velocity;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		pos -= up * velocity;
	camera->setPosition(pos);
}

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
		toggleFullscreen(window, reinterpret_cast<AEngine *>(glfwGetWindowUserPointer(window))->getCamera());
}
