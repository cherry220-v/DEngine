#pragma once
#include <fstream>
#include <bitset>
#include <cstdint>
#include <core/platform/window/IWindow.hpp>
#include <glm/vec2.hpp>

enum class Key : uint16_t
{
    Unknown = 0,

    // Буквы
    A = 'A', B, C, D, E, F, G, H, I, J,
    K, L, M, N, O, P, Q, R, S, T,
    U, V, W, X, Y, Z,

    // Цифры (верхний ряд)
    Num0 = '0', Num1, Num2, Num3, Num4,
    Num5, Num6, Num7, Num8, Num9,

    // Функциональные клавиши
    F1, F2, F3, F4, F5, F6,
    F7, F8, F9, F10, F11, F12,
    F13, F14, F15, F16, F17, F18,
    F19, F20, F21, F22, F23, F24,

    // Управляющие клавиши
    Escape,
    Enter,
    Space,
    Tab,
    Backspace,
    Insert,
    Delete,
    Home,
    End,
    PageUp,
    PageDown,

    // Стрелки
    Left,
    Right,
    Up,
    Down,

    // Модификаторы
    LeftShift,
    RightShift,
    LeftCtrl,
    RightCtrl,
    LeftAlt,
    RightAlt,
    LeftSuper,
    RightSuper,

    // Специальные
    CapsLock,
    ScrollLock,
    NumLock,
    PrintScreen,
    Pause,

    // Символы (верхний ряд)
    GraveAccent,   // `
    Minus,         // -
    Equal,         // =
    LeftBracket,   // [
    RightBracket,  // ]
    Backslash,     // \"
    Semicolon,     // ;
    Apostrophe,    // '
    Comma,         // ,
    Period,        // .
    Slash,         // /

    // Numpad
    KP0, KP1, KP2, KP3, KP4,
    KP5, KP6, KP7, KP8, KP9,
    KPDecimal,
    KPDivide,
    KPMultiply,
    KPSubtract,
    KPAdd,
    KPEnter,
    KPEqual,

    // Дополнительные
    Menu,
    KeyCount,

};

enum class MouseButton : uint16_t
{
    Unknown = 0,

    Left,
    Right,
    Middle,

    Button1,
    Button2,
    Button3,
    Button4,
    Button5,
    Button6,
    Button7,
    Button8,
    ButtonLast,
    MouseButtonCount,
};

enum class KeyState { Released, Pressed, JustPressed, JustReleased };

class IInput
{
public:
    virtual ~IInput() = default;

    virtual IWindow* window() const = 0;

    virtual void update() = 0;

    virtual bool getKey(Key key) const = 0;
    virtual bool getKeyDown(Key key) const = 0;
    virtual bool getKeyUp(Key key) const = 0;

    virtual KeyState getKeyState(Key key) const = 0;

    virtual void getMousePosition(double& x, double& y) const = 0;
    virtual void getMouseDelta(double& dx, double& dy) const = 0;
    virtual glm::vec2 getMousePosition() const = 0;
    virtual glm::vec2 getMouseDelta() const = 0;

    virtual bool getMouseButton(MouseButton button) const = 0;
    virtual bool getMouseButtonDown(MouseButton button) const = 0;
    virtual bool getMouseButtonUp(MouseButton button) const = 0;
};