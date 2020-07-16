#pragma once
#include "../main.hpp"
#ifdef GLFW_VERSION_MAJOR
#include "Input/WindowSystem/GLFWInputHandler/GLFWMapping.hpp"
#else
#error NO INPUT MAPPING FOUND ;(
#endif
