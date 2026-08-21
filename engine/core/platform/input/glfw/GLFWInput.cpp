#include "GLFWInput.hpp"

#include <GLFW/glfw3.h>

GLFWInput::GLFWInput(GLFWWindow* win)
    : InputSystem(win), m_window(win), gwindow(win->gwindow())
{
    if (!gwindow) return;

    glfwSetWindowUserPointer(gwindow, this);

    glfwSetKeyCallback(gwindow, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        auto* input = static_cast<GLFWInput*>(glfwGetWindowUserPointer(window));
        if (!input) return;

        Key mappedKey = input->mapKey(key);
        if (mappedKey != Key::Unknown) {
            size_t idx = static_cast<size_t>(mappedKey);
            if (action == GLFW_PRESS)   input->current[idx] = true;
            if (action == GLFW_RELEASE) input->current[idx] = false;
        }
        });

    glfwSetMouseButtonCallback(gwindow, [](GLFWwindow* window, int button, int action, int mods) {
        auto* input = static_cast<GLFWInput*>(glfwGetWindowUserPointer(window));
        if (!input) return;

        MouseButton mappedBtn = input->mapMouse(button);
        size_t idx = static_cast<size_t>(mappedBtn);

        if (action == GLFW_PRESS)   input->mouseCurrent[idx] = true;
        if (action == GLFW_RELEASE) input->mouseCurrent[idx] = false;

        if (mappedBtn == MouseButton::Button1) input->mouseCurrent[static_cast<size_t>(MouseButton::Left)] = (action == GLFW_PRESS);
        if (mappedBtn == MouseButton::Button2) input->mouseCurrent[static_cast<size_t>(MouseButton::Right)] = (action == GLFW_PRESS);
        if (mappedBtn == MouseButton::Button3) input->mouseCurrent[static_cast<size_t>(MouseButton::Middle)] = (action == GLFW_PRESS);
        });
}

Key GLFWInput::mapKey(int key) const
{
    // Буквы
    if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z)
        return static_cast<Key>(static_cast<int>(Key::A) + (key - GLFW_KEY_A));

    // Цифры (верхний ряд)
    if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9)
        return static_cast<Key>(static_cast<int>(Key::Num0) + (key - GLFW_KEY_0));

    // Функциональные клавиши
    if (key >= GLFW_KEY_F1 && key <= GLFW_KEY_F24)
        return static_cast<Key>(static_cast<int>(Key::F1) + (key - GLFW_KEY_F1));

    switch (key)
    {
        // Управляющие
    case GLFW_KEY_ESCAPE: return Key::Escape;
    case GLFW_KEY_ENTER: return Key::Enter;
    case GLFW_KEY_TAB: return Key::Tab;
    case GLFW_KEY_BACKSPACE: return Key::Backspace;
    case GLFW_KEY_INSERT: return Key::Insert;
    case GLFW_KEY_DELETE: return Key::Delete;
    case GLFW_KEY_HOME: return Key::Home;
    case GLFW_KEY_END: return Key::End;
    case GLFW_KEY_PAGE_UP: return Key::PageUp;
    case GLFW_KEY_PAGE_DOWN: return Key::PageDown;

        // Стрелки
    case GLFW_KEY_LEFT: return Key::Left;
    case GLFW_KEY_RIGHT: return Key::Right;
    case GLFW_KEY_UP: return Key::Up;
    case GLFW_KEY_DOWN: return Key::Down;

        // Модификаторы
    case GLFW_KEY_LEFT_SHIFT: return Key::LeftShift;
    case GLFW_KEY_RIGHT_SHIFT: return Key::RightShift;
    case GLFW_KEY_LEFT_CONTROL: return Key::LeftCtrl;
    case GLFW_KEY_RIGHT_CONTROL: return Key::RightCtrl;
    case GLFW_KEY_LEFT_ALT: return Key::LeftAlt;
    case GLFW_KEY_RIGHT_ALT: return Key::RightAlt;
    case GLFW_KEY_LEFT_SUPER: return Key::LeftSuper;
    case GLFW_KEY_RIGHT_SUPER: return Key::RightSuper;

        // Специальные
    case GLFW_KEY_CAPS_LOCK: return Key::CapsLock;
    case GLFW_KEY_SCROLL_LOCK: return Key::ScrollLock;
    case GLFW_KEY_NUM_LOCK: return Key::NumLock;
    case GLFW_KEY_PRINT_SCREEN: return Key::PrintScreen;
    case GLFW_KEY_PAUSE: return Key::Pause;
    case GLFW_KEY_SPACE: return Key::Space;

        // Символы
    case GLFW_KEY_GRAVE_ACCENT: return Key::GraveAccent;
    case GLFW_KEY_MINUS: return Key::Minus;
    case GLFW_KEY_EQUAL: return Key::Equal;
    case GLFW_KEY_LEFT_BRACKET: return Key::LeftBracket;
    case GLFW_KEY_RIGHT_BRACKET: return Key::RightBracket;
    case GLFW_KEY_BACKSLASH: return Key::Backslash;
    case GLFW_KEY_SEMICOLON: return Key::Semicolon;
    case GLFW_KEY_APOSTROPHE: return Key::Apostrophe;
    case GLFW_KEY_COMMA: return Key::Comma;
    case GLFW_KEY_PERIOD: return Key::Period;
    case GLFW_KEY_SLASH: return Key::Slash;

        // Numpad
    case GLFW_KEY_KP_0: return Key::KP0;
    case GLFW_KEY_KP_1: return Key::KP1;
    case GLFW_KEY_KP_2: return Key::KP2;
    case GLFW_KEY_KP_3: return Key::KP3;
    case GLFW_KEY_KP_4: return Key::KP4;
    case GLFW_KEY_KP_5: return Key::KP5;
    case GLFW_KEY_KP_6: return Key::KP6;
    case GLFW_KEY_KP_7: return Key::KP7;
    case GLFW_KEY_KP_8: return Key::KP8;
    case GLFW_KEY_KP_9: return Key::KP9;

    case GLFW_KEY_KP_DECIMAL: return Key::KPDecimal;
    case GLFW_KEY_KP_DIVIDE: return Key::KPDivide;
    case GLFW_KEY_KP_MULTIPLY: return Key::KPMultiply;
    case GLFW_KEY_KP_SUBTRACT: return Key::KPSubtract;
    case GLFW_KEY_KP_ADD: return Key::KPAdd;
    case GLFW_KEY_KP_ENTER: return Key::KPEnter;
    case GLFW_KEY_KP_EQUAL: return Key::KPEqual;

        // Остальное
    case GLFW_KEY_MENU: return Key::Menu;

    default:
        return Key::Unknown;
    }
}

MouseButton GLFWInput::mapMouse(int glfwButton) const
{
    if (glfwButton >= GLFW_MOUSE_BUTTON_1 && glfwButton <= GLFW_MOUSE_BUTTON_8)
    {
        return static_cast<MouseButton>(static_cast<int>(MouseButton::Button1) + (glfwButton - GLFW_MOUSE_BUTTON_1));
    }
    return MouseButton::Left;
}

void GLFWInput::update()
{
    InputSystem::update();

    if (!gwindow) return;

    glfwGetCursorPos(gwindow, &mouseX, &mouseY);
}