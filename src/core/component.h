#pragma once

class Entity;

class Component {
private:
    Entity *entity = nullptr;

public:
    virtual ~Component() = default;

    Entity *getEntity();
    void setEntity(Entity *entity);

    virtual void init();
    virtual void update();
    virtual void remove();
};

class DebugComponent : public Component {
public:
    void update() override;
};