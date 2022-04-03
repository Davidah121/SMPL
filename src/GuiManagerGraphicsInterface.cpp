#include "GuiManager.h"
#include "ext/GLGraphics.h"

namespace glib
{
    #pragma region GRAPHICS_STUFF

    GuiSurfaceInterface* GuiGraphicsInterface::createSurface(int width, int height)
    {
        if(type == TYPE_SOFTWARE)
            return GuiSurfaceInterface::createSoftwareSurface(width, height);
        else if(type == TYPE_OPENGL)
            return GuiSurfaceInterface::createGLSurface(width, height);
        return nullptr;
    }

    GuiImageInterface* GuiGraphicsInterface::createImage(File f)
    {
        if(type == TYPE_SOFTWARE)
            return GuiImageInterface::createSoftwareImage(f);
        else if(type == TYPE_OPENGL)
            return GuiImageInterface::createGLImage(f);
        return nullptr;
    }

    GuiSpriteInterface* GuiGraphicsInterface::createSprite(File f)
    {
        if(type == TYPE_SOFTWARE)
            return GuiSpriteInterface::createSoftwareSprite(f);
        else if(type == TYPE_OPENGL)
            return GuiSpriteInterface::createGLSprite(f);
        return nullptr;
    }

    GuiFontInterface* GuiGraphicsInterface::createFont(File f)
    {
        if(type == TYPE_SOFTWARE)
            return GuiFontInterface::createSoftwareFont(f);
        else if(type == TYPE_OPENGL)
            return GuiFontInterface::createGLFont(f);
        return nullptr;
    }


    GuiGraphicsInterface::GuiGraphicsInterface()
    {

    }

    GuiGraphicsInterface::GuiGraphicsInterface(unsigned char v)
    {
        type = MathExt::clamp<unsigned char>(v, 0, 1);
    }
    
    GuiGraphicsInterface::~GuiGraphicsInterface()
    {
        type = GuiGraphicsInterface::TYPE_INVALID;
    }

    unsigned char GuiGraphicsInterface::getType()
    {
        return type;
    }

    void GuiGraphicsInterface::setBoundSurface(GuiSurfaceInterface* surface)
    {
        boundSurface = surface;
    }

    GuiSurfaceInterface* GuiGraphicsInterface::getBoundSurface()
    {
        return boundSurface;
    }
    
    void GuiGraphicsInterface::setColor(Vec4f v)
    {
        if(type == TYPE_SOFTWARE)
        {
            Color c;
            c.red = (unsigned char)MathExt::clamp(v.x*255.0, 0.0, 255.0);
            c.green = (unsigned char)MathExt::clamp(v.y*255.0, 0.0, 255.0);
            c.blue = (unsigned char)MathExt::clamp(v.z*255.0, 0.0, 255.0);
            c.alpha = (unsigned char)MathExt::clamp(v.w*255.0, 0.0, 255.0);

            SimpleGraphics::setColor(c);
        }
        else if(type == TYPE_OPENGL)
        {
            GLGraphics::setDrawColor(v);
            GLGraphics::setClearColor(v);
        }
    }
    
    void GuiGraphicsInterface::setColor(Color c)
    {
        if(type == TYPE_SOFTWARE)
        {
            SimpleGraphics::setColor(c);
        }
        else if(type == TYPE_OPENGL)
        {
            Vec4f v = Vec4f( (double)c.red / 255.0, (double)c.green / 255.0, (double)c.blue / 255.0, (double)c.alpha / 255.0);

            GLGraphics::setDrawColor(v);
            GLGraphics::setClearColor(v);
        }
    }

    Color GuiGraphicsInterface::getColor()
    {
        if(type == TYPE_SOFTWARE)
        {
            return SimpleGraphics::getColor();
        }
        else if(type == TYPE_OPENGL)
        {
            Vec4f v = GLGraphics::getDrawColor();

            Color c;
            c.red = (unsigned char)MathExt::clamp(v.x*255.0, 0.0, 255.0);
            c.green = (unsigned char)MathExt::clamp(v.y*255.0, 0.0, 255.0);
            c.blue = (unsigned char)MathExt::clamp(v.z*255.0, 0.0, 255.0);
            c.alpha = (unsigned char)MathExt::clamp(v.w*255.0, 0.0, 255.0);

            return c;
        }

        return Color();
    }

