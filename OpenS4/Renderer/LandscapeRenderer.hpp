#pragma once

#include <Logger/Logger.hpp>
#include <glm/gtx/matrix_interpolation.hpp>
#include <glm/gtx/normal.hpp>
#include <glm/matrix.hpp>

#include "Batch/TriangleBatch.hpp"
#include "OrthoGraphicCamera.hpp"
#include "Landscape.hpp"
#include "TransformationPipeline.hpp"

namespace OpenS4::Renderer
{
    class LandscapeRenderer
    {
        OpenS4::Renderer::Landscape* m_landscape;

        std::vector<std::vector<TriangleBatch>> m_batches;

        u64 CHUNK_SIZE = 64;

        int XSTEP = 24;
        int YSTEP = 12;

        float getGradient(u64 x, u64 y)
        {
            if (x == 0 || y == 0 || x >= m_landscape->getWidth() - 1 ||
                y >= m_landscape->getHeight() - 1)
            {
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

        OpenS4::Renderer::OrthographicCamera* m_camera;

       public:
        LandscapeRenderer(OpenS4::Renderer::Landscape* landscape,
                          OpenS4::Renderer::OrthographicCamera* camera)
        {
            m_landscape = landscape;
            m_camera = camera;

            u32 chunksX = landscape->getWidth() / CHUNK_SIZE;
            u32 chunksY = landscape->getHeight() / CHUNK_SIZE;

            m_batches.resize(chunksY);
            for (u32 chunkY = 0; chunkY < chunksY; chunkY++)
            {
                m_batches[chunkY].resize(chunksX);
                for (u32 chunkX = 0; chunkX < chunksX; chunkX++)
                {
                    updateChunk(chunkX, chunkY);
                }
            }
        }

        void draw()
        {
            glm::vec2 tl = screenToWorld(glm::vec2(0, 0));
            glm::vec2 br = screenToWorld(glm::vec2(m_camera->getScreenWidth(), m_camera->getScreenHeight()));

            tl = toModelPosition(tl);
            br = toModelPosition(br);

            tl /= CHUNK_SIZE;
            br /= CHUNK_SIZE;

            tl = glm::floor(tl);
            br = glm::ceil(br);

            glm::vec2 visibleVolumeLowerBound = tl;
            glm::vec2 visibleVolumeUpperBound = br;

            for (int y = 0; y < m_batches.size(); y++)
            {
                if (y >= visibleVolumeLowerBound.y &&
                    y <= visibleVolumeUpperBound.y)
                {
                    for (int x = 0; x < m_batches.size(); x++)
                    {
                        if (x >= visibleVolumeLowerBound.x &&
                            x <= visibleVolumeUpperBound.x)
                            m_batches[y][x].draw();
                    }
                }
            }
        }

        void updateChunk(u32 chunkX, u32 chunkY)
        {
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
            u64 numberOfCoordinates =
                (x_max - x_min) * (y_max - y_min) * 2 * 3 * 2;

            // reserve memory
            vertices.reserve(numberOfCoordinates);
            texture.reserve(numberOfCoordinates);

            std::vector<float> color;
            color.reserve(numberOfCoordinates / 2 * 3);

            for (auto y = y_min; y < y_max; y++)
            {
                for (auto x = x_min; x < x_max; x++)
                {
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
                    glm::vec2 p2 = toPixelPosition(glm::vec2(x + 1, y));

                    glm::vec2 p3 = toPixelPosition(glm::vec2(x, y - 1));
                    glm::vec2 p4 = toPixelPosition(glm::vec2(x + 1, y - 1));

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

            m_batches[chunkY][chunkX].updateData(
                vertices, 2, texture, 2, color, 1);
        }

        void updateAllChunks()
        {
            u32 chunksX = m_landscape->getWidth() / CHUNK_SIZE;
            u32 chunksY = m_landscape->getHeight() / CHUNK_SIZE;
            for (u32 chunkY = 0; chunkY < chunksY; chunkY++)
            {
                for (u32 chunkX = 0; chunkX < chunksX; chunkX++)
                {
                    updateChunk(chunkX, chunkY);
                }
            }
        }

        __inline glm::vec2 toPixelPosition(glm::vec2 model)
        {
            glm::vec2 pos;
            pos.x = model.x * XSTEP - (model.y + 1) * XSTEP / 2;
            pos.y = -(model.y + 1) * YSTEP;

            if (model.x >= 0 && model.y >= 0 &&
                m_landscape->getHeight() > model.y &&
                m_landscape->getWidth() > model.x)
                pos.y += m_landscape->getTerrainHeight(model.x, model.y) * 1.2;

            return pos;
        }

        glm::vec2 toModelPosition(glm::vec2 pixel)
        {
            glm::vec2 model;

            model.y = -(pixel.y / YSTEP) - 1;
            model.x = (pixel.x + (model.y + 1) * XSTEP / 2) / XSTEP;

            return model;
        }

       private:
        bool isInTriangle(const glm::vec2& p,
                          const glm::vec2& p0,
                          const glm::vec2& p1,
                          const glm::vec2& p2)
        {
            auto s = p0.y * p2.x - p0.x * p2.y + (p2.y - p0.y) * p.x +
                     (p0.x - p2.x) * p.y;
            auto t = p0.x * p1.y - p0.y * p1.x + (p0.y - p1.y) * p.x +
                     (p1.x - p0.x) * p.y;

            if ((s < 0) != (t < 0)) return false;

            auto A = -p1.y * p2.x + p0.y * (p2.x - p1.x) +
                     p0.x * (p1.y - p2.y) + p1.x * p2.y;

            return A < 0 ? (s <= 0 && s + t >= A) : (s >= 0 && s + t <= A);
        }

        struct BarycentricFactors
        {
            float w1;
            float w2;
            float w3;
        };

        BarycentricFactors getBarycentric(const glm::vec2& pixel,
                                          const glm::vec2& _1,
                                          const glm::vec2& _2,
                                          const glm::vec2& _3)
        {
            BarycentricFactors factors;
            factors.w1 =
                ((_2.y - _3.y) * (pixel.x - _3.x) +
                 (_3.x - _2.x) * (pixel.y - _3.y)) /
                ((_2.y - _3.y) * (_1.x - _3.x) + (_3.x - _2.x) * (_1.y - _3.y));
            factors.w2 =
                ((_3.y - _1.y) * (pixel.x - _3.x) +
                 (_1.x - _3.x) * (pixel.y - _3.y)) /
                ((_2.y - _3.y) * (_1.x - _3.x) + (_3.x - _2.x) * (_1.y - _3.y));
            factors.w3 = 1 - factors.w1 - factors.w2;
            return factors;
        }

       public:
        glm::vec2 toModelPositionHighPrecision(glm::vec2 pixel)
        {
            glm::vec2 expected = toModelPosition(pixel);

            int x = expected.x;
            int y = expected.y;

            glm::vec2 neighbors[] = {glm::vec2(0, -1),
                                     glm::vec2(1, -1),
                                     glm::vec2(1, 0),
                                     glm::vec2(1, 1),
                                     glm::vec2(0, 1),
                                     glm::vec2(-1, 1),
                                     glm::vec2(-1, 0),
                                     glm::vec2(-1, -1)};

            int maxX = m_landscape->getWidth() - 1;
            int maxY = m_landscape->getHeight();
            int minX = 0;
            int minY = 1;

            x = std::min(x, maxX - 1);
            x = std::max(x, minX);

            y = std::min(y, maxY - 1);
            y = std::max(y, minY);

            bool changed = false;

            if (x >= 0 && x < m_landscape->getWidth() - 1 && y > 0 &&
                y < m_landscape->getHeight())
            {
                glm::vec2 actual = toPixelPosition(glm::vec2(x, y));

                float dist = glm::distance(pixel, actual);

                bool changed = false;

                do
                {
                    changed = false;
                    for (glm::vec2 neighbor : neighbors)
                    {
                        int xN = neighbor.x + x;
                        int yN = neighbor.y + y;
                        if (xN < maxX && xN >= minX && yN < maxY && yN >= minX)
                        {
                            glm::vec2 neigh =
                                toPixelPosition(glm::vec2(xN, yN));
                            if (glm::distance(neigh, pixel) < dist)
                            {
                                x = xN;
                                y = yN;
                                dist = glm::distance(neigh, pixel);
                                changed = true;
                            }
                        }
                    }

                } while (changed);
                int fX = x;
                int fY = y;
                for (auto y = std::max(fY - 5, minY);
                     y < std::min(fY + 5, maxY);
                     y++)
                {
                    for (auto x = std::max(fX - 5, minX);
                         x < std::min(fX + 5, maxX);
                         x++)
                    {
                        glm::vec2 _1 = toPixelPosition(glm::vec2(x, y));
                        glm::vec2 _2 = toPixelPosition(glm::vec2(x + 1, y));
                        glm::vec2 _3 = toPixelPosition(glm::vec2(x, y - 1));
                        glm::vec2 _4 = toPixelPosition(glm::vec2(x + 1, y - 1));

                        if (isInTriangle(pixel, _1, _2, _3))
                        {
                            BarycentricFactors factors =
                                getBarycentric(pixel, _1, _2, _3);

                            auto x_ = factors.w1 * x + factors.w2 * (x + 1) +
                                      factors.w3 * x;
                            auto y_ = factors.w1 * y + factors.w2 * y +
                                      factors.w3 * (y - 1);

                            return glm::vec2(x_, y_);
                        }
                        else if (isInTriangle(pixel, _2, _4, _3))
                        {
                            BarycentricFactors factors =
                                getBarycentric(pixel, _2, _4, _3);

                            auto x_ = factors.w1 * (x + 1) +
                                      factors.w2 * (x + 1) + factors.w3 * x;
                            auto y_ = factors.w1 * y + factors.w2 * (y - 1) +
                                      factors.w3 * (y - 1);

                            return glm::vec2(x_, y_);
                        }

                        if (isInTriangle(pixel, _1, _2, _3) ||
                            isInTriangle(pixel, _2, _4, _3))
                        {
                            auto q11 = glm::vec2(x, y);

                            auto y_ = (_3.y - pixel.y) / (_3.y - _1.y) * (1) +
                                      (y - 1);

                            auto x_ = (pixel.x + (y_ + 1) * XSTEP / 2) / XSTEP;

                            if (x_ < 0)
                            {
                                int z = 0;
                                z++;
                            }

                            {
                            }

                            // return glm::vec2(x_, y_);
                        }
                    }
                }
            }

            glm::vec2 model;
            model.x = -1;
            model.y = -1;
            return model;
        }

        glm::vec2 screenToWorld(glm::vec2 screen)
        {
            glm::vec2 clip(2 * screen.x / m_camera->getScreenWidth() - 1.0,
                           1 - (2 * screen.y) / m_camera->getScreenHeight());

            glm::vec3 clip3(clip.x, clip.y, -1);

            glm::vec4 ray(clip3, 1);

            glm::vec4 rayEye =
                glm::inverse(m_camera->getProjectionMatrix()) * ray;

            rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0, 0);

            glm::vec3 rayWorld = glm::vec3(
                glm::inverse(m_camera->getModelViewMatrix()) * rayEye);

            return glm::vec2(rayWorld.x, rayWorld.y);
        }

        glm::vec2 m_tr;
        glm::vec2 m_tl;
        glm::vec2 m_bl;
        glm::vec2 m_br;

        void updateBoundingBox()
        {

            m_tr = screenToWorld(glm::vec2(m_camera->getScreenWidth(), 0));
            m_bl = screenToWorld(glm::vec2(0, m_camera->getScreenHeight()));

            m_tr = toModelPosition(m_tr);
            m_bl = toModelPosition(m_bl);

            m_tl = screenToWorld(glm::vec2(0, 0));
            m_br = screenToWorld(glm::vec2(m_camera->getScreenWidth(),
                                           m_camera->getScreenHeight()));

            m_tl = toModelPosition(m_tl);
            m_br = toModelPosition(m_br);

            m_br.y += 20;
        }

        bool isVisibleModel(glm::vec2 model)
        {
            glm::vec2 visibleVolumeLowerBound = m_tl;
            glm::vec2 visibleVolumeUpperBound = m_br;

            int x = model.x;
            int y = model.y;

            if (y >= visibleVolumeLowerBound.y &&
                y <= visibleVolumeUpperBound.y)
            {
                if (x >= visibleVolumeLowerBound.x &&
                    x <= visibleVolumeUpperBound.x)
                    return true;
            }
            return false;
        }

        struct BoundingBox
        {
            glm::vec2 lower;
            glm::vec2 upper;
        };
        auto getBoundingBox()
        {
            BoundingBox box;
            box.lower = m_tl;
            box.upper = m_br;
            return box;
        }

        auto getHeight() { return m_landscape->getHeight(); }
        auto getWidth() { return m_landscape->getWidth(); }

        void setXStep(int step)
        {
            XSTEP = step;
            YSTEP = step / 2;
            u32 chunksX = m_landscape->getWidth() / CHUNK_SIZE;
            u32 chunksY = m_landscape->getHeight() / CHUNK_SIZE;
            for (u32 chunkY = 0; chunkY < chunksY; chunkY++)
            {
                for (u32 chunkX = 0; chunkX < chunksX; chunkX++)
                {
                    updateChunk(chunkX, chunkY);
                }
            }
        }

    };  // namespace OpenS4::Renderer

}  // namespace OpenS4::Renderer
