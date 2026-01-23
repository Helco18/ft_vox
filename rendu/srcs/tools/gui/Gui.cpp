#include "Gui.hpp"
#include "WindowManager.hpp"
#include "imgui/imgui.h"
#include "GLFW/glfw3.h"

void Gui::generateGui(GLFWwindow * window)
{
	WindowManager * windowManager = reinterpret_cast<WindowManager *>(glfwGetWindowUserPointer(window));
	Camera * camera = windowManager->getCamera();

	ImGui::NewFrame();
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
	ImGui::Begin("ft_vox");
	
	// FPS
	ImGui::Text("Bozo and Zibo Circus");

	int currentRenderDistance = camera->getRenderDistance();
	ImGui::SliderInt("Render distance", &currentRenderDistance, 2, 32, "%d chunks");
	if (camera->getRenderDistance() != currentRenderDistance)
		camera->setRenderDistance(currentRenderDistance);

	ImGui::End();
}
