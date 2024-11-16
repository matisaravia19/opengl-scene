#include "guiRenderer.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

GuiRenderer::GuiRenderer(Window *window, Input *input) : window(window), visible(false), input(input) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window->getWindow(), true);
    ImGui_ImplOpenGL3_Init();
}

void GuiRenderer::init() {
    Renderer::getActive()->registerRenderable(this);
}

void GuiRenderer::render() {
    if (!visible) return;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::ShowDemoWindow(); // Show demo window! :)

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GuiRenderer::update() {
    static bool debounce = false;

    if (input->getKeysDown() & KeyEsc) {
        if (!debounce) {
            visible = !visible;
            debounce = true;
            input->togglePause();
        }
    } else { // debounce is reset only when the key is released
        debounce = false;
    }
}

void GuiRenderer::remove()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
