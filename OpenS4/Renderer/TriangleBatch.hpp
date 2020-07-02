#pragma once

#include "../main.hpp"

namespace OpenS4::Renderer {
class TriangleBatch {
   public:
    GLuint m_vertexArray = 0;
    GLuint m_textureArray = 0;
    GLuint m_vertexArrayObject = 0;

    u64 m_numberOfVertices = 0;
    u64 m_vertexArrayBufferSize = 0;
    u64 m_textureArrayBufferSize = 0;

    GLuint m_colorArray = 0;
    u64 m_colorArrayaBufferSize = 0;

   public:
    TriangleBatch() {}
    ~TriangleBatch() {
        if (m_vertexArray != 0) glDeleteBuffers(1, &m_vertexArray);
        if (m_textureArray != 0) glDeleteBuffers(1, &m_textureArray);
        if (m_textureArrayBufferSize != 0) glDeleteBuffers(1, &m_colorArray);
        if (m_vertexArrayObject != 0)
            glDeleteVertexArrays(1, &m_vertexArrayObject);
    }

    void begin(u64 numberOfVertices) {
        glGenVertexArrays(1, &m_vertexArrayObject);
        glBindVertexArray(m_vertexArrayObject);

        m_numberOfVertices = numberOfVertices;
    }
    void end() {
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayObject);
        glUnmapBuffer(GL_ARRAY_BUFFER);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexArray);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

        glEnableVertexAttribArray(1),
            glBindBuffer(GL_ARRAY_BUFFER, m_textureArray);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

        glBindVertexArray(0);
    }

    void copyVertexData(float* xyData) {
        glGenBuffers(1, &m_vertexArray);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexArray);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_numberOfVertices,
                     xyData, GL_DYNAMIC_DRAW);
    }

    void copyTextureData(float* xyTexture) {
        glGenBuffers(1, &m_textureArray);
        glBindBuffer(GL_ARRAY_BUFFER, m_textureArray);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_numberOfVertices,
                     xyTexture, GL_DYNAMIC_DRAW);
    }

    void draw() {
        glBindVertexArray(m_vertexArrayObject);
        glDrawArrays(GL_TRIANGLES, 0, m_numberOfVertices);
        glBindVertexArray(0);
    }

    void updateData(float* xyData, float* xyTexture, u64 numberOfElements) {
        if (m_vertexArrayObject == 0) {
            glGenVertexArrays(1, &m_vertexArrayObject);
        }
        glBindVertexArray(m_vertexArrayObject);

        if (m_vertexArray == 0) {
            glGenBuffers(1, &m_vertexArray);
        }
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexArray);
        if (m_vertexArrayBufferSize < numberOfElements) {
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numberOfElements,
                         xyData, GL_DYNAMIC_DRAW);
            m_vertexArrayBufferSize = numberOfElements;
        } else {
            glBufferSubData(GL_ARRAY_BUFFER, 0,
                            sizeof(float) * numberOfElements, xyData);
        }
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

        if (m_textureArray == 0) {
            glGenBuffers(1, &m_textureArray);
        }
        glBindBuffer(GL_ARRAY_BUFFER, m_textureArray);
        if (m_textureArrayBufferSize < numberOfElements) {
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numberOfElements,
                         xyTexture, GL_DYNAMIC_DRAW);
        } else {
            glBufferSubData(GL_ARRAY_BUFFER, 0,
                            sizeof(float) * numberOfElements, xyTexture);
        }
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glDisableVertexAttribArray(2);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        m_numberOfVertices = numberOfElements;
    }

    void updateData(float* xyData, float* xyTexture, float* rgbaColor,
                    u64 numberOfElements) {
        if (m_vertexArrayObject == 0) {
            glGenVertexArrays(1, &m_vertexArrayObject);
        }
        glBindVertexArray(m_vertexArrayObject);

        if (m_vertexArray == 0) {
            glGenBuffers(1, &m_vertexArray);
        }
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexArray);
        if (m_vertexArrayBufferSize < numberOfElements) {
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numberOfElements,
                         xyData, GL_DYNAMIC_DRAW);
            m_vertexArrayBufferSize = numberOfElements;
        } else {
            glBufferSubData(GL_ARRAY_BUFFER, 0,
                            sizeof(float) * numberOfElements, xyData);
        }
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

        if (m_textureArray == 0) {
            glGenBuffers(1, &m_textureArray);
        }
        glBindBuffer(GL_ARRAY_BUFFER, m_textureArray);
        if (m_textureArrayBufferSize < numberOfElements) {
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numberOfElements,
                         xyTexture, GL_DYNAMIC_DRAW);
            m_textureArrayBufferSize = numberOfElements;
        } else {
            glBufferSubData(GL_ARRAY_BUFFER, 0,
                            sizeof(float) * numberOfElements, xyTexture);
        }
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

        if (m_colorArray == 0) {
            glGenBuffers(1, &m_colorArray);
        }
        glBindBuffer(GL_ARRAY_BUFFER, m_colorArray);
        if (m_colorArrayaBufferSize < numberOfElements * 2) {
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numberOfElements * 2,
                         rgbaColor, GL_DYNAMIC_DRAW);
            m_colorArrayaBufferSize = numberOfElements * 2;
        } else {
            glBufferSubData(GL_ARRAY_BUFFER, 0,
                            sizeof(float) * numberOfElements * 2, rgbaColor);
        }
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, 0);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        m_numberOfVertices = numberOfElements;
    }
};

}  // namespace OpenS4::Renderer
