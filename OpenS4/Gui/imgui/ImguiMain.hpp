//#include <GLFW/glfw3.h>
#include <imgui/imgui.h>

//#include "ImguiInputListener.hpp"
#include <Renderer/LandscapeRenderer.hpp>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace OpenS4::Gui::Imgui
{
    void InitImgui(GLFWwindow* window);
    void NewFrame();
    void Cleanup();

    void BeginRenderWindows();
    void EndRenderWindows();

    void DemoWindow();

    void RenderViewInformationWindow(
        GLFWwindow* window,
        OpenS4::Renderer::LandscapeRenderer* landscapeRenderer, int hoveredEntity);

}  // namespace OpenS4::Gui::Imgui
