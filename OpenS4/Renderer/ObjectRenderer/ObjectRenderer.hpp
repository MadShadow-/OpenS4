#pragma once

#include "../Batch/TriangleBatch.hpp"
#include "../LandscapeRenderer.hpp"
#include "MultiAtlas.hpp"

namespace OpenS4::Renderer::ObjectRenderer {
class ObjectRenderer {
    OpenS4::Renderer::ObjectRenderer::MultiAtlas m_multiAtlas;

    std::vector<OpenS4::Renderer::TriangleBatch> m_batches;

    std::vector<std::vector<float>> m_vertexBuffer;
    std::vector<std::vector<float>> m_textureBuffer;
    std::vector<std::vector<float>> m_colorBuffer;

    OpenS4::Renderer::LandscapeRenderer* m_landscapeRenderer;

   public:
    ObjectRenderer(OpenS4::Renderer::LandscapeRenderer* landscapeRenderer) {
        m_landscapeRenderer = landscapeRenderer;
    }

    u32 addImage(OpenS4::Import::ImageData image) {
        return m_multiAtlas.addImage(image);
    }

    void buildAtlasesContinuous(u32 size, bool finish = false) {
        if (!finish) {
            m_multiAtlas.buildAtlasesContinuous(size);
        } else {
            m_multiAtlas.buildAtlasesContinuousFinish(size);

            auto n = m_multiAtlas.getNumberOfAtlases();

            for (int i = 0; i < n; i++)
                m_batches.push_back(OpenS4::Renderer::TriangleBatch());
        }
    }

    void buildAtlases(u32 size) {
        m_multiAtlas.buildAtlases(size);

        auto n = m_multiAtlas.getNumberOfAtlases();

        for (int i = 0; i < n; i++)
            m_batches.push_back(OpenS4::Renderer::TriangleBatch());
    }

    void clear() {
        m_vertexBuffer.clear();
        m_textureBuffer.clear();
        m_colorBuffer.clear();

        m_vertexBuffer.resize(m_batches.size());
        m_textureBuffer.resize(m_batches.size());
        m_colorBuffer.resize(m_batches.size());

        //  for (auto& batch : m_batches)
        //      batch.updateData(nullptr, 0, 2, nullptr, 0, 2, nullptr, 0, 4);
    }

    void render(u32 x, u32 y, u32 imageID, u32 colorID) {
        // if (!m_landscapeRenderer->isVisibleModel(glm::vec2(x, y))) return;

        auto uv = m_multiAtlas.getUV(imageID);

        auto& vertexBuffer = m_vertexBuffer[uv->atlasID];
        auto& textureBuffer = m_textureBuffer[uv->atlasID];
        auto& colorBuffer = m_colorBuffer[uv->atlasID];

        auto coords = m_landscapeRenderer->toPixelPosition(glm::vec2(x, y));

        coords.x -= uv->width / 2;

        // clang-format off
        float vertices[12] = {
            coords.x, coords.y,
            coords.x, coords.y + uv->height,
            coords.x + uv->width, coords.y,
            coords.x + uv->width, coords.y,
            coords.x + uv->width, coords.y + uv->height,
            coords.x, coords.y + uv->height
        };
        float textures[12] = {
            uv->uBegin, uv->vEnd,
            uv->uBegin, uv->vBegin,
            uv->uEnd, uv->vEnd,
            uv->uEnd, uv->vEnd,
            uv->uEnd, uv->vBegin,
            uv->uBegin, uv->vBegin,
        };
        float colors[6] = {
            colorID, colorID, colorID, colorID, colorID, colorID
        };
        // clang-format on

        vertexBuffer.insert(vertexBuffer.end(), vertices, vertices + 12);
        textureBuffer.insert(textureBuffer.end(), textures, textures + 12);
        colorBuffer.insert(colorBuffer.end(), colors, colors + 6);

        return;

        // Vertex 1 Bottom Left
        vertexBuffer.push_back(coords.x);
        vertexBuffer.push_back(coords.y);
        textureBuffer.push_back(uv->uBegin);
        textureBuffer.push_back(uv->vEnd);
        colorBuffer.push_back(1);
        colorBuffer.push_back(1);
        colorBuffer.push_back(1);
        colorBuffer.push_back(1);
        // Vertex 2 Top Left
        vertexBuffer.push_back(coords.x);
        vertexBuffer.push_back(coords.y + uv->height);
        textureBuffer.push_back(uv->uBegin);
        textureBuffer.push_back(uv->vBegin);
        colorBuffer.push_back(1);
        colorBuffer.push_back(1);
        colorBuffer.push_back(1);
        colorBuffer.push_back(1);
        // Vertex 3 Bottom Right
        vertexBuffer.push_back(coords.x + uv->width);
        vertexBuffer.push_back(coords.y);
        textureBuffer.push_back(uv->uEnd);
        textureBuffer.push_back(uv->vEnd);
        colorBuffer.push_back(1);
        colorBuffer.push_back(1);
        colorBuffer.push_back(1);
        colorBuffer.push_back(1);

        // Vertex 3 Bottom Right
        vertexBuffer.push_back(coords.x + uv->width);
        vertexBuffer.push_back(coords.y);
        textureBuffer.push_back(uv->uEnd);
        textureBuffer.push_back(uv->vEnd);
        colorBuffer.push_back(1);
        colorBuffer.push_back(1);
        colorBuffer.push_back(1);
        colorBuffer.push_back(1);

        // Vertex 3 Top Right
        vertexBuffer.push_back(coords.x + uv->width);
        vertexBuffer.push_back(coords.y + uv->height);
        textureBuffer.push_back(uv->uEnd);
        textureBuffer.push_back(uv->vBegin);
        colorBuffer.push_back(1);
        colorBuffer.push_back(1);
        colorBuffer.push_back(1);
        colorBuffer.push_back(1);

        // Vertex 2 Top Left
        vertexBuffer.push_back(coords.x);
        vertexBuffer.push_back(coords.y + uv->height);
        textureBuffer.push_back(uv->uBegin);
        textureBuffer.push_back(uv->vBegin);
        colorBuffer.push_back(1);
        colorBuffer.push_back(1);
        colorBuffer.push_back(1);
        colorBuffer.push_back(1);
    }

    void draw() {
        for (int i = 0; i < m_batches.size(); i++) {
            auto& batch = m_batches[i];

            auto& vertexBuffer = m_vertexBuffer[i];

            if (vertexBuffer.size() > 0) {
                auto& textureBuffer = m_textureBuffer[i];
                auto& colorBuffer = m_colorBuffer[i];

                batch.updateData(vertexBuffer, 2, textureBuffer, 2, colorBuffer,
                                 1);
                glBindTexture(GL_TEXTURE_2D, m_multiAtlas.getAtlasTextureID(i));
                batch.draw();
            }
        }
    }

    u32 getNumberOfAtlases() { return m_multiAtlas.getNumberOfAtlases(); }
};
}  // namespace OpenS4::Renderer::ObjectRenderer
