#include "GraphicsExt.h"
#include "SimpleGraphics.h"
#include "BitmapFont.h"

#ifdef USE_OPENGL
    #include "ext/GLGraphics.h"
#endif

#ifdef USE_DIRECTX
    #include "ext/DXGraphics.h"
#endif

namespace smpl
{
    #pragma region GRAPHICS_STUFF

    unsigned char GraphicsInterface::type = TYPE_SOFTWARE;
    unsigned char GraphicsInterface::defaultType = TYPE_SOFTWARE;
    SurfaceInterface* GraphicsInterface::boundSurface = nullptr;
    FontInterface* GraphicsInterface::boundFont = nullptr;
    bool GraphicsInterface::ownedFont = false;
    Box2D GraphicsInterface::clippingRect = Box2D(0, 0, 65535, 65535);
    Vec2f GraphicsInterface::scalingFactor = Vec2f(1, 1);
    Vec2f GraphicsInterface::translationFactor = Vec2f(0, 0);
    bool GraphicsInterface::useScaling = true;

    SurfaceInterface* GraphicsInterface::createSurface(int width, int height)
    {
        int actualType = getType();
        
        if(actualType == TYPE_SOFTWARE)
            return SurfaceInterface::createSoftwareSurface(width, height);
        #ifdef USE_OPENGL
            if(actualType == TYPE_OPENGL)
                return SurfaceInterface::createGLSurface(width, height);
        #endif
        #ifdef USE_DIRECTX
            if(actualType == TYPE_DIRECTX)
                return SurfaceInterface::createDXSurface(width, height);
        #endif

        return nullptr;
    }

    ImageInterface* GraphicsInterface::createImage(File f)
    {
        int actualType = getType();

        if(actualType == TYPE_SOFTWARE)
            return ImageInterface::createSoftwareImage(f);
        #ifdef USE_OPENGL
            if(actualType == TYPE_OPENGL)
                return ImageInterface::createGLImage(f);
        #endif
        #ifdef USE_DIRECTX
            if(actualType == TYPE_DIRECTX)
                return ImageInterface::createDXImage(f);
        #endif
        return nullptr;
    }

    SpriteInterface* GraphicsInterface::createSprite(File f)
    {
        int actualType = getType();

        if(actualType == TYPE_SOFTWARE)
            return SpriteInterface::createSoftwareSprite(f);
        #ifdef USE_OPENGL
            if(actualType == TYPE_OPENGL)
                return SpriteInterface::createGLSprite(f);
        #endif
        #ifdef USE_DIRECTX
            if(actualType == TYPE_DIRECTX)
                return SpriteInterface::createDXSprite(f);
        #endif
        return nullptr;
    }

    FontInterface* GraphicsInterface::createFont(File f)
    {
        int actualType = getType();

        if(actualType == TYPE_SOFTWARE)
            return FontInterface::createSoftwareFont(f);
        #ifdef USE_OPENGL
            if(actualType == TYPE_OPENGL)
                return FontInterface::createGLFont(f);
        #endif
        #ifdef USE_DIRECTX
            if(actualType == TYPE_DIRECTX)
                return FontInterface::createDXFont(f);
        #endif
        return nullptr;
    }

    void GraphicsInterface::dispose()
    {
        if(ownedFont)
        {
            if(boundFont != nullptr)
                delete boundFont;
        }
        boundFont = nullptr;
    }

    void GraphicsInterface::setDefaultType(unsigned char v)
    {
        GraphicsInterface::defaultType = v;
    }

    unsigned char GraphicsInterface::getDefaultType()
    {
        return GraphicsInterface::defaultType;
    }

    unsigned char GraphicsInterface::getType()
    {
        if(type == TYPE_DEFAULT)
            return GraphicsInterface::type;
        return type;
    }
    
	void GraphicsInterface::setType(unsigned char v)
    {
        type = v;
    }

    void GraphicsInterface::setBoundSurface(SurfaceInterface* surface)
    {
        boundSurface = surface;
    }

    SurfaceInterface* GraphicsInterface::getBoundSurface()
    {
        return boundSurface;
    }
    
    void GraphicsInterface::setColor(Vec4f v)
    {
        int actualType = getType();

        if(actualType == TYPE_SOFTWARE)
        {
            SimpleGraphics::setColor(SimpleGraphics::convertVec4fToColor(v));
        }
        #ifdef USE_OPENGL
            if(actualType == TYPE_OPENGL)
            {
                GLGraphics::setDrawColor(v);
                GLGraphics::setClearColor(v);
            }
        #endif

        
        #ifdef USE_DIRECTX
            if(actualType == TYPE_DIRECTX)
            {
                DXGraphics::setDrawColor(v);
                DXGraphics::setClearColor(v);
            }
        #endif
    }
    
    void GraphicsInterface::setColor(Color c)
    {
        int actualType = getType();

        if(actualType == TYPE_SOFTWARE)
        {
            SimpleGraphics::setColor(c);
        }
        
        #ifdef USE_OPENGL
            if(actualType == TYPE_OPENGL)
            {
                Vec4f v = SimpleGraphics::convertColorToVec4f(c);

                GLGraphics::setDrawColor(v);
                GLGraphics::setClearColor(v);
            }
        #endif

        #ifdef USE_DIRECTX
            if(actualType == TYPE_DIRECTX)
            {
                Vec4f v = SimpleGraphics::convertColorToVec4f(c);

                DXGraphics::setDrawColor(v);
                DXGraphics::setClearColor(v);
            }
        #endif
    }

    Color GraphicsInterface::getColor()
    {
        int actualType = getType();

        if(actualType == TYPE_SOFTWARE)
        {
            return SimpleGraphics::getColor();
        }
        
        #ifdef USE_OPENGL
            if(actualType == TYPE_OPENGL)
            {
                return SimpleGraphics::convertVec4fToColor(GLGraphics::getDrawColor());
            }
        #endif

        #ifdef USE_DIRECTX
            if(actualType == TYPE_DIRECTX)
            {
                return SimpleGraphics::convertVec4fToColor(DXGraphics::getDrawColor());
            }
        #endif

        return Color();
    }

    Vec4f GraphicsInterface::getColorVec4f()
    {
        int actualType = getType();

        if(actualType == TYPE_SOFTWARE)
        {
            return SimpleGraphics::convertColorToVec4f(SimpleGraphics::getColor());
        }
        
        #ifdef USE_OPENGL
            if(actualType == TYPE_OPENGL)
            {
                return GLGraphics::getDrawColor();
            }
        #endif

        #ifdef USE_DIRECTX
            if(actualType == TYPE_DIRECTX)
            {
                return DXGraphics::getDrawColor();
            }
        #endif
        
        return Vec4f();
    }


