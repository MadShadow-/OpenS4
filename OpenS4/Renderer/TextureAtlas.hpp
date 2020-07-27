#pragma once

#include <stdint.h>

#include "../Import/Graphics/Image.hpp"
#include "../Logger/Logger.hpp"
#include "OpenGL.hpp"

namespace OpenS4::Renderer {
class TextureAtlas {
   public:
    struct TextureAtlasPosition {
        int x;
        int y;
        int width;
        int height;
    };

    int get_width();
    int get_height();

    GLuint getGLTextureID() {
        if (m_gl_texture_ID != NULL) return m_gl_texture_ID;

        m_gl_texture_ID = create_gl_texture_atlas();

        return m_gl_texture_ID;
    }

    TextureAtlas(int width, int height);
    ~TextureAtlas();

    TextureAtlasPosition add_texture_to_atlas(
        const OpenS4::Import::ImageData* img);

   protected:
    enum class TextureSizeSlot {
        TEXTURE_HEIGHT_66,
        TEXTURE_HEIGHT_258,
        _TEXTURE_HEIGHT_COUNT
    };

    TextureAtlasPosition add_texture_to_atlas(uint32_t* data, int width,
                                              int height, int repeatWidth = 0);

    GLuint create_gl_texture_atlas();

   private:
    int m_slot_pos_x[(int)TextureSizeSlot::_TEXTURE_HEIGHT_COUNT];
    int m_slot_pos_y[(int)TextureSizeSlot::_TEXTURE_HEIGHT_COUNT];
    unsigned int* m_img_buffer;
    int m_width;
    int m_height;

    int m_filled_y_pos;

    int m_fill_state = 0;

    GLuint m_gl_texture_ID = 0;

    TextureSizeSlot get_slot_by_height(int height);

    void copy_image(unsigned int* dest, unsigned int* src, int pos_x, int pos_y,
                    int widht, int height, int dest_width, int src_width = -1);

    static int get_height_by_slot(TextureSizeSlot slot) {
        switch (slot) {
            case TextureSizeSlot::TEXTURE_HEIGHT_66:
                return 66;
            case TextureSizeSlot::TEXTURE_HEIGHT_258:
                return 258;  // TODO 258??
            default:
                __debugbreak();  // TODO
        }
    }
};

}  // namespace OpenS4::Renderer
