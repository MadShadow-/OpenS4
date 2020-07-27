
#include "TextureAtlas.hpp"

#include "../main.hpp"

namespace OpenS4::Renderer {
TextureAtlas::TextureAtlas(int width, int height) {
    m_width = width;
    m_height = height;

    m_img_buffer = new unsigned int[m_width * m_height];

    for (int i = 0; i < (int)TextureSizeSlot::_TEXTURE_HEIGHT_COUNT; i++) {
        m_slot_pos_x[i] = m_width;
        m_slot_pos_y[i] = 0;
    }

    m_filled_y_pos = 0;
}

TextureAtlas::~TextureAtlas() {
    if (m_img_buffer != NULL) delete[] m_img_buffer;
    m_img_buffer = NULL;
    m_width = 0;
    m_height = 0;

    if (m_gl_texture_ID != NULL) glDeleteTextures(1, &m_gl_texture_ID);
    m_gl_texture_ID = NULL;
}

TextureAtlas::TextureSizeSlot TextureAtlas::get_slot_by_height(int height) {
    for (int i = 0; i < (int)TextureSizeSlot::_TEXTURE_HEIGHT_COUNT; i++) {
        if (height <= get_height_by_slot((TextureSizeSlot)i))
            return (TextureSizeSlot)i;
    }

    getLogger().err("%s: %s", __FUNCTION__, "height doesn't fit!");

    return TextureAtlas::TextureSizeSlot::_TEXTURE_HEIGHT_COUNT;
}

TextureAtlas::TextureAtlasPosition TextureAtlas::add_texture_to_atlas(
    const OpenS4::Import::ImageData* img) {
    OpenS4::Import::ImageData biggerImage(img->getWidth() + 2,
                                          img->getHeight() + 2, 0, 0, 0, 0, 0);
    for (int y = 0; y < biggerImage.getHeight(); y++) {
        for (int x = 0; x < biggerImage.getWidth(); x++) {
            int yidx = y - 1;
            int xidx = x - 1;

            uint32_t color;

            if (yidx < 0) yidx = 0;
            if (xidx < 0) xidx = 0;

            if (yidx >= img->getHeight()) yidx = img->getHeight() - 1;
            if (xidx >= img->getWidth()) xidx = img->getWidth() - 1;

            biggerImage.setPixel(x, y, img->getColor(xidx, yidx));
        }
    }

    // Mirror in y dir.
    OpenS4::Import::ImageData biggerImage2(
        biggerImage.getWidth(), biggerImage.getHeight(), 0, 0, 0, 0, 0);
    for (int y = 0; y < biggerImage.getHeight(); y++) {
        for (int x = 0; x < biggerImage.getWidth(); x++) {
            biggerImage2.setPixel(
                x, y, biggerImage.getColor(x, biggerImage.getWidth() - y));
        }
    }
    biggerImage = biggerImage2;

    TextureAtlas::TextureAtlasPosition pos;
    pos.height = 0;
    pos.width = 0;
    pos.x = 0;
    pos.y = 0;

    if (m_img_buffer == NULL) {
        getLogger().err(
            "%s: %s", __FUNCTION__,
            "Texture can't by copied to Texture Atlas! No texture given.");

        return pos;
    }

    TextureSizeSlot slot = get_slot_by_height(biggerImage.getHeight());

    if (slot == TextureSizeSlot::_TEXTURE_HEIGHT_COUNT) {
        getLogger().err(
            "%s: %s", __FUNCTION__,
            "Texture can't by copied to Texture Atlas! Texture height is out "
            "of range.");
        return pos;
    }

    if (biggerImage.getWidth() > m_width) {
        getLogger().err(
            "%s: %s", __FUNCTION__,
            "Texture can't by copied to Texture Atlas! Texture width is to "
            "large!");
        return pos;
    }

    if ((m_slot_pos_x[(int)slot] + biggerImage.getWidth()) > m_width) {
        if ((m_filled_y_pos + get_height_by_slot(slot)) > m_height) {
            getLogger().err(
                "%s: %s", __FUNCTION__,
                "Texture can't by copied to Texture Atlas! Texture Atlas is "
                "FULL!");
            return pos;
        }

        //- slot is full - create new!
        m_slot_pos_y[(int)slot] = m_filled_y_pos;
        m_slot_pos_x[(int)slot] = 0;
        m_filled_y_pos += get_height_by_slot(slot);
    }

    int outX = m_slot_pos_x[(int)slot];
    int outY = m_slot_pos_y[(int)slot];

    copy_image(m_img_buffer, (unsigned int*)biggerImage.getData(), outX, outY,
               biggerImage.getWidth(), biggerImage.getHeight(), m_width);

    //- move slot position
    m_slot_pos_x[(int)slot] += biggerImage.getWidth();

    pos.x = outX + 1;
    pos.y = outY + 1;
    pos.width = img->getWidth();
    pos.height = img->getHeight();

    //- update fill state
    m_fill_state += biggerImage.getWidth() * biggerImage.getHeight();

    return pos;
}

/*
TextureAtlas::TextureAtlasPosition TextureAtlas::add_texture_to_atlas(
    uint32_t* data, int width, int height, int repeatWidth) {
    TextureAtlas::TextureAtlasPosition pos;
    pos.height = 0;
    pos.width = 0;
    pos.x = 0;
    pos.y = 0;

    if (m_img_buffer == NULL) {
        getLogger().err(
            "%s: %s", __FUNCTION__,
            "Texture can't by copied to Texture Atlas! No texture given.");

        return pos;
    }

    int width_sum = width + repeatWidth;

    TextureSizeSlot slot = get_slot_by_height(height);

    if (slot == TextureSizeSlot::_TEXTURE_HEIGHT_COUNT) {
        getLogger().err(
            "%s: %s", __FUNCTION__,
            "Texture can't by copied to Texture Atlas! Texture height is out "
            "of range.");
        return pos;
    }

    if (width_sum > m_width) {
        getLogger().err(
            "%s: %s", __FUNCTION__,
            "Texture can't by copied to Texture Atlas! Texture width is to "
            "large!");
        return pos;
    }

    if ((m_slot_pos_x[(int)slot] + width_sum) > m_width) {
        if ((m_filled_y_pos + get_height_by_slot(slot)) > m_height) {
            getLogger().err(
                "%s: %s", __FUNCTION__,
                "Texture can't by copied to Texture Atlas! Texture Atlas is "
                "FULL!");
            return pos;
        }

        //- slot is full - create new!
        m_slot_pos_y[(int)slot] = m_filled_y_pos;
        m_slot_pos_x[(int)slot] = 0;
        m_filled_y_pos += get_height_by_slot(slot);
    }

    int outX = m_slot_pos_x[(int)slot];
    int outY = m_slot_pos_y[(int)slot];

    copy_image(m_img_buffer, data, outX, outY, width, height, m_width);

    //- repeat a part of the image at the end of the original image
    if (repeatWidth > 0) {
        copy_image(m_img_buffer, data, outX + width, outY, repeatWidth, height,
                   m_width, width);
    }

    //- move slot position
    m_slot_pos_x[(int)slot] += width_sum;

    pos.x = outX;
    pos.y = outY;
    pos.width = width;
    pos.height = height;

    //- update fill state
    m_fill_state += width_sum * height;

    return pos;
}
*/
//-------------------------------------//
void TextureAtlas::copy_image(unsigned int* dest, unsigned int* src, int posX,
                              int posY, int width, int height, int destWidth,
                              int srcWidth) {
    if (srcWidth < 0) srcWidth = width;

    unsigned int* destP = dest + destWidth * posY;

    for (int y = 0; y < height; y++) {
        unsigned int* curDestP = destP + destWidth * y + posX;
        unsigned int* srcP = src + (height - y - 1) * srcWidth;

        for (int x = width; x > 0; x--) {
            *curDestP = *srcP;
            curDestP++;
            srcP++;
        }
    }
}

//-------------------------------------//
GLuint TextureAtlas::create_gl_texture_atlas() {
    if (m_img_buffer == NULL) return m_gl_texture_ID;

    //- Create one OpenGL texture
    glGenTextures(1, &m_gl_texture_ID);

    //- "Bind" the newly created texture
    glBindTexture(GL_TEXTURE_2D, m_gl_texture_ID);

    //-	Give the image to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA,
                 GL_UNSIGNED_INT_8_8_8_8, m_img_buffer);