    void GraphicsInterface::setFont(FontInterface* f)
    {
        int actualType = getType();

        if(f->getType() != actualType)
            return; //Should throw an error
        
        if(ownedFont == true && boundFont != f)
        {
            delete boundFont;
            boundFont = nullptr;
        }

        boundFont = f;
        ownedFont = false;
        if(actualType == TYPE_SOFTWARE)
        {
            SimpleGraphics::setFont( (BitmapFont*)f->getFont() );
        }
        
        #ifdef USE_OPENGL
            if(actualType == TYPE_OPENGL)
            {
                GLGraphics::setFont( (GLFont*)f->getFont() );
            }
        #endif

        
        #ifdef USE_DIRECTX
            if(actualType == TYPE_DIRECTX)
            {
                DXGraphics::setFont( (DXFont*)f->getFont() );
            }
        #endif
    }

    FontInterface* GraphicsInterface::getFont()
    {
        int actualType = getType();

        Font* f = nullptr;
        if(actualType == TYPE_SOFTWARE)
        {
            f = SimpleGraphics::getFont();
        }
        
        #ifdef USE_OPENGL
            if(actualType == TYPE_OPENGL)
            {
                f = GLGraphics::getFont();
            }
        #endif
        
        #ifdef USE_DIRECTX
            if(actualType == TYPE_DIRECTX)
            {
                f = DXGraphics::getFont();
            }
        #endif


        //check if f is the current bound font.
        if(boundFont != nullptr)
        {
            if(boundFont->getFont() == f)
            {
                return boundFont;
            }
            else
            {
                if(f != nullptr)
                {
                    //create a new font interface with the font we got and bind it.
                    setFont(FontInterface::createFromFont(f, actualType));
                    ownedFont = true;
                }
            }
        }
        else
        {
            if(f != nullptr)
            {
                //create a new font interface with the font we got and bind it.
                setFont(FontInterface::createFromFont(f, actualType));
                ownedFont = true;
            }
        }

        return boundFont;
    }
    
    void GraphicsInterface::clear()
    {
        int actualType = getType();

        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }

        if(actualType == GraphicsInterface::TYPE_SOFTWARE && boundSurface != nullptr)
        {
            SimpleGraphics::clearImage( (Image*)boundSurface->getSurface() );
        }
        
