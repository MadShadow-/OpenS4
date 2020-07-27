
#include "main.hpp"

#include <bitset>
#include <chrono>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/matrix.hpp>
#include <nlohmann/json.hpp>
#include <sstream>
#include <thread>

#include "Import/Map/Map.hpp"
#include "Input/InputHandler.hpp"
#include "Input/InputListener.hpp"
#include "Input/WindowSystem/GLFWInputHandler/GLFWInputHandler.hpp"
#include "Logic/Logic.hpp"
#include "Logic/Map/Landscape.hpp"
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

void registerMapperTextures(
    OpenS4::Import::GraphicsRegistry& registry,
    OpenS4::Logic::Map::LandscapeTextureMapper* ltm,
    OpenS4::Logic::Map::LandscapeTextureConverter* converter,
    OpenS4::Renderer::LandscapeTextures* textures,
    OpenS4::Renderer::TextureMapper* mapper, u32 ghID) {
    std::vector<std::string> textureNames = {"WATER_0",
                                             "WATER_1",
                                             "WATER_2",
                                             "WATER_3",
                                             "WATER_4",
                                             "WATER_5",
                                             "WATER_6",
                                             "GRASS_TO_DARK_GRASS",
                                             "GRASS_TO_DESERT_GRASS",
                                             "DESERT_GRASS_TO_DESERT",
                                             "GRASS_TO_GRASS_MOUNTAIN",
                                             "GRASS_MOUNTAIN_TO_MOUNTAIN",
                                             "GRASS_TO_SWAMP_GRASS",
                                             "SWAMP_GRASS_TO_SWAMP",
                                             "MUD_GRASS_TO_MUD",
                                             "GRASS_TO_MUD_GRASS",
                                             "MOUNTAIN_TO_SNOW_MOUNTAIN",
                                             "SNOW_MOUNTAIN_TO_SNOW",
                                             "RIVER_0",
                                             "RIVER_1",
                                             "RIVER_2",
                                             "RIVER_3"};

    for (std::string& texName : textureNames) {
        ltm->registerTexture(texName);

        auto rendererTextureID = mapper->createRendererLandscapeTextureID();
        mapper->setTextureMapping(ltm->getTextureID(texName),
                                  rendererTextureID);
    }

    std::vector<int> ids = {32, 0, 2, 1, 4, 7, 8, 10, 13, 15, 17, 20, 21};
    std::vector<std::string> textures2 = {
        "UNKNOWN", "GRASS",    "DARK_GRASS", "GRASS_ISLAND", "WATER_7",
        "BEACH",   "MOUNTAIN", "DESERT",     "MUD",          "SWAMP",
        "SNOW",    "EARTH",    "PATH"};

    for (int i = 0; i < ids.size(); i++) {
        OpenS4::Import::ImageData data = registry.getImage(ghID, ids[i]);
        int id = textures->add_texture_plane_256x256(&data);

        ltm->registerTexture(textures2[i]);

        auto rendererID = mapper->createRendererLandscapeTextureID();
        mapper->setTextureMapping(ltm->getTextureID(textures2[i]), rendererID);
        mapper->setLandscapeTextureID(rendererID, id);
    }

    // Water
    {
        OpenS4::Import::ImageData data = registry.getImage(ghID, 5);
        mapper->setLandscapeTextureID(
            mapper->getRendererTextureID(ltm->getTextureID("WATER_0")),
            textures->add_texture(64, 64, 0, 0, &data));
        mapper->setLandscapeTextureID(
            mapper->getRendererTextureID(ltm->getTextureID("WATER_1")),
            textures->add_texture(64, 64, 1, 0, &data));
        mapper->setLandscapeTextureID(
            mapper->getRendererTextureID(ltm->getTextureID("WATER_2")),
            textures->add_texture(64, 64, 2, 0, &data));
        mapper->setLandscapeTextureID(
            mapper->getRendererTextureID(ltm->getTextureID("WATER_3")),
            textures->add_texture(64, 64, 3, 0, &data));
        mapper->setLandscapeTextureID(
            mapper->getRendererTextureID(ltm->getTextureID("WATER_4")),
            textures->add_texture(64, 64, 0, 1, &data));
        mapper->setLandscapeTextureID(
            mapper->getRendererTextureID(ltm->getTextureID("WATER_5")),
            textures->add_texture(64, 64, 1, 1, &data));
        mapper->setLandscapeTextureID(
            mapper->getRendererTextureID(ltm->getTextureID("WATER_6")),
            textures->add_texture(64, 64, 2, 1, &data));
    }

    converter->setTextureConversion(16, 8, ltm->getTextureID("GRASS"));

    converter->setTextureConversion(
        17, 8, ltm->getTextureID("GRASS_TO_GRASS_MOUNTAIN"));

    converter->setTextureConversion(18, 8, ltm->getTextureID("GRASS_ISLAND"));

    converter->setTextureConversion(20, 8,
                                    ltm->getTextureID("GRASS_TO_DESERT_GRASS"));

    converter->setTextureConversion(21, 8,
                                    ltm->getTextureID("GRASS_TO_SWAMP_GRASS"));

    converter->setTextureConversion(23, 8,
                                    ltm->getTextureID("GRASS_TO_MUD_GRASS"));
    converter->setTextureConversion(24, 8, ltm->getTextureID("DARK_GRASS"));

    converter->setTextureConversion(25, 8,
                                    ltm->getTextureID("GRASS_TO_DARK_GRASS"));

    converter->setTextureConversion(28, 8, ltm->getTextureID("EARTH"));
    converter->setTextureConversion(29, 8, ltm->getTextureID("PATH"));

    converter->setTextureConversion(32, 8, ltm->getTextureID("MOUNTAIN"));
    converter->setTextureConversion(
        35, 8, ltm->getTextureID("MOUNTAIN_TO_SNOW_MOUNTAIN"));

    converter->setTextureConversion(
        33, 8, ltm->getTextureID("GRASS_MOUNTAIN_TO_MOUNTAIN"));

    converter->setTextureConversion(48, 8, ltm->getTextureID("BEACH"));
    converter->setTextureConversion(48, 7, ltm->getTextureID("BEACH"));

    converter->setTextureConversion(64, 8, ltm->getTextureID("DESERT"));

    converter->setTextureConversion(
        65, 8, ltm->getTextureID("DESERT_GRASS_TO_DESERT"));

    converter->setTextureConversion(80, 8, ltm->getTextureID("SWAMP"));

    converter->setTextureConversion(81, 8,
                                    ltm->getTextureID("SWAMP_GRASS_TO_SWAMP"));

    converter->setTextureConversion(128, 8, ltm->getTextureID("SNOW"));
    converter->setTextureConversion(129, 8,
                                    ltm->getTextureID("SNOW_MOUNTAIN_TO_SNOW"));

    converter->setTextureConversion(144, 8, ltm->getTextureID("MUD"));
    converter->setTextureConversion(145, 8,
                                    ltm->getTextureID("MUD_GRASS_TO_MUD"));

    converter->setTextureConversion(7, 8, ltm->getTextureID("WATER_7"));
    converter->setTextureConversion(6, 8, ltm->getTextureID("WATER_6"));
    converter->setTextureConversion(5, 8, ltm->getTextureID("WATER_5"));
    converter->setTextureConversion(4, 8, ltm->getTextureID("WATER_4"));
    converter->setTextureConversion(3, 8, ltm->getTextureID("WATER_3"));
    converter->setTextureConversion(2, 8, ltm->getTextureID("WATER_2"));
    converter->setTextureConversion(1, 8, ltm->getTextureID("WATER_1"));
    converter->setTextureConversion(0, 8, ltm->getTextureID("WATER_0"));

    converter->setRangedTextureConversion(16, 0, 69,
                                          ltm->getTextureID("GRASS"));
    converter->setRangedTextureConversion(48, 0, 69,
                                          ltm->getTextureID("BEACH"));

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
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("GRASS")),
        mapper->getRendererTextureID(ltm->getTextureID("GRASS_TO_DARK_GRASS")),
        info);

    update_info(1, 0);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("GRASS_TO_DARK_GRASS")),
        mapper->getRendererTextureID(ltm->getTextureID("GRASS")), info);

    update_info(0, 2);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("DARK_GRASS")),
        mapper->getRendererTextureID(ltm->getTextureID("GRASS_TO_DARK_GRASS")),
        info);

    update_info(1, 2);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("GRASS_TO_DARK_GRASS")),
        mapper->getRendererTextureID(ltm->getTextureID("DARK_GRASS")), info);

    /*
    update_info(1, 2);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("GRASS_TO_DARK_GRASS")),
        mapper->getRendererTextureID(ltm->getTextureID("DARK_GRASS")),
        info);

    update_info(0, 3);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("DARK_GRASS")),
        mapper->getRendererTextureID(ltm->getTextureID("GRASS_TO_DARK_GRASS")),
        info);
    update_info(1, 3);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("GRASS_TO_DARK_GRASS")),
        mapper->getRendererTextureID(ltm->getTextureID("DARK_GRASS")), info);
*/

    update_info(2, 1);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("GRASS")),
        mapper->getRendererTextureID(ltm->getTextureID("BEACH")), info);

    update_info(3, 1);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("BEACH")),
        mapper->getRendererTextureID(ltm->getTextureID("GRASS")), info);

    update_info(2, 0);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("GRASS")),
        mapper->getRendererTextureID(ltm->getTextureID("BEACH")), info);

    update_info(3, 0);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("BEACH")),
        mapper->getRendererTextureID(ltm->getTextureID("GRASS")), info);

    update_info(2, 2);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("GRASS")),
        mapper->getRendererTextureID(ltm->getTextureID("GRASS_ISLAND")), info);

    update_info(3, 2);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("GRASS_ISLAND")),
        mapper->getRendererTextureID(ltm->getTextureID("GRASS")), info);

    data = registry.getImage(ghID, 5);
    update_info(0, 2);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("WATER_0")),
        mapper->getRendererTextureID(ltm->getTextureID("BEACH")), info);
    update_info(1, 2);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("BEACH")),
        mapper->getRendererTextureID(ltm->getTextureID("WATER_0")), info);

    data = registry.getImage(ghID, 11);
    // GRASS -> GRASS_TO_DESERT_GRASS -> DESERT_GRASS_TO_DESERT -> DESERT
    update_info(3, 0);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(
            ltm->getTextureID("DESERT_GRASS_TO_DESERT")),
        mapper->getRendererTextureID(ltm->getTextureID("DESERT")), info);

    update_info(2, 0);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("DESERT")),
        mapper->getRendererTextureID(
            ltm->getTextureID("DESERT_GRASS_TO_DESERT")),
        info);

    update_info(0, 2);
    mapper->addTextureHexagon(mapper->getRendererTextureID(
                                  ltm->getTextureID("DESERT_GRASS_TO_DESERT")),
                              mapper->getRendererTextureID(
                                  ltm->getTextureID("GRASS_TO_DESERT_GRASS")),
                              info);

    update_info(1, 2);
    mapper->addTextureHexagon(mapper->getRendererTextureID(
                                  ltm->getTextureID("GRASS_TO_DESERT_GRASS")),
                              mapper->getRendererTextureID(
                                  ltm->getTextureID("DESERT_GRASS_TO_DESERT")),
                              info);
    update_info(3, 2);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("GRASS")),
        mapper->getRendererTextureID(
            ltm->getTextureID("GRASS_TO_DESERT_GRASS")),
        info);
    update_info(2, 2);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(
            ltm->getTextureID("GRASS_TO_DESERT_GRASS")),
        mapper->getRendererTextureID(ltm->getTextureID("GRASS")), info);

    // GRASS_TO_GRASS_MOUNTAIN, GRASS_MOUNTAIN_TO_MOUNTAIN
    data = registry.getImage(ghID, 9);

    update_info(3, 0);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("GRASS")),
        mapper->getRendererTextureID(
            ltm->getTextureID("GRASS_TO_GRASS_MOUNTAIN")),
        info);
    update_info(2, 0);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(
            ltm->getTextureID("GRASS_TO_GRASS_MOUNTAIN")),
        mapper->getRendererTextureID(ltm->getTextureID("GRASS")), info);

    update_info(1, 2);
    mapper->addTextureHexagon(mapper->getRendererTextureID(
                                  ltm->getTextureID("GRASS_TO_GRASS_MOUNTAIN")),
                              mapper->getRendererTextureID(ltm->getTextureID(
                                  "GRASS_MOUNTAIN_TO_MOUNTAIN")),
                              info);
    update_info(0, 2);
    mapper->addTextureHexagon(mapper->getRendererTextureID(ltm->getTextureID(
                                  "GRASS_MOUNTAIN_TO_MOUNTAIN")),
                              mapper->getRendererTextureID(
                                  ltm->getTextureID("GRASS_TO_GRASS_MOUNTAIN")),
                              info);

    update_info(3, 2);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(
            ltm->getTextureID("GRASS_MOUNTAIN_TO_MOUNTAIN")),
        mapper->getRendererTextureID(ltm->getTextureID("MOUNTAIN")), info);

    update_info(2, 2);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("MOUNTAIN")),
        mapper->getRendererTextureID(
            ltm->getTextureID("GRASS_MOUNTAIN_TO_MOUNTAIN")),
        info);

    data = registry.getImage(ghID, 14);
    //"GRASS_TO_SWAMP_GRASS", "SWAMP_GRASS_TO_SWAMP"
    update_info(3, 0);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("GRASS")),
        mapper->getRendererTextureID(ltm->getTextureID("GRASS_TO_SWAMP_GRASS")),
        info);
    update_info(2, 0);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("GRASS_TO_SWAMP_GRASS")),
        mapper->getRendererTextureID(ltm->getTextureID("GRASS")), info);

    update_info(1, 2);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("GRASS_TO_SWAMP_GRASS")),
        mapper->getRendererTextureID(ltm->getTextureID("SWAMP_GRASS_TO_SWAMP")),
        info);
    update_info(0, 2);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("SWAMP_GRASS_TO_SWAMP")),
        mapper->getRendererTextureID(ltm->getTextureID("GRASS_TO_SWAMP_GRASS")),
        info);
    update_info(3, 2);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("SWAMP_GRASS_TO_SWAMP")),
        mapper->getRendererTextureID(ltm->getTextureID("SWAMP")), info);

    update_info(2, 2);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("SWAMP")),
        mapper->getRendererTextureID(ltm->getTextureID("SWAMP_GRASS_TO_SWAMP")),
        info);

    data = registry.getImage(ghID, 12);
    //"MUD_GRASS_TO_MUD", "GRASS_TO_MUD_GRASS"
    update_info(3, 2);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("GRASS")),
        mapper->getRendererTextureID(ltm->getTextureID("GRASS_TO_MUD_GRASS")),
        info);
    update_info(2, 2);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("GRASS_TO_MUD_GRASS")),
        mapper->getRendererTextureID(ltm->getTextureID("GRASS")), info);

    update_info(1, 2);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("GRASS_TO_MUD_GRASS")),
        mapper->getRendererTextureID(ltm->getTextureID("MUD_GRASS_TO_MUD")),
        info);
    update_info(0, 2);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("MUD_GRASS_TO_MUD")),
        mapper->getRendererTextureID(ltm->getTextureID("GRASS_TO_MUD_GRASS")),
        info);
    update_info(3, 0);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("MUD_GRASS_TO_MUD")),
        mapper->getRendererTextureID(ltm->getTextureID("MUD")), info);

    update_info(2, 0);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("MUD")),
        mapper->getRendererTextureID(ltm->getTextureID("MUD_GRASS_TO_MUD")),
        info);

    data = registry.getImage(ghID, 16);
    //"MOUNTAIN_TO_SNOW_MOUNTAIN", "SNOW_MOUNTAIN_TO_SNOW"
    update_info(3, 0);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("MOUNTAIN")),
        mapper->getRendererTextureID(
            ltm->getTextureID("MOUNTAIN_TO_SNOW_MOUNTAIN")),
        info);
    update_info(2, 0);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(
            ltm->getTextureID("MOUNTAIN_TO_SNOW_MOUNTAIN")),
        mapper->getRendererTextureID(ltm->getTextureID("MOUNTAIN")), info);

    update_info(1, 2);
    mapper->addTextureHexagon(mapper->getRendererTextureID(ltm->getTextureID(
                                  "MOUNTAIN_TO_SNOW_MOUNTAIN")),
                              mapper->getRendererTextureID(
                                  ltm->getTextureID("SNOW_MOUNTAIN_TO_SNOW")),
                              info);
    update_info(0, 2);
    mapper->addTextureHexagon(mapper->getRendererTextureID(
                                  ltm->getTextureID("SNOW_MOUNTAIN_TO_SNOW")),
                              mapper->getRendererTextureID(ltm->getTextureID(
                                  "MOUNTAIN_TO_SNOW_MOUNTAIN")),
                              info);
    update_info(3, 2);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(
            ltm->getTextureID("SNOW_MOUNTAIN_TO_SNOW")),
        mapper->getRendererTextureID(ltm->getTextureID("SNOW")), info);

    update_info(2, 2);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("SNOW")),
        mapper->getRendererTextureID(
            ltm->getTextureID("SNOW_MOUNTAIN_TO_SNOW")),
        info);

    data = registry.getImage(ghID, 19);
    update_info(0, 0);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("GRASS")),
        mapper->getRendererTextureID(ltm->getTextureID("EARTH")), info);
    update_info(1, 0);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("EARTH")),
        mapper->getRendererTextureID(ltm->getTextureID("GRASS")), info);

    update_info(2, 0);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("GRASS")),
        mapper->getRendererTextureID(ltm->getTextureID("PATH")), info);
    update_info(3, 0);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("PATH")),
        mapper->getRendererTextureID(ltm->getTextureID("GRASS")), info);

    data = registry.getImage(ghID, 6);
    update_info(0, 0);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("WATER_1")),
        mapper->getRendererTextureID(ltm->getTextureID("WATER_0")), info);
    update_info(1, 0);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("WATER_2")),
        mapper->getRendererTextureID(ltm->getTextureID("WATER_1")), info);
    update_info(2, 0);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("WATER_3")),
        mapper->getRendererTextureID(ltm->getTextureID("WATER_2")), info);
    update_info(3, 0);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("WATER_4")),
        mapper->getRendererTextureID(ltm->getTextureID("WATER_3")), info);

    update_info(0, 1);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("WATER_0")),
        mapper->getRendererTextureID(ltm->getTextureID("WATER_1")), info);
    update_info(1, 1);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("WATER_1")),
        mapper->getRendererTextureID(ltm->getTextureID("WATER_2")), info);
    update_info(2, 1);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("WATER_2")),
        mapper->getRendererTextureID(ltm->getTextureID("WATER_3")), info);
    update_info(3, 1);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("WATER_3")),
        mapper->getRendererTextureID(ltm->getTextureID("WATER_4")), info);

    update_info(0, 2);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("WATER_5")),
        mapper->getRendererTextureID(ltm->getTextureID("WATER_4")), info);
    update_info(1, 2);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("WATER_6")),
        mapper->getRendererTextureID(ltm->getTextureID("WATER_5")), info);
    update_info(2, 2);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("WATER_7")),
        mapper->getRendererTextureID(ltm->getTextureID("WATER_6")), info);

    update_info(0, 3);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("WATER_4")),
        mapper->getRendererTextureID(ltm->getTextureID("WATER_5")), info);
    update_info(1, 3);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("WATER_5")),
        mapper->getRendererTextureID(ltm->getTextureID("WATER_6")), info);
    update_info(2, 3);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("WATER_6")),
        mapper->getRendererTextureID(ltm->getTextureID("WATER_7")), info);

    data = registry.getImage(ghID, 11);
    update_info(2, 2);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("DESERT")),
        mapper->getRendererTextureID(ltm->getTextureID("GRASS")), info);
    update_info(1, 1);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("GRASS")),
        mapper->getRendererTextureID(ltm->getTextureID("DESERT")), info);

    data = registry.getImage(ghID, 9);
    update_info(2, 0);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("MOUNTAIN")),
        mapper->getRendererTextureID(ltm->getTextureID("GRASS")), info);
    update_info(1, 2);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("GRASS")),
        mapper->getRendererTextureID(ltm->getTextureID("MOUNTAIN")), info);

    data = registry.getImage(ghID, 14);
    update_info(2, 0);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("SWAMP")),
        mapper->getRendererTextureID(ltm->getTextureID("GRASS")), info);
    update_info(3, 0);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("GRASS")),
        mapper->getRendererTextureID(ltm->getTextureID("SWAMP")), info);

    //    converter.setTextureConversion(16, 8, ltm->getTextureID("GRASS"));
    //   converter.setTextureConversion(48, 8, ltm->getTextureID("BEACH"));

    converter->setTextureConversion(96, 8, ltm->getTextureID("RIVER_0"));
    converter->setTextureConversion(97, 8, ltm->getTextureID("RIVER_1"));
    converter->setTextureConversion(98, 8, ltm->getTextureID("RIVER_2"));
    converter->setTextureConversion(99, 8, ltm->getTextureID("RIVER_3"));

    data = registry.getImage(ghID, 18);

    update_info(0, 0);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("RIVER_0")),
        mapper->getRendererTextureID(ltm->getTextureID("GRASS")), info);
    update_info(0, 2);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("GRASS")),
        mapper->getRendererTextureID(ltm->getTextureID("RIVER_0")), info);

    update_info(1, 0);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("RIVER_1")),
        mapper->getRendererTextureID(ltm->getTextureID("GRASS")), info);
    update_info(1, 2);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("GRASS")),
        mapper->getRendererTextureID(ltm->getTextureID("RIVER_1")), info);

    update_info(2, 0);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("RIVER_2")),
        mapper->getRendererTextureID(ltm->getTextureID("GRASS")), info);
    update_info(2, 2);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("GRASS")),
        mapper->getRendererTextureID(ltm->getTextureID("RIVER_2")), info);

    update_info(3, 0);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("RIVER_3")),
        mapper->getRendererTextureID(ltm->getTextureID("GRASS")), info);
    update_info(3, 2);
    mapper->addTextureHexagon(
        mapper->getRendererTextureID(ltm->getTextureID("GRASS")),
        mapper->getRendererTextureID(ltm->getTextureID("RIVER_3")), info);

    auto river_3 = mapper->getRendererTextureID(ltm->getTextureID("RIVER_3"));
    auto grass = mapper->getRendererTextureID(ltm->getTextureID("GRASS"));

    auto beach = mapper->getRendererTextureID(ltm->getTextureID("BEACH"));

    auto water_0 = mapper->getRendererTextureID(ltm->getTextureID("WATER_0"));

    mapper->addTextureHexagon(grass, water_0, river_3, water_0, river_3,
                              water_0, river_3, water_0, river_3, water_0,
                              river_3, water_0, river_3, info);
    mapper->addTextureHexagon(grass, river_3, water_0, river_3, water_0,
                              river_3, water_0, river_3, water_0, river_3,
                              water_0, river_3, water_0, info);

    data = registry.getImage(ghID, 5);
    update_info(0, 0);
    mapper->addTextureHexagon(water_0, river_3, info);
    mapper->addTextureHexagon(river_3, water_0, info);

    data = registry.getImage(ghID, 18);

    update_info(3, 2);
    mapper->addTextureHexagon(grass, water_0, info);

    update_info(3, 0);
    mapper->addTextureHexagon(water_0, grass, info);

    data = registry.getImage(ghID, 5);

    update_info(3, 2);
    mapper->addTextureHexagon(water_0, beach, grass, beach, grass, beach, grass,
                              beach, grass, beach, grass, beach, grass, info);

    update_info(2, 2);
    mapper->addTextureHexagon(water_0, grass, beach, grass, beach, grass, beach,
                              grass, beach, grass, beach, grass, beach, info);

    // TODO ERROR IN TEXTURE HANDLING UV-COORDINATES ORIGIN IN BOTTOM LEFT; NOT
    // TOP LEFT IN OPENGL

    // 48, 0, 16

    /*
    SEE TEXTURE MAPPER 183ff
            _6.y = _2.y;
        _2.y = _5.y;
        _5.y = _6.y;
        _3.y = _2.y;
    */

    /* mapper->addTextureHexagon(
         mapper->getRendererTextureID(ltm->getTextureID("RIVER_3")),
         mapper->getRendererTextureID(ltm->getTextureID("GRASS")), info);
     */

    // converter->setTextureConversion(0, 8, ltm->getTextureID("WATER_0"));
}

