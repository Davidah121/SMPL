#pragma once
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "Image.h"

namespace glib
{

    class GLTexture
    {
    public:
        static const bool TEXTURE2D = false;
        static const bool TEXTURE3D = true;
        
        GLTexture(File f, bool includeAlpha=true);
        GLTexture(Image* img, bool includeAlpha=true);
        ~GLTexture();

        void loadImage(File f, bool includeAlpha=true);
        void setImage(Image* img, bool includeAlpha=true);

        void bind(int location = 0);

    private:
        int edgeBehavior = GL_CLAMP_TO_EDGE;
        int filterType = GL_NEAREST;

        unsigned int textureID = 0;
        bool textureType = TEXTURE2D;
    };

} // namespace glib

