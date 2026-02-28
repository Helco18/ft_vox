#include "Gui.hpp"
#include "Environment.hpp"
#include "WindowManager.hpp"
#include "imgui/imgui.h"
#include "Chunk.hpp"
#include "GLFW/glfw3.h"
#include "WorldManager.hpp"

void Gui::generateGui(GLFWwindow * window)
{
	WindowManager * windowManager = reinterpret_cast<WindowManager *>(glfwGetWindowUserPointer(window));
	Camera * camera = windowManager->getCamera();
	const glm::vec3 & position = camera->getPosition();
	const glm::ivec3 & chunkLocation = Chunk::locToChunkLoc(position);
	const glm::ivec3 chunkPos = Chunk::posToChunkPos(position);

	ImGui::NewFrame();
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
	ImGui::Begin("Bozo and Zibo Circus");
	
	// FPS
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "XYZ: %.3f / %.3f / %.3f",
		position.x, position.y, position.z);
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Chunk XYZ: %d / %d / %d",
		chunkLocation.x, chunkLocation.y, chunkLocation.z);
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Chunk Pos: %d / %d / %d",
		chunkPos.x, chunkPos.y, chunkPos.z);

	static int currentRenderDistance = camera->getRenderDistance();
	if (ImGui::SliderInt("Render distance", &currentRenderDistance, 2, 32, "%d chunks"))
		camera->setRenderDistance(currentRenderDistance);

	static float fov = camera->getFOV();
	if (ImGui::SliderFloat("FOV", &fov, 1.0f, 120.0f, "%.0f", ImGuiSliderFlags_AlwaysClamp))
		camera->setFOV(fov);

	static bool isLocked = WorldManager::getWorld(WORLD_NAME)->isLocked();
	if (ImGui::Checkbox("Lock Generation", &isLocked))
		WorldManager::getWorld(WORLD_NAME)->lockGeneration(isLocked);

	ImGui::SameLine();
	static bool isVsync = windowManager->isVsyncEnabled();
	if (ImGui::Checkbox("VSync", &isVsync))
		windowManager->setVsync(isVsync);

	ImGui::End();
}
