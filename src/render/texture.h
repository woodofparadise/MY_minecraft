#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "../utils/stb_image.h"


class Texture
{
    public:
        unsigned int TextureID;
        bool isInit = false;
        Texture()
        {

        }

        Texture(char const * path)
        {
            load_texture(path);
        }

        void load_texture(char const * path);

        void clear()
        {
            glDeleteTextures(1, &TextureID);
        }
};

#endif
