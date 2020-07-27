#include "ImguiInputListener.hpp"
#include "imgui_impl_glfw.h"

namespace OpenS4::Gui::Imgui
{
    ImguiInputListener::ImguiInputListener(std::string name, GLFWwindow* window) : Input::InputListener(name)
    {
        m_window = window;
    }

    Input::EventDispatching ImguiInputListener::keyPressed(const Input::KeyEvent* event)
    {
        ImGui_ImplGlfw_KeyCallback(m_window, (int)event->getKey(), 0, GLFW_PRESS, 0);
        return shouldPassEventKeyEvent();
    }

    Input::EventDispatching ImguiInputListener::keyReleased(const Input::KeyEvent* event)
    {
        ImGui_ImplGlfw_KeyCallback(m_window, (int)event->getKey(), 0, GLFW_RELEASE, 0);
        return shouldPassEventKeyEvent();
    }

    Input::EventDispatching ImguiInputListener::characterInput(const Input::KeyEvent* event)
    {
        ImGui_ImplGlfw_CharCallback(m_window, event->getChar());
        return shouldPassEventKeyEvent();
    }

    Input::EventDispatching ImguiInputListener::mouseButtonPressed(const Input::MouseEvent* event)
    {
        ImGui_ImplGlfw_MouseButtonCallback(m_window, (int)event->getButton(), GLFW_PRESS, 0);
        return shouldPassEventMouseEvent();
    }

    Input::EventDispatching ImguiInputListener::mouseButtonReleased(const Input::MouseEvent* event)
    {
        ImGui_ImplGlfw_MouseButtonCallback(m_window, (int)event->getButton(), GLFW_RELEASE, 0);
        return shouldPassEventMouseEvent();
    }

    Input::EventDispatching ImguiInputListener::mouseMoved(const Input::MouseEvent* event)
    {
        return Input::EventDispatching::PassEvent;
    }

    Input::EventDispatching ImguiInputListener::mouseScrolled(const Input::MouseEvent* event)
    {
        ImGui_ImplGlfw_ScrollCallback(m_window, 0, event->getMouseScroll());
        return shouldPassEventMouseEvent();
    }

    Input::EventDispatching ImguiInputListener::mouseEntered(const Input::MouseEvent* event)
    {
        return Input::EventDispatching::PassEvent;
    }

    OpenS4::Input::EventDispatching ImguiInputListener::shouldPassEventMouseEvent()
    {
        ImGuiIO& io = ImGui::GetIO();
        if (io.WantCaptureMouse)
        {
            return OpenS4::Input::EventDispatching::CatchEvent;
        }
        return OpenS4::Input::EventDispatching::PassEvent;
    }

    OpenS4::Input::EventDispatching ImguiInputListener::shouldPassEventKeyEvent()
    {
        ImGuiIO& io = ImGui::GetIO();
        if (io.WantCaptureKeyboard)
        {
            return OpenS4::Input::EventDispatching::CatchEvent;
        }
        return OpenS4::Input::EventDispatching::PassEvent;
    }

}  // namespace OpenS4::Input

