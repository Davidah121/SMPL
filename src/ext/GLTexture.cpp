#include "ext/GLTexture.h"

namespace glib
{

    GLTexture::GLTexture(File f, bool includeAlpha)
    {
        loadImage(f, includeAlpha);
    }

    GLTexture::GLTexture(Image* img, bool includeAlpha)
    {
        setImage(img, includeAlpha);
    }

    GLTexture::~GLTexture()
    {
        if(textureID != 0)
            glDeleteTextures(1, &textureID);

        textureID = 0;
    }

    void GLTexture::loadImage(File f, bool includeAlpha)
    {
        int imgSize = 0;
        Image** imgPointer = Image::loadImage(f, &imgSize);

        if(imgSize > 0)
        {
            setImage(imgPointer[0], includeAlpha);
        }

        for(int i=0; i<imgSize; i++)
        {
            delete imgPointer[i];
        }
        delete[] imgPointer;
    }

    void GLTexture::setImage(Image* img, bool includeAlpha)
    {
        if(img==nullptr)
            return;
        
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, edgeBehavior);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, edgeBehavior);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterType);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterType);
        

        if(includeAlpha)
        {
            int width = img->getWidth();
            int height = img->getHeight();
            Color* data = img->getPixels();

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        else
        {
            int width = img->getWidth();
            int height = img->getHeight();
            Color* data = img->getPixels();

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void GLTexture::bind(int location)
    {
        int textureLocation = MathExt::clamp(GL_TEXTURE0 + location, GL_TEXTURE0, GL_MAX_TEXTURE_UNITS);
        
        glActiveTexture(textureLocation);
        glBindTexture(GL_TEXTURE_2D, textureID);
    }

}