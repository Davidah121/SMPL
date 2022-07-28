#include "GuiGraphics.h"
#include "SimpleGraphics.h"
#include "BitmapFont.h"
#include "ext/GLGraphics.h"

namespace glib
{
    #pragma region GRAPHICS_STUFF

    GuiSurfaceInterface* GuiGraphicsInterface::createSurface(int width, int height)
    {
        if(type == TYPE_SOFTWARE)
            return GuiSurfaceInterface::createSoftwareSurface(width, height);
        #ifdef USE_OPENGL
            if(type == TYPE_OPENGL)
                return GuiSurfaceInterface::createGLSurface(width, height);
        #endif
        return nullptr;
    }

    GuiImageInterface* GuiGraphicsInterface::createImage(File f)
    {
        if(type == TYPE_SOFTWARE)
            return GuiImageInterface::createSoftwareImage(f);
        #ifdef USE_OPENGL
            if(type == TYPE_OPENGL)
                return GuiImageInterface::createGLImage(f);
        #endif
        return nullptr;
    }

    GuiSpriteInterface* GuiGraphicsInterface::createSprite(File f)
    {
        if(type == TYPE_SOFTWARE)
            return GuiSpriteInterface::createSoftwareSprite(f);
        #ifdef USE_OPENGL
            if(type == TYPE_OPENGL)
                return GuiSpriteInterface::createGLSprite(f);
        #endif
        return nullptr;
    }

    GuiFontInterface* GuiGraphicsInterface::createFont(File f)
    {
        if(type == TYPE_SOFTWARE)
            return GuiFontInterface::createSoftwareFont(f);
        #ifdef USE_OPENGL
            if(type == TYPE_OPENGL)
                return GuiFontInterface::createGLFont(f);
        #endif
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
        #ifdef USE_OPENGL
            if(type == TYPE_OPENGL)
            {
                GLGraphics::setDrawColor(v);
                GLGraphics::setClearColor(v);
            }
        #endif
    }
    
    void GuiGraphicsInterface::setColor(Color c)
    {
        if(type == TYPE_SOFTWARE)
        {
            SimpleGraphics::setColor(c);
        }
        
        #ifdef USE_OPENGL
            if(type == TYPE_OPENGL)
            {
                Vec4f v = Vec4f( (double)c.red / 255.0, (double)c.green / 255.0, (double)c.blue / 255.0, (double)c.alpha / 255.0);

                GLGraphics::setDrawColor(v);
                GLGraphics::setClearColor(v);
            }
        #endif
    }

    Color GuiGraphicsInterface::getColor()
    {
        if(type == TYPE_SOFTWARE)
        {
            return SimpleGraphics::getColor();
        }
        
        #ifdef USE_OPENGL
            if(type == TYPE_OPENGL)
            {
                Vec4f v = GLGraphics::getDrawColor();

                Color c;
                c.red = (unsigned char)MathExt::clamp(v.x*255.0, 0.0, 255.0);
                c.green = (unsigned char)MathExt::clamp(v.y*255.0, 0.0, 255.0);
                c.blue = (unsigned char)MathExt::clamp(v.z*255.0, 0.0, 255.0);
                c.alpha = (unsigned char)MathExt::clamp(v.w*255.0, 0.0, 255.0);

                return c;
            }
        #endif

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
        
        #ifdef USE_OPENGL
            if(type == TYPE_OPENGL)
            {
                Vec4f v = GLGraphics::getDrawColor();
                
                return v;
            }
        #endif
        
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
        
        #ifdef USE_OPENGL
            if(getType() == TYPE_OPENGL)
            {
                GLGraphics::setFont( (GLFont*)f->getFont() );
            }
        #endif
    }

    GuiFontInterface* GuiGraphicsInterface::getFont()
    {
        Font* f;
        if(getType() == TYPE_SOFTWARE)
        {
            f = SimpleGraphics::getFont();
        }
        
        #ifdef USE_OPENGL
            if(getType() == TYPE_OPENGL)
            {
                f = GLGraphics::getFont();
            }
        #endif

        return GuiFontInterface::createFromFont(f, getType());
    }
    
