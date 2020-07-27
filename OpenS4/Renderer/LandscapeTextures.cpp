#include "LandscapeTextures.hpp"

#include <Import/Graphics/Gfx.hpp>

namespace OpenS4::Renderer {
LandscapeTextures::LandscapeTextures(int texture_atlas_width,
                                     int texture_atlas_height)
    : TextureAtlas(texture_atlas_width, texture_atlas_height) {}

LandscapeTextures::~LandscapeTextures() {}

int LandscapeTextures::add_texture_plane_256x256(
    const OpenS4::Import::ImageData* data) {
    OpenS4::Import::ImageData newImage(data->getWidth(), data->getHeight(),
                                       data->getPalette(), 0, 0, 0, 0);

    for (int y = 0; y < data->getHeight(); y++)
        for (int x = 0; x < data->getWidth(); x++) {
            uint32_t num = data->getColor(x, y);
            uint32_t swapped =
                ((num >> 24) & 0xff) |       // move byte 3 to byte 0
                ((num << 8) & 0xff0000) |    // move byte 1 to byte 2
                ((num >> 8) & 0xff00) |      // move byte 2 to byte 1
                ((num << 24) & 0xff000000);  // byte 0 to byte 3
            newImage.setPixel(x, y, swapped);
        }

    //  auto atlas_pos = add_texture_to_atlas(
    //      (unsigned int*)dat.data(), data->getWidth(), data->getHeight());

    auto atlas_pos = add_texture_to_atlas(&newImage);

    TextureInformation texture;
    texture.x = atlas_pos.x;
    texture.y = atlas_pos.y;
    texture.width = atlas_pos.width;
    texture.height = atlas_pos.height;

    texture.tex_type = TextureType::_256x256;

    m_textures.push_back(texture);
    return m_textures.size() - 1;
}

int LandscapeTextures::add_texture_hexagon(
    int hexSizeX, int hexSizeY, int slotX, int slotY,
    const OpenS4::Import::ImageData* data) {
    ::std::vector<uint32_t> dat;
    for (int i = 0; i < data->getWidth() * data->getHeight(); i++) {
        uint32_t num = data->getData()[i];
        uint32_t swapped = ((num >> 24) & 0xff) |       // move byte 3 to byte 0
                           ((num << 8) & 0xff0000) |    // move byte 1 to byte 2
                           ((num >> 8) & 0xff00) |      // move byte 2 to byte 1
                           ((num << 24) & 0xff000000);  // byte 0 to byte 3

        if (num == 0xFFF803F8) {
            swapped = 0;
        }

        dat.push_back(swapped);
    }

    int xMin = hexSizeX * slotX;
    int yMin = hexSizeY * slotY;

    int xMax = hexSizeX * (slotX + 1);
    int yMax = hexSizeY * (slotY + 1);

    ::std::vector<uint32_t> sub;

    OpenS4::Import::ImageData subImage(hexSizeX, hexSizeY, 0, 0, 0, 0, 0);

    int y2 = 0;
    int x2 = 0;
    for (int y = yMin; y < yMax; y++) {
        x2 = 0;
        for (int x = xMin; x < xMax; x++) {
            subImage.setPixel(y2 * hexSizeX + x2,
                              dat[y * data->getWidth() + x]);
            x2++;
        }
        y2++;
    }

    for (int y = yMin; y < yMax; y++) {
        for (int x = xMin; x < xMax; x++) {
            sub.push_back(dat[y * data->getWidth() + x]);
        }
    }

    dat = sub;

    auto atlas_pos = add_texture_to_atlas(&subImage);

    // auto atlas_pos =
    //  add_texture_to_atlas((unsigned int*)dat.data(), hexSizeX, hexSizeY);

    TextureInformation texture;
    texture.x = atlas_pos.x;
    texture.y = atlas_pos.y;
    texture.width = atlas_pos.width;
    texture.height = atlas_pos.height;

    texture.tex_type = TextureType::FULL_HEXAGON;

    m_textures.push_back(texture);
    return m_textures.size() - 1;
}

int LandscapeTextures::add_texture(int sizeX, int sizeY, int slotX, int slotY,
                                   const OpenS4::Import::ImageData* data) {
    ::std::vector<uint32_t> dat;
    for (int i = 0; i < data->getWidth() * data->getHeight(); i++) {
        uint32_t num = data->getData()[i];
        uint32_t swapped = ((num >> 24) & 0xff) |       // move byte 3 to byte 0
                           ((num << 8) & 0xff0000) |    // move byte 1 to byte 2
                           ((num >> 8) & 0xff00) |      // move byte 2 to byte 1
                           ((num << 24) & 0xff000000);  // byte 0 to byte 3
        dat.push_back(swapped);
    }

    int xMin = sizeX * slotX;
    int yMin = sizeY * slotY;

    int xMax = sizeX * (slotX + 1);
    int yMax = sizeY * (slotY + 1);

    ::std::vector<uint32_t> sub;
    for (int y = yMin; y < yMax; y++) {
        for (int x = xMin; x < xMax; x++) {
            sub.push_back(dat[y * data->getWidth() + x]);
        }
    }

    OpenS4::Import::ImageData subImage(sizeX, sizeY, 0, 0, 0, 0, 0);

    int y2 = 0;
    int x2 = 0;
    for (int y = yMin; y < yMax; y++) {
        x2 = 0;
        for (int x = xMin; x < xMax; x++) {
            subImage.setPixel(y2 * sizeX + x2, dat[y * data->getWidth() + x]);
            x2++;
        }
        y2++;
    }

    dat = sub;

    auto atlas_pos = add_texture_to_atlas(&subImage);

    // auto atlas_pos =
    //    add_texture_to_atlas((unsigned int*)dat.data(), sizeX, sizeY);

    TextureInformation texture;
    texture.x = atlas_pos.x;
    texture.y = atlas_pos.y;
    texture.width = atlas_pos.width;
    texture.height = atlas_pos.height;

    texture.tex_type = TextureType::_TEX;

    m_textures.push_back(texture);
    return m_textures.size() - 1;
}

}  // namespace OpenS4::Renderer
