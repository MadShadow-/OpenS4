#pragma once

#include <glm/glm.hpp>
#include <glm/matrix.hpp>

#include "TransformationPipeline.hpp"

namespace OpenS4::Renderer
{
    class OrthographicCamera
    {
        OpenS4::Renderer::TransformationPipeline m_transformation;

        float m_width = 0;
        float m_height = 0;

        float m_zoom = 1.0f;

        glm::vec2 m_translation;

        glm::mat4x4 m_mvp;

       public:
        OrthographicCamera() { update(); }

        void addTranslation(glm::vec2 translation)
        {
            m_translation += translation;
        }

        void setZoom(float zoom) { m_zoom = zoom; }

        void setTranslation(glm::vec2 translation)
        {
            m_translation = translation;
        }

        void setWindowSize(u32 width, u32 height)
        {
            m_width = width;
            m_height = height;
        }

        float getScreenHeight() { return m_height; }
        float getScreenWidth() { return m_width; }

        glm::mat4x4 getProjectionMatrix()
        {
            return m_transformation.getProjectionMatrix();
        }

        glm::mat4x4 getModelViewMatrix()
        {
            return m_transformation.getModelViewMatrix();
        }

        glm::mat4x4 getModelViewProjectionMatrix() { return m_mvp; }

        void setScreenSize(float width, float height)
        {
            m_width = width;
            m_height = height;
        }

        void update()
        {
            glm::mat4x4 ortho = glm::ortho(-m_width / 2,
                                           m_width / 2,
                                           -m_height / 2,
                                           m_height / 2,
                                           -200.0f,
                                           200.0f);

            m_transformation.getProjectionStack()->loadMatrix(ortho);

            m_transformation.getProjectionStack()->scale(m_zoom, m_zoom, 1);
            m_transformation.getProjectionStack()->translate(
                m_translation.x, m_translation.y, 0);

            m_mvp = m_transformation.getModelViewProjectionMatrix();
        }
    };

}  // namespace OpenS4::Renderer
