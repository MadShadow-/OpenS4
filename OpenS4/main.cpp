
#include "main.hpp"

#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/matrix.hpp>
#include <sstream>
#include <thread>

#include "Import/Map/Map.hpp"
#include "Renderer/Landscape.hpp"
#include "Renderer/LandscapeRenderer.hpp"
#include "Renderer/LandscapeTextures.hpp"
#include "Renderer/LoadShader.hpp"
#include "Renderer/Shaders.hpp"
#include "Renderer/TextureMapper.hpp"
#include "Renderer/TransformationPipeline.hpp"
#include "Renderer/TriangleBatch.hpp"

void registerMapperTextures(OpenS4::Import::GraphicsRegistry& registry,
                            OpenS4::Renderer::LandscapeTextures* textures,
                            OpenS4::Renderer::TextureMapper* mapper, u32 ghID) {
    mapper->setGameTextureMapping(48, 8, "BEACH");
    mapper->setGameTextureMapping(48, 7, "BEACH");

    mapper->setGameTextureMapping(16, 8, "GRASS");

    mapper->setGameTextureMapping(17, 8, "GRASS");
    mapper->setGameTextureMapping(18, 8, "GRASS");
    mapper->setGameTextureMapping(19, 8, "GRASS");
    mapper->setGameTextureMapping(20, 8, "GRASS");
    mapper->setGameTextureMapping(21, 8, "GRASS");
    mapper->setGameTextureMapping(22, 8, "GRASS");
    mapper->setGameTextureMapping(23, 8, "GRASS");

    mapper->setGameTextureMapping(16, 40, "GRASS");
    mapper->setGameTextureMapping(16, 7, "GRASS");
    mapper->setGameTextureMapping(16, 6, "GRASS");
    mapper->setGameTextureMapping(16, 5, "GRASS");
    mapper->setGameTextureMapping(16, 4, "GRASS");
    mapper->setGameTextureMapping(16, 3, "GRASS");
    mapper->setGameTextureMapping(16, 10, "GRASS");
    mapper->setGameTextureMapping(16, 9, "GRASS");
    mapper->setGameTextureMapping(16, 11, "GRASS");
    mapper->setGameTextureMapping(16, 12, "GRASS");
    mapper->setGameTextureMapping(16, 13, "GRASS");
    mapper->setGameTextureMapping(16, 14, "GRASS");
    mapper->setGameTextureMapping(16, 15, "GRASS");

    mapper->setGameTextureMapping(24, 8, "DARK_GRASS");
    mapper->setGameTextureMapping(25, 8, "DARK_GRASS");

    mapper->setGameTextureMapping(18, 8, "GRASS_ISLAND");

    mapper->setGameTextureMapping(28, 8, "EARTH");
    mapper->setGameTextureMapping(32, 8, "MOUNTAIN");
    mapper->setGameTextureMapping(29, 8, "PATH");
    mapper->setGameTextureMapping(64, 8, "DESERT");
    mapper->setGameTextureMapping(144, 8, "MUD");
    mapper->setGameTextureMapping(128, 8, "SNOW");

    mapper->setGameTextureMapping(80, 8, "SWAMP");

    mapper->setGameTextureMapping(20, 8, "DESERT");
    mapper->setGameTextureMapping(17, 8, "MOUNTAIN");
    mapper->setGameTextureMapping(65, 8, "DESERT");

    mapper->setGameTextureMapping(33, 8, "MOUNTAIN");

    // mapper->setGameTextureMapping(21, 8, "SWAMP");
    // mapper->setGameTextureMapping(23, 8, "SWAMP");

    mapper->setGameTextureMapping(81, 8, "SWAMP");

    mapper->setGameTextureMapping(7, 8,
                                  "WATER_7");  // TODO Water levels.
    mapper->setGameTextureMapping(6, 8, "WATER_6");
    mapper->setGameTextureMapping(5, 8, "WATER_5");
    mapper->setGameTextureMapping(4, 8, "WATER_4");
    mapper->setGameTextureMapping(3, 8, "WATER_3");
    mapper->setGameTextureMapping(2, 8, "WATER_2");
    mapper->setGameTextureMapping(1, 8, "WATER_1");
    mapper->setGameTextureMapping(0, 8, "WATER_0");

    std::vector<int> ids = {32, 0, 1, 2, 4, 7, 8, 10, 13, 15, 17, 20, 21};
    std::vector<std::string> textures2 = {
        "UNKNOWN", "GRASS",    "DARK_GRASS", "GRASS_ISLAND", "WATER_7",
        "BEACH",   "MOUNTAIN", "DESERT",     "MUD",          "SWAMP",
        "SNOW",    "EARTH",    "PATH"};

    for (int i = 0; i < ids.size(); i++) {
        OpenS4::Import::ImageData data = registry.getImage(ghID, ids[i]);
        int id = textures->add_texture_plane_256x256(&data);

        mapper->setLandscapeTextureID(textures2[i], id);
    }

    // Water
    {
        OpenS4::Import::ImageData data = registry.getImage(ghID, 5);
        mapper->setLandscapeTextureID(
            "WATER_0", textures->add_texture(64, 64, 0, 0, &data));
        mapper->setLandscapeTextureID(
            "WATER_1", textures->add_texture(64, 64, 1, 0, &data));
        mapper->setLandscapeTextureID(
            "WATER_2", textures->add_texture(64, 64, 2, 0, &data));
        mapper->setLandscapeTextureID(
            "WATER_3", textures->add_texture(64, 64, 3, 0, &data));
        mapper->setLandscapeTextureID(
            "WATER_4", textures->add_texture(64, 64, 0, 1, &data));
        mapper->setLandscapeTextureID(
            "WATER_5", textures->add_texture(64, 64, 1, 1, &data));
        mapper->setLandscapeTextureID(
            "WATER_6", textures->add_texture(64, 64, 2, 1, &data));
    }

    int id;
    OpenS4::Import::ImageData data = registry.getImage(ghID, 3);

    OpenS4::Renderer::HexInfo info;

    double width = textures->get_width();
    double height = textures->get_height();

    info.size.width = (double)64 / width;
    info.size.height = (double)64 / height;

    auto update_info = [&info, &data, &width, &height, &textures](int offsetX,
                                                                  int offsetY) {
        info.hexagon_texture_id =
            textures->add_texture_hexagon(64, 64, offsetX, offsetY, &data);
        info.offset.x =
            textures->getTextureInformation(info.hexagon_texture_id)->x / width;
        info.offset.y =
            textures->getTextureInformation(info.hexagon_texture_id)->y /
            height;
    };

    update_info(0, 0);
    mapper->addTextureHexagon("GRASS", "GRASS_ISLAND", info);

    update_info(1, 0);
    mapper->addTextureHexagon("GRASS_ISLAND", "GRASS", info);

    update_info(2, 0);
    mapper->addTextureHexagon("GRASS", "BEACH", info);

    update_info(3, 0);
    mapper->addTextureHexagon("BEACH", "GRASS", info);

    update_info(2, 1);
    mapper->addTextureHexagon("GRASS", "BEACH", info);

    update_info(3, 1);
    mapper->addTextureHexagon("BEACH", "GRASS", info);

    update_info(2, 2);
    mapper->addTextureHexagon("GRASS", "DARK_GRASS", info);

    update_info(3, 2);
    mapper->addTextureHexagon("DARK_GRASS", "GRASS", info);

    data = registry.getImage(ghID, 5);
    update_info(0, 2);
    mapper->addTextureHexagon("WATER_0", "BEACH", info);
    update_info(1, 2);
    mapper->addTextureHexagon("BEACH", "WATER_0", info);

    data = registry.getImage(ghID, 19);
    update_info(0, 0);
    mapper->addTextureHexagon("GRASS", "EARTH", info);
    update_info(1, 0);
    mapper->addTextureHexagon("EARTH", "GRASS", info);

    update_info(2, 0);
    mapper->addTextureHexagon("GRASS", "PATH", info);
    update_info(3, 0);
    mapper->addTextureHexagon("PATH", "GRASS", info);

    data = registry.getImage(ghID, 6);
    update_info(0, 0);
    mapper->addTextureHexagon("WATER_1", "WATER_0", info);
    update_info(1, 0);
    mapper->addTextureHexagon("WATER_2", "WATER_1", info);
    update_info(2, 0);
    mapper->addTextureHexagon("WATER_3", "WATER_2", info);
    update_info(3, 0);
    mapper->addTextureHexagon("WATER_4", "WATER_3", info);

    update_info(0, 1);
    mapper->addTextureHexagon("WATER_0", "WATER_1", info);
    update_info(1, 1);
    mapper->addTextureHexagon("WATER_1", "WATER_2", info);
    update_info(2, 1);
    mapper->addTextureHexagon("WATER_2", "WATER_3", info);
    update_info(3, 1);
    mapper->addTextureHexagon("WATER_3", "WATER_4", info);

    update_info(0, 2);
    mapper->addTextureHexagon("WATER_5", "WATER_4", info);
    update_info(1, 2);
    mapper->addTextureHexagon("WATER_6", "WATER_5", info);
    update_info(2, 2);
    mapper->addTextureHexagon("WATER_7", "WATER_6", info);

    update_info(0, 3);
    mapper->addTextureHexagon("WATER_4", "WATER_5", info);
    update_info(1, 3);
    mapper->addTextureHexagon("WATER_5", "WATER_6", info);
    update_info(2, 3);
    mapper->addTextureHexagon("WATER_6", "WATER_7", info);

    data = registry.getImage(ghID, 11);
    update_info(2, 2);
    mapper->addTextureHexagon("DESERT", "GRASS", info);
    update_info(1, 1);
    mapper->addTextureHexagon("GRASS", "DESERT", info);

    data = registry.getImage(ghID, 9);
    update_info(2, 0);
    mapper->addTextureHexagon("MOUNTAIN", "GRASS", info);
    update_info(1, 2);
    mapper->addTextureHexagon("GRASS", "MOUNTAIN", info);

    data = registry.getImage(ghID, 14);
    update_info(2, 0);
    mapper->addTextureHexagon("SWAMP", "GRASS", info);
    update_info(3, 0);
    mapper->addTextureHexagon("GRASS", "SWAMP", info);

    /*



    data = registry.get_image(2, 5);
    info.hexagon_texture_id = textures->add_texture_hexagon(64, 64,
    0, 2, &data); mapper->addTextureHexagon(Textures::BEACH,
    Textures::BEACH, Textures::WATER_0, info); info.hexagon_texture_id =
    textures->add_texture_hexagon(64, 64, 1, 2, &data);
    mapper->addTextureHexagon(Textures::WATER_0, Textures::WATER_0,
    Textures::BEACH, info);


    data = registry.get_image(2, 19);
    info.hexagon_texture_id = textures->add_texture_hexagon(64, 64,
    0, 0, &data); mapper->addTextureHexagon(Textures::EARTH,
    Textures::EARTH, Textures::GRASS, info); info.hexagon_texture_id =
    textures->add_texture_hexagon(64, 64, 1, 0, &data);
    mapper->addTextureHexagon(Textures::GRASS, Textures::GRASS,
    Textures::EARTH, info);

    info.hexagon_texture_id = textures->add_texture_hexagon(64, 64,
    2, 0, &data); mapper->addTextureHexagon(Textures::GRASS,
    Textures::PATH, Textures::PATH, info); info.hexagon_texture_id =
    textures->add_texture_hexagon(64, 64, 3, 0, &data);
    mapper->addTextureHexagon(Textures::GRASS, Textures::GRASS,
    Textures::PATH, info);

    data = registry.get_image(2, 6);
    info.hexagon_texture_id = textures->add_texture_hexagon(64, 64,
    0, 0, &data); mapper->addTextureHexagon(Textures::WATER_0,
    Textures::WATER_0, Textures::WATER_1, info); info.hexagon_texture_id =
    textures->add_texture_hexagon(64, 64, 1, 0, &data);
    mapper->addTextureHexagon(Textures::WATER_1, Textures::WATER_1,
    Textures::WATER_2, info); info.hexagon_texture_id =
    textures->add_texture_hexagon(64, 64, 2, 0, &data);
    mapper->addTextureHexagon(Textures::WATER_2, Textures::WATER_2,
    Textures::WATER_3, info); info.hexagon_texture_id =
    textures->add_texture_hexagon(64, 64, 3, 0, &data);
    mapper->addTextureHexagon(Textures::WATER_3, Textures::WATER_3,
    Textures::WATER_4, info);

    info.hexagon_texture_id = textures->add_texture_hexagon(64, 64,
    0, 1, &data); mapper->addTextureHexagon(Textures::WATER_0,
    Textures::WATER_1, Textures::WATER_1, info); info.hexagon_texture_id =
    textures->add_texture_hexagon(64, 64, 1, 1, &data);
    mapper->addTextureHexagon(Textures::WATER_1, Textures::WATER_2,
    Textures::WATER_2, info); info.hexagon_texture_id =
    textures->add_texture_hexagon(64, 64, 2, 1, &data);
    mapper->addTextureHexagon(Textures::WATER_2, Textures::WATER_3,
    Textures::WATER_3, info); info.hexagon_texture_id =
    textures->add_texture_hexagon(64, 64, 3, 1, &data);
    mapper->addTextureHexagon(Textures::WATER_3, Textures::WATER_4,
    Textures::WATER_4, info);

    info.hexagon_texture_id = textures->add_texture_hexagon(64, 64,
    0, 2, &data); mapper->addTextureHexagon(Textures::WATER_4,
    Textures::WATER_4, Textures::WATER_5, info); info.hexagon_texture_id =
    textures->add_texture_hexagon(64, 64, 1, 2, &data);
    mapper->addTextureHexagon(Textures::WATER_5, Textures::WATER_5,
    Textures::WATER_6, info); info.hexagon_texture_id =
    textures->add_texture_hexagon(64, 64, 2, 2, &data);
    mapper->addTextureHexagon(Textures::WATER_6, Textures::WATER_6,
    Textures::WATER_7, info);

    info.hexagon_texture_id = textures->add_texture_hexagon(64, 64,
    0, 3, &data); mapper->addTextureHexagon(Textures::WATER_4,
    Textures::WATER_5, Textures::WATER_5, info); info.hexagon_texture_id =
    textures->add_texture_hexagon(64, 64, 1, 3, &data);
    mapper->addTextureHexagon(Textures::WATER_5, Textures::WATER_6,
    Textures::WATER_6, info); info.hexagon_texture_id =
    textures->add_texture_hexagon(64, 64, 2, 3, &data);
    mapper->addTextureHexagon(Textures::WATER_6, Textures::WATER_7,
    Textures::WATER_7, info);

    data = registry.get_image(2, 11);
    info.hexagon_texture_id = textures->add_texture_hexagon(64, 64,
    2, 2, &data); mapper->addTextureHexagon(Textures::GRASS,
    Textures::GRASS, Textures::DESERT, info); info.hexagon_texture_id =
    textures->add_texture_hexagon(64, 64, 1, 1, &data);
    mapper->addTextureHexagon(Textures::DESERT, Textures::DESERT,
    Textures::GRASS, info);

    data = registry.get_image(2, 9);
    info.hexagon_texture_id = textures->add_texture_hexagon(64, 64,
    2, 0, &data); mapper->addTextureHexagon(Textures::GRASS,
    Textures::GRASS, Textures::MOUNTAIN, info); info.hexagon_texture_id =
    textures->add_texture_hexagon(64, 64, 1, 2, &data);
    mapper->addTextureHexagon(Textures::MOUNTAIN, Textures::MOUNTAIN,
    Textures::GRASS, info);

    data = registry.get_image(2, 14);
    info.hexagon_texture_id = textures->add_texture_hexagon(64, 64,
    2, 0, &data); mapper->addTextureHexagon(Textures::GRASS,
    Textures::GRASS, Textures::SWAMP, info); info.hexagon_texture_id =
    textures->add_texture_hexagon(64, 64, 3, 0, &data);
    mapper->addTextureHexagon(Textures::SWAMP, Textures::SWAMP,
    Textures::GRASS, info);
    */
}

