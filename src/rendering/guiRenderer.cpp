#include "guiRenderer.h"
#include "../core/time.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

GuiRenderer::GuiRenderer(Window *window, Input *input)
        : window(window), input(input), visible(false), full_screen(false) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window->getWindow(), true);
    ImGui_ImplOpenGL3_Init();
}

void GuiRenderer::init() {
    Renderer::getActive()->registerRenderable(this);
}

void GuiRenderer::showSettingsWindow() {
    ImGui::Begin("Settings");

    ImGui::Checkbox("Fullscreen", &full_screen);

    ImGui::End();
}

void GuiRenderer::render() {
    if (!visible) return;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //ImGui::ShowDemoWindow();
    showSettingsWindow();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GuiRenderer::update() {
    static bool debounce = false;
    const auto cfg = Settings::ActiveSettings;

    if (input->wasKeyPressed(KeyCode::Escape)) {
        if (!debounce) {
            visible = !visible;
            debounce = true;
            Time::setTimeScale(visible ? 0 : 1);
            input->setCursorVisibility(visible);
        }
    } else { // debounce is reset only when the key is released
        debounce = false;
    }

    if (full_screen) {
        cfg->setFullscreen(true);
    } else {
        cfg->setFullscreen(false);
    }
}

GuiRenderer::~GuiRenderer() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GuiRenderer::remove() {
    Renderer::getActive()->removeRenderable(this);
}
