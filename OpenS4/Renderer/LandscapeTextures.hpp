#pragma once

#include <Import/Graphics/Image.hpp>
#include <vector>

#include "TextureAtlas.hpp"

namespace OpenS4::Renderer {
class LandscapeTextures : public TextureAtlas {
   public:
    LandscapeTextures(int texture_atlas_width, int texture_atlas_height);
    ~LandscapeTextures();

    enum class TextureType {
        NOT_FOUND,
        _256x256,
        HEXAGON,
        FULL_HEXAGON,

        _TEX,
    };

    struct TextureInformation {
        int x;
        int y;

        int width;
        int height;

        TextureType tex_type = TextureType::NOT_FOUND;
    };

    ::std::vector<TextureInformation> m_textures;

    int add_texture_plane_256x256(const OpenS4::Import::ImageData* data);

    int add_texture_hexagon(int hexSizeX, int hexSizeY, int slotX, int slotY,
                            const OpenS4::Import::ImageData* data);

    int add_texture(int sizeX, int sizeY, int slotX, int slotY,
                    const OpenS4::Import::ImageData* data);

    TextureInformation* getTextureInformation(int index) {
        return &m_textures[index];
    }

    int getTextureCount() { return m_textures.size(); }

   private:
    GLuint m_GLTextureID = NULL;
};
}  // namespace OpenS4::Renderer