OpenS4::Logic::Map::Landscape makeLandscape2(
    OpenS4::Import::Map::Map* map,
    OpenS4::Renderer::LandscapeTextures* textures,
    OpenS4::Logic::Map::LandscapeTextureConverter* converter,
    OpenS4::Renderer::TextureMapper* mapper) {
    OpenS4::Import::Map::Landscape* mapLandscape =
        OpenS4::Import::Map::fromMap(map);

    auto height = mapLandscape->getHeight();
    auto width = mapLandscape->getWidth();

    OpenS4::Logic::Map::Landscape landscape(width, height);

    for (auto y = 0; y < height; y++) {
        for (auto x = 0; x < width; x++) {
            OpenS4::Import::Map::LandscapePosition pos =
                mapLandscape->getLandscapePosition(x, y);

            auto logicID =
                converter->convert(pos.terrainType, pos.terrainSubtype);

            landscape.setTerrainDebug(x, y, pos.terrainType,
                                      pos.terrainSubtype);

            landscape.setTerrain(x, y, logicID);
            landscape.setTerrainHeight(x, y, pos.height);
        }
    }

    delete mapLandscape;
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

float g_heightScale = 1.2f;

bool g_drawPoints = false;

OpenS4::Logic::Direction g_direction = OpenS4::Logic::Direction::EAST;

OpenS4::Renderer::LandscapeRenderer* g_landscapeRenderer = nullptr;

struct XYDir {
    s32 x;
    s32 y;
    u32 dir;
};

struct BuildingFrames {
    std::string name;
    u32 baseFrameID = 0;
    u32 gfxID = 0;
    u32 width = 0;
    u32 height = 0;

    s32 xOffset = 0;
    s32 yOffset = 0;

    s32 doorX = 0;
    s32 doorY = 0;

    std::vector<XYDir> builders;

    s32 hotspotX;
    s32 hotspotY;

    std::vector<std::bitset<32>> lines;
};
std::map<std::string, BuildingFrames> buildingFramesMap;

std::string g_builderFrameIndex = "";

auto g_imgID = 0;

class TestListener : public OpenS4::Input::InputListener {
    GLFWwindow* m_window = nullptr;

   public:
    TestListener(std::string name, GLFWwindow* window)
        : OpenS4::Input::InputListener(name), m_window(window) {}


    OpenS4::Input::EventDispatching mouseScrolled(const OpenS4::Input::MouseEvent* event) { 
        double scrollValue = event->getMouseScroll();
        if (scrollValue >= 0)
        {
            g_zoom *= 1+(scrollValue/10);
        }
        else
        {
            g_zoom /= 1 - (scrollValue / 10);
        }
        return OpenS4::Input::EventDispatching::CatchEvent;
    }

    OpenS4::Input::EventDispatching keyPressed(
        const OpenS4::Input::KeyEvent* event) {
        OpenS4::Input::EventDispatching dispatched = keyBindAction(event);
        if (dispatched != OpenS4::Input::EventDispatching::NoKeyBindFound) {
            return dispatched;
        }

        using namespace OpenS4::Input;

        static int drawMode = 0;

        static int xstep = 24;

        switch (event->getKey()) {
            case Key::T:
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
            case Key::Y:  // This is Key::Z (german keyboard)
                g_drawPoints = !g_drawPoints;
                break;

            case Key::J:
                g_zoom *= 1.5f;
                break;
            case Key::K:
                g_zoom /= 1.5f;
                break;
            case Key::B:
                g_zoom += 0.01f;
                break;
            case Key::N:
                g_zoom -= 0.01f;
                break;

            case Key::Right:
                g_mapPosX += 2 / g_zoom;
                break;
            case Key::Left:
                g_mapPosX -= 2 / g_zoom;
                break;
            case Key::Down:
                g_mapPosY += 2 / g_zoom;
                break;
            case Key::Up:
                g_mapPosY -= 2 / g_zoom;
                break;
            case Key::Esc:
                glfwSetWindowShouldClose(m_window, GLFW_TRUE);

            case Key::F:
                g_heightScale += 0.1f;
                g_landscapeRenderer->updateAllChunks();

                break;
            case Key::G:
                g_heightScale -= 0.1f;

                g_landscapeRenderer->updateAllChunks();
                break;

            case Key::I: {
                std::string prev;
                for (auto& frame : buildingFramesMap) {
                    if (prev == "") {
                        prev = frame.first;
                    }
                    if (frame.first == g_builderFrameIndex) {
                        g_builderFrameIndex = prev;
                        break;
                    }
                    prev = frame.first;
                }
            } break;
            case Key::O: {
                bool is_next = false;
                for (auto& frame : buildingFramesMap) {
                    if (is_next) {
                        g_builderFrameIndex = frame.first;
                        break;
                    }
                    if (frame.first == g_builderFrameIndex) {
                        is_next = true;
                    }
                }
            } break;

            case Key::C:
                g_imgID++;
                break;
            case Key::X:
                g_imgID += 10;
                break;

            case Key::V:
                g_imgID--;
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
              OpenS4::Renderer::TextureMapper* mapper,
              OpenS4::Logic::Map::Landscape* logicLandscape,
              OpenS4::Logic::Map::LandscapeTextureMapper* ltm) {
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

    std::map<u32, std::map<u32, u32>> registryFrameIDtoImageID;

    g_landscapeRenderer = p_renderer;

    // gfx: 10 roman, 11 viking, 12 maya, 13 dark, 14 trojan

    // Build atlas(es) 20 = roman settlers
    for (int i = 10; i < 25; i++) {
        registryFrameIDtoImageID[i] = {};
        for (int j = 0; j < registry->getNumberOfImages(i); j++) {
            OpenS4::Import::ImageData image = registry->getImage(i, j);
            auto imageID = objRenderer.addImage(image);
            maxImageID++;

            registryFrameIDtoImageID[i][j] = imageID;
            if (maxImageID % 1000 == 0)
                objRenderer.buildAtlasesContinuous(1024);
        }
    }
    objRenderer.buildAtlasesContinuous(1024, true);

    OpenS4::getLogger().info("Built %d atlases for objects!",
                             objRenderer.getNumberOfAtlases());
    OpenS4::getLogger().info("Atlases contain %d images!", maxImageID);

    // JSON

    {
        std::ifstream ifs("Config/romanBuildingsBaseFrame.json");
        nlohmann::json buildingFrames = nlohmann::json::parse(ifs);

        for (auto& jo : buildingFrames) {
            BuildingFrames frames;
            frames.name = jo["name"];
            frames.gfxID = jo["gfx"];
            frames.baseFrameID = jo["baseFrame"];

            frames.width =
                registry->getImage(frames.gfxID, frames.baseFrameID).getWidth();
            frames.height = registry->getImage(frames.gfxID, frames.baseFrameID)
                                .getHeight();

            buildingFramesMap[jo["name"]] = frames;
        }

        {
            std::ifstream ifs("Config/romanBuildings.json");
            nlohmann::json builders = nlohmann::json::parse(ifs);

            for (auto& jo : builders) {
                BuildingFrames& frame = buildingFramesMap[jo["name"]];

                for (auto& builder : jo["builders"]) {
                    XYDir xydir;
                    xydir.x = builder["x"];
                    xydir.y = builder["y"];
                    xydir.dir = builder["direction"];
                    frame.builders.push_back(xydir);
                }

                frame.hotspotX = jo["hotspotX"];
                frame.hotspotY = jo["hotspotY"];

                for (auto& line : jo["lines"]) {
                    std::string l = line;
                    std::bitset<32> bits = std::bitset<32>(l);
                    frame.lines.push_back(bits);
                }
            }
        }
    }

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

    OpenS4::Renderer::TransformationPipeline transform;

    OpenS4::Renderer::PointBatch pointBatch;

    while (!glfwWindowShouldClose(window)) {
        // Setup camera & translation for this frame.
        glm::mat4x4 ortho =
            glm::ortho((float)-width / 2, (float)width / 2, (float)-height / 2,
                       (float)height / 2, (float)-200, (float)200);
        transform.getProjectionStack()->loadMatrix(ortho);
        transform.getProjectionStack()->scale(g_zoom, g_zoom, 1);
        transform.getProjectionStack()->translate(-g_mapPosX * 10,
                                                  10 * g_mapPosY, 0);

        // Pass the tranformation to the landscape renderer (so that it can
        // translate visible pixels to coordinates).
        renderer.setTransform(&transform, width, height);

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
            sstream << " HeightScale: " << g_heightScale;
            sstream << " Tick: " << tick;
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

            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);

            glm::vec2 model = g_landscapeRenderer->toModelPositionHighPrecision(
                g_landscapeRenderer->screenToWorld(glm::vec2(xpos, ypos)));
            model.x = round(model.x);
            model.y = round(model.y);

            if (model.x >= 0 && model.y >= 0 &&
                model.y < logicLandscape->getHeight() &&
                model.x < logicLandscape->getWidth()) {
                sstream
                    << logicLandscape->getTerrainDebug(model.x, model.y).type
                    << " "
                    << logicLandscape->getTerrainDebug(model.x, model.y)
                           .subType;
            }

            sstream << " HeightScale: " << g_heightScale;
            sstream << " Tick: " << tick;
            sstream << " Tick2: " << tick2;

            sstream << " FPS: " << lastFrameCount;

            glfwSetWindowTitle(window, sstream.str().c_str());
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        // Only render if window has size > 0
        if (width && height) {
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

            objRenderer.render(10 + 6, 10 + 15,
                               registryFrameIDtoImageID[20][frame],
                               g_playerColor % 8);

            auto imgMaxID = registry->getNumberOfImages(24);
            objRenderer.render(
                10, 10, registryFrameIDtoImageID[24][g_imgID % imgMaxID], 2);

            auto palette = registry->getJobIndex(24, g_imgID);

            for (auto y = 0; y < 40; y++)
                for (auto x = 0; x < 40; x++) {
                    logicLandscape->setTerrain(x, y,
                                               ltm->getTextureID("GRASS"));
                }

            XYDir door;

            std::map<u32, std::set<u32>> pointsToDraw;
            {
                BuildingFrames& frame2 = buildingFramesMap[g_builderFrameIndex];

                OpenS4::Import::ImageData img =
                    registry->getImage(frame2.gfxID, frame2.baseFrameID);

                objRenderer.render(
                    20, 20,
                    registryFrameIDtoImageID[frame2.gfxID][frame2.baseFrameID],
                    0);

                door.x = 20 + frame2.doorX;
                door.y = 20 + frame2.doorY;

                int y = 20 - frame2.hotspotY;
                for (auto& line : frame2.lines) {
                    for (int i = 0; i < 32; i++)
                        if (line.test(i)) {
                            int x = 20 - frame2.hotspotX + (31 - i);
                            pointsToDraw[x].emplace(y);

                            logicLandscape->setTerrain(
                                x, y, ltm->getTextureID("EARTH"));
                        }
                    y++;
                }

                for (auto& xydir : frame2.builders) {
                    objRenderer.render(
                        20 + xydir.x, 20 + xydir.y,
                        registryFrameIDtoImageID[20][3840 + xydir.dir * 8], 0);
                }
            }

            objRenderer.draw();

            renderer.updateChunk(0, 0);

            if (g_drawPoints) {
                // Draw map points
                glUseProgram(pointShader);
                glUniformMatrix4fv(
                    glGetUniformLocation(pointShader, "mvpMatrix"), 1, GL_FALSE,
                    glm::value_ptr(transform.getModelViewProjectionMatrix()));

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
                box.upper.y =
                    std::min((float)renderer.getHeight(), box.upper.y);

                for (auto y = box.lower.y; y < box.upper.y; y++) {
                    for (auto x = box.lower.x; x < box.upper.x; x++) {
                        glm::vec2 pixel =
                            renderer.toPixelPosition(glm::vec2(x, y));
                        points.push_back(pixel.x);
                        points.push_back(pixel.y);

                        auto terrain = landscape->getTerrain(x, y);

                        if ((x == door.x && y == door.y)) {
                            pointColors.push_back(1);
                            pointColors.push_back(1);
                            pointColors.push_back(1);
                            pointColors.push_back(1);
                        } else if (x == 20 && y == 20) {
                            pointColors.push_back(0);
                            pointColors.push_back(1);
                            pointColors.push_back(0);
                            pointColors.push_back(1);
                        } else if (pointsToDraw.count(x) &&
                                   pointsToDraw[x].count(y)) {
                            pointColors.push_back(0);
                            pointColors.push_back(0);
                            pointColors.push_back(0);
                            pointColors.push_back(1);

                        } else if ((y == 10 && x == 10) || (y == 5 && x == 5) ||
                                   (y == 7 && x == 7)) {
                            pointColors.push_back(0);
                            pointColors.push_back(1);
                            pointColors.push_back(0);
                            pointColors.push_back(1);
                        } else if (  // mapper->getTexture("GRASS") == terrain
                                     // || mapper->getTexture("BEACH") ==
                                     // terrain

                            //  ||
                            (x == 1 && y == 6) || (x == 8 && y == 6) ||
                            (x == 3 && y == 9) || (x == 6 && y == 9)

                            || (x == 20 && y == 20) ||
                            (x == door.x && y == door.y)) {
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
        }

        // tell imgui a new frame starts
        OpenS4::Gui::Imgui::RenderWindows();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    OpenS4::Gui::Imgui::Cleanup();
}

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

    // Init imgui
    OpenS4::Gui::Imgui::InitImgui(window);

    // Init Landscape Texture Atlas & Texture Mapper
    OpenS4::Renderer::LandscapeTextures landscape_textures(2048, 2048);

    OpenS4::Logic::Map::LandscapeTextureMapper landscapeTextureMapper;
    OpenS4::Logic::Map::LandscapeTextureConverter converter;

    OpenS4::Renderer::TextureMapper textureMapper;

    registerMapperTextures(registry, &landscapeTextureMapper, &converter,
                           &landscape_textures, &textureMapper, 2);

    // Load simple map
    OpenS4::Import::Map::Map* map =
        OpenS4::Import::Map::readMap("Config/Maps/576_AlleTexturen.map");
    if (!map) {
        OpenS4::getLogger().warn("Map not found!");
    }

    // Make Landscape from Map

    OpenS4::Logic::Map::Landscape landscape =
        makeLandscape2(map, &landscape_textures, &converter, &textureMapper);

    OpenS4::Renderer::Landscape rendererLandscape(
        &landscape, &landscape_textures, &textureMapper);

    if (map != nullptr) {
        delete map;
        map = nullptr;
    }

    // Loop forever
    drawLoop(window, &rendererLandscape, &landscape_textures, &registry,
             &textureMapper, &landscape, &landscapeTextureMapper);

    glfwTerminate();
    OpenS4::getLogger().info("Program is exiting correctly!");

    return 0;
}
