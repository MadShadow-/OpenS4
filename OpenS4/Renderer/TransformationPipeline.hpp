#pragma once

#include "MatrixStack.hpp"

namespace OpenS4::Renderer {

class TransformationPipeline {
    OpenS4::Renderer::MatrixStack m_modelView;
    OpenS4::Renderer::MatrixStack m_projection;

    glm::mat4x4 m_mvp;

   public:
    OpenS4::Renderer::MatrixStack* getModelViewStack() {
        return &m_modelView;
    }
    OpenS4::Renderer::MatrixStack* getProjectionStack() {
        return &m_projection;
    }

    const glm::mat4x4& getProjectionMatrix() const {
        return m_projection.getMatrix();
    }

    const glm::mat4x4& getModelViewMatrix() const {
        return m_modelView.getMatrix();
    }

    const glm::mat4x4& getModelViewProjectionMatrix() {
        m_mvp = getProjectionMatrix() * getModelViewMatrix();
        return m_mvp;
    }
};

}  // namespace OpenS4::Renderer
