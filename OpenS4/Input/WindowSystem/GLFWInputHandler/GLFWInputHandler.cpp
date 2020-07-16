#include "GLFWInputHandler.hpp"

#include "../../../main.hpp"
#include "../../Events/KeyEvent.hpp"
#include "../../Events/MouseEvent.hpp"

namespace OpenS4::Input
{
    namespace GLFWCallbacks
    {
        void keyCallbackWrapper(GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            Modifier modifier{Modifier::None};
            EventType eventType{EventType::None};
            switch (action)
            {
                case GLFW_PRESS:
                    eventType = EventType::Pressed;
                    break;
                case GLFW_REPEAT:
                    eventType = EventType::PressedRepeat;
                    break;
                case GLFW_RELEASE:
                    eventType = EventType::Released;
                    break;
            }
            if (mods & GLFW_MOD_SHIFT)
            {
                modifier |= Modifier::Shift;
            }
            if (mods & GLFW_MOD_ALT)
            {
                modifier |= Modifier::Alt;
            }
            if (mods & GLFW_MOD_CONTROL)
            {
                modifier |= Modifier::Control;
            }
            if (mods & GLFW_MOD_CAPS_LOCK)
            {
                modifier |= Modifier::CapsLock;
            }
            if (mods & GLFW_MOD_NUM_LOCK)
            {
                modifier |= Modifier::NumPad;
            }
            KeyEvent keyEvent(eventType, (Key)key, modifier, 0);
            InputHandler::getInstance()->populateKeyEvent(&keyEvent);
        }

        void characterCallbackWrapper(GLFWwindow* window, unsigned int codepoint)
        {
            KeyEvent keyEvent(EventType::CharacterInput, Key::None, Modifier::None, codepoint);
            InputHandler::getInstance()->populateKeyEvent(&keyEvent);
        }

        void cursorPositionCallbackWrapper(GLFWwindow* window, double xpos, double ypos)
        {
            MouseEvent mouseEvent(EventType::Moved, xpos, ypos, MouseButton::None, false, 0, Modifier::None);
            InputHandler::getInstance()->populateMouseEvent(&mouseEvent);
        }

        void cursorEnterCallbackWrapper(GLFWwindow* window, int entered)
        {
            MouseEvent mouseEvent(EventType::Moved, 0, 0, MouseButton::None, entered, 0, Modifier::None);
            InputHandler::getInstance()->populateMouseEvent(&mouseEvent);
        }

        void mouseButtonCallbackWrapper(GLFWwindow* window, int button, int action, int mods)
        {
            Modifier modifier{Modifier::None};
            EventType eventType{EventType::None};
            switch (action)
            {
                case GLFW_PRESS:
                    eventType = EventType::Pressed;
                    break;
                case GLFW_REPEAT:
                    eventType = EventType::PressedRepeat;
                    break;
                case GLFW_RELEASE:
                    eventType = EventType::Released;
                    break;
            }
            if (mods & GLFW_MOD_SHIFT)
            {
                modifier |= Modifier::Shift;
            }
            if (mods & GLFW_MOD_ALT)
            {
                modifier |= Modifier::Alt;
            }
            if (mods & GLFW_MOD_CONTROL)
            {
                modifier |= Modifier::Control;
            }
            if (mods & GLFW_MOD_CAPS_LOCK)
            {
                modifier |= Modifier::CapsLock;
            }
            if (mods & GLFW_MOD_NUM_LOCK)
            {
                modifier |= Modifier::NumPad;
            }
            MouseEvent mouseEvent(eventType, 0, 0, (MouseButton)button, false, 0, modifier);
            InputHandler::getInstance()->populateMouseEvent(&mouseEvent);
        }

        void scrollCallbackWrapper(GLFWwindow* window, double xoffset, double yoffset)
        {
            MouseEvent mouseEvent(EventType::Scrolled, 0, 0, MouseButton::None, false, yoffset, Modifier::None);
            InputHandler::getInstance()->populateMouseEvent(&mouseEvent);
        }

    }  // namespace GLFWCallbacks

    GLFWInputHandler::GLFWInputHandler() : InputHandler() { instance = this; }

    void GLFWInputHandler::registerCallbacks(GLFWwindow* window)
    {
        // enable capslock and numpad modifiers:
        glfwSetInputMode(window, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
        glfwSetKeyCallback(window, GLFWCallbacks::keyCallbackWrapper);
        glfwSetCharCallback(window, GLFWCallbacks::characterCallbackWrapper);
        glfwSetCursorPosCallback(window, GLFWCallbacks::cursorPositionCallbackWrapper);
        glfwSetCursorEnterCallback(window, GLFWCallbacks::cursorEnterCallbackWrapper);
        glfwSetMouseButtonCallback(window, GLFWCallbacks::mouseButtonCallbackWrapper);
        glfwSetScrollCallback(window, GLFWCallbacks::scrollCallbackWrapper);
    }

}  // namespace OpenS4::Input
