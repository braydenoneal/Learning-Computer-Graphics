#include <stb_image.h>
#include <string>

class Texture {
public:
    unsigned int renderer_id{};
    unsigned char *local_buffer{};
    int width{0}, height{0}, bits_per_pixel{0};

    explicit Texture(const std::vector<std::string> &paths) {
        stbi_set_flip_vertically_on_load(1);

        glGenTextures(1, &renderer_id);
        glBindTexture(GL_TEXTURE_2D_ARRAY, renderer_id);

        glPixelStorei(GL_UNPACK_ROW_LENGTH, 16);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, 16, 16, (int) paths.size(), 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, nullptr);

        for (int i = 0; i < paths.size(); i++) {
            local_buffer = stbi_load(paths[i].c_str(), &width, &height, &bits_per_pixel, 4);
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, local_buffer);
        }

        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

        if (local_buffer) {
            stbi_image_free(local_buffer);
        }
    }

    ~Texture() {
        glDeleteTextures(1, &renderer_id);
    }

    void bind(unsigned int slot) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D_ARRAY, renderer_id);
    }

    static void unbind() {
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    }
};
