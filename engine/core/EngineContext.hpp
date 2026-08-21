#pragma once

#include "platform/input/IInput.hpp"
#include "platform/window/IWindow.hpp"
#include <render/renderer/IRenderer.hpp>

class Engine;

struct EngineContext
{
	Engine* engine = nullptr;
    IRenderer* renderer = nullptr;
    IInput* input = nullptr;
    IWindow* window = nullptr;

    bool isValid() const
    {
        return engine != nullptr &&
            renderer != nullptr &&
            input != nullptr &&
            window != nullptr;
    }
};

class EngineContextScope
{
public:
    explicit EngineContextScope(EngineContext* ctx)
        : context_(ctx)
    {
        if (!context_ || !context_->isValid()) {
            throw std::invalid_argument("Invalid engine context");
        }
    }

    ~EngineContextScope()
    {
        delete context_;
    }

    EngineContext* operator->() { return context_; }
    const EngineContext* operator->() const { return context_; }

    EngineContextScope(const EngineContextScope&) = delete;
    EngineContextScope& operator=(const EngineContextScope&) = delete;

private:
    EngineContext* context_;
};