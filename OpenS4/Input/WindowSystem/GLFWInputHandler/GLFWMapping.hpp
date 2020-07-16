#pragma once
#include "../../../main.hpp"

namespace OpenS4::Input
{
    enum class Key
    {
        None = 0,
        Esc = GLFW_KEY_ESCAPE,

        F1 = GLFW_KEY_F1,
        F2 = GLFW_KEY_F2,
        F3 = GLFW_KEY_F3,
        F4 = GLFW_KEY_F4,
        F5 = GLFW_KEY_F5,
        F6 = GLFW_KEY_F6,
        F7 = GLFW_KEY_F7,
        F8 = GLFW_KEY_F8,
        F9 = GLFW_KEY_F9,
        F10 = GLFW_KEY_F10,
        F11 = GLFW_KEY_F11,
        F12 = GLFW_KEY_F12,

        Print = GLFW_KEY_PRINT_SCREEN,
        ScrollLock = GLFW_KEY_SCROLL_LOCK,
        Pause = GLFW_KEY_PAUSE,

        A = GLFW_KEY_A,
        B = GLFW_KEY_B,
        C = GLFW_KEY_C,
        D = GLFW_KEY_D,
        E = GLFW_KEY_E,
        F = GLFW_KEY_F,
        G = GLFW_KEY_G,
        H = GLFW_KEY_H,
        I = GLFW_KEY_I,
        J = GLFW_KEY_J,
        K = GLFW_KEY_K,
        L = GLFW_KEY_L,
        M = GLFW_KEY_M,
        N = GLFW_KEY_N,
        O = GLFW_KEY_O,
        P = GLFW_KEY_P,
        Q = GLFW_KEY_Q,
        R = GLFW_KEY_R,
        S = GLFW_KEY_S,
        T = GLFW_KEY_T,
        U = GLFW_KEY_U,
        V = GLFW_KEY_V,
        W = GLFW_KEY_W,
        X = GLFW_KEY_X,
        Y = GLFW_KEY_Y,
        Z = GLFW_KEY_Z,

        N0 = GLFW_KEY_0,
        N1 = GLFW_KEY_1,
        N2 = GLFW_KEY_2,
        N3 = GLFW_KEY_3,
        N4 = GLFW_KEY_4,
        N5 = GLFW_KEY_5,
        N6 = GLFW_KEY_6,
        N7 = GLFW_KEY_7,
        N8 = GLFW_KEY_8,
        N9 = GLFW_KEY_9,

        Tab = GLFW_KEY_TAB,
        CapsLock = GLFW_KEY_CAPS_LOCK,
        Space = GLFW_KEY_SPACE,
        ShiftLeft = GLFW_KEY_LEFT_SHIFT,
        ShiftRight = GLFW_KEY_RIGHT_SHIFT,
        CtrlLeft = GLFW_KEY_LEFT_CONTROL,
        CtrlRight = GLFW_KEY_RIGHT_CONTROL,
        WindowsLeft = GLFW_KEY_LEFT_SUPER,
        WindowsRight = GLFW_KEY_RIGHT_SUPER,

        Backspace = GLFW_KEY_BACKSPACE,
        Enter = GLFW_KEY_ENTER,

        Insert = GLFW_KEY_INSERT,
        Home = GLFW_KEY_HOME,
        Delete = GLFW_KEY_DELETE,
        End = GLFW_KEY_DELETE,
        PageUp = GLFW_KEY_PAGE_UP,
        PageDown = GLFW_KEY_PAGE_DOWN,

        Up = GLFW_KEY_UP,
        Down = GLFW_KEY_DOWN,
        Left = GLFW_KEY_LEFT,
        Right = GLFW_KEY_RIGHT,

        NumLock = GLFW_KEY_NUM_LOCK,
        NumDivide = GLFW_KEY_KP_DIVIDE,
        NumMultiply = GLFW_KEY_KP_MULTIPLY,
        NumSubtract = GLFW_KEY_KP_SUBTRACT,
        NumAdd = GLFW_KEY_KP_ADD,
        NumEnter = GLFW_KEY_KP_ENTER,
        NumDelete = GLFW_KEY_KP_DECIMAL,

        Num0 = GLFW_KEY_KP_0,
        Num1 = GLFW_KEY_KP_1,
        Num2 = GLFW_KEY_KP_2,
        Num3 = GLFW_KEY_KP_3,
        Num4 = GLFW_KEY_KP_4,
        Num5 = GLFW_KEY_KP_5,
        Num6 = GLFW_KEY_KP_6,
        Num7 = GLFW_KEY_KP_7,
        Num8 = GLFW_KEY_KP_8,
        Num9 = GLFW_KEY_KP_9,

        OemTilde = GLFW_KEY_GRAVE_ACCENT,          // (german: ^)
        OemMinus = GLFW_KEY_MINUS,                 // ß
        OemEqual = GLFW_KEY_EQUAL,                 // ´ (next to backspace)
        OemOpenBracket = GLFW_KEY_LEFT_BRACKET,    // ü
        OemCloseBracket = GLFW_KEY_RIGHT_BRACKET,  // +
        OemBackslash = GLFW_KEY_BACKSLASH,         // #
        OemSemicolon = GLFW_KEY_SEMICOLON,         // ö
        OemApostrophe = GLFW_KEY_APOSTROPHE,       // ä
        OemComma = GLFW_KEY_COMMA,
        OemPeriod = GLFW_KEY_PERIOD,
        OemSlash = GLFW_KEY_SLASH,     // -
        WorldKey1 = GLFW_KEY_WORLD_1,  // <,>,| (non US key)
        WorldKey2 = GLFW_KEY_WORLD_2,
    };

    enum class MouseButton
    {
        None = 0,
        Left = GLFW_MOUSE_BUTTON_LEFT,
        Right = GLFW_MOUSE_BUTTON_RIGHT,
        Middle = GLFW_MOUSE_BUTTON_MIDDLE,
    };
}  // namespace OpenS4::Input::GLFWMapping
