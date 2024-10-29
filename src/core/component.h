#pragma once


class Component {
public:
    virtual void init();
    virtual void update();
    virtual void remove();
};

class DebugComponent : public Component {
public:
    void update() override;
};