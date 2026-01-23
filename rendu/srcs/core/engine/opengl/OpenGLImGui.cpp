#include "OpenGLEngine.hpp"
#include "utils.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"
#include "Gui.hpp"

void OpenGLEngine::_initImGui()
{
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplOpenGL3_Init("#version 330");
	ImGui_ImplGlfw_InitForOpenGL(_window, true);
	ImGuiIO &io = ImGui::GetIO();
	ImFontConfig config;
	config.OversampleH = 3;
	config.OversampleV = 3;
	config.GlyphExtraAdvanceX = 0.8f;
	io.Fonts->AddFontFromFileTTF("resources/assets/font/Minecraft.ttf", 14.0f, &config);
	io.Fonts->Build();

	ImGuiStyle & style = ImGui::GetStyle();
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.5f, 0.0f, 0.9f, 0.002f);
}

void OpenGLEngine::beginImGui()
{
	if (!_imGuiEnabled)
		return;
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	Gui::generateGui(_window);
	_imGuiThisFrame = true;
}

void OpenGLEngine::_renderImGui()
{
	if (!_imGuiThisFrame)
		return;
	ImGui::Render();
	ImGui::GetDrawData();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	_imGuiThisFrame = false;
}

void OpenGLEngine::_shutdownImGui()
{
	ImGui_ImplGlfw_Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();
}
