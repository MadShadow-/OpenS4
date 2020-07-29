#pragma once

#include "Core/Types.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

namespace OpenS4::Renderer {

class MatrixStack {
    glm::mat4x4* m_matrixStack = nullptr;
    u32 m_stackSize = 0;

    u32 m_index = 0;

   public:
    MatrixStack(u32 stackSize = 64) {
        m_stackSize = stackSize;
        m_matrixStack = new glm::mat4x4[m_stackSize];

        m_matrixStack[0] = glm::mat4x4();  // identity
    }
    MatrixStack(const MatrixStack&) = delete;
    ~MatrixStack() { delete m_matrixStack; }

    void pushMatrix() {
        if (m_index + 1 == m_stackSize) {
            OpenS4::getLogger().err("MatrixStack overflow!");
            return;
        }

        const glm::mat4x4 c = m_matrixStack[m_index];
        m_matrixStack[++m_index] = c;
    }

    void popMatrix() {
        if (m_index == 0) {
            OpenS4::getLogger().err("MatrixStack underflow!");
            return;
        }

        m_index--;
    }

    void loadIdentity() { m_matrixStack[m_index] = glm::mat4x4(); }

    void loadMatrix(const glm::mat4x4& mat) { m_matrixStack[m_index] = mat; }

    const glm::mat4x4& getMatrix() const { return m_matrixStack[m_index]; }

    void scale(float x, float y, float z) {
        m_matrixStack[m_index] *= glm::scale(glm::vec3(x, y, z));
    }

    void translate(float x, float y, float z) {
        m_matrixStack[m_index] *= glm::translate(glm::vec3(x, y, z));
    }
};
}  // namespace OpenS4::Renderer