    Vec4f GuiGraphicsInterface::getColorVec4f()
    {
        if(type == TYPE_SOFTWARE)
        {
            Color c = SimpleGraphics::getColor();
            Vec4f v = Vec4f( (double)c.red / 255.0, (double)c.green / 255.0, (double)c.blue / 255.0, (double)c.alpha / 255.0);

            return v;
        }
        else if(type == TYPE_OPENGL)
        {
            Vec4f v = GLGraphics::getDrawColor();
            
            return v;
        }
        
        return Vec4f();
    }


    void GuiGraphicsInterface::setFont(GuiFontInterface* f)
    {
        if(f->getType() != getType())
            return; //Should throw an error
        
        if(getType() == TYPE_SOFTWARE)
        {
            SimpleGraphics::setFont( (BitmapFont*)f->getFont() );
        }
        else if(getType() == TYPE_OPENGL)
        {
            GLGraphics::setFont( (GLFont*)f->getFont() );
        }
    }

    GuiFontInterface* GuiGraphicsInterface::getFont()
    {
        Font* f;
        if(getType() == TYPE_SOFTWARE)
        {
            f = SimpleGraphics::getFont();
        }
        else if(getType() == TYPE_OPENGL)
        {
            f = GLGraphics::getFont();
        }

        return GuiFontInterface::createFromFont(f, getType());
    }
    
    void GuiGraphicsInterface::clear()
    {
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }

