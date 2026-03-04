#include "CustomExceptions.hpp"
#include "OpenGLEngine.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"
#include "Gui.hpp"
#include "utils.hpp"
#include <filesystem>

void OpenGLEngine::_initImGui()
{
	const std::string fontPath = "resources/assets/font/Minecraft.ttf";
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplOpenGL3_Init("#version 330");
	ImGui_ImplGlfw_InitForOpenGL(_window, true);
	_imGuiInit = true;

	ImGuiIO &io = ImGui::GetIO();
	ImFontConfig config;
	config.OversampleH = 3;
	config.OversampleV = 3;
	config.GlyphExtraAdvanceX = 0.8f;
	if (!fileExists(fontPath))
		throw OpenGLException("ImGui font file doesn't exist: " + fontPath);
	io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 14.0f, &config);
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
	if (!_imGuiInit)
		return;
	ImGui_ImplGlfw_Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();
	_imGuiInit = false;
}
