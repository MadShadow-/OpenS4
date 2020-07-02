#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdint.h>

#include <filesystem>
#include <iostream>


#include <Import/Graphics/Gfx.hpp>

#ifdef _WIN32
#pragma comment(lib, "opengl32")
#endif

#include <glm/vec2.hpp>

#include "Renderer/TransformationPipeline.hpp"

glm::vec2 toPixelPosition(glm::vec2 model);
glm::vec2 toModelPosition(glm::vec2 pixel);
glm::vec2 screenToWorld(glm::vec2 screen,
                        OpenS4::Renderer::TransformationPipeline* transform,
                        int width, int height);

void TERMINATE(u64 errorCode = 1);
