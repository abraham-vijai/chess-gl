#ifndef TEXTURE_H
#define TEXTURE_H
#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <iostream>
#include <string>
#include "stb_image.h"

class Texture
{
public:
    unsigned int texture;
    int width, height, nrChannels;

    Texture(const char *filename, unsigned int type, GLenum colorChannel, GLenum sWrap, GLenum tWrap)
    {
        glGenTextures(1, &texture);
        glBindTexture(type, texture);
        // set the texture wrapping parameters
        glTexParameteri(type, GL_TEXTURE_WRAP_S, sWrap); // set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(type, GL_TEXTURE_WRAP_T, tWrap);
        // set texture filtering parameters
        glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // load image, create texture and generate mipmaps
        stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.

        unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
        if (data)
        {
            // Determine the format based on the number of channels
            GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
            glTexImage2D(type, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(type);
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }

        glActiveTexture(0);
        glBindTexture(type, 0);

        stbi_image_free(data);
    }

    GLuint getID()
    {
        return texture;
    }

    void bind(GLint textureUnit, GLenum type)
    {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(type, texture);
    }

    void unbind(GLenum type)
    {
        glActiveTexture(0);
        glBindTexture(type, 0);
    }

    ~Texture()
    {
        glDeleteTextures(1, &texture);
    }
};

#endif
