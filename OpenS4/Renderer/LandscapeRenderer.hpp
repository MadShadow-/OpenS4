#pragma once

#include <glm/matrix.hpp>

#include <Logger/Logger.hpp>
#include "../main.hpp"
#include "Landscape.hpp"
#include "TransformationPipeline.hpp"
#include "TriangleBatch.hpp"

namespace OpenS4::Renderer {

class LandscapeRenderer {
    OpenS4::Renderer::Landscape* m_landscape;

    std::vector<std::vector<TriangleBatch>> m_batches;

    u64 CHUNK_SIZE = 64;

   public:
    LandscapeRenderer(OpenS4::Renderer::Landscape* landscape) {
        m_landscape = landscape;

        u32 chunksX = landscape->getWidth() / CHUNK_SIZE;
        u32 chunksY = landscape->getHeight() / CHUNK_SIZE;

        m_batches.resize(chunksY);

        for (u32 chunkY = 0; chunkY < chunksY; chunkY++) {
            m_batches[chunkY].resize(chunksX);
            for (u32 chunkX = 0; chunkX < chunksX; chunkX++) {
                updateChunk(chunkX, chunkY);
            }
        }
    }

    void draw(GLFWwindow* window, TransformationPipeline* transformPipeline) {
        int width = 0;
        int height = 0;

        glfwGetFramebufferSize(window, &width, &height);

        glm::vec2 tl =
            screenToWorld(glm::vec2(0, 0), transformPipeline, width, height);
        glm::vec2 tr = screenToWorld(glm::vec2(width, 0), transformPipeline,
                                     width, height);
        glm::vec2 bl = screenToWorld(glm::vec2(0, height), transformPipeline,
                                     width, height);
        glm::vec2 br = screenToWorld(glm::vec2(width, height),
                                     transformPipeline, width, height);

        tl = toModelPosition(tl);
        tr = toModelPosition(tr);
        bl = toModelPosition(bl);
        br = toModelPosition(br);

        tl /= CHUNK_SIZE;
        tr /= CHUNK_SIZE;
        bl /= CHUNK_SIZE;
        br /= CHUNK_SIZE;

        tl = glm::floor(tl);
        br = glm::ceil(br);

        glm::vec2 visibleVolumeLowerBound = tl;
        glm::vec2 visibleVolumeUpperBound = br;

        for (int y = 0; y < m_batches.size(); y++) {
            if (y >= visibleVolumeLowerBound.y &&
                y <= visibleVolumeUpperBound.y) {
                for (int x = 0; x < m_batches.size(); x++) {
                    if (x >= visibleVolumeLowerBound.x &&
                        x <= visibleVolumeUpperBound.x)
                        m_batches[y][x].draw();
                }
            }
        }
    }

    void updateChunk(u32 chunkX, u32 chunkY) {
        std::vector<float> vertices;
        std::vector<float> texture;


        // calculate borders for this chunk
        u64 x_max = std::min((chunkX + 1) * CHUNK_SIZE,
                             (u64)(m_landscape->getWidth() - 1));
        u64 y_max = std::min((chunkY + 1) * CHUNK_SIZE,
                             (u64)(m_landscape->getHeight()));

        u64 x_min = std::max(chunkX * CHUNK_SIZE, (u64)0);
        u64 y_min = std::max(chunkY * CHUNK_SIZE, (u64)1);

        // * 2 (quads -> tri)
        // * 3 (tri -> #vertices)
        // * 2 (#coordinates (x,y))
        u64 numberOfCoordinates = (x_max - x_min) * (y_max - y_min) * 2 * 3 * 2;

        // reserve memory
        vertices.reserve(numberOfCoordinates);
        texture.reserve(numberOfCoordinates);

        for (auto y = y_min; y < y_max; y++) {
            for (auto x = x_min; x < x_max; x++) {
                const OpenS4::Renderer::LandscapeTexturePosition& pos =
                    m_landscape->calcGetTexturePositionLow(x, y);

                const OpenS4::Renderer::LandscapeTexturePosition& pos2 =
                    m_landscape->calcGetTexturePositionHigh(x, y);

                /*
                Positions:
                       3 --- 4
                      /     /
                     /     /
                    1 --- 2
                */

                glm::vec2 p1 = toPixelPosition(glm::vec2(x, y));
                p1.y += m_landscape->getTerrainHeight(x, y);

                glm::vec2 p2 = toPixelPosition(glm::vec2(x + 1, y));
                p2.y += m_landscape->getTerrainHeight(x + 1, y);

                glm::vec2 p3 = toPixelPosition(glm::vec2(x, y - 1));
                p3.y += m_landscape->getTerrainHeight(x, y - 1);

                glm::vec2 p4 = toPixelPosition(glm::vec2(x + 1, y - 1));
                p4.y += m_landscape->getTerrainHeight(x + 1, y - 1);

                vertices.push_back(p1.x);
                vertices.push_back(p1.y);
                texture.push_back(pos.p.x);
                texture.push_back(pos.p.y);

                vertices.push_back(p2.x);
                vertices.push_back(p2.y);
                texture.push_back(pos.p2.x);
                texture.push_back(pos.p2.y);

                vertices.push_back(p3.x);
                vertices.push_back(p3.y);
                texture.push_back(pos.p3.x);
                texture.push_back(pos.p3.y);

                vertices.push_back(p2.x);
                vertices.push_back(p2.y);
                texture.push_back(pos2.p.x);
                texture.push_back(pos2.p.y);

                vertices.push_back(p4.x);
                vertices.push_back(p4.y);
                texture.push_back(pos2.p2.x);
                texture.push_back(pos2.p2.y);

                vertices.push_back(p3.x);
                vertices.push_back(p3.y);
                texture.push_back(pos2.p3.x);
                texture.push_back(pos2.p3.y);
            }
        }

        m_batches[chunkY][chunkX].updateData(vertices.data(), texture.data(),
                                             numberOfCoordinates);
    }
};

}  // namespace OpenS4::Renderer
