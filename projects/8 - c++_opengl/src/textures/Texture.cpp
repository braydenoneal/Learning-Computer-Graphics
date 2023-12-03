#include <glad/glad.h>
#include <stb_image.h>
#include <vector>

#include "Texture.h"

Texture::Texture(const std::vector<std::string> &paths) :
        m_RendererID(0),
        m_LocalBuffer(nullptr),
        m_Width(0),
        m_Height(0),
        m_BPP(0) {
    stbi_set_flip_vertically_on_load(1);

    glGenTextures(1, &m_RendererID);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID);

    glPixelStorei(GL_UNPACK_ROW_LENGTH, 16);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, 16, 16, (int) paths.size(), 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, nullptr);

    for (int i = 0; i < paths.size(); i++) {
        m_LocalBuffer = stbi_load(paths[i].c_str(), &m_Width, &m_Height, &m_BPP, 4);
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, m_Width, m_Height, 1, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer);
    }

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    if (m_LocalBuffer) {
        stbi_image_free(m_LocalBuffer);
    }
}

Texture::~Texture() {
    glDeleteTextures(1, &m_RendererID);
}

void Texture::Bind(unsigned int slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID);
}

void Texture::Unbind() const {
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}
