#pragma once

#include "../main.hpp"

namespace OpenS4::Renderer {
GLuint LoadShader(const char* vs, const char* fs) {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    if (vertexShader == 0) {
        OpenS4::getLogger().err("%s: glCreateShader(VERTEX_SHADER) failed!",
                                __FILE__);
        TERMINATE();
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    if (fragmentShader == 0) {
        OpenS4::getLogger().err(
            "%s: glCreateShader(GL_FRAGMENT_SHADER) failed!", __FILE__);
        TERMINATE();
    }

    const char** vsA = &vs;
    const char** fsA = &fs;

    glShaderSource(vertexShader, 1, vsA, NULL);
    glShaderSource(fragmentShader, 1, fsA, NULL);

    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);

    GLint status = 0;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        OpenS4::getLogger().err("%s: glCompileShader for VERTEX_SHADER failed!",
                                __FILE__);
        TERMINATE();
    }

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        OpenS4::getLogger().err(
            "%s: glCompileShader for FRAGMENT_SHADER failed!", __FILE__);
        TERMINATE();
    }

    GLint shader = glCreateProgram();
    if (shader == 0) {
        OpenS4::getLogger().err("%s: glCreateProgram failed!", __FILE__);
        TERMINATE();
    }

    glAttachShader(shader, vertexShader);
    glAttachShader(shader, fragmentShader);
    glLinkProgram(shader);

    glGetProgramiv(shader, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        OpenS4::getLogger().err("%s: glLinkProgram failed!", __FILE__);
        TERMINATE();
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shader;
}
}  // namespace OpenS4::Renderer
