#pragma once

class Scene;
struct EngineContext;

class ISystem
{
public:
    ISystem() = default;
    ~ISystem() = default;

    virtual void init(EngineContext* context) = 0;
    virtual void update(Scene&, float dt) = 0;
};