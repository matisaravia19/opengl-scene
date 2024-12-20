#pragma once

#include "../core/window.h"
#include "../core/input.h"


#include "renderer.h"

class GuiRenderer : public Component, public Renderable {
    Window *window;
    Input *input;

    bool visible;
    bool full_screen;

    void showSettingsWindow();
public:
    explicit GuiRenderer(Window *window, Input *input);
    ~GuiRenderer() override;

    void init() override;
    void render() override;
    void update() override;
    void remove() override;
};



