//#include <GLFW/glfw3.h>
#include <imgui/imgui.h>

//#include "ImguiInputListener.hpp"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace OpenS4::Gui::Imgui
{
    void InitImgui(GLFWwindow* window);
    void NewFrame();
    void Cleanup();
    void RenderWindows();

    void DemoWindow();
}  // namespace OpenS4::Gui::Imgui