OpenS4::Renderer::Landscape* makeLandscape(
    OpenS4::Import::Map::Map* map,
    OpenS4::Renderer::LandscapeTextures* textures,
    OpenS4::Renderer::TextureMapper* mapper) {
    OpenS4::Import::Map::Landscape* mapLandscape =
        OpenS4::Import::Map::fromMap(map);

    auto height = mapLandscape->getHeight();
    auto width = mapLandscape->getWidth();

    OpenS4::Renderer::Landscape* landscape =
        new OpenS4::Renderer::Landscape(textures, mapper, width, height);

    for (auto y = 0; y < height; y++) {
        for (auto x = 0; x < width; x++) {
            OpenS4::Import::Map::LandscapePosition pos =
                mapLandscape->getLandscapePosition(x, y);

            landscape->setTerrain(x, y,
                                  mapper->getGameTextureMapping(
                                      pos.terrainType, pos.terrainSubtype));
            landscape->setTerrainHeight(x, y, pos.height);
        }
    }

    //   landscape->update();

    return landscape;
}

OpenS4::Renderer::Landscape* makeRandomLandscape(
    OpenS4::Renderer::LandscapeTextures* textures,
    OpenS4::Renderer::TextureMapper* mapper) {
    auto height = 1024;
    auto width = 1024;
    OpenS4::Renderer::Landscape* landscape =
        new OpenS4::Renderer::Landscape(textures, mapper, width, height);

    std::vector<OpenS4::Renderer::LandscapeTextureID> rndTextures;
    rndTextures.push_back(mapper->getLandscapeTextureID("GRASS"));
    rndTextures.push_back(mapper->getLandscapeTextureID("BEACH"));

    for (auto y = 0; y < height; y++) {
        for (auto x = 0; x < width; x++) {
            // landscape->setTerrain(x, y,
            //                       mapper->getLandscapeTextureID("GRASS"));
            landscape->setTerrain(x, y,
                                  rndTextures[rand() % rndTextures.size()]);
            landscape->setTerrainHeight(x, y, rand() % 15 + 1);
        }
    }

    return landscape;
}

