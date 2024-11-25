#pragma once

class Entity;

enum ComponentLifeState {
    UNINITIALIZED,
    INITIALIZED,
    REMOVED
};

class Component {
private:
    Entity *entity = nullptr;
    ComponentLifeState lifeState = UNINITIALIZED;

public:
    virtual ~Component() = default;

    Entity *getEntity() const;
    void setEntity(Entity *entity);

    virtual void init();
    virtual void update();
    virtual void remove();

    ComponentLifeState getLifeState();
};

class DebugComponent : public Component {
public:
    void update() override;
};