    auto err = glGetError();

    if (err != GL_NO_ERROR) {
        const char* GL_Error_text = "";
        switch (err) {
            case GL_INVALID_ENUM:
                GL_Error_text = "GL_INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                GL_Error_text = "GL_INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                GL_Error_text = "GL_INVALID_OPERATION";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                GL_Error_text = "GL_INVALID_VALUE";
                break;
            case GL_OUT_OF_MEMORY:
                GL_Error_text = "GL_OUT_OF_MEMORY";
                break;
            case GL_STACK_UNDERFLOW:
                GL_Error_text = "GL_STACK_UNDERFLOW";
                break;
            case GL_STACK_OVERFLOW:
                GL_Error_text = "GL_STACK_OVERFLOW";
                break;
        }

        getLogger().err("%s: error: %s", __FUNCTION__, GL_Error_text);
        TERMINATE();
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    getLogger().info("%s: fillstate: %f", __FUNCTION__,
                     (m_fill_state * 100.0) / (m_height * m_width));

    //- delete image buffer
    delete[] m_img_buffer;
    m_img_buffer = NULL;

    return m_gl_texture_ID;
}

int TextureAtlas::get_height() { return m_height; }

int TextureAtlas::get_width() { return m_width; }

}  // namespace OpenS4::Renderer
