#pragma once
#include "ext/GLSingleton.h"
#include "Image.h"

namespace glib
{

    struct GLTextureParams
    {
        int edgeBehavior = GL_CLAMP_TO_EDGE;
        int filterType = GL_LINEAR;
    };

    class GLTexture
    {
    public:
        static const bool TEXTURE2D = false;
        static const bool TEXTURE3D = true;

        static const int BEHAVIOR_CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE;
        static const int BEHAVIOR_CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER;
        
        static const int FILTER_NEAREST = GL_NEAREST;
        static const int FILTER_LINEAR = GL_LINEAR;
        
        GLTexture(File f, bool includeAlpha=true, GLTextureParams params = {BEHAVIOR_CLAMP_TO_EDGE, FILTER_LINEAR});
        GLTexture(Image* img, bool includeAlpha=true, GLTextureParams params = {BEHAVIOR_CLAMP_TO_EDGE, FILTER_LINEAR});
        ~GLTexture();

        void loadImage(File f, bool includeAlpha=true, GLTextureParams params = {BEHAVIOR_CLAMP_TO_EDGE, FILTER_LINEAR});
        void setImage(Image* img, bool includeAlpha=true, GLTextureParams params = {BEHAVIOR_CLAMP_TO_EDGE, FILTER_LINEAR});

        void bind(int location = 0);

        int getWidth();
        int getHeight();

    private:
        int edgeBehavior = GL_CLAMP_TO_EDGE;
        int filterType = GL_LINEAR;

        unsigned int textureID = 0;
        bool textureType = TEXTURE2D;

        int width = 0;
        int height = 0;
    };

} // namespace glib

