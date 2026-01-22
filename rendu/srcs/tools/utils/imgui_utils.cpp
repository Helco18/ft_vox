#include "utils.hpp"
#include "imgui/imgui.h"

void generateImGui()
{
	ImGui::NewFrame();
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
	ImGui::Begin("ft_vox");
	
	// FPS
	ImGui::Text("Minecraft 0.00.1 (0.00.1/vanilla)");
	// ImGui::Text("%.3f ms/frame (%.1f FPS) Delta: %.3f", 1000.0f / io.Framerate, io.Framerate, deltaTime);

	ImGui::End();
}
