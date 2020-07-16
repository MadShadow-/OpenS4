#pragma once

#include <Logger/Logger.hpp>
#include <glm/gtx/normal.hpp>
#include <glm/matrix.hpp>

#include "../main.hpp"
#include "Landscape.hpp"
#include "TransformationPipeline.hpp"
#include "TriangleBatch.hpp"

namespace OpenS4::Renderer {

class LandscapeRenderer {
    OpenS4::Renderer::Landscape* m_landscape;

    std::vector<std::vector<TriangleBatch>> m_batches;

    u64 CHUNK_SIZE = 64;

    float getGradient(u64 x, u64 y) {
        if (x == 0 || y == 0 || x >= m_landscape->getWidth() - 1 ||
            y >= m_landscape->getHeight() - 1) {
            return 1.0f;
        }

        auto h1 = m_landscape->getTerrainHeight(x, y + 1) * 2;
        auto h2 = m_landscape->getTerrainHeight(x + 1, y + 1) * 2;
        auto h3 = m_landscape->getTerrainHeight(x + 1, y) * 2;
        auto h4 = m_landscape->getTerrainHeight(x, y - 1) * 2;
        auto h5 = m_landscape->getTerrainHeight(x - 1, y - 1) * 2;
        auto h6 = m_landscape->getTerrainHeight(x - 1, y) * 2;

        auto h0 = m_landscape->getTerrainHeight(x, y) * 2;

        int n = 0;

        auto diff1 = h0 - h1;
        auto diff2 = h0 - h2;
        auto diff3 = h0 - h3;
        auto diff4 = h0 - h4;
        auto diff5 = h0 - h5;
        auto diff6 = h0 - h6;

        if (diff1 > 0) n++;
        if (diff2 > 0) n++;

        if (diff5 > 0) n--;
        if (diff4 > 0) n--;

        if (diff3 > 0) n--;
        if (diff6 > 0) n--;

        n = 0;
        if (diff1 < 0) n += 3 * diff1;
        if (diff2 < 0) n += 3 * diff2;

        n -= 1 * diff5;
        n -= 1 * diff4;

        n -= diff3;
        n += diff6;

        return 1.0f + n / 150.0f;
    }

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

        std::vector<float> color;
        color.reserve(numberOfCoordinates / 2 * 3);

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
                p1.y += m_landscape->getTerrainHeight(x, y) * 2;

                glm::vec2 p2 = toPixelPosition(glm::vec2(x + 1, y));
                p2.y += m_landscape->getTerrainHeight(x + 1, y) * 2;

                glm::vec2 p3 = toPixelPosition(glm::vec2(x, y - 1));
                p3.y += m_landscape->getTerrainHeight(x, y - 1) * 2;

                glm::vec2 p4 = toPixelPosition(glm::vec2(x + 1, y - 1));
                p4.y += m_landscape->getTerrainHeight(x + 1, y - 1) * 2;

                // p1.x += m_landscape->getTerrainHeight(x, y);
                // p2.x += m_landscape->getTerrainHeight(x+1, y);
                // p3.x += m_landscape->getTerrainHeight(x, y-1);
                // p4.x += m_landscape->getTerrainHeight(x+1, y-1);

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

                auto h1 = m_landscape->getTerrainHeight(x, y);
                auto h2 = m_landscape->getTerrainHeight(x + 1, y);
                auto h3 = m_landscape->getTerrainHeight(x, y - 1);
                auto h4 = m_landscape->getTerrainHeight(x + 1, y - 1);

                auto diffA = (h3 - h1) / 255.0f;
                auto diffB = (h4 - h2) / 255.0f;

                // gradA += diffA * 20.0f;
                // gradB += diffB * 20.0f;

                auto gradient = getGradient(x, y);
                color.push_back(gradient);

                gradient = getGradient(x + 1, y);
                color.push_back(gradient);

                gradient = getGradient(x, y - 1);
                color.push_back(gradient);

                gradient = getGradient(x + 1, y);
                color.push_back(gradient);

                gradient = getGradient(x + 1, y - 1);
                color.push_back(gradient);

                gradient = getGradient(x, y - 1);
                color.push_back(gradient);
            }
        }

        m_batches[chunkY][chunkX].updateData(vertices, 2, texture, 2, color, 1);
    }
};  // namespace OpenS4::Renderer

}  // namespace OpenS4::Renderer
