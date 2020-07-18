
#include "main.hpp"

#include <chrono>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/matrix.hpp>
#include <sstream>
#include <thread>

#include "Import/Map/Map.hpp"
#include "Input/InputHandler.hpp"
#include "Input/InputListener.hpp"
#include "Logic/Logic.hpp"
#include "Renderer/Batch/PointBatch.hpp"
#include "Renderer/Batch/TriangleBatch.hpp"
#include "Renderer/Landscape.hpp"
#include "Renderer/LandscapeRenderer.hpp"
#include "Renderer/LandscapeTextures.hpp"
#include "Renderer/LoadShader.hpp"
#include "Renderer/ObjectRenderer/ObjectRenderer.hpp"
#include "Renderer/Shaders.hpp"
#include "Renderer/TextureMapper.hpp"
#include "Renderer/TransformationPipeline.hpp"

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

OpenS4::Logic::Direction g_direction = OpenS4::Logic::Direction::EAST;

class TestListener : public OpenS4::Input::InputListener {
    GLFWwindow* m_window = nullptr;

   public:
    TestListener(std::string name, GLFWwindow* window)
        : OpenS4::Input::InputListener(name), m_window(window) {}

    OpenS4::Input::EventDispatching keyPressed(
        const OpenS4::Input::KeyEvent* event) {
        OpenS4::Input::EventDispatching dispatched = keyBindAction(event);
        if (dispatched != OpenS4::Input::EventDispatching::NoKeyBindFound) {
            return dispatched;
        }

        using namespace OpenS4::Input;

        static int drawMode = 0;

        switch (event->getKey()) {
            case Key::Q:
                g_direction = (OpenS4::Logic::Direction)((int)g_direction + 1);
                if (g_direction == OpenS4::Logic::Direction::_DIRECTION_COUNT)
                    g_direction = OpenS4::Logic::Direction::EAST;
                break;
            case Key::W:
                g_playerColor++;
                break;
            case Key::D:
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
            case Key::J:
                g_zoom *= 1.5f;
                break;
            case Key::K:
                g_zoom /= 1.5f;
                break;

            case Key::Right:
                g_mapPosX += 1 / g_zoom;
                break;
            case Key::Left:
                g_mapPosX -= 1 / g_zoom;
                break;
            case Key::Down:
                g_mapPosY += 1 / g_zoom;
                break;
            case Key::Up:
                g_mapPosY -= 1 / g_zoom;
                break;
            case Key::Esc:
                glfwSetWindowShouldClose(m_window, GLFW_TRUE);
                break;
            default:
                return OpenS4::Input::EventDispatching::PassEvent;
        }

        return OpenS4::Input::EventDispatching::CatchEvent;
    }
};

void TERMINATE(u64 errorCode) {
    OpenS4::getLogger().flush();
    exit(errorCode);
}