        if(getType() == TYPE_SOFTWARE && boundSurface != nullptr)
        {
            SimpleGraphics::clearImage( (Image*)boundSurface->getSurface() );
        }
        else
        {
            if(boundSurface->getSurface() != nullptr)
            {
                ((GLSurface*)boundSurface->getSurface())->bind();
            }
            GLGraphics::clear(GLGraphics::COLOR_BUFFER | GLGraphics::DEPTH_BUFFER);
        }
    }

    void GuiGraphicsInterface::drawRect(int x, int y, int x2, int y2, bool outline)
    {
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }

        if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            SimpleGraphics::drawRect(x, y, x2, y2, outline, (Image*)boundSurface->getSurface());
        }
        else
        {
            if(boundSurface->getSurface() != nullptr)
            {
                ((GLSurface*)boundSurface->getSurface())->bind();
            }
            GLGraphics::drawRectangle(x, y, x2, y2, outline);
        }
    }

    void GuiGraphicsInterface::drawLine(int x, int y, int x2, int y2)
    {
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }

        if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            SimpleGraphics::drawLine( x, y, x2, y2, (Image*)boundSurface->getSurface() );
        }
        else
        {
            if(boundSurface->getSurface() != nullptr)
            {
                ((GLSurface*)boundSurface->getSurface())->bind();
            }
            GLGraphics::drawLine(x, y, x2, y2);
        }
    }

    void GuiGraphicsInterface::drawCircle(int x, int y, int radius, bool outline)
    {
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }

        if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            SimpleGraphics::drawCircle(x, y, radius, outline, (Image*)boundSurface->getSurface() );
        }
        else
        {
            if(boundSurface->getSurface() != nullptr)
            {
                ((GLSurface*)boundSurface->getSurface())->bind();
            }
            GLGraphics::drawCircle(x, y, radius, outline);
        }
    }

    void GuiGraphicsInterface::drawSprite(GuiImageInterface* img, int x, int y)
    {
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        if(img != nullptr)
        {
            if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE)
            {
                Image* imgData = (Image*)img->getImage();
                SimpleGraphics::drawSprite(imgData, x, y, (Image*)boundSurface->getSurface());
            }
            else
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((GLSurface*)boundSurface->getSurface())->bind();
                }
                GLTexture* imgData = (GLTexture*)img->getImage();
                GLGraphics::drawTexture(x, y, imgData);
            }
        }
    }

    void GuiGraphicsInterface::drawSprite(GuiImageInterface* img, int x1, int y1, int x2, int y2)
    {
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        if(img != nullptr)
        {
            if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE)
            {
                Image* imgData = (Image*)img->getImage();
                SimpleGraphics::drawSprite(imgData, x1, y1, x2, y2, (Image*)boundSurface->getSurface());
            }
            else
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((GLSurface*)boundSurface->getSurface())->bind();
                }
                GLTexture* imgData = (GLTexture*)img->getImage();
                GLGraphics::drawTexture(x1, y1, x2, y2, imgData);
            }
        }
    }

    void GuiGraphicsInterface::drawSpritePart(GuiImageInterface* img, int x, int y, int imgX, int imgY, int imgW, int imgH)
    {
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        if(img != nullptr)
        {
            if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE)
            {
                Image* imgData = (Image*)img->getImage();
                SimpleGraphics::drawSpritePart(imgData, x, y, imgX, imgY, imgW, imgH, (Image*)boundSurface->getSurface());
            }
            else
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((GLSurface*)boundSurface->getSurface())->bind();
                }
                GLTexture* imgData = (GLTexture*)img->getImage();
                Vec4f positionData = Vec4f(x, y, x+imgW, y+imgH);
                Vec4f textureData = Vec4f(imgX, imgY, imgX+imgW, imgY+imgH);
                textureData.x /= imgData->getWidth();
                textureData.y /= imgData->getHeight();
                textureData.z /= imgData->getWidth();
                textureData.w /= imgData->getHeight();
                
                GLGraphics::drawTexturePart(positionData, textureData, imgData);
            }
        }
    }

    void GuiGraphicsInterface::drawText(std::string str, int x, int y)
    {
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            SimpleGraphics::drawText(str, x, y, (Image*)boundSurface->getSurface());
        }
        else
        {
            if(boundSurface->getSurface() != nullptr)
            {
                ((GLSurface*)boundSurface->getSurface())->bind();
            }
            GLGraphics::drawText(str, x, y);
        }
    }

    void GuiGraphicsInterface::drawText(std::wstring str, int x, int y)
    {
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            SimpleGraphics::drawText(str, x, y, (Image*)boundSurface->getSurface());
        }
        else
        {
            if(boundSurface->getSurface() != nullptr)
            {
                ((GLSurface*)boundSurface->getSurface())->bind();
            }
            GLGraphics::drawText(str, x, y);
        }
    }

    void GuiGraphicsInterface::drawTextLimits(std::wstring str, int x, int y, int maxWidth, int maxHeight, bool useLineBreak)
    {
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            SimpleGraphics::drawTextLimits(str, x, y, maxWidth, maxHeight, useLineBreak, (Image*)boundSurface->getSurface());
        }
        else
        {
            if(boundSurface->getSurface() != nullptr)
            {
                ((GLSurface*)boundSurface->getSurface())->bind();
            }
            GLGraphics::drawTextLimits(str, x, y, maxWidth, maxHeight, useLineBreak);
        }
    }

    void GuiGraphicsInterface::drawTextLimits(std::string str, int x, int y, int maxWidth, int maxHeight, bool useLineBreak)
    {
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            SimpleGraphics::drawTextLimits(str, x, y, maxWidth, maxHeight, useLineBreak, (Image*)boundSurface->getSurface());
        }
        else
        {
            if(boundSurface->getSurface() != nullptr)
            {
                ((GLSurface*)boundSurface->getSurface())->bind();
            }
            GLGraphics::drawTextLimits(str, x, y, maxWidth, maxHeight, useLineBreak);
        }
    }
    
    void GuiGraphicsInterface::drawTextLimitsHighlighted(std::wstring str, int x, int y, int maxWidth, int maxHeight, bool useLineBreak, int highlightStart, int highlightEnd, Color highlightColor)
    {
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            SimpleGraphics::drawTextLimitsHighlighted(str, x, y, maxWidth, maxHeight, useLineBreak, highlightStart, highlightEnd, highlightColor, (Image*)boundSurface->getSurface());
        }
        else
        {
            if(boundSurface->getSurface() != nullptr)
            {
                ((GLSurface*)boundSurface->getSurface())->bind();
            }
            Vec4f vColor = SimpleGraphics::convertColorToVec4f(highlightColor);
            GLGraphics::drawTextLimitsHighlighted(str, x, y, maxWidth, maxHeight, useLineBreak, highlightStart, highlightEnd, vColor);
        }
    }

    void GuiGraphicsInterface::drawTextLimitsHighlighted(std::string str, int x, int y, int maxWidth, int maxHeight, bool useLineBreak, int highlightStart, int highlightEnd, Color highlightColor)
    {
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            SimpleGraphics::drawTextLimitsHighlighted(str, x, y, maxWidth, maxHeight, useLineBreak, highlightStart, highlightEnd, highlightColor, (Image*)boundSurface->getSurface());
        }
        else
        {
            if(boundSurface->getSurface() != nullptr)
            {
                ((GLSurface*)boundSurface->getSurface())->bind();
            }
            Vec4f vColor = SimpleGraphics::convertColorToVec4f(highlightColor);
            GLGraphics::drawTextLimitsHighlighted(str, x, y, maxWidth, maxHeight, useLineBreak, highlightStart, highlightEnd, vColor);
        }
    }
    
    void GuiGraphicsInterface::drawTextLimitsHighlighted(std::wstring str, int x, int y, int maxWidth, int maxHeight, bool useLineBreak, int highlightStart, int highlightEnd, Vec4f highlightColor)
    {
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            Color cColor = SimpleGraphics::convertVec4fToColor(highlightColor);
            SimpleGraphics::drawTextLimitsHighlighted(str, x, y, maxWidth, maxHeight, useLineBreak, highlightStart, highlightEnd, cColor, (Image*)boundSurface->getSurface());
        }
        else
        {
            if(boundSurface->getSurface() != nullptr)
            {
                ((GLSurface*)boundSurface->getSurface())->bind();
            }
            GLGraphics::drawTextLimitsHighlighted(str, x, y, maxWidth, maxHeight, useLineBreak, highlightStart, highlightEnd, highlightColor);
        }
    }

    void GuiGraphicsInterface::drawTextLimitsHighlighted(std::string str, int x, int y, int maxWidth, int maxHeight, bool useLineBreak, int highlightStart, int highlightEnd, Vec4f highlightColor)
    {
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            Color cColor = SimpleGraphics::convertVec4fToColor(highlightColor);
            SimpleGraphics::drawTextLimitsHighlighted(str, x, y, maxWidth, maxHeight, useLineBreak, highlightStart, highlightEnd, cColor, (Image*)boundSurface->getSurface());
        }
        else
        {
            if(boundSurface->getSurface() != nullptr)
            {
                ((GLSurface*)boundSurface->getSurface())->bind();
            }
            GLGraphics::drawTextLimitsHighlighted(str, x, y, maxWidth, maxHeight, useLineBreak, highlightStart, highlightEnd, highlightColor);
        }
    }

    void GuiGraphicsInterface::setClippingRect(Box2D b)
    {
        clippingRect = b;
        if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            SimpleGraphics::setClippingRect(b);
        }
        else
        {
            //Set the ortho graphic projection matrix using this
            int x = (int)b.getLeftBound();
            int y = (int)b.getTopBound();
            int wid = (int)b.getWidth();
            int hei = (int)b.getHeight();
            GLGraphics::setClippingRectangle(x, y, wid, hei);
        }
    }

    Box2D GuiGraphicsInterface::getClippingRect()
    {
        return clippingRect;
    }

    void GuiGraphicsInterface::resetClippingPlane()
    {
        clippingRect = Box2D(0, 0, 65535, 65535);
        if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            SimpleGraphics::resetClippingPlane();
        }
        else
        {
            GLGraphics::resetClippingRectangle();
        }
    }

    void GuiGraphicsInterface::drawSurface(GuiSurfaceInterface* img, int x, int y)
    {
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            if(img != nullptr)
            {
                SimpleGraphics::drawSprite((Image*)img->getSurface(), x, y, (Image*)boundSurface->getSurface());
            }
        }
        else
        {
            if(boundSurface->getSurface() != nullptr)
            {
                ((GLSurface*)boundSurface->getSurface())->bind();
            }
            if(img != nullptr)
            {
                GLSurface* tempSurf = (GLSurface*)boundSurface->getSurface();
                if(tempSurf != nullptr)
                {
                    GLGraphics::drawSurface(x, y, x+tempSurf->getWidth(), y+tempSurf->getHeight(), tempSurf);
                }
            }
        }
    }
    void GuiGraphicsInterface::drawSurface(GuiSurfaceInterface* img, int x1, int y1, int x2, int y2)
    {
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            if(img != nullptr)
            {
                SimpleGraphics::drawSprite((Image*)img->getSurface(), x1, y1, x2, y2, (Image*)boundSurface->getSurface());
            }
        }
        else
        {
            if(boundSurface->getSurface() != nullptr)
            {
                ((GLSurface*)boundSurface->getSurface())->bind();
            }
            if(img != nullptr)
            {
                GLSurface* tempSurf = (GLSurface*)img->getSurface();
                GLGraphics::drawSurface(x1, y1, x2, y2, tempSurf);
            }
        }
    }

    void GuiGraphicsInterface::drawToScreen()
    {
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
        }
        else
        {
            if(boundSurface->getSurface() != nullptr)
            {
                ((GLSurface*)boundSurface->getSurface())->unbind();
                int width = boundSurface->getWidth();
                int height = boundSurface->getHeight();
                GLGraphics::drawSurface(0, 0, width, height, (GLSurface*)boundSurface->getSurface());
            }
        }
    }

    void GuiGraphicsInterface::setOrthoProjection(int width, int height)
    {
        if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
        }
        else
        {
            GLGraphics::setOrthoProjection(width, height);
        }
    }

    void GuiGraphicsInterface::setProjection(Mat4f proj)
    {
        if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
        }
        else
        {
            GLGraphics::setProjection(proj);
        }
    }

    #pragma endregion


    GuiSurfaceInterface* GuiSurfaceInterface::createSoftwareSurface(int width, int height)
    {
        return new GuiSurfaceInterface(width, height, GuiGraphicsInterface::TYPE_SOFTWARE);
    }

    GuiSurfaceInterface* GuiSurfaceInterface::createGLSurface(int width, int height)
    {
        return new GuiSurfaceInterface(width, height, GuiGraphicsInterface::TYPE_OPENGL);
    }

    GuiSurfaceInterface::GuiSurfaceInterface(int width, int height, unsigned char type)
    {
        if(type == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            surface = new Image(width, height);
        }
        else if(type == GuiGraphicsInterface::TYPE_OPENGL)
        {
            surface = new GLSurface(width, height);
        }
        this->type = type;
    }

    GuiSurfaceInterface::GuiSurfaceInterface(const GuiSurfaceInterface& other)
    {
        type = other.type;
        surface = other.surface;
    }

    void GuiSurfaceInterface::operator=(const GuiSurfaceInterface& other)
    {
        type = other.type;
        surface = other.surface;
    }
    
    GuiSurfaceInterface::~GuiSurfaceInterface()
    {
        if(surface != nullptr)
        {
            if(type == GuiGraphicsInterface::TYPE_SOFTWARE)
                delete ((Image*)surface);
            else if(type == GuiGraphicsInterface::TYPE_OPENGL)
                delete ((GLSurface*)surface);
        }
        
        surface = nullptr;
    }
    
	int GuiSurfaceInterface::getType()
    {
        return type;
    }

    int GuiSurfaceInterface::getWidth()
    {
        if(surface != nullptr)
        {
            if(type == GuiGraphicsInterface::TYPE_SOFTWARE)
                return ((Image*)surface)->getWidth();
            else if(type == GuiGraphicsInterface::TYPE_OPENGL)
                return ((GLSurface*)surface)->getWidth();
        }
        return 0;
    }

    int GuiSurfaceInterface::getHeight()
    {
        if(surface != nullptr)
        {
            if(type == GuiGraphicsInterface::TYPE_SOFTWARE)
                return ((Image*)surface)->getHeight();
            else if(type == GuiGraphicsInterface::TYPE_OPENGL)
                return ((GLSurface*)surface)->getHeight();
        }
        return 0;
    }

    void* GuiSurfaceInterface::getSurface()
    {
        return surface;
    }

    GuiImageInterface* GuiImageInterface::createSoftwareImage(File f)
    {
        return new GuiImageInterface(f, GuiGraphicsInterface::TYPE_SOFTWARE);
    }

    GuiImageInterface* GuiImageInterface::createGLImage(File f)
    {
        return new GuiImageInterface(f, GuiGraphicsInterface::TYPE_OPENGL);
    }

    GuiImageInterface::GuiImageInterface(File file, unsigned char type)
    {
        if(type == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            int amountImg = 0;
            Image** loadedImgArray = Image::loadImage(file, &amountImg);

            if(amountImg > 0)
            {
                image = loadedImgArray[0];
            }

            delete loadedImgArray;
        }
        else if(type == GuiGraphicsInterface::TYPE_OPENGL)
        {
            image = new GLTexture(file);
        }
        this->type = type;
    }
    
	GuiImageInterface::GuiImageInterface()
    {
    }

    GuiImageInterface::GuiImageInterface(const GuiImageInterface& other)
    {
        type = other.type;
        image = other.image;
    }
    
    void GuiImageInterface::operator=(const GuiImageInterface& other)
    {
        type = other.type;
        image = other.image;
    }
    
    GuiImageInterface::~GuiImageInterface()
    {
        if(image != nullptr)
        {
            if(type == GuiGraphicsInterface::TYPE_SOFTWARE)
                delete (Image*)image;
            else if(type == GuiGraphicsInterface::TYPE_OPENGL)
                delete (GLTexture*)image;
        }
        
        image = nullptr;
    }

    void* GuiImageInterface::getImage()
    {
        return image;
    }

    int GuiImageInterface::getType()
    {
        return type;
    }

    int GuiImageInterface::getWidth()
    {
        if(type == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            return ((Image*)image)->getWidth();
        }
        else if(type == GuiGraphicsInterface::TYPE_OPENGL)
        {
            return ((GLTexture*)image)->getWidth();
        }
        return 0;
    }
    int GuiImageInterface::getHeight()
    {
        if(type == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            return ((Image*)image)->getHeight();
        }
        else if(type == GuiGraphicsInterface::TYPE_OPENGL)
        {
            return ((GLTexture*)image)->getHeight();
        }
        return 0;
    }

    GuiSpriteInterface* GuiSpriteInterface::createSoftwareSprite(File f)
    {
        return new GuiSpriteInterface(f, GuiGraphicsInterface::TYPE_SOFTWARE);
    }

    GuiSpriteInterface* GuiSpriteInterface::createGLSprite(File f)
    {
        return new GuiSpriteInterface(f, GuiGraphicsInterface::TYPE_OPENGL);
    }

    GuiSpriteInterface::GuiSpriteInterface(File file, unsigned char type)
    {
        if(type == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            sprite = new Sprite();
            ((Sprite*)sprite)->loadImage(file);
        }
        else if(type == GuiGraphicsInterface::TYPE_OPENGL)
        {
            sprite = new GLSprite();
            ((GLSprite*)sprite)->loadImage(file);
        }
        this->type = type;
    }

    GuiSpriteInterface::~GuiSpriteInterface()
    {
        if(sprite != nullptr)
        {
            if(type == GuiGraphicsInterface::TYPE_SOFTWARE)
                delete (Sprite*)sprite;
            else if(type == GuiGraphicsInterface::TYPE_OPENGL)
                delete (GLSprite*)sprite;
        }
        
        sprite = nullptr;
    }

    void* GuiSpriteInterface::getSprite()
    {
        return sprite;
    }

    int GuiSpriteInterface::getType()
    {
        return type;
    }

    GuiImageInterface* GuiSpriteInterface::getImage(int index)
    {
        if(type == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            GuiImageInterface* result = new GuiImageInterface();
            result->type = GuiGraphicsInterface::TYPE_SOFTWARE;
            result->image = ((Sprite*)sprite)->getImage(index);
            return result;
        }
        else if(type == GuiGraphicsInterface::TYPE_OPENGL)
        {
            GuiImageInterface* result = new GuiImageInterface();
            result->type = GuiGraphicsInterface::TYPE_OPENGL;
            result->image = ((GLSprite*)sprite)->getTexture(index);
            return result;
        }

        return nullptr;
    }

    int GuiSpriteInterface::getDelayTime(int index)
    {
        if(type == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            return ((Sprite*)sprite)->getDelayTime(index);
        }
        else if(type == GuiGraphicsInterface::TYPE_OPENGL)
        {
            return ((GLSprite*)sprite)->getDelayTime(index);
        }
        return -1;
    }

    int GuiSpriteInterface::getSize()
    {
        if(type == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            return ((Sprite*)sprite)->getSize();
        }
        else if(type == GuiGraphicsInterface::TYPE_OPENGL)
        {
            return ((GLSprite*)sprite)->getSize();
        }
        return 0;
    }
    
    bool GuiSpriteInterface::shouldLoop()
    {
        if(type == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            return ((Sprite*)sprite)->shouldLoop();
        }
        else if(type == GuiGraphicsInterface::TYPE_OPENGL)
        {
            return ((GLSprite*)sprite)->shouldLoop();
        }
        return false;
    }


    GuiFontInterface* GuiFontInterface::createSoftwareFont(File f)
    {
        return new GuiFontInterface(f, GuiGraphicsInterface::TYPE_SOFTWARE);
    }

    GuiFontInterface* GuiFontInterface::createGLFont(File f)
    {
        return new GuiFontInterface(f, GuiGraphicsInterface::TYPE_OPENGL);
    }

    
	GuiFontInterface* GuiFontInterface::createFromFont(Font* f, unsigned char type)
    {
        return new GuiFontInterface(f, type);
    }

    GuiFontInterface::GuiFontInterface(File file, unsigned char type)
    {
        if(type == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            font = new BitmapFont(file);
        }
        else if(type == GuiGraphicsInterface::TYPE_OPENGL)
        {
            font = new GLFont(file);
        }
        this->type = type;
    }

    GuiFontInterface::GuiFontInterface(Font* f, unsigned char type)
    {
        font = f;
        this->type = type;
    }

    GuiFontInterface::GuiFontInterface(const GuiFontInterface& other)
    {
        font = other.font;
        type = other.type;
    }
    void GuiFontInterface::operator=(const GuiFontInterface& other)
    {
        font = other.font;
        type = other.type;
    }
    
    GuiFontInterface::~GuiFontInterface()
    {
        if(font != nullptr)
        {
            if(type == GuiGraphicsInterface::TYPE_SOFTWARE)
                delete (BitmapFont*)font;
            else if(type == GuiGraphicsInterface::TYPE_OPENGL)
                delete (GLFont*)font;
        }
        
        font = nullptr;
    }
    
    Font* GuiFontInterface::getFont()
    {
        return font;
    }

    int GuiFontInterface::getType()
    {
        return type;
    }
};