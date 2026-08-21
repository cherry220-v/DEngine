#include "InputSystem.hpp"

InputSystem::InputSystem(IWindow* win) : m_window(win)
{
}

void InputSystem::update()
{
    previous = current;
    mousePrevious = mouseCurrent;

    deltaX = mouseX - lastMouseX;
    deltaY = mouseY - lastMouseY;

    lastMouseX = mouseX;
    lastMouseY = mouseY;
}


bool InputSystem::getKey(Key key) const {
    return current[static_cast<size_t>(key)];
}

bool InputSystem::getKeyDown(Key key) const {
    size_t idx = static_cast<size_t>(key);
    return current[idx] && !previous[idx];
}

bool InputSystem::getKeyUp(Key key) const {
    size_t idx = static_cast<size_t>(key);
    return !current[idx] && previous[idx];
}

KeyState InputSystem::getKeyState(Key key) const {
    size_t idx = static_cast<size_t>(key);
    bool curr = current[idx];
    bool prev = previous[idx];

    if (curr && !prev) return KeyState::JustPressed;
    if (!curr && prev) return KeyState::JustReleased;
    if (curr)           return KeyState::Pressed;
    return KeyState::Released;
}

void InputSystem::getMousePosition(double& x, double& y) const { x = mouseX; y = mouseY; }
glm::vec2 InputSystem::getMousePosition() const { return { mouseX, mouseY }; }
void InputSystem::getMouseDelta(double& dx, double& dy) const { dx = deltaX; dy = deltaY; }
glm::vec2 InputSystem::getMouseDelta() const { return { deltaX, deltaY }; }

bool InputSystem::getMouseButton(MouseButton button) const {
    return mouseCurrent[static_cast<size_t>(button)];
}

bool InputSystem::getMouseButtonDown(MouseButton button) const {
    size_t idx = static_cast<size_t>(button);
    return mouseCurrent[idx] && !mousePrevious[idx];
}

bool InputSystem::getMouseButtonUp(MouseButton button) const {
    size_t idx = static_cast<size_t>(button);
    return !mouseCurrent[idx] && mousePrevious[idx];
}