        #ifdef USE_OPENGL
            if(actualType == GraphicsInterface::TYPE_OPENGL)
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((GLSurface*)boundSurface->getSurface())->bind();
                }
                GLGraphics::clear(GLGraphics::COLOR_BUFFER | GLGraphics::DEPTH_BUFFER);
            }
        #endif

        #ifdef USE_DIRECTX
            if(actualType == GraphicsInterface::TYPE_DIRECTX)
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((DXSurface*)boundSurface->getSurface())->bind();
                }
                DXGraphics::clear(DXGraphics::COLOR_BUFFER | DXGraphics::DEPTH_BUFFER);
            }
        #endif
    }

    void GraphicsInterface::drawRect(int x, int y, int width, int height, bool outline)
    {
        int actualType = getType();
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        int nx, ny, nw, nh;
        x += translationFactor.x;
        y += translationFactor.y;
        if(useScaling)
        {
            nx = (int)MathExt::round(x*scalingFactor.x);
            ny = (int)MathExt::round(y*scalingFactor.y);
            nw = (int)MathExt::round(width*scalingFactor.x);
            nh = (int)MathExt::round(height*scalingFactor.y);
        }
        else
        {
            nx = x;
            ny = y;
            nw = width;
            nh = height;
        }

        if(actualType == GraphicsInterface::TYPE_SOFTWARE)
        {
            SimpleGraphics::drawRect(nx, ny, nw, nh, outline, (Image*)boundSurface->getSurface());
        }
        
        
        #ifdef USE_OPENGL
            if(actualType == GraphicsInterface::TYPE_OPENGL)
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((GLSurface*)boundSurface->getSurface())->bind();
                }
                GLGraphics::drawRectangle(nx, ny, nw, nh, outline);
            }
        #endif

        
        #ifdef USE_DIRECTX
            if(actualType == GraphicsInterface::TYPE_DIRECTX)
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((DXSurface*)boundSurface->getSurface())->bind();
                }
                DXGraphics::drawRectangle(nx, ny, nw, nh, outline);
            }
        #endif
    }

    void GraphicsInterface::drawLine(int x, int y, int x2, int y2)
    {
        int actualType = getType();
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        
        int nx, ny, nx2, ny2;
        x += translationFactor.x;
        x2 += translationFactor.x;
        y += translationFactor.y;
        y2 += translationFactor.y;
        if(useScaling)
        {
            nx = (int)MathExt::round(x * scalingFactor.x);
            ny = (int)MathExt::round(y * scalingFactor.y);
            nx2 = (int)MathExt::round(x2 * scalingFactor.x);
            ny2 = (int)MathExt::round(y2 * scalingFactor.y);
        }
        else
        {
            nx = x;
            ny = y;
            nx2 = x2;
            ny2 = y2;
        }

        if(actualType == GraphicsInterface::TYPE_SOFTWARE)
        {
            SimpleGraphics::drawLine(nx, ny, nx2, ny2, (Image*)boundSurface->getSurface() );
        }
        
        #ifdef USE_OPENGL
            if(actualType == GraphicsInterface::TYPE_OPENGL)
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((GLSurface*)boundSurface->getSurface())->bind();
                }
                GLGraphics::drawLine(nx, ny, nx2, ny2);
            }
        #endif

        #ifdef USE_DIRECTX
            if(actualType == GraphicsInterface::TYPE_DIRECTX)
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((DXSurface*)boundSurface->getSurface())->bind();
                }
                DXGraphics::drawLine(nx, ny, nx2, ny2);
            }
        #endif
    }

    void GraphicsInterface::drawCircle(int x, int y, int radius, bool outline)
    {
        int actualType = getType();
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        
        int nx, ny;
        x += translationFactor.x;
        y += translationFactor.y;
        //int nRad; //Currently not used
        
        if(useScaling)
        {
            nx = (int)MathExt::round(x * scalingFactor.x);
            ny = (int)MathExt::round(y * scalingFactor.y);
        }
        else
        {
            nx = x;
            ny = y;
        }

        if(actualType == GraphicsInterface::TYPE_SOFTWARE)
        {
            SimpleGraphics::drawCircle(nx, ny, radius, outline, (Image*)boundSurface->getSurface() );
        }
        
        #ifdef USE_OPENGL
            if(actualType == GraphicsInterface::TYPE_OPENGL)
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((GLSurface*)boundSurface->getSurface())->bind();
                }
                GLGraphics::drawCircle(nx, ny, radius, outline);
            }
        #endif

        #ifdef USE_DIRECTX
            if(actualType == GraphicsInterface::TYPE_DIRECTX)
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((DXSurface*)boundSurface->getSurface())->bind();
                }
                DXGraphics::drawCircle(nx, ny, radius, outline);
            }
        #endif
    }
    
	void GraphicsInterface::drawEllipse(int x, int y, int xRad, int yRad, bool outline)
    {
        int actualType = getType();
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        
        int nx, ny, nXRad, nYRad;
        x += translationFactor.x;
        y += translationFactor.y;

        if(useScaling)
        {
            nx = (int)MathExt::round(x * scalingFactor.x);
            ny = (int)MathExt::round(y * scalingFactor.y);
            nXRad = (int)MathExt::round(xRad * scalingFactor.x);
            nYRad = (int)MathExt::round(yRad * scalingFactor.y);
        }
        else
        {
            nx = x;
            ny = y;
            nXRad = xRad;
            nYRad = yRad;
        }

        if(actualType == GraphicsInterface::TYPE_SOFTWARE)
        {
            SimpleGraphics::drawEllipse(nx, ny, nXRad, nYRad, outline, (Image*)boundSurface->getSurface() );
        }
        
        #ifdef USE_OPENGL
            if(actualType == GraphicsInterface::TYPE_OPENGL)
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((GLSurface*)boundSurface->getSurface())->bind();
                }
                GLGraphics::drawEllipse(nx, ny, nXRad, nYRad, outline);
            }
        #endif

        #ifdef USE_DIRECTX
            if(actualType == GraphicsInterface::TYPE_DIRECTX)
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((DXSurface*)boundSurface->getSurface())->bind();
                }
                DXGraphics::drawEllipse(nx, ny, nXRad, nYRad, outline);
            }
        #endif
    }

    void GraphicsInterface::drawSprite(ImageInterface* img, int x, int y)
    {
        int actualType = getType();
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        if(img != nullptr)
        {
            int nx, ny;
            x += translationFactor.x;
            y += translationFactor.y;

            if(useScaling)
            {
                nx = (int)MathExt::round(x * scalingFactor.x);
                ny = (int)MathExt::round(y * scalingFactor.y);
            }
            else
            {
                nx = x;
                ny = y;
            }

            if(actualType == GraphicsInterface::TYPE_SOFTWARE)
            {
                Image* imgData = (Image*)img->getImage();
                SimpleGraphics::drawSprite(imgData, nx, ny, (Image*)boundSurface->getSurface());
            }
            
            #ifdef USE_OPENGL
                if(actualType == GraphicsInterface::TYPE_OPENGL)
                {
                    if(boundSurface->getSurface() != nullptr)
                    {
                        ((GLSurface*)boundSurface->getSurface())->bind();
                    }
                    GLTexture* imgData = (GLTexture*)img->getImage();
                    GLGraphics::drawTexture(nx, ny, imgData);
                }
            #endif

            #ifdef USE_DIRECTX
                if(actualType == GraphicsInterface::TYPE_DIRECTX)
                {
                    if(boundSurface->getSurface() != nullptr)
                    {
                        ((DXSurface*)boundSurface->getSurface())->bind();
                    }
                    DXTexture* imgData = (DXTexture*)img->getImage();
                    DXGraphics::drawTexture(nx, ny, imgData);
                }
            #endif
        }
    }

    void GraphicsInterface::drawSprite(ImageInterface* img, int x1, int y1, int width, int height)
    {
        int actualType = getType();
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        if(img != nullptr)
        {
            
            int nx, ny, nw, nh;
            x1 += translationFactor.x;
            y1 += translationFactor.y;
            if(useScaling)
            {
                nx = (int)MathExt::round(x1 * scalingFactor.x);
                ny = (int)MathExt::round(y1 * scalingFactor.y);
                nw = (int)MathExt::round(width * scalingFactor.x);
                nh = (int)MathExt::round(height * scalingFactor.y);
            }
            else
            {
                nx = x1;
                ny = y1;
                nw = width;
                nh = height;
            }

            if(actualType == GraphicsInterface::TYPE_SOFTWARE)
            {
                Image* imgData = (Image*)img->getImage();
                SimpleGraphics::drawSprite(imgData, nx, ny, nw, nh, (Image*)boundSurface->getSurface());
            }
            
            
            #ifdef USE_OPENGL
                if(actualType == GraphicsInterface::TYPE_OPENGL)
                {
                    if(boundSurface->getSurface() != nullptr)
                    {
                        ((GLSurface*)boundSurface->getSurface())->bind();
                    }
                    GLTexture* imgData = (GLTexture*)img->getImage();
                    GLGraphics::drawTexture(nx, ny, nw, nh, imgData);
                }
            #endif

            #ifdef USE_DIRECTX
                if(actualType == GraphicsInterface::TYPE_DIRECTX)
                {
                    if(boundSurface->getSurface() != nullptr)
                    {
                        ((DXSurface*)boundSurface->getSurface())->bind();
                    }
                    DXTexture* imgData = (DXTexture*)img->getImage();
                    DXGraphics::drawTexture(nx, ny, nw, nh, imgData);
                }
            #endif
        }
    }

    void GraphicsInterface::drawSpritePart(ImageInterface* img, int x, int y, int imgX, int imgY, int imgW, int imgH)
    {
        int actualType = getType();
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        if(img != nullptr)
        {
            int nx, ny;
            x += translationFactor.x;
            y += translationFactor.y;
            if(useScaling)
            {
                nx = (int)MathExt::round(x * scalingFactor.x);
                ny = (int)MathExt::round(y * scalingFactor.y);
            }
            else
            {
                nx = x;
                ny = y;
            }

            if(actualType == GraphicsInterface::TYPE_SOFTWARE)
            {
                Image* imgData = (Image*)img->getImage();
                SimpleGraphics::drawSpritePart(imgData, nx, ny, imgX, imgY, imgW, imgH, (Image*)boundSurface->getSurface());
            }
            
            #ifdef USE_OPENGL
                if(actualType == GraphicsInterface::TYPE_OPENGL)
                {
                    if(boundSurface->getSurface() != nullptr)
                    {
                        ((GLSurface*)boundSurface->getSurface())->bind();
                    }
                    GLTexture* imgData = (GLTexture*)img->getImage();
                    Vec2f p1 = Vec2f(nx, ny);
                    Vec2f p2 = Vec2f(nx+imgW, ny+imgH);
                    Vec2f uv1 = Vec2f((float)imgX / imgData->getWidth(), (float)imgY / imgData->getHeight());
                    Vec2f uv2 = Vec2f((float)(imgX+imgW) / imgData->getWidth(), (float)(imgY+imgH) / imgData->getHeight());
                    
                    
                    GLGraphics::drawTexturePart(p1, p2, uv1, uv2, imgData);
                }
            #endif

            #ifdef USE_DIRECTX
                if(actualType == GraphicsInterface::TYPE_DIRECTX)
                {
                    if(boundSurface->getSurface() != nullptr)
                    {
                        ((DXSurface*)boundSurface->getSurface())->bind();
                    }
                    DXTexture* imgData = (DXTexture*)img->getImage();
                    Vec4f positionData = Vec4f(nx, ny, nx+imgW, ny+imgH);
                    Vec4f textureData = Vec4f(imgX, imgY, imgX+imgW, imgY+imgH);
                    textureData.x /= imgData->getWidth();
                    textureData.y /= imgData->getHeight();
                    textureData.z /= imgData->getWidth();
                    textureData.w /= imgData->getHeight();
                    
                    DXGraphics::drawTexturePart(positionData, textureData, imgData);
                }
            #endif
        }
    }

    void GraphicsInterface::drawText(StringBridge strBridge, int x, int y)
    {
        int actualType = getType();
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        
        int nx, ny;
        x += translationFactor.x;
        y += translationFactor.y;
        if(useScaling)
        {
            nx = (int)MathExt::round(x * scalingFactor.x);
            ny = (int)MathExt::round(y * scalingFactor.y);
        }
        else
        {
            nx = x;
            ny = y;
        }

        if(actualType == GraphicsInterface::TYPE_SOFTWARE)
        {
            SimpleGraphics::drawText(strBridge, nx, ny, (Image*)boundSurface->getSurface());
        }
        
        #ifdef USE_OPENGL
            if(actualType == GraphicsInterface::TYPE_OPENGL)
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((GLSurface*)boundSurface->getSurface())->bind();
                }
                GLGraphics::drawText(strBridge, nx, ny);
            }
        #endif

        #ifdef USE_DIRECTX
            if(actualType == GraphicsInterface::TYPE_DIRECTX)
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((DXSurface*)boundSurface->getSurface())->bind();
                }
                DXGraphics::drawText(strBridge, nx, ny);
            }
        #endif
    }
    
    void GraphicsInterface::drawTextHighlighted(StringBridge strBridge, int x, int y, int highlightStart, int highlightEnd, Color highlightColor)
    {
        int actualType = getType();
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        
        int nx, ny;
        x += translationFactor.x;
        y += translationFactor.y;
        if(useScaling)
        {
            nx = (int)MathExt::round(x * scalingFactor.x);
            ny = (int)MathExt::round(y * scalingFactor.y);
        }
        else
        {
            nx = x;
            ny = y;
        }

        if(actualType == GraphicsInterface::TYPE_SOFTWARE)
        {
            SimpleGraphics::drawTextHighlighted(strBridge, nx, ny, highlightStart, highlightEnd, highlightColor, (Image*)boundSurface->getSurface());
        }
        
        #ifdef USE_OPENGL
            if(actualType == GraphicsInterface::TYPE_OPENGL)
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((GLSurface*)boundSurface->getSurface())->bind();
                }
                GLGraphics::drawTextHighlighted(strBridge, nx, ny, highlightStart, highlightEnd, SimpleGraphics::convertColorToVec4f(highlightColor));
            }
        #endif

        #ifdef USE_DIRECTX
            if(actualType == GraphicsInterface::TYPE_DIRECTX)
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((DXSurface*)boundSurface->getSurface())->bind();
                }
                DXGraphics::drawTextHighlighted(strBridge, nx, ny, highlightStart, highlightEnd, SimpleGraphics::convertColorToVec4f(highlightColor));
            }
        #endif
    }

    void GraphicsInterface::drawTextHighlighted(StringBridge strBridge, int x, int y, int highlightStart, int highlightEnd, Vec4f highlightColor)
    {
        int actualType = getType();
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        
        int nx, ny;
        x += translationFactor.x;
        y += translationFactor.y;
        if(useScaling)
        {
            nx = (int)MathExt::round(x * scalingFactor.x);
            ny = (int)MathExt::round(y * scalingFactor.y);
        }
        else
        {
            nx = x;
            ny = y;
        }

        if(actualType == GraphicsInterface::TYPE_SOFTWARE)
        {
            SimpleGraphics::drawTextHighlighted(strBridge, nx, ny, highlightStart, highlightEnd, SimpleGraphics::convertVec4fToColor(highlightColor), (Image*)boundSurface->getSurface());
        }
        
        #ifdef USE_OPENGL
            if(actualType == GraphicsInterface::TYPE_OPENGL)
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((GLSurface*)boundSurface->getSurface())->bind();
                }
                GLGraphics::drawTextHighlighted(strBridge, nx, ny, highlightStart, highlightEnd, highlightColor);
            }
        #endif

        #ifdef USE_DIRECTX
            if(actualType == GraphicsInterface::TYPE_DIRECTX)
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((DXSurface*)boundSurface->getSurface())->bind();
                }
                DXGraphics::drawTextHighlighted(strBridge, nx, ny, highlightStart, highlightEnd, highlightColor);
            }
        #endif
    }

    void GraphicsInterface::drawTextLimits(StringBridge strBridge, int x, int y, int maxWidth, int maxHeight, char wrapMode)
    {
        int actualType = getType();
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        int nx, ny;
        x += translationFactor.x;
        y += translationFactor.y;
        if(useScaling)
        {
            nx = (int)MathExt::round(x * scalingFactor.x);
            ny = (int)MathExt::round(y * scalingFactor.y);
        }
        else
        {
            nx = x;
            ny = y;
        }

        if(actualType == GraphicsInterface::TYPE_SOFTWARE)
        {
            SimpleGraphics::drawTextLimits(strBridge, nx, ny, maxWidth, maxHeight, wrapMode, (Image*)boundSurface->getSurface());
        }
        
        #ifdef USE_OPENGL
            if(actualType == GraphicsInterface::TYPE_OPENGL)
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((GLSurface*)boundSurface->getSurface())->bind();
                }
                GLGraphics::drawTextLimits(strBridge, nx, ny, maxWidth, maxHeight, wrapMode);
            }
        #endif

        #ifdef USE_DIRECTX
            if(actualType == GraphicsInterface::TYPE_DIRECTX)
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((DXSurface*)boundSurface->getSurface())->bind();
                }
                DXGraphics::drawTextLimits(strBridge, nx, ny, maxWidth, maxHeight, wrapMode);
            }
        #endif
    }

    void GraphicsInterface::drawTextLimitsHighlighted(StringBridge strBridge, int x, int y, int maxWidth, int maxHeight, char wrapMode, int highlightStart, int highlightEnd, Color highlightColor)
    {
        int actualType = getType();
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        int nx, ny;
        x += translationFactor.x;
        y += translationFactor.y;
        if(useScaling)
        {
            nx = (int)MathExt::round(x*scalingFactor.x);
            ny = (int)MathExt::round(y*scalingFactor.y);
        }
        else
        {
            nx = x;
            ny = y;
        }

        if(actualType == GraphicsInterface::TYPE_SOFTWARE)
        {
            SimpleGraphics::drawTextLimitsHighlighted(strBridge, nx, ny, maxWidth, maxHeight, wrapMode, highlightStart, highlightEnd, highlightColor, (Image*)boundSurface->getSurface());
        }
        
        #ifdef USE_OPENGL
            if(actualType == GraphicsInterface::TYPE_OPENGL)
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((GLSurface*)boundSurface->getSurface())->bind();
                }
                GLGraphics::drawTextLimitsHighlighted(strBridge, nx, ny, maxWidth, maxHeight, wrapMode, highlightStart, highlightEnd, SimpleGraphics::convertColorToVec4f(highlightColor));
            }
        #endif

        #ifdef USE_DIRECTX
            if(actualType == GraphicsInterface::TYPE_DIRECTX)
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((DXSurface*)boundSurface->getSurface())->bind();
                }
                DXGraphics::drawTextLimitsHighlighted(strBridge, nx, ny, maxWidth, maxHeight, wrapMode, highlightStart, highlightEnd, SimpleGraphics::convertColorToVec4f(highlightColor));
            }
        #endif
    }

    void GraphicsInterface::drawTextLimitsHighlighted(StringBridge strBridge, int x, int y, int maxWidth, int maxHeight, char wrapMode, int highlightStart, int highlightEnd, Vec4f highlightColor)
    {
        int actualType = getType();
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        int nx, ny;
        x += translationFactor.x;
        y += translationFactor.y;
        if(useScaling)
        {
            nx = (int)MathExt::round(x*scalingFactor.x);
            ny = (int)MathExt::round(y*scalingFactor.y);
        }
        else
        {
            nx = x;
            ny = y;
        }

        if(actualType == GraphicsInterface::TYPE_SOFTWARE)
        {
            SimpleGraphics::drawTextLimitsHighlighted(strBridge, nx, ny, maxWidth, maxHeight, wrapMode, highlightStart, highlightEnd, SimpleGraphics::convertVec4fToColor(highlightColor), (Image*)boundSurface->getSurface());
        }
        
        #ifdef USE_OPENGL
            if(actualType == GraphicsInterface::TYPE_OPENGL)
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((GLSurface*)boundSurface->getSurface())->bind();
                }
                GLGraphics::drawTextLimitsHighlighted(strBridge, nx, ny, maxWidth, maxHeight, wrapMode, highlightStart, highlightEnd, highlightColor);
            }
        #endif

        #ifdef USE_DIRECTX
            if(actualType == GraphicsInterface::TYPE_DIRECTX)
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((DXSurface*)boundSurface->getSurface())->bind();
                }
                DXGraphics::drawTextLimitsHighlighted(strBridge, nx, ny, maxWidth, maxHeight, wrapMode, highlightStart, highlightEnd, highlightColor);
            }
        #endif
    }

    void GraphicsInterface::setClippingRect(Box2D b)
    {
        int actualType = getType();
        clippingRect = b;

        if(useScaling)
            b.setScale(Vec3f(scalingFactor, 1.0));
        else
            b.setScale(Vec3f(1.0, 1.0, 1.0));

        if(actualType == GraphicsInterface::TYPE_SOFTWARE)
        {
            SimpleGraphics::setClippingRect(b);
        }
        
        #ifdef USE_OPENGL
            if(actualType == GraphicsInterface::TYPE_OPENGL)
            {
                //Set the ortho graphic projection matrix using this
                int x = (int)b.getLeftBound();
                int y = (int)b.getTopBound();
                int wid = (int)b.getWidth();
                int hei = (int)b.getHeight();
                GLGraphics::setClippingRectangle(x, y, wid, hei);
            }
        #endif

        #ifdef USE_DIRECTX
            if(actualType == GraphicsInterface::TYPE_DIRECTX)
            {
                //Set the ortho graphic projection matrix using this
                int x = (int)b.getLeftBound();
                int y = (int)b.getTopBound();
                int wid = (int)b.getWidth();
                int hei = (int)b.getHeight();
                DXGraphics::setClippingRectangle(x, y, wid, hei);
            }
        #endif
    }

    Box2D GraphicsInterface::getClippingRect()
    {
        return clippingRect;
    }

    void GraphicsInterface::resetClippingPlane()
    {
        int actualType = getType();
        clippingRect = Box2D(0, 0, 65535, 65535);
        if(actualType == GraphicsInterface::TYPE_SOFTWARE)
        {
            SimpleGraphics::resetClippingPlane();
        }
        
        #ifdef USE_OPENGL
            if(actualType == GraphicsInterface::TYPE_OPENGL)
            {
                GLGraphics::resetClippingRectangle();
            }
        #endif

        #ifdef USE_DIRECTX
            if(actualType == GraphicsInterface::TYPE_DIRECTX)
            {
                DXGraphics::resetClippingRectangle();
            }
        #endif
    }

    void GraphicsInterface::drawSurface(SurfaceInterface* img, int x, int y)
    {
        int actualType = getType();
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        int nx, ny;
        x += translationFactor.x;
        y += translationFactor.y;
        if(useScaling)
        {
            nx = (int)MathExt::round(x*scalingFactor.x);
            ny = (int)MathExt::round(y*scalingFactor.y);
        }
        else
        {
            nx = x;
            ny = y;
        }

        if(actualType == GraphicsInterface::TYPE_SOFTWARE)
        {
            if(img != nullptr)
            {
                SimpleGraphics::drawSprite((Image*)img->getSurface(), nx, ny, (Image*)boundSurface->getSurface());
            }
        }
        
        #ifdef USE_OPENGL
            if(actualType == GraphicsInterface::TYPE_OPENGL)
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

        #ifdef USE_DIRECTX
            if(actualType == GraphicsInterface::TYPE_DIRECTX)
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((DXSurface*)boundSurface->getSurface())->bind();
                }
                if(img != nullptr)
                {
                    DXSurface* tempSurf = (DXSurface*)boundSurface->getSurface();
                    if(tempSurf != nullptr)
                    {
                        DXGraphics::drawSurface(nx, ny, nx+tempSurf->getWidth(), ny+tempSurf->getHeight(), tempSurf);
                    }
                }
            }
        #endif
    }
    void GraphicsInterface::drawSurface(SurfaceInterface* img, int x1, int y1, int width, int height)
    {
        int actualType = getType();
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        int nx1, ny1, nw, nh;
        x1 += translationFactor.x;
        y1 += translationFactor.y;
        if(useScaling)
        {
            nx1 = (int)MathExt::round(x1*scalingFactor.x);
            ny1 = (int)MathExt::round(y1*scalingFactor.y);
            nw = (int)MathExt::round(width*scalingFactor.x);
            nh = (int)MathExt::round(height*scalingFactor.y);
        }
        else
        {
            nx1 = x1;
            ny1 = y1;
            nw = width;
            nh = height;
        }

        if(actualType == GraphicsInterface::TYPE_SOFTWARE)
        {
            if(img != nullptr)
            {
                SimpleGraphics::drawSprite((Image*)img->getSurface(), nx1, ny1, nw, nh, (Image*)boundSurface->getSurface());
            }
        }
        
        #ifdef USE_OPENGL
            if(actualType == GraphicsInterface::TYPE_OPENGL)
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((GLSurface*)boundSurface->getSurface())->bind();
                }
                if(img != nullptr)
                {
                    GLSurface* tempSurf = (GLSurface*)img->getSurface();
                    GLGraphics::drawSurface(nx1, ny1, nw, nh, tempSurf);
                }
            }
        #endif

        #ifdef USE_DIRECTX
            if(actualType == GraphicsInterface::TYPE_DIRECTX)
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((DXSurface*)boundSurface->getSurface())->bind();
                }
                if(img != nullptr)
                {
                    DXSurface* tempSurf = (DXSurface*)img->getSurface();
                    DXGraphics::drawSurface(nx1, ny1, nw, nh, tempSurf);
                }
            }
        #endif
    }

    void GraphicsInterface::drawToScreen()
    {
        int actualType = getType();
        if(boundSurface == nullptr)
        {
            return; //Even though opengl does not need a bound surface, return as an error.
        }
        if(actualType == GraphicsInterface::TYPE_SOFTWARE)
        {
        }
        
        #ifdef USE_OPENGL
            if(actualType == GraphicsInterface::TYPE_OPENGL)
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

        #ifdef USE_DIRECTX
            if(actualType == GraphicsInterface::TYPE_DIRECTX)
            {
                if(boundSurface->getSurface() != nullptr)
                {
                    ((DXSurface*)boundSurface->getSurface())->unbind();
                    int width = boundSurface->getWidth();
                    int height = boundSurface->getHeight();
                    DXGraphics::drawSurface(0, 0, width, height, (DXSurface*)boundSurface->getSurface());
                }
            }
        #endif
    }

    void GraphicsInterface::setOrthoProjection(int width, int height)
    {
        int actualType = getType();
        if(actualType == GraphicsInterface::TYPE_SOFTWARE)
        {
        }
        
        #ifdef USE_OPENGL
            if(actualType == GraphicsInterface::TYPE_OPENGL)
            {
                GLGraphics::setOrthoProjection(width, height);
            }
        #endif

        #ifdef USE_DIRECTX
            if(actualType == GraphicsInterface::TYPE_DIRECTX)
            {
                DXGraphics::setOrthoProjection(width, height);
            }
        #endif
    }
    
    Vec2f GraphicsInterface::getTranslationFactor()
    {
        return translationFactor;
    }

    void GraphicsInterface::setTranslationFactor(Vec2f v)
    {
        translationFactor = v;
    }

    Vec2f GraphicsInterface::getScalingFactor()
    {
        return scalingFactor;
    }

    void GraphicsInterface::setScalingFactor(Vec2f v)
    {
        scalingFactor = v;
    }

    void GraphicsInterface::enableScaling(bool v)
    {
        useScaling = v;
    }

    void GraphicsInterface::setProjection(Mat4f proj)
    {
        int actualType = getType();
        if(actualType == GraphicsInterface::TYPE_SOFTWARE)
        {
        }
        
        #ifdef USE_OPENGL
            if(actualType == GraphicsInterface::TYPE_OPENGL)
            {
                GLGraphics::setProjection(proj);
            }
        #endif

        #ifdef USE_DIRECTX
            if(actualType == GraphicsInterface::TYPE_DIRECTX)
            {
                DXGraphics::setProjection(proj);
            }
        #endif
    }

    #pragma endregion


    SurfaceInterface* SurfaceInterface::createSoftwareSurface(int width, int height)
    {
        return new SurfaceInterface(width, height, GraphicsInterface::TYPE_SOFTWARE);
    }

    SurfaceInterface* SurfaceInterface::createGLSurface(int width, int height)
    {
        return new SurfaceInterface(width, height, GraphicsInterface::TYPE_OPENGL);
    }

    SurfaceInterface* SurfaceInterface::createDXSurface(int width, int height)
    {
        return new SurfaceInterface(width, height, GraphicsInterface::TYPE_DIRECTX);
    }

    SurfaceInterface::SurfaceInterface(int width, int height, unsigned char type)
    {
        if(type == GraphicsInterface::TYPE_SOFTWARE)
        {
            surface = new Image(width, height);
        }
        
        #ifdef USE_OPENGL
            if(type == GraphicsInterface::TYPE_OPENGL)
            {
                surface = new GLSurface(width, height);
            }
        #endif

        #ifdef USE_DIRECTX
            if(type == GraphicsInterface::TYPE_DIRECTX)
            {
                surface = new DXSurface(width, height);
            }
        #endif
        this->type = type;
    }

    SurfaceInterface::SurfaceInterface(const SurfaceInterface& other)
    {
        type = other.type;
        surface = other.surface;
    }

    void SurfaceInterface::operator=(const SurfaceInterface& other)
    {
        type = other.type;
        surface = other.surface;
    }
    
    SurfaceInterface::~SurfaceInterface()
    {
        if(surface != nullptr)
        {
            if(this == GraphicsInterface::getBoundSurface())
                GraphicsInterface::setBoundSurface(nullptr);

            if(type == GraphicsInterface::TYPE_SOFTWARE)
                delete ((Image*)surface);
            
            #ifdef USE_OPENGL
                if(type == GraphicsInterface::TYPE_OPENGL)
                    delete ((GLSurface*)surface);
            #endif

            #ifdef USE_DIRECTX
                if(type == GraphicsInterface::TYPE_DIRECTX)
                    delete ((DXSurface*)surface);
            #endif
        }
        
        surface = nullptr;
    }
    
	int SurfaceInterface::getType()
    {
        return type;
    }

    int SurfaceInterface::getWidth()
    {
        if(surface != nullptr)
        {
            if(type == GraphicsInterface::TYPE_SOFTWARE)
                return ((Image*)surface)->getWidth();
            
            #ifdef USE_OPENGL
                if(type == GraphicsInterface::TYPE_OPENGL)
                    return ((GLSurface*)surface)->getWidth();
            #endif

            #ifdef USE_DIRECTX
                if(type == GraphicsInterface::TYPE_DIRECTX)
                    return ((DXSurface*)surface)->getWidth();
            #endif
        }
        return 0;
    }

    int SurfaceInterface::getHeight()
    {
        if(surface != nullptr)
        {
            if(type == GraphicsInterface::TYPE_SOFTWARE)
                return ((Image*)surface)->getHeight();
            #ifdef USE_OPENGL
                if(type == GraphicsInterface::TYPE_OPENGL)
                    return ((GLSurface*)surface)->getHeight();
            #endif
            #ifdef USE_DIRECTX
                if(type == GraphicsInterface::TYPE_DIRECTX)
                    return ((DXSurface*)surface)->getHeight();
            #endif
        }
        return 0;
    }

    void* SurfaceInterface::getSurface()
    {
        return surface;
    }

    ImageInterface* ImageInterface::createSoftwareImage(File f)
    {
        return new ImageInterface(f, GraphicsInterface::TYPE_SOFTWARE);
    }

    ImageInterface* ImageInterface::createGLImage(File f)
    {
        return new ImageInterface(f, GraphicsInterface::TYPE_OPENGL);
    }

    ImageInterface* ImageInterface::createDXImage(File f)
    {
        return new ImageInterface(f, GraphicsInterface::TYPE_DIRECTX);
    }

    ImageInterface::ImageInterface(File file, unsigned char type)
    {
        shouldDelete = true;
        if(type == GraphicsInterface::TYPE_SOFTWARE)
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
            if(type == GraphicsInterface::TYPE_OPENGL)
            {
                image = new GLTexture(file);
            }
        #endif

        #ifdef USE_DIRECTX
            if(type == GraphicsInterface::TYPE_DIRECTX)
            {
                image = new DXTexture(file);
            }
        #endif
        this->type = type;
    }
    
	ImageInterface::ImageInterface()
    {
        shouldDelete = false;
    }

    ImageInterface::ImageInterface(const ImageInterface& other)
    {
        copy(other);
    }
    
    void ImageInterface::operator=(const ImageInterface& other)
    {
        copy(other);
    }

    void ImageInterface::copy(const ImageInterface& other)
    {
        type = other.type;
        shouldDelete = other.shouldDelete;
        image = nullptr;

        if(shouldDelete)
        {
            if(type == GraphicsInterface::TYPE_SOFTWARE)
            {
                Image* oImg = (Image*)other.image;
                image = new Image(*oImg);
            }

            #ifdef USE_OPENGL
                if(type == GraphicsInterface::TYPE_OPENGL)
                {
                    GLTexture* oImg = (GLTexture*)other.image;
                    image = new GLTexture(*oImg);
                }
            #endif

            #ifdef USE_DIRECTX
                if(type == GraphicsInterface::TYPE_DIRECTX)
                {
                    DXTexture* oImg = (DXTexture*)other.image;
                    image = new DXTexture(*oImg);
                }
            #endif
        }
        else
        {
            image = other.image;
        }
    }
    
    ImageInterface::~ImageInterface()
    {
        if(shouldDelete)
        {
            if(image != nullptr)
            {
                if(type == GraphicsInterface::TYPE_SOFTWARE)
                    delete (Image*)image;
                
                #ifdef USE_OPENGL
                    if(type == GraphicsInterface::TYPE_OPENGL)
                        delete (GLTexture*)image;
                #endif

                #ifdef USE_DIRECTX
                    if(type == GraphicsInterface::TYPE_DIRECTX)
                        delete (DXTexture*)image;
                #endif
            }
        }
        
        image = nullptr;
        type = -1;
    }

    void* ImageInterface::getImage()
    {
        return image;
    }

    int ImageInterface::getType()
    {
        return type;
    }

    int ImageInterface::getWidth()
    {
        if(type == GraphicsInterface::TYPE_SOFTWARE)
        {
            return ((Image*)image)->getWidth();
        }
        
        #ifdef USE_OPENGL
            if(type == GraphicsInterface::TYPE_OPENGL)
            {
                return ((GLTexture*)image)->getWidth();
            }
        #endif

        #ifdef USE_DIRECTX
            if(type == GraphicsInterface::TYPE_DIRECTX)
            {
                return ((DXTexture*)image)->getWidth();
            }
        #endif

        return 0;
    }
    int ImageInterface::getHeight()
    {
        if(type == GraphicsInterface::TYPE_SOFTWARE)
        {
            return ((Image*)image)->getHeight();
        }
        
        #ifdef USE_OPENGL
            if(type == GraphicsInterface::TYPE_OPENGL)
            {
                return ((GLTexture*)image)->getHeight();
            }
        #endif

        #ifdef USE_DIRECTX
            if(type == GraphicsInterface::TYPE_DIRECTX)
            {
                return ((DXTexture*)image)->getHeight();
            }
        #endif

        return 0;
    }

    SpriteInterface* SpriteInterface::createSoftwareSprite(File f)
    {
        return new SpriteInterface(f, GraphicsInterface::TYPE_SOFTWARE);
    }

    SpriteInterface* SpriteInterface::createGLSprite(File f)
    {
        return new SpriteInterface(f, GraphicsInterface::TYPE_OPENGL);
    }

    SpriteInterface* SpriteInterface::createDXSprite(File f)
    {
        return new SpriteInterface(f, GraphicsInterface::TYPE_DIRECTX);
    }

    SpriteInterface::SpriteInterface(File file, unsigned char type)
    {
        if(type == GraphicsInterface::TYPE_SOFTWARE)
        {
            sprite = new Sprite();
            ((Sprite*)sprite)->loadImage(file);
        }
        
        #ifdef USE_OPENGL
            if(type == GraphicsInterface::TYPE_OPENGL)
            {
                sprite = new GLSprite();
                ((GLSprite*)sprite)->loadImage(file);
            }
        #endif

        #ifdef USE_DIRECTX
            if(type == GraphicsInterface::TYPE_DIRECTX)
            {
                sprite = new DXSprite();
                ((DXSprite*)sprite)->loadImage(file);
            }
        #endif
        this->type = type;
    }
    
    SpriteInterface::SpriteInterface(const SpriteInterface& other)
    {
        copy(other);
    }

    void SpriteInterface::operator=(const SpriteInterface& other)
    {
        copy(other);
    }

    void SpriteInterface::copy(const SpriteInterface& other)
    {
        type = other.type;
        sprite = nullptr;
        
        if(type == GraphicsInterface::TYPE_SOFTWARE)
        {
            Sprite* oSprite = (Sprite*)other.sprite;
            sprite = new Sprite(*oSprite);
        }
        #ifdef USE_OPENGL
            if(type == GraphicsInterface::TYPE_OPENGL)
            {
                GLSprite* oSprite = (GLSprite*)other.sprite;
                sprite = new GLSprite(*oSprite);
            }
        #endif

        
        #ifdef USE_DIRECTX
            if(type == GraphicsInterface::TYPE_DIRECTX)
            {
                DXSprite* oSprite = (DXSprite*)other.sprite;
                sprite = new DXSprite(*oSprite);
            }
        #endif
    }

    SpriteInterface::~SpriteInterface()
    {
        if(sprite != nullptr)
        {
            if(type == GraphicsInterface::TYPE_SOFTWARE)
                delete (Sprite*)sprite;
            
            #ifdef USE_OPENGL
                if(type == GraphicsInterface::TYPE_OPENGL)
                    delete (GLSprite*)sprite;
            #endif
            
            #ifdef USE_DIRECTX
                if(type == GraphicsInterface::TYPE_DIRECTX)
                    delete (DXSprite*)sprite;
            #endif
        }
        
        sprite = nullptr;
    }

    void* SpriteInterface::getSprite()
    {
        return sprite;
    }

    int SpriteInterface::getType()
    {
        return type;
    }

    ImageInterface SpriteInterface::getImage(size_t index)
    {
        if(sprite == nullptr)
            return ImageInterface();
        
        if(index >= this->getSize())
            return ImageInterface();
        
        if(type == GraphicsInterface::TYPE_SOFTWARE)
        {
            ImageInterface result = ImageInterface();
            result.shouldDelete = false;
            result.type = GraphicsInterface::TYPE_SOFTWARE;
            result.image = ((Sprite*)sprite)->getImage(index);
            return result;
        }
        
        #ifdef USE_OPENGL
            if(type == GraphicsInterface::TYPE_OPENGL)
            {
                ImageInterface result = ImageInterface();
                result.shouldDelete = false;
                result.type = GraphicsInterface::TYPE_OPENGL;
                result.image = ((GLSprite*)sprite)->getTexture(index);
                return result;
            }
        #endif

        #ifdef USE_DIRECTX
            if(type == GraphicsInterface::TYPE_DIRECTX)
            {
                ImageInterface result = ImageInterface();
                result.shouldDelete = false;
                result.type = GraphicsInterface::TYPE_DIRECTX;
                result.image = ((DXSprite*)sprite)->getTexture(index);
                return result;
            }
        #endif

        return ImageInterface();
    }

    int SpriteInterface::getDelayTime(size_t index)
    {
        if(sprite == nullptr)
            return -1;
        
        if(type == GraphicsInterface::TYPE_SOFTWARE)
        {
            return ((Sprite*)sprite)->getDelayTime(index);
        }
        
        #ifdef USE_OPENGL
            if(type == GraphicsInterface::TYPE_OPENGL)
            {
                return ((GLSprite*)sprite)->getDelayTime(index);
            }
        #endif
        
        #ifdef USE_DIRECTX
            if(type == GraphicsInterface::TYPE_DIRECTX)
            {
                return ((DXSprite*)sprite)->getDelayTime(index);
            }
        #endif

        return -1;
    }

    size_t SpriteInterface::getSize()
    {
        if(sprite == nullptr)
            return -1;
        
        if(type == GraphicsInterface::TYPE_SOFTWARE)
        {
            return ((Sprite*)sprite)->getSize();
        }
        
        #ifdef USE_OPENGL
            if(type == GraphicsInterface::TYPE_OPENGL)
            {
                return ((GLSprite*)sprite)->getSize();
            }
        #endif

        #ifdef USE_DIRECTX
            if(type == GraphicsInterface::TYPE_DIRECTX)
            {
                return ((DXSprite*)sprite)->getSize();
            }
        #endif

        return 0;
    }
    
    bool SpriteInterface::shouldLoop()
    {
        if(sprite == nullptr)
            return false;
        
        if(type == GraphicsInterface::TYPE_SOFTWARE)
        {
            return ((Sprite*)sprite)->shouldLoop();
        }
        
        #ifdef USE_OPENGL
            if(type == GraphicsInterface::TYPE_OPENGL)
            {
                return ((GLSprite*)sprite)->shouldLoop();
            }
        #endif

        #ifdef USE_DIRECTX
            if(type == GraphicsInterface::TYPE_DIRECTX)
            {
                return ((DXSprite*)sprite)->shouldLoop();
            }
        #endif

        return false;
    }

    int StaticFontCount = 0;
    FontInterface* FontInterface::createSoftwareFont(File f)
    {
        return new FontInterface(f, GraphicsInterface::TYPE_SOFTWARE);
    }

    FontInterface* FontInterface::createGLFont(File f)
    {
        return new FontInterface(f, GraphicsInterface::TYPE_OPENGL);
    }

    FontInterface* FontInterface::createDXFont(File f)
    {
        return new FontInterface(f, GraphicsInterface::TYPE_DIRECTX);
    }

    
	FontInterface* FontInterface::createFromFont(Font* f, unsigned char type)
    {
        return new FontInterface(f, type);
    }

    FontInterface::FontInterface(File file, unsigned char type)
    {
        if(type == GraphicsInterface::TYPE_SOFTWARE)
        {
            font = new BitmapFont(file);
        }
        
        #ifdef USE_OPENGL
            if(type == GraphicsInterface::TYPE_OPENGL)
            {
                font = new GLFont(file);
            }
        #endif

        #ifdef USE_DIRECTX
            if(type == GraphicsInterface::TYPE_DIRECTX)
            {
                font = new DXFont(file);
            }
        #endif

        this->type = type;
        shouldDelete = true;
    }

    FontInterface::FontInterface(Font* f, unsigned char type)
    {
        font = f;
        this->type = type;
        shouldDelete = false;
    }

    FontInterface::FontInterface(const FontInterface& other)
    {
        font = other.font;
        type = other.type;
        shouldDelete = false;
    }
    void FontInterface::operator=(const FontInterface& other)
    {
        font = other.font;
        type = other.type;
        shouldDelete = false;
    }
    
    FontInterface::~FontInterface()
    {
        if(shouldDelete)
        {
            if(font != nullptr)
            {
                if(type == GraphicsInterface::TYPE_SOFTWARE)
                    delete (BitmapFont*)font;
                    
                #ifdef USE_OPENGL
                    if(type == GraphicsInterface::TYPE_OPENGL)
                        delete (GLFont*)font;
                #endif

                #ifdef USE_DIRECTX
                    if(type == GraphicsInterface::TYPE_DIRECTX)
                        delete (DXFont*)font;
                #endif
            }
        }
        
        font = nullptr;
    }
    
    Font* FontInterface::getFont()
    {
        return font;
    }

    int FontInterface::getType()
    {
        return type;
    }
};