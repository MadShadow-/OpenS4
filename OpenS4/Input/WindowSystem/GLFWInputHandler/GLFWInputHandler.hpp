#pragma once
#include "Input/InputHandler.hpp"

namespace OpenS4::Input
{
    class GLFWInputHandler : public OpenS4::Input::InputHandler
    {
       public:
        static GLFWInputHandler* getInstance()
        {
            if(instance==nullptr)
            {
                instance = new GLFWInputHandler();
            }
            return (GLFWInputHandler*)instance;
        }
        void registerCallbacks(GLFWwindow* window);

       private:
        GLFWInputHandler();
    };
}  // namespace OpenS4::Input
