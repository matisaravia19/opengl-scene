#include "texture.h"

#include "stb_image.h"
#include "glad/gl.h"
#include "../core/constants.h"

Texture::~Texture() {
    glDeleteTextures(1, &id);
}

void Texture::bind(unsigned int slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, id);
}

ColorTexture::ColorTexture(glm::vec4 color) {
    this->color = color;
}

ColorTexture::ColorTexture(glm::vec3 color) {
    this->color = glm::vec4(color, 1);
}

ColorTexture::ColorTexture(float intensity) {
    this->color = glm::vec4(intensity, intensity, intensity, 1);
}

void ColorTexture::upload() {
    if (id != 0) {
        return;
    }

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    unsigned char data[4] = {
            static_cast<unsigned char>(color.r * 255),
            static_cast<unsigned char>(color.g * 255),
            static_cast<unsigned char>(color.b * 255),
            static_cast<unsigned char>(color.a * 255)
    };

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

ImageTexture::ImageTexture(const std::string &path) {
    this->path = RESOURCES_PATH / path;
}

void ImageTexture::upload() {
    if (id != 0) {
        return;
    }

    // Load image
    int width, height, channels;
    unsigned char *data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);

    if (!data) {
        throw std::runtime_error("Failed to load texture: " + path.string());
    }

    // Upload image
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    if (channels == 3) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    } else if (channels == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    } else {
        throw std::runtime_error("Unsupported number of channels: " + std::to_string(channels));
    }

    // Generate mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Free image
    stbi_image_free(data);
}
