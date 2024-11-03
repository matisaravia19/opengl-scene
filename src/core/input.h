#pragma once


#include "window.h"

class Input {
private:
    Window *window;

public:
    Input(Window *window);
    void init();
    void poll();
};
