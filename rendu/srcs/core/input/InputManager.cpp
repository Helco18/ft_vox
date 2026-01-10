#include "InputManager.hpp"
#include <iostream>

void InputManager::interceptScroll(GLFWwindow * window, double xoffset, double yoffset)
{
	(void) xoffset;

	WindowManager * windowManager = reinterpret_cast<WindowManager *>(glfwGetWindowUserPointer(window));
	if (!windowManager->getEngine()->isInitialized())
		return;

	Camera * camera = windowManager->getCamera();
	if (yoffset > 0)
		camera->changeSpeed(SCROLL_SPEED);
	else if (camera->getSpeed() >= 0.2f)
		camera->changeSpeed(-SCROLL_SPEED);
}

void InputManager::interceptMouse(WindowManager * windowManager)
{
	if (!windowManager->getEngine()->isInitialized())
		return;

	GLFWwindow * window = windowManager->getWindow();
	Camera * camera = windowManager->getCamera();

	int width, height;
	double mouseX, mouseY;

	width = windowManager->getWidth();
	height = windowManager->getHeight();
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

void InputManager::interceptMovements(WindowManager * windowManager, float deltaTime)
{
	if (!windowManager->getEngine()->isInitialized())
		return;

	GLFWwindow * window = windowManager->getWindow();
	Camera * camera = windowManager->getCamera();

	float velocity;
	float speed;
	glm::vec3 forward;
	glm::vec3 pos;
	glm::vec3 up;
	glm::vec3 right;

	speed = camera->getSpeed();
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

	// std::cout << camera->getPosition().x << " " << camera->getPosition().y << " " << camera->getPosition().z << " | " << camera->getPitch() << " " << camera->getYaw() << std::endl;
}

void InputManager::interceptInputs(GLFWwindow * window, int key, int, int action, int)
{
	WindowManager * windowManager = reinterpret_cast<WindowManager *>(glfwGetWindowUserPointer(window));

	if (!windowManager->getEngine()->isInitialized())
		return;

	if (action != GLFW_PRESS)
		return;

	if (key == GLFW_KEY_ESCAPE)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
		return;
	}
	if (key == GLFW_KEY_TAB)
		windowManager->requestSwap();

	if (key == GLFW_KEY_F11)
		windowManager->toggleFullscreen();
	if (key == GLFW_KEY_F7)
		windowManager->toggleWireframe();
}