void drawLoop(GLFWwindow* window, OpenS4::Renderer::Landscape* landscape,
              OpenS4::Renderer::TextureAtlas* landscape_atlas,
              OpenS4::Import::GraphicsRegistry* registry,
              OpenS4::Renderer::TextureMapper* mapper) {
    glClearColor(0, 0, 0, 1);

    // Setup Shaders
    GLuint mapShader = OpenS4::Renderer::LoadShader(
        OpenS4::Renderer::Shader::VERTEXSHADER_MAP,
        OpenS4::Renderer::Shader::FRAGMENTSHADER_MAP);

    GLuint settlerShader = OpenS4::Renderer::LoadShader(
        OpenS4::Renderer::Shader::VERTEXSHADER_SETTLER,
        OpenS4::Renderer::Shader::FRAGMENTSHADER_SETTLER);

    GLuint pointShader = OpenS4::Renderer::LoadShader(
        OpenS4::Renderer::Shader::VERTEXSHADER_POINT,
        OpenS4::Renderer::Shader::FRAGMENTSHADER_POINT);

    // Init clock
    auto start = std::chrono::high_resolution_clock::now();

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

    std::vector<float> floatColors;
    for (PlayerColor& color : colors) {
        floatColors.push_back(color.r / 255.0f);
        floatColors.push_back(color.g / 255.0f);
        floatColors.push_back(color.b / 255.0f);
        floatColors.push_back(color.a / 255.0f);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    OpenS4::Renderer::LandscapeRenderer renderer(landscape);

    OpenS4::Renderer::LandscapeRenderer* p_renderer = &renderer;

    OpenS4::Renderer::ObjectRenderer::ObjectRenderer objRenderer(p_renderer);

    int maxImageID = 0;

    std::map<u32, u32> registryFrameIDtoImageID;

    // Build atlas(es) 20 = roman settlers
    for (int i = 20; i < 21; i++) {
        for (int j = 0; j < registry->getNumberOfImages(i); j++) {
            OpenS4::Import::ImageData image = registry->getImage(i, j);
            auto imageID = objRenderer.addImage(image);
            maxImageID++;

            registryFrameIDtoImageID[j] = imageID;
            if (maxImageID % 1000 == 0) objRenderer.buildAtlasesContinuous(1024);
        }
    }
    objRenderer.buildAtlasesContinuous(1024, true);

    
    OpenS4::getLogger().info("Built %d atlases for objects!",
                             objRenderer.getNumberOfAtlases());
    OpenS4::getLogger().info("Atlases contain %d images!", maxImageID);


    // Create a test entity with test properties.
    OpenS4::Logic::EntityProperties* entityProperties =
        OpenS4::Logic::test_makeProperties(registry);
    OpenS4::Logic::Entity entity;
    entity.m_properties = entityProperties;


    auto imageID = 0;
    int width = 0;
    int height = 0;

    u64 frameCount = 0;
    double lastFrameCount = 0;

    // Logic game tick.
    u32 tick = 0;

    // Current Tick (tick2 "interpolated")
    double tick2 = tick;

    while (!glfwWindowShouldClose(window)) {
        frameCount++;
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = (now - start).count();
        if (duration > 1000000000 || tick == 0) {
            tick++;
            tick2 = tick;

            start = now;
            lastFrameCount =
                ((double)frameCount / duration * 1000 * 1000 * 1000);
            frameCount = 0;

            std::stringstream sstream;
            sstream << "Tick: " << tick;
            sstream << " Tick2: " << tick2;
            sstream << " FPS: " << lastFrameCount;
            glfwSetWindowTitle(window, sstream.str().c_str());

            // Update test entity
            if (entity.end_tick <= tick) {
                entity.start_tick = tick;

                entity.current_animation++;
                if (entity.current_animation >=
                    entity.m_properties->animations.size())
                    entity.current_animation = 0;

                entity.end_tick =
                    tick +
                    entity.m_properties->animations[entity.current_animation]
                        .duration;
            }
        } else {
            tick2 = tick + duration / 1000000000.0;

            std::stringstream sstream;
            sstream << "Tick: " << tick;
            sstream << " Tick2: " << tick2;
            sstream << " FPS: " << lastFrameCount;
            glfwSetWindowTitle(window, sstream.str().c_str());
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        OpenS4::Renderer::TransformationPipeline transform;

        // Only render if window has size > 0
        if (width && height) {

            // Setup camera & translation for this frame.
            glm::mat4x4 ortho = glm::ortho(
                (float)-width / 2, (float)width / 2, (float)-height / 2,
                (float)height / 2, (float)-200, (float)200);
            transform.getProjectionStack()->loadMatrix(ortho);
            transform.getProjectionStack()->scale(g_zoom, g_zoom, 1);
            transform.getProjectionStack()->translate(-g_mapPosX * 10,
                                                      10 * g_mapPosY, 0);

            // Pass the tranformation to the landscape renderer (so that it can translate visible pixels to coordinates).
            renderer.setTransform(&transform, width, height);

            // Draw Landscape
            glUseProgram(mapShader);
            glUniformMatrix4fv(
                glGetUniformLocation(mapShader, "mvpMatrix"), 1, GL_FALSE,
                glm::value_ptr(transform.getModelViewProjectionMatrix()));
            glBindTexture(GL_TEXTURE_2D, landscape_atlas->getGLTextureID());
            renderer.draw(window, &transform);

            // Draw objects
            objRenderer.clear();
            glUseProgram(settlerShader);
            glUniformMatrix4fv(
                glGetUniformLocation(settlerShader, "mvpMatrix"), 1, GL_FALSE,
                glm::value_ptr(transform.getModelViewProjectionMatrix()));
            glUniform4fv(glGetUniformLocation(settlerShader, "playerColors"), 8,
                         floatColors.data());


            double percentage = (tick2 - entity.start_tick) /
                                (entity.end_tick - entity.start_tick);

            int n_frames =
                (entity.m_properties->animations[entity.current_animation]
                     .frameEnd -
                 entity.m_properties->animations[entity.current_animation]
                     .frameStart +
                 1) /
                (u32)OpenS4::Logic::Direction::_DIRECTION_COUNT;

            entity.direction = g_direction;

            int frameOffset = n_frames * (u32)entity.direction;

            u32 frame =
                entity.m_properties->animations[entity.current_animation]
                    .frameStart +
                frameOffset + std::round(percentage * (n_frames - 1));

            objRenderer.render(10, 10, registryFrameIDtoImageID[frame],
                               g_playerColor % 8);
            objRenderer.draw();

            // Draw map points
            glUseProgram(pointShader);
            glUniformMatrix4fv(
                glGetUniformLocation(pointShader, "mvpMatrix"), 1, GL_FALSE,
                glm::value_ptr(transform.getModelViewProjectionMatrix()));

            OpenS4::Renderer::PointBatch pointBatch;

            std::vector<float> points;
            std::vector<float> pointColors;

            auto box = renderer.getBoundingBox();

            box.lower.x = std::floor(box.lower.x);
            box.lower.y = std::floor(box.lower.y);
            box.upper.x = std::floor(box.upper.x);
            box.upper.y = std::floor(box.upper.y);
            box.lower.x = std::max(0.0f, box.lower.x);
            box.lower.y = std::max(0.0f, box.lower.y);
            box.upper.x = std::min((float)renderer.getWidth(), box.upper.x);
            box.upper.y = std::min((float)renderer.getHeight(), box.upper.y);

            for (auto y = box.lower.y; y < box.upper.y; y++) {
                for (auto x = box.lower.x; x < box.upper.x; x++) {
                    glm::vec2 pixel = renderer.toPixelPosition(glm::vec2(x, y));
                    points.push_back(pixel.x);
                    points.push_back(pixel.y);

                    auto terrain = landscape->getTerrain(x, y);

                    if (y == 10 && x == 10) {
                        pointColors.push_back(0);
                        pointColors.push_back(1);
                        pointColors.push_back(0);
                        pointColors.push_back(1);
                    } else if (mapper->getTexture("GRASS") == terrain ||
                               mapper->getTexture("BEACH") == terrain) {
                        pointColors.push_back(1);
                        pointColors.push_back(0);
                        pointColors.push_back(0);
                        pointColors.push_back(1);
                    } else {
                        pointColors.push_back(1);
                        pointColors.push_back(1);
                        pointColors.push_back(0);
                        pointColors.push_back(1);
                    }
                }
            }

            glPointSize(5);
            pointBatch.updateData(points, 2, pointColors, 4);
            pointBatch.draw();
        }


        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

#include "Input/WindowSystem/GLFWInputHandler/GLFWInputHandler.hpp"

int main(int argc, char* argv[]) {
    // Setup logger to also print to console
    OpenS4::getLogger().printToConsole(true);

    // Initialize Graphics Registry
    std::string graphicsPath = OpenS4::Import::getGraphicsPathByRegistry();
    if (graphicsPath == "") graphicsPath = "./Gfx";
    OpenS4::Import::GraphicsRegistry registry =
        OpenS4::Import::getGraphicsRegistry(graphicsPath);

    // Log some generic information
    std::string current_path = std::filesystem::current_path().generic_string();
    OpenS4::getLogger().info("Graphics Path is: %s", graphicsPath.c_str());
    OpenS4::getLogger().info("Current Directory is: %s", current_path.c_str());

    // Setup and initialize glfw
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

    // Setup Input Subsystem
    OpenS4::Input::GLFWInputHandler::getInstance()->registerCallbacks(window);


    // Add simple test listener
    std::shared_ptr<TestListener> listener =
        std::make_shared<TestListener>("test", window);
    OpenS4::Input::InputHandler* handler =
        OpenS4::Input::InputHandler::getInstance();
    handler->registerListenerAtTop(listener);

    // Init glew
    auto init = glewInit();
    if (init != GLEW_OK) {
        OpenS4::getLogger().err("glewInit failed!");
        return 1;
    }

    // Init Landscape Texture Atlas & Texture Mapper
    OpenS4::Renderer::LandscapeTextures landscape_textures(2048, 2048);
    OpenS4::Renderer::TextureMapper textureMapper;
    registerMapperTextures(registry, &landscape_textures, &textureMapper, 2);

    // Load simple map
    OpenS4::Import::Map::Map* map =
        OpenS4::Import::Map::readMap("576_AlleTexturen.map");
    if (!map) {
        OpenS4::getLogger().warn("Map not found!");
    }

    // Make Landscape from Map
    OpenS4::Renderer::Landscape* landscape;
    if (map)
        landscape = makeLandscape(map, &landscape_textures, &textureMapper);
    else
        landscape = makeRandomLandscape(&landscape_textures, &textureMapper);

    if (map != nullptr) {
        delete map;
        map = nullptr;
    }

    // Loop forever
    drawLoop(window, landscape, &landscape_textures, &registry, &textureMapper);

    glfwTerminate();
    OpenS4::getLogger().info("Program is exiting correctly!");

    return 0;
}
