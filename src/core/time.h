#pragma once


class Time {
private:
    static double deltaTime;
    static double lastFrame;

public:
    static double getDeltaTime();
    static void endFrame();
};
