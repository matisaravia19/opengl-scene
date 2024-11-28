#include "guiRenderer.h"
#include "../core/time.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

GuiRenderer::GuiRenderer(Window *window, Input *input)
        : window(window), input(input), settingsVisible(false) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window->getWindow(), true);
    ImGui_ImplOpenGL3_Init();
}

void GuiRenderer::init() {
    Component::init();

    Renderer::getActive()->registerRenderable(this, RenderPass::GUI);

    settings = &SettingsManager::getSettings();
}

void GuiRenderer::showFPS() {
    ImGui::Begin("FPS", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    ImGui::Text("FPS: %.1f", Time::getFPS());

    ImGui::End();
}

void GuiRenderer::showSettingsWindow() {
    ImGui::Begin("Settings");

    ImGui::Checkbox("Fullscreen", &settings->fullscreen);

    ImGui::ColorPicker3("Ambient Light", &settings->ambientLight.r);
    ImGui::ColorPicker3("Fog Color", &settings->fogColor.r);
    ImGui::SliderFloat("Fog Density", &settings->fogDensity, 0.0f, 1.0f);

    ImGui::Checkbox("Shadows", &settings->showShadows);
    ImGui::SliderFloat("Directional Shadow Distance", &settings->directionalShadowDistance, 0.0f, 1000.0f);

    ImGui::SliderFloat("Tree Render Distance", &settings->treeDistance, 10.0f, 1000.0f);

    ImGui::Checkbox("Day/Night Cycle", &settings->dayNightCycle);
    ImGui::SliderFloat("Day Length", &settings->dayLength, 1.0f, 48.0f);
    ImGui::SliderFloat("Day Time", &settings->dayTime, 0.0f, settings->dayLength);

    ImGui::End();
}

void GuiRenderer::render() {

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    showFPS();

    if (settingsVisible) {
        showSettingsWindow();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GuiRenderer::update() {
    Component::update();

    static bool debounce = false;

    if (input->wasKeyPressed(KeyCode::Escape)) {
        if (!debounce) {
            settingsVisible = !settingsVisible;
            debounce = true;
            Time::setTimeScale(settingsVisible ? 0 : 1);
            input->setCursorVisibility(settingsVisible);
        }
    } else { // debounce is reset only when the key is released
        debounce = false;
    }
}

GuiRenderer::~GuiRenderer() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GuiRenderer::remove() {
    Component::remove();

    Renderer::getActive()->removeRenderable(this, RenderPass::GUI);
}
