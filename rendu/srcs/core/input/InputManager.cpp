#include "InputManager.hpp"
#include "Environment.hpp"
#include "WorldManager.hpp"

void InputManager::interceptScroll(GLFWwindow * window, double, double yoffset)
{
	WindowManager * windowManager = reinterpret_cast<WindowManager *>(glfwGetWindowUserPointer(window));
	if (!windowManager->getEngine()->isInitialized())
		return;

	Camera * camera = windowManager->getCamera();
	if (yoffset > 0)
		camera->changeSpeed(SCROLL_SPEED);
	else if (camera->getSpeed() > SCROLL_SPEED)
		camera->changeSpeed(-SCROLL_SPEED);
}

void InputManager::interceptMouse(WindowManager * windowManager)
{
	static double oldMouseX, oldMouseY = 0;
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

		if (oldMouseX != mouseX || oldMouseY != mouseY)
		{
			oldMouseX = mouseX;
			oldMouseY = mouseY;
			camera->updateOrientation(mouseX, mouseY, 0);
		}

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

void InputManager::interceptMovements(WindowManager * windowManager)
{
	if (!windowManager->getEngine()->isInitialized())
		return;

	GLFWwindow * window = windowManager->getWindow();
	Camera * camera = windowManager->getCamera();

	int width, height;
	width = windowManager->getWidth();
	height = windowManager->getHeight();

	double deltaTime;
	float velocity;
	float speed;
	glm::vec3 forward;
	glm::vec3 pos;
	glm::vec3 up;
	glm::vec3 right;

	speed = camera->getSpeed();
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		speed *= SPEED_MULTIPLICATOR;

	deltaTime = windowManager->getDeltaTime();
	velocity = speed * deltaTime;
	pos = camera->getPosition();
	up = camera->computeUp();
	forward = camera->computeForward(camera->isIgnoringYMovement());
	right = camera->computeRight();
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		pos += forward * velocity;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		pos -= forward * velocity;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		pos -= right * velocity;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		pos += right * velocity;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera->updateOrientation((static_cast<float>(width) / 2), (static_cast<float>(height) / 2), -120.0f * deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera->updateOrientation((static_cast<float>(width) / 2), (static_cast<float>(height) / 2), 120.0f * deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		pos += up * velocity;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		pos -= up * velocity;
	camera->setPosition(pos);
}

void InputManager::interceptInputs(GLFWwindow * window, int key, int, int action, int)
{
	WindowManager * windowManager = reinterpret_cast<WindowManager *>(glfwGetWindowUserPointer(window));
	Camera * camera = windowManager->getCamera();

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

	if (key == GLFW_KEY_F3)
		windowManager->getEngine()->toggleGui();
	if (key == GLFW_KEY_F6)
		windowManager->toggleChunkBorders();
	if (key == GLFW_KEY_F7)
		windowManager->toggleWireframe();
	if (key == GLFW_KEY_F11)
		windowManager->toggleFullscreen();

	if (key == GLFW_KEY_1)
		camera->setCameraType(EULER);
	if (key == GLFW_KEY_2)
		camera->setCameraType(SIX_DOF);
	if (key == GLFW_KEY_3)
		camera->setCameraType(FPS);

	/* DEBUG-ONLY */
	// Trigger generation
	if (key == GLFW_KEY_V)
		WorldManager::getWorld(WORLD_NAME)->requestProcedural();
}