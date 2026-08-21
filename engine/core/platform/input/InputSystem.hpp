#pragma once

#include "IInput.hpp"
#include <glm/vec2.hpp>

class InputSystem : public IInput
{
protected:
    IWindow* m_window = nullptr;

    std::bitset<static_cast<size_t>(Key::KeyCount)> current;
    std::bitset< static_cast<size_t>(Key::KeyCount)> previous;

    std::bitset<static_cast<size_t>(MouseButton::MouseButtonCount)> mouseCurrent;
    std::bitset<static_cast<size_t>(MouseButton::MouseButtonCount)> mousePrevious;

    double mouseX = 0.0, mouseY = 0.0;
    double lastMouseX = 0.0, lastMouseY = 0.0;
    double deltaX = 0.0, deltaY = 0.0;

public:
    InputSystem(IWindow* window);

    IWindow* window() const override { return m_window; }

    void update() override;

    bool getKey(Key key) const override;

    bool getKeyDown(Key key) const override;

    bool getKeyUp(Key key) const override;

    KeyState getKeyState(Key key) const override;

    void getMousePosition(double& x, double& y) const override;
    glm::vec2 getMousePosition() const override;
    void getMouseDelta(double& dx, double& dy) const override;
    glm::vec2 getMouseDelta() const override;

    bool getMouseButton(MouseButton button) const override;

    bool getMouseButtonDown(MouseButton button) const override;

    bool getMouseButtonUp(MouseButton button) const override;
};
