#pragma once
#include "../../Input/InputListener.hpp"

namespace OpenS4::Gui::Imgui
{
    /// @brief Redirects inputs to imgui.
    class ImguiInputListener : public OpenS4::Input::InputListener
    {
       public:
        ImguiInputListener(std::string name, GLFWwindow* window);
        OpenS4::Input::EventDispatching keyPressed(const OpenS4::Input::KeyEvent* event) override;
        OpenS4::Input::EventDispatching keyReleased(const OpenS4::Input::KeyEvent* event) override;
        OpenS4::Input::EventDispatching characterInput(const OpenS4::Input::KeyEvent* event) override;
        OpenS4::Input::EventDispatching mouseButtonPressed(const OpenS4::Input::MouseEvent* event) override;
        OpenS4::Input::EventDispatching mouseButtonReleased(const OpenS4::Input::MouseEvent* event) override;
        OpenS4::Input::EventDispatching mouseMoved(const OpenS4::Input::MouseEvent* event) override;
        OpenS4::Input::EventDispatching mouseScrolled(const OpenS4::Input::MouseEvent* event) override;
        OpenS4::Input::EventDispatching mouseEntered(const OpenS4::Input::MouseEvent* event) override;

       private:
        GLFWwindow* m_window{};

        OpenS4::Input::EventDispatching shouldPassEventMouseEvent();
        OpenS4::Input::EventDispatching shouldPassEventKeyEvent();
    };
}  // namespace OpenS4::Gui