void error_callback(int error, const char* description) {
    OpenS4::getLogger().err("GLFW: %d %s", error, description);
};

// Needs refactoring when better input handling is available.
float g_zoom = 1.0f;
float g_mapPosX = 1.0f;
float g_mapPosY = 1.0f;

// Testing only.
int g_playerColor = 0;
int g_frameID = 0;
int g_tapID = 0;

const int XSTEP = 32;
const int YSTEP = XSTEP / 2;
glm::vec2 toPixelPosition(glm::vec2 model) {
    glm::vec2 pos;
    pos.x = model.x * XSTEP - (model.y + 1) * XSTEP / 2;
    pos.y = -(model.y + 1) * YSTEP;
    return pos;
}
glm::vec2 toModelPosition(glm::vec2 pixel) {
    glm::vec2 model;

    model.y = -(pixel.y / YSTEP) - 1;
    model.x = (pixel.x + (model.y + 1) * XSTEP / 2) / XSTEP;
    return model;
}
glm::vec2 screenToWorld(glm::vec2 screen,
                        OpenS4::Renderer::TransformationPipeline* transform,
                        int width, int height) {
    glm::vec2 clip(2 * screen.x / width - 1.0, 1 - (2 * screen.y) / height);

    glm::vec3 clip3(clip.x, clip.y, -1);

    glm::vec4 ray(clip3, 1);

    glm::vec4 rayEye = glm::inverse(transform->getProjectionMatrix()) * ray;

    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0, 0);

    glm::vec3 rayWorld =
        glm::vec3(glm::inverse(transform->getModelViewMatrix()) * rayEye);

    return glm::vec2(rayWorld.x, rayWorld.y);
}

