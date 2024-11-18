#pragma once


class Time {
private:
    static double deltaTime;
    static double lastFrame;
    static double timeScale;

public:
    static double getDeltaTime();
    static void endFrame();
    static void setTimeScale(double timeScale);
};