    void GuiGraphicsInterface::clear()
    {
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }

        if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE && boundSurface != nullptr)
        {
            SimpleGraphics::clearImage( (Image*)boundSurface->getSurface() );
        }
        
        #ifdef USE_OPENGL
            if(getType() == GuiGraphicsInterface::TYPE_OPENGL)
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((GLSurface*)boundSurface->getSurface())->bind();
                }
                GLGraphics::clear(GLGraphics::COLOR_BUFFER | GLGraphics::DEPTH_BUFFER);
            }
        #endif
    }

    void GuiGraphicsInterface::drawRect(int x, int y, int x2, int y2, bool outline)
    {
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        int nx, ny, nx2, ny2;
        if(useScaling)
        {
            nx = x*scalingFactor.x;
            ny = y*scalingFactor.y;
            nx2 = x2*scalingFactor.x;
            ny2 = y2*scalingFactor.y;
        }
        else
        {
            nx = x;
            ny = y;
            nx2 = x2;
            ny2 = y2;
        }

        if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            SimpleGraphics::drawRect(nx, ny, nx2, ny2, outline, (Image*)boundSurface->getSurface());
        }
        
        
        #ifdef USE_OPENGL
            if(getType() == GuiGraphicsInterface::TYPE_OPENGL)
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((GLSurface*)boundSurface->getSurface())->bind();
                }
                GLGraphics::drawRectangle(nx, ny, nx2, ny2, outline);
            }
        #endif
    }

    void GuiGraphicsInterface::drawLine(int x, int y, int x2, int y2)
    {
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        
        int nx, ny, nx2, ny2;
        if(useScaling)
        {
            nx = x*scalingFactor.x;
            ny = y*scalingFactor.y;
            nx2 = x2*scalingFactor.x;
            ny2 = y2*scalingFactor.y;
        }
        else
        {
            nx = x;
            ny = y;
            nx2 = x2;
            ny2 = y2;
        }

        if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            SimpleGraphics::drawLine(nx, ny, nx2, ny2, (Image*)boundSurface->getSurface() );
        }
        
        #ifdef USE_OPENGL
            if(getType() == GuiGraphicsInterface::TYPE_OPENGL)
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((GLSurface*)boundSurface->getSurface())->bind();
                }
                GLGraphics::drawLine(nx, ny, nx2, ny2);
            }
        #endif
    }

    void GuiGraphicsInterface::drawCircle(int x, int y, int radius, bool outline)
    {
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        
        int nx, ny, nRad;
        if(useScaling)
        {
            nx = x*scalingFactor.x;
            ny = y*scalingFactor.y;
        }
        else
        {
            nx = x;
            ny = y;
        }

        if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            SimpleGraphics::drawCircle(nx, ny, radius, outline, (Image*)boundSurface->getSurface() );
        }
        
        #ifdef USE_OPENGL
            if(getType() == GuiGraphicsInterface::TYPE_OPENGL)
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((GLSurface*)boundSurface->getSurface())->bind();
                }
                GLGraphics::drawCircle(nx, ny, radius, outline);
            }
        #endif
    }
    
	void GuiGraphicsInterface::drawEllipse(int x, int y, int xRad, int yRad, bool outline)
    {
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        
        int nx, ny, nXRad, nYRad;
        if(useScaling)
        {
            nx = x*scalingFactor.x;
            ny = y*scalingFactor.y;
            nXRad = xRad*scalingFactor.x;
            nYRad = yRad*scalingFactor.y;
        }
        else
        {
            nx = x;
            ny = y;
            nXRad = xRad;
            nYRad = yRad;
        }

        if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            SimpleGraphics::drawEllipse(nx, ny, nXRad, nYRad, outline, (Image*)boundSurface->getSurface() );
        }
        
        #ifdef USE_OPENGL
            if(getType() == GuiGraphicsInterface::TYPE_OPENGL)
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((GLSurface*)boundSurface->getSurface())->bind();
                }
                GLGraphics::drawEllipse(nx, ny, nXRad, nYRad, outline);
            }
        #endif
    }

    void GuiGraphicsInterface::drawSprite(GuiImageInterface* img, int x, int y)
    {
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        if(img != nullptr)
        {
            int nx, ny;
            if(useScaling)
            {
                nx = x*scalingFactor.x;
                ny = y*scalingFactor.y;
            }
            else
            {
                nx = x;
                ny = y;
            }

            if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE)
            {
                Image* imgData = (Image*)img->getImage();
                SimpleGraphics::drawSprite(imgData, nx, ny, (Image*)boundSurface->getSurface());
            }
            
            #ifdef USE_OPENGL
                if(getType() == GuiGraphicsInterface::TYPE_OPENGL)
                {
                    if(boundSurface->getSurface() != nullptr)
                    {
                        ((GLSurface*)boundSurface->getSurface())->bind();
                    }
                    GLTexture* imgData = (GLTexture*)img->getImage();
                    GLGraphics::drawTexture(nx, ny, imgData);
                }
            #endif
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
            
            int nx, ny, nx2, ny2;
            if(useScaling)
            {
                nx = x1*scalingFactor.x;
                ny = y1*scalingFactor.y;
                nx2 = x2*scalingFactor.x;
                ny2 = y2*scalingFactor.y;
            }
            else
            {
                nx = x1;
                ny = y1;
                nx2 = x2;
                ny2 = y2;
            }

            if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE)
            {
                Image* imgData = (Image*)img->getImage();
                SimpleGraphics::drawSprite(imgData, nx, ny, nx2, ny2, (Image*)boundSurface->getSurface());
            }
            
            
            #ifdef USE_OPENGL
                if(getType() == GuiGraphicsInterface::TYPE_OPENGL)
                {
                    if(boundSurface->getSurface() != nullptr)
                    {
                        ((GLSurface*)boundSurface->getSurface())->bind();
                    }
                    GLTexture* imgData = (GLTexture*)img->getImage();
                    GLGraphics::drawTexture(nx, ny, nx2, ny2, imgData);
                }
            #endif
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
            int nx, ny;
            if(useScaling)
            {
                nx = x*scalingFactor.x;
                ny = y*scalingFactor.y;
            }
            else
            {
                nx = x;
                ny = y;
            }

            if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE)
            {
                Image* imgData = (Image*)img->getImage();
                SimpleGraphics::drawSpritePart(imgData, nx, ny, imgX, imgY, imgW, imgH, (Image*)boundSurface->getSurface());
            }
            
            #ifdef USE_OPENGL
                if(getType() == GuiGraphicsInterface::TYPE_OPENGL)
                {
                    if(boundSurface->getSurface() != nullptr)
                    {
                        ((GLSurface*)boundSurface->getSurface())->bind();
                    }
                    GLTexture* imgData = (GLTexture*)img->getImage();
                    Vec4f positionData = Vec4f(nx, ny, nx+imgW, ny+imgH);
                    Vec4f textureData = Vec4f(imgX, imgY, imgX+imgW, imgY+imgH);
                    textureData.x /= imgData->getWidth();
                    textureData.y /= imgData->getHeight();
                    textureData.z /= imgData->getWidth();
                    textureData.w /= imgData->getHeight();
                    
                    GLGraphics::drawTexturePart(positionData, textureData, imgData);
                }
            #endif
        }
    }

    void GuiGraphicsInterface::drawText(std::string str, int x, int y)
    {
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        
        int nx, ny;
        if(useScaling)
        {
            nx = x*scalingFactor.x;
            ny = y*scalingFactor.y;
        }
        else
        {
            nx = x;
            ny = y;
        }

        if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            SimpleGraphics::drawText(str, nx, ny, (Image*)boundSurface->getSurface());
        }
        
        #ifdef USE_OPENGL
            if(getType() == GuiGraphicsInterface::TYPE_OPENGL)
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((GLSurface*)boundSurface->getSurface())->bind();
                }
                GLGraphics::drawText(str, nx, ny);
            }
        #endif
    }

    void GuiGraphicsInterface::drawText(std::wstring str, int x, int y)
    {
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        int nx, ny;
        if(useScaling)
        {
            nx = x*scalingFactor.x;
            ny = y*scalingFactor.y;
        }
        else
        {
            nx = x;
            ny = y;
        }

        if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            SimpleGraphics::drawText(str, nx, ny, (Image*)boundSurface->getSurface());
        }
        
        #ifdef USE_OPENGL
            if(getType() == GuiGraphicsInterface::TYPE_OPENGL)
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((GLSurface*)boundSurface->getSurface())->bind();
                }
                GLGraphics::drawText(str, nx, ny);
            }
        #endif
    }

    void GuiGraphicsInterface::drawTextLimits(std::wstring str, int x, int y, int maxWidth, int maxHeight, bool useLineBreak)
    {
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        
        int nx, ny;
        if(useScaling)
        {
            nx = x*scalingFactor.x;
            ny = y*scalingFactor.y;
        }
        else
        {
            nx = x;
            ny = y;
        }

        if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            SimpleGraphics::drawTextLimits(str, nx, ny, maxWidth, maxHeight, useLineBreak, (Image*)boundSurface->getSurface());
        }
        
        #ifdef USE_OPENGL
            if(getType() == GuiGraphicsInterface::TYPE_OPENGL)
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((GLSurface*)boundSurface->getSurface())->bind();
                }
                GLGraphics::drawTextLimits(str, nx, ny, maxWidth, maxHeight, useLineBreak);
            }
        #endif
    }

    void GuiGraphicsInterface::drawTextLimits(std::string str, int x, int y, int maxWidth, int maxHeight, bool useLineBreak)
    {
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        int nx, ny;
        if(useScaling)
        {
            nx = x*scalingFactor.x;
            ny = y*scalingFactor.y;
        }
        else
        {
            nx = x;
            ny = y;
        }

        if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            SimpleGraphics::drawTextLimits(str, nx, ny, maxWidth, maxHeight, useLineBreak, (Image*)boundSurface->getSurface());
        }
        
        #ifdef USE_OPENGL
            if(getType() == GuiGraphicsInterface::TYPE_OPENGL)
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((GLSurface*)boundSurface->getSurface())->bind();
                }
                GLGraphics::drawTextLimits(str, nx, ny, maxWidth, maxHeight, useLineBreak);
            }
        #endif
    }
    
    void GuiGraphicsInterface::drawTextLimitsHighlighted(std::wstring str, int x, int y, int maxWidth, int maxHeight, bool useLineBreak, int highlightStart, int highlightEnd, Color highlightColor)
    {
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        int nx, ny;
        if(useScaling)
        {
            nx = x*scalingFactor.x;
            ny = y*scalingFactor.y;
        }
        else
        {
            nx = x;
            ny = y;
        }

        if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            SimpleGraphics::drawTextLimitsHighlighted(str, nx, ny, maxWidth, maxHeight, useLineBreak, highlightStart, highlightEnd, highlightColor, (Image*)boundSurface->getSurface());
        }
        
        #ifdef USE_OPENGL
            if(getType() == GuiGraphicsInterface::TYPE_OPENGL)
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((GLSurface*)boundSurface->getSurface())->bind();
                }
                Vec4f vColor = SimpleGraphics::convertColorToVec4f(highlightColor);
                GLGraphics::drawTextLimitsHighlighted(str, nx, ny, maxWidth, maxHeight, useLineBreak, highlightStart, highlightEnd, vColor);
            }
        #endif
    }

    void GuiGraphicsInterface::drawTextLimitsHighlighted(std::string str, int x, int y, int maxWidth, int maxHeight, bool useLineBreak, int highlightStart, int highlightEnd, Color highlightColor)
    {
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        int nx, ny;
        if(useScaling)
        {
            nx = x*scalingFactor.x;
            ny = y*scalingFactor.y;
        }
        else
        {
            nx = x;
            ny = y;
        }

        if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            SimpleGraphics::drawTextLimitsHighlighted(str, nx, ny, maxWidth, maxHeight, useLineBreak, highlightStart, highlightEnd, highlightColor, (Image*)boundSurface->getSurface());
        }
        
        #ifdef USE_OPENGL
            if(getType() == GuiGraphicsInterface::TYPE_OPENGL)
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((GLSurface*)boundSurface->getSurface())->bind();
                }
                Vec4f vColor = SimpleGraphics::convertColorToVec4f(highlightColor);
                GLGraphics::drawTextLimitsHighlighted(str, nx, ny, maxWidth, maxHeight, useLineBreak, highlightStart, highlightEnd, vColor);
            }
        #endif
    }
    
    void GuiGraphicsInterface::drawTextLimitsHighlighted(std::wstring str, int x, int y, int maxWidth, int maxHeight, bool useLineBreak, int highlightStart, int highlightEnd, Vec4f highlightColor)
    {
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        int nx, ny;
        if(useScaling)
        {
            nx = x*scalingFactor.x;
            ny = y*scalingFactor.y;
        }
        else
        {
            nx = x;
            ny = y;
        }

        if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            Color cColor = SimpleGraphics::convertVec4fToColor(highlightColor);
            SimpleGraphics::drawTextLimitsHighlighted(str, nx, ny, maxWidth, maxHeight, useLineBreak, highlightStart, highlightEnd, cColor, (Image*)boundSurface->getSurface());
        }
        
        #ifdef USE_OPENGL
            if(getType() == GuiGraphicsInterface::TYPE_OPENGL)
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((GLSurface*)boundSurface->getSurface())->bind();
                }
                GLGraphics::drawTextLimitsHighlighted(str, nx, ny, maxWidth, maxHeight, useLineBreak, highlightStart, highlightEnd, highlightColor);
            }
        #endif
    }

    void GuiGraphicsInterface::drawTextLimitsHighlighted(std::string str, int x, int y, int maxWidth, int maxHeight, bool useLineBreak, int highlightStart, int highlightEnd, Vec4f highlightColor)
    {
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        int nx, ny;
        if(useScaling)
        {
            nx = x*scalingFactor.x;
            ny = y*scalingFactor.y;
        }
        else
        {
            nx = x;
            ny = y;
        }

        if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            Color cColor = SimpleGraphics::convertVec4fToColor(highlightColor);
            SimpleGraphics::drawTextLimitsHighlighted(str, nx, ny, maxWidth, maxHeight, useLineBreak, highlightStart, highlightEnd, cColor, (Image*)boundSurface->getSurface());
        }
        
        #ifdef USE_OPENGL
            if(getType() == GuiGraphicsInterface::TYPE_OPENGL)
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((GLSurface*)boundSurface->getSurface())->bind();
                }
                GLGraphics::drawTextLimitsHighlighted(str, nx, ny, maxWidth, maxHeight, useLineBreak, highlightStart, highlightEnd, highlightColor);
            }
        #endif
    }

    void GuiGraphicsInterface::setClippingRect(Box2D b)
    {
        clippingRect = b;

        if(useScaling)
            b.setScale(Vec3f(scalingFactor, 1.0));
        else
            b.setScale(Vec3f(1.0, 1.0, 1.0));

        if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            SimpleGraphics::setClippingRect(b);
        }
        
        #ifdef USE_OPENGL
            if(getType() == GuiGraphicsInterface::TYPE_OPENGL)
            {
                //Set the ortho graphic projection matrix using this
                int x = (int)b.getLeftBound();
                int y = (int)b.getTopBound();
                int wid = (int)b.getWidth();
                int hei = (int)b.getHeight();
                GLGraphics::setClippingRectangle(x, y, wid, hei);
            }
        #endif
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
        
        #ifdef USE_OPENGL
            if(getType() == GuiGraphicsInterface::TYPE_OPENGL)
            {
                GLGraphics::resetClippingRectangle();
            }
        #endif
    }

    void GuiGraphicsInterface::drawSurface(GuiSurfaceInterface* img, int x, int y)
    {
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        int nx, ny;
        if(useScaling)
        {
            nx = x*scalingFactor.x;
            ny = y*scalingFactor.y;
        }
        else
        {
            nx = x;
            ny = y;
        }

        if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            if(img != nullptr)
            {
                SimpleGraphics::drawSprite((Image*)img->getSurface(), nx, ny, (Image*)boundSurface->getSurface());
            }
        }
        
        #ifdef USE_OPENGL
            if(getType() == GuiGraphicsInterface::TYPE_OPENGL)
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
                        GLGraphics::drawSurface(nx, ny, nx+tempSurf->getWidth(), ny+tempSurf->getHeight(), tempSurf);
                    }
                }
            }
        #endif
    }
    void GuiGraphicsInterface::drawSurface(GuiSurfaceInterface* img, int x1, int y1, int x2, int y2)
    {
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        int nx1, ny1, nx2, ny2;
        if(useScaling)
        {
            nx1 = x1*scalingFactor.x;
            ny1 = y1*scalingFactor.y;
            nx2 = x2*scalingFactor.x;
            ny2 = y2*scalingFactor.y;
        }
        else
        {
            nx1 = x1;
            ny1 = y1;
            nx2 = x2;
            ny2 = y2;
        }

        if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            if(img != nullptr)
            {
                SimpleGraphics::drawSprite((Image*)img->getSurface(), nx1, ny1, nx2, ny2, (Image*)boundSurface->getSurface());
            }
        }
        
        #ifdef USE_OPENGL
            if(getType() == GuiGraphicsInterface::TYPE_OPENGL)
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((GLSurface*)boundSurface->getSurface())->bind();
                }
                if(img != nullptr)
                {
                    GLSurface* tempSurf = (GLSurface*)img->getSurface();
                    GLGraphics::drawSurface(nx1, ny1, nx2, ny2, tempSurf);
                }
            }
        #endif
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
        
        #ifdef USE_OPENGL
            if(getType() == GuiGraphicsInterface::TYPE_OPENGL)
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((GLSurface*)boundSurface->getSurface())->unbind();
                    int width = boundSurface->getWidth();
                    int height = boundSurface->getHeight();
                    GLGraphics::drawSurface(0, 0, width, height, (GLSurface*)boundSurface->getSurface());
                }
            }
        #endif
    }

    void GuiGraphicsInterface::setOrthoProjection(int width, int height)
    {
        if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
        }
        
        #ifdef USE_OPENGL
            if(getType() == GuiGraphicsInterface::TYPE_OPENGL)
            {
                GLGraphics::setOrthoProjection(width, height);
            }
        #endif
    }
    
    void GuiGraphicsInterface::setScalingFactor(Vec2f v)
    {
        scalingFactor = v;
    }

    void GuiGraphicsInterface::enableScaling(bool v)
    {
        useScaling = v;
    }

    void GuiGraphicsInterface::setProjection(Mat4f proj)
    {
        if(getType() == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
        }
        
        #ifdef USE_OPENGL
            if(getType() == GuiGraphicsInterface::TYPE_OPENGL)
            {
                GLGraphics::setProjection(proj);
            }
        #endif
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
        
        #ifdef USE_OPENGL
            if(type == GuiGraphicsInterface::TYPE_OPENGL)
            {
                surface = new GLSurface(width, height);
            }
        #endif
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
            
            #ifdef USE_OPENGL
                if(type == GuiGraphicsInterface::TYPE_OPENGL)
                    delete ((GLSurface*)surface);
            #endif
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
            
            #ifdef USE_OPENGL
                if(type == GuiGraphicsInterface::TYPE_OPENGL)
                    return ((GLSurface*)surface)->getWidth();
            #endif
        }
        return 0;
    }

    int GuiSurfaceInterface::getHeight()
    {
        if(surface != nullptr)
        {
            if(type == GuiGraphicsInterface::TYPE_SOFTWARE)
                return ((Image*)surface)->getHeight();
            #ifdef USE_OPENGL
                if(type == GuiGraphicsInterface::TYPE_OPENGL)
                    return ((GLSurface*)surface)->getHeight();
            #endif
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
        
        #ifdef USE_OPENGL
            if(type == GuiGraphicsInterface::TYPE_OPENGL)
            {
                image = new GLTexture(file);
            }
        #endif
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
            
            #ifdef USE_OPENGL
                if(type == GuiGraphicsInterface::TYPE_OPENGL)
                    delete (GLTexture*)image;
            #endif
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
        
        #ifdef USE_OPENGL
            if(type == GuiGraphicsInterface::TYPE_OPENGL)
            {
                return ((GLTexture*)image)->getWidth();
            }
        #endif

        return 0;
    }
    int GuiImageInterface::getHeight()
    {
        if(type == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            return ((Image*)image)->getHeight();
        }
        
        #ifdef USE_OPENGL
            if(type == GuiGraphicsInterface::TYPE_OPENGL)
            {
                return ((GLTexture*)image)->getHeight();
            }
        #endif

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
        
        #ifdef USE_OPENGL
            if(type == GuiGraphicsInterface::TYPE_OPENGL)
            {
                sprite = new GLSprite();
                ((GLSprite*)sprite)->loadImage(file);
            }
        #endif
        this->type = type;
    }

    GuiSpriteInterface::~GuiSpriteInterface()
    {
        if(sprite != nullptr)
        {
            if(type == GuiGraphicsInterface::TYPE_SOFTWARE)
                delete (Sprite*)sprite;
            
            #ifdef USE_OPENGL
                if(type == GuiGraphicsInterface::TYPE_OPENGL)
                    delete (GLSprite*)sprite;
            #endif
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
        
        #ifdef USE_OPENGL
            if(type == GuiGraphicsInterface::TYPE_OPENGL)
            {
                GuiImageInterface* result = new GuiImageInterface();
                result->type = GuiGraphicsInterface::TYPE_OPENGL;
                result->image = ((GLSprite*)sprite)->getTexture(index);
                return result;
            }
        #endif

        return nullptr;
    }

    int GuiSpriteInterface::getDelayTime(int index)
    {
        if(type == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            return ((Sprite*)sprite)->getDelayTime(index);
        }
        
        #ifdef USE_OPENGL
            if(type == GuiGraphicsInterface::TYPE_OPENGL)
            {
                return ((GLSprite*)sprite)->getDelayTime(index);
            }
        #endif

        return -1;
    }

    int GuiSpriteInterface::getSize()
    {
        if(type == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            return ((Sprite*)sprite)->getSize();
        }
        
        #ifdef USE_OPENGL
            if(type == GuiGraphicsInterface::TYPE_OPENGL)
            {
                return ((GLSprite*)sprite)->getSize();
            }
        #endif

        return 0;
    }
    
    bool GuiSpriteInterface::shouldLoop()
    {
        if(type == GuiGraphicsInterface::TYPE_SOFTWARE)
        {
            return ((Sprite*)sprite)->shouldLoop();
        }
        
        #ifdef USE_OPENGL
            if(type == GuiGraphicsInterface::TYPE_OPENGL)
            {
                return ((GLSprite*)sprite)->shouldLoop();
            }
        #endif

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
        
        #ifdef USE_OPENGL
            if(type == GuiGraphicsInterface::TYPE_OPENGL)
            {
                font = new GLFont(file);
            }
        #endif

        this->type = type;
        shouldDelete = true;
    }

    GuiFontInterface::GuiFontInterface(Font* f, unsigned char type)
    {
        font = f;
        this->type = type;
        shouldDelete = false;
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
        if(shouldDelete)
        {
            if(font != nullptr)
            {
                if(type == GuiGraphicsInterface::TYPE_SOFTWARE)
                    delete (BitmapFont*)font;
                    
                #ifdef USE_OPENGL
                    if(type == GuiGraphicsInterface::TYPE_OPENGL)
                        delete (GLFont*)font;
                #endif
            }
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