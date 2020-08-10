#include "ImguiMain.hpp"


#include "ImguiInputListener.hpp"

namespace OpenS4::Gui::Imgui
{
    void InitImgui(GLFWwindow* window)
    {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;
        // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable
        // Keyboard Controls io.ConfigFlags |=
        // ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        // ImGui::StyleColorsClassic();

        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window, false);
        ImGui_ImplOpenGL3_Init();

        std::shared_ptr<ImguiInputListener> listener =
            std::make_shared<ImguiInputListener>("ImguiListener", window);
        OpenS4::Input::InputHandler* handler =
            OpenS4::Input::InputHandler::getInstance();
        handler->registerListenerAtTop(listener);
    }

    void NewFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void Cleanup()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void BeginRenderWindows() { NewFrame(); }
    void EndRenderWindows()
    {
        // Render imgui content
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void DemoWindow()
    {
        static bool show_demo_window{false};
        static bool show_another_window{false};
        static float clear_color[3] = {};
        static float f = 0.0f;
        static int counter = 0;

        // my imgui content
        ImGui::Begin("Hello, world!");  // Create a window called "Hello,
                                        // world!" and append into it.

        ImGui::Text("This is some useful text.");  // Display some text (you can
                                                   // use a format strings too)
        ImGui::Checkbox("Demo Window",
                        &show_demo_window);  // Edit bools storing our window
                                             // open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat(
            "float",
            &f,
            0.0f,
            1.0f);  // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3(
            "clear color",
            (float*)&clear_color);  // Edit 3 floats representing a color

        if (ImGui::Button(
                "Button"))  // Buttons return true when clicked (most widgets
                            // return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                    1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);
        ImGui::End();
    }

    void RenderViewInformationWindow(
        GLFWwindow* window,
        OpenS4::Renderer::LandscapeRenderer* landscapeRenderer, int hoveredEntity)
    {
        ImGui::Begin("View Information");

        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        if (landscapeRenderer)
        {
            auto world = landscapeRenderer->screenToWorld(glm::vec2(xpos, ypos));
            auto vec = landscapeRenderer->toModelPositionHighPrecision(world);
            ImGui::Text("Grid Cursor Position: %lf, %lf",
                        vec.x, vec.y);
        }
        ImGui::Text("Entity under mouse: %d", hoveredEntity);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                    1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);

        ImGui::End();
    }

}  // namespace OpenS4::Gui::Imgui