void TERMINATE(u64 errorCode) {
    OpenS4::getLogger().flush();
    exit(errorCode);
}

void drawLoop(GLFWwindow* window, OpenS4::Renderer::Landscape* landscape,
              OpenS4::Renderer::TextureAtlas* landscape_atlas,
              OpenS4::Import::GraphicsRegistry* registry) {
    glClearColor(0, 0, 0, 1);

    GLuint mapShader = OpenS4::Renderer::LoadShader(
        OpenS4::Renderer::Shader::VERTEXSHADER_MAP,
        OpenS4::Renderer::Shader::FRAGMENTSHADER_MAP);

    GLuint settlerShader = OpenS4::Renderer::LoadShader(
        OpenS4::Renderer::Shader::VERTEXSHADER_SETTLER,
        OpenS4::Renderer::Shader::FRAGMENTSHADER_SETTLER);

    auto start = std::chrono::high_resolution_clock::now();

    OpenS4::Renderer::TextureAtlas settler_atlas(8192, 8192);

    std::vector<
        std::vector<OpenS4::Renderer::TextureAtlas::TextureAtlasPosition>>
        taps;

    OpenS4::Import::ImageData image;
    int idx = 7;
    for (int i = 0; i < 200; i++) {
        taps.resize(taps.size() + 1);
        for (int j = 0; j < 12; j++) {
            image = registry->getImage(20, idx++);
            taps[i].push_back(
                settler_atlas.add_texture_to_atlas_swapped(&image));
        }
        idx++;
    }

    OpenS4::Renderer::TriangleBatch settler_batch;

    struct PlayerColor {
        uint8_t a;
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };
    std::vector<PlayerColor> colors = {

        {0xFF, 0xF0, 0x15, 0x15}, {0xFF, 0x45, 0x3E, 0xD2},
        {0xFF, 0x00, 0xC8, 0x36}, {0xFF, 0xFA, 0xEE, 0x00},
        {0xFF, 0xBE, 0x4B, 0xFF}, {0xFF, 0xFD, 0xA0, 0x24},
        {0xFF, 0x21, 0xC5, 0xB5}, {0xFF, 0xE6, 0xFF, 0xFF}};

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    OpenS4::Renderer::LandscapeRenderer renderer(landscape);

    int width = 0;
    int height = 0;

    u64 frameCount = 0;
    double lastFrameCount = 0;

    while (!glfwWindowShouldClose(window)) {
        frameCount++;
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = (now - start).count();
        if (duration > 1000000000) {
            start = now;
            lastFrameCount =
                ((double)frameCount / duration * 1000 * 1000 * 1000);
            frameCount = 0;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        OpenS4::Renderer::TransformationPipeline transform;

        if (width && height) {
            glm::mat4x4 ortho = glm::ortho(
                (float)-width / 2, (float)width / 2, (float)-height / 2,
                (float)height / 2, (float)-200, (float)200);
            transform.getProjectionStack()->loadMatrix(ortho);
            transform.getProjectionStack()->scale(g_zoom, g_zoom, 1);
            transform.getProjectionStack()->translate(-g_mapPosX * 10,
                                                      10 * g_mapPosY, 0);

            // getting cursor position
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);

            std::stringstream sstream;

            // Set Title
            glm::vec2 mousePositionInModelCoordinates =
                toModelPosition(screenToWorld(glm::vec2(xpos, ypos), &transform,
                                              width, height));
            sstream << std::fixed << std::setprecision(2)
                    << " MouseMapPos: " << mousePositionInModelCoordinates.x
                    << " " << mousePositionInModelCoordinates.y;
            sstream << " FPS: " << lastFrameCount;
            glfwSetWindowTitle(window, sstream.str().c_str());

            // Draw Landscape
            glUseProgram(mapShader);
            glUniformMatrix4fv(
                glGetUniformLocation(mapShader, "mvpMatrix"), 1, GL_FALSE,
                glm::value_ptr(transform.getModelViewProjectionMatrix()));
            glBindTexture(GL_TEXTURE_2D, landscape_atlas->getGLTextureID());
            renderer.draw(window, &transform);

            // Draw Settler
            glUseProgram(settlerShader);
            glUniformMatrix4fv(
                glGetUniformLocation(settlerShader, "mvpMatrix"), 1, GL_FALSE,
                glm::value_ptr(transform.getModelViewProjectionMatrix()));

            glBindTexture(GL_TEXTURE_2D, settler_atlas.getGLTextureID());

            // Update test settler based on user input....
            {
                auto& tap = taps[g_tapID % taps.size()];
                auto position = tap[g_frameID % tap.size()];

                glm::vec2 topLeft =
                    glm::vec2(1.0f * position.x / settler_atlas.get_width(),
                              1.0f * position.y / settler_atlas.get_height());
                glm::vec2 bottomRight =
                    glm::vec2(1.0f * (position.x + position.width) /
                                  settler_atlas.get_width(),
                              1.0f * (position.y + position.height) /
                                  settler_atlas.get_height());

                float settler_tex[] = {
                    topLeft.x,     topLeft.y,      // 0
                    topLeft.x,     bottomRight.y,  // 0
                    bottomRight.x, bottomRight.y,  // 0
                    bottomRight.x, bottomRight.y,  // 0
                    bottomRight.x, topLeft.y,      // 0
                    topLeft.x,     topLeft.y,      // 0
                };

                float settler_pos[] = {
                    0,
                    0,  // 0 0
                    0,
                    (float)position.height,  // 0 1
                    (float)position.width,
                    (float)position.height,  // 1 1
                    (float)position.width,
                    (float)position.height,  // 1 1
                    (float)position.width,
                    0,  // 1 0
                    0,
                    0  // 0 0
                };

                PlayerColor& color = colors[(g_playerColor) % 8];
                std::vector<float> settler_color;
                for (int j = 0; j < 6; j++) {
                    settler_color.push_back(color.r / 255.0);
                    settler_color.push_back(color.g / 255.0);
                    settler_color.push_back(color.b / 255.0);
                    settler_color.push_back(color.a / 255.0);
                }

                settler_batch.updateData(settler_pos, 12, 2,
                                         settler_tex, 12, 2,
                                         settler_color.data(), 24, 4);

                settler_batch.draw();
            }

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }
}

int main(int argc, char* argv[]) {
    std::string graphicsPath = OpenS4::Import::getGraphicsPathByRegistry();

    if (graphicsPath == "") graphicsPath = "./Gfx";

    OpenS4::getLogger().printToConsole(true);

    OpenS4::Import::GraphicsRegistry registry =
        OpenS4::Import::getGraphicsRegistry(graphicsPath);

    std::string current_path = std::filesystem::current_path().generic_string();

    OpenS4::getLogger().info("Graphics Path is: %s", graphicsPath.c_str());
    OpenS4::getLogger().info("Current Directory is: %s", current_path.c_str());

    glfwSetErrorCallback(error_callback);

    if (glfwInit() == GLFW_FALSE) {
        OpenS4::getLogger().err("glfwInit() failed!");
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenS4", NULL, NULL);

    if (window == nullptr) {
        OpenS4::getLogger().err("glfwCreateWindow failed!");
        return 1;
    }

    glfwMakeContextCurrent(window);

    auto key_cb = [](GLFWwindow* window, int key, int scancode, int action,
                     int mods) {
        static int drawMode = 0;

        if (action == GLFW_PRESS || action == GLFW_REPEAT) {
            switch (key) {
                case GLFW_KEY_ESCAPE:
                    glfwSetWindowShouldClose(window, GLFW_TRUE);
                    break;

                case 'q':
                case 'Q':
                    g_frameID++;
                    break;
                case 'w':
                case 'W':
                    g_playerColor++;
                    break;
                case 'a':
                case 'A':
                    g_tapID++;
                    break;

                case 'd':
                case 'D':
                    drawMode = ++drawMode % 3;
                    if (drawMode == 0)  // fill mode
                    {
                        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                    } else if (drawMode == 1)  // wireframe mode
                    {
                        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    } else  // point mode
                    {
                        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
                    }
                    break;
                case 'j':
                case 'J':
                    g_zoom *= 1.5f;
                    break;
                case 'k':
                case 'K':
                    g_zoom /= 1.5f;
                    break;

                case GLFW_KEY_RIGHT:
                    g_mapPosX += 1 / g_zoom;
                    break;
                case GLFW_KEY_LEFT:
                    g_mapPosX -= 1 / g_zoom;
                    break;

                case GLFW_KEY_DOWN:
                    g_mapPosY += 1 / g_zoom;
                    break;
                case GLFW_KEY_UP:
                    g_mapPosY -= 1 / g_zoom;
                    break;
            }
        }
    };

    glfwSetKeyCallback(window, key_cb);

    auto init = glewInit();
    if (init != GLEW_OK) {
        OpenS4::getLogger().err("glewInit failed!");
        return 1;
    }

    OpenS4::Renderer::LandscapeTextures landscape_textures(2048, 2048);
    OpenS4::Renderer::TextureMapper textureMapper;

    registerMapperTextures(registry, &landscape_textures, &textureMapper, 2);

    OpenS4::Import::Map::Map* map =
        OpenS4::Import::Map::readMap("576_AlleTexturen.map");

    if (!map) {
        OpenS4::getLogger().warn("Map not found!");
    }
    OpenS4::Renderer::Landscape* landscape;
    if (map)
        landscape = makeLandscape(map, &landscape_textures, &textureMapper);
    else
        landscape = makeRandomLandscape(&landscape_textures, &textureMapper);

    if (map != nullptr) {
        delete map;
        map = nullptr;
    }

    //- setup OpenGL

    glBindTexture(GL_TEXTURE_2D, landscape_textures.getGLTextureID());

    drawLoop(window, landscape, &landscape_textures, &registry);

    glfwTerminate();

    OpenS4::getLogger().info("Program is exiting correctly!");

    return 0;
}
