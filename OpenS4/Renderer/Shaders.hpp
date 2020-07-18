#pragma once

#include "../main.hpp"

#define STRINGIFY(a) (#a)

namespace OpenS4::Renderer::Shader {
extern const char* FRAGMENTSHADER_MAP;
extern const char* VERTEXSHADER_MAP;
extern const char* FRAGMENTSHADER_SETTLER;
extern const char* VERTEXSHADER_SETTLER;

extern const char* VERTEXSHADER_POINT;
extern const char* FRAGMENTSHADER_POINT;
}  // namespace OpenS4::Renderer::Shader
