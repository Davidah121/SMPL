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
	SurfaceInterface GraphicsInterface::boundSurface;
	FontInterface GraphicsInterface::boundFont;
	Box2D GraphicsInterface::clippingRect = Box2D(0, 0, 65535, 65535);
	Vec2f GraphicsInterface::scalingFactor = Vec2f(1, 1);
	Vec2f GraphicsInterface::translationFactor = Vec2f(0, 0);
	bool GraphicsInterface::useScaling = true;

	SurfaceInterface GraphicsInterface::createSurface(int width, int height)
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

		return SurfaceInterface();
	}

	ImageInterface GraphicsInterface::createImage(const File& f)
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

		return ImageInterface();
	}

	SpriteInterface GraphicsInterface::createSprite(const File& f)
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

		return SpriteInterface();
	}

	FontInterface GraphicsInterface::createFont(const File& f)
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

		return FontInterface();
	}

	void GraphicsInterface::dispose()
	{
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

	void GraphicsInterface::setBoundSurface(const SurfaceInterface& surface)
	{
		boundSurface = surface;
	}
	void GraphicsInterface::unbindDeletedSurface(const SurfaceInterface& surface)
	{
		if(surface == boundSurface && surface.getSurface().getRawPointer() != nullptr)
			boundSurface = SurfaceInterface();
	}
	SurfaceInterface GraphicsInterface::getBoundSurface()
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


	void GraphicsInterface::setFont(const FontInterface& f)
	{
		int actualType = getType();

		if(f.getType() != actualType)
			return; //Should throw an error
		
		boundFont = f;
		if(actualType == TYPE_SOFTWARE)
		{
			
			SimpleGraphics::setFont( f.getFont() );
		}
		
		#ifdef USE_OPENGL
		if(actualType == TYPE_OPENGL)
		{
			GLGraphics::setFont( (SmartMemory<GLFont>)f.getFont() );
		}
		#endif

		
		#ifdef USE_DIRECTX
		if(actualType == TYPE_DIRECTX)
		{
			DXGraphics::setFont( (SmartMemory<DXFont>)f.getFont() );
		}
		#endif
	}

	FontInterface GraphicsInterface::getFont()
	{
		int actualType = getType();

		GenericSmartMemory f;
		if(actualType == TYPE_SOFTWARE)
			f = SimpleGraphics::getFont();
		
		#ifdef USE_OPENGL
		if(actualType == TYPE_OPENGL)
			f = GLGraphics::getFont();
		#endif
		
		#ifdef USE_DIRECTX
		if(actualType == TYPE_DIRECTX)
			f = DXGraphics::getFont();
		#endif

		return FontInterface::createFromFont((SmartMemory<Font>)f, actualType);
	}
	
	void GraphicsInterface::clear()
	{
		int actualType = getType();

		if(!boundSurface.isValid())
			return; //Even though opengl does not need a bound surface, return as an error.

		if(boundSurface.getType() != actualType)
			return; //type mismatch
		
		if(boundSurface.getType() == GraphicsInterface::TYPE_SOFTWARE)
		{
			SimpleGraphics::clearImage( (Image*)boundSurface.getSurface().getRawPointer() );
		}
		
		#ifdef USE_OPENGL
		if(boundSurface.getType() == GraphicsInterface::TYPE_OPENGL)
		{
			GLGraphics::clear(GLGraphics::COLOR_BUFFER | GLGraphics::DEPTH_BUFFER);
		}
		#endif

		#ifdef USE_DIRECTX
		if(boundSurface.getType() == GraphicsInterface::TYPE_DIRECTX)
		{
			DXGraphics::clear(DXGraphics::COLOR_BUFFER | DXGraphics::DEPTH_BUFFER);
		}
		#endif
	}

	void GraphicsInterface::drawRect(int x, int y, int width, int height, bool outline)
	{
		int actualType = getType();
		if(!boundSurface.isValid())
			return; //Even though opengl does not need a bound surface, return as an error.
		
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
			SimpleGraphics::drawRect(nx, ny, nw, nh, outline, (Image*)boundSurface.getSurface().getRawPointer());
		}
		
		
		#ifdef USE_OPENGL
		if(actualType == GraphicsInterface::TYPE_OPENGL)
		{
			GLGraphics::drawRectangle(nx, ny, nw, nh, outline);
		}
		#endif

		
		#ifdef USE_DIRECTX
		if(actualType == GraphicsInterface::TYPE_DIRECTX)
		{
			DXGraphics::drawRectangle(nx, ny, nw, nh, outline);
		}
		#endif
	}

	void GraphicsInterface::drawLine(int x, int y, int x2, int y2)
	{
		int actualType = getType();
		if(!boundSurface.isValid())
			return; //Even though opengl does not need a bound surface, return as an error.
		
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
			SimpleGraphics::drawLine(nx, ny, nx2, ny2, (Image*)boundSurface.getSurface().getRawPointer() );
		}
		
		#ifdef USE_OPENGL
		if(actualType == GraphicsInterface::TYPE_OPENGL)
		{
			GLGraphics::drawLine(nx, ny, nx2, ny2);
		}
		#endif

		#ifdef USE_DIRECTX
		if(actualType == GraphicsInterface::TYPE_DIRECTX)
		{
			DXGraphics::drawLine(nx, ny, nx2, ny2);
		}
		#endif
	}

	void GraphicsInterface::drawCircle(int x, int y, int radius, bool outline)
	{
		int actualType = getType();
		if(!boundSurface.isValid())
			return; //Even though opengl does not need a bound surface, return as an error.
		
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
			SimpleGraphics::drawCircle(nx, ny, radius, outline, (Image*)boundSurface.getSurface().getRawPointer() );
		}
		
		#ifdef USE_OPENGL
		if(actualType == GraphicsInterface::TYPE_OPENGL)
		{
			GLGraphics::drawCircle(nx, ny, radius, outline);
		}
		#endif

		#ifdef USE_DIRECTX
		if(actualType == GraphicsInterface::TYPE_DIRECTX)
		{
			DXGraphics::drawCircle(nx, ny, radius, outline);
		}
		#endif
	}
	
	void GraphicsInterface::drawEllipse(int x, int y, int xRad, int yRad, bool outline)
	{
		int actualType = getType();
		if(!boundSurface.isValid())
			return; //Even though opengl does not need a bound surface, return as an error.
		
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
			SimpleGraphics::drawEllipse(nx, ny, nXRad, nYRad, outline, (Image*)boundSurface.getSurface().getRawPointer() );
		}
		
		#ifdef USE_OPENGL
		if(actualType == GraphicsInterface::TYPE_OPENGL)
		{
			GLGraphics::drawEllipse(nx, ny, nXRad, nYRad, outline);
		}
		#endif

		#ifdef USE_DIRECTX
		if(actualType == GraphicsInterface::TYPE_DIRECTX)
		{
			DXGraphics::drawEllipse(nx, ny, nXRad, nYRad, outline);
		}
		#endif
	}

	void GraphicsInterface::drawSprite(const ImageInterface& img, int x, int y)
	{
		int actualType = getType();
		if(!boundSurface.isValid())
			return; //Even though opengl does not need a bound surface, return as an error.

		if(img.isValid())
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
				Image* imgData = (Image*)img.getImage().getRawPointer();
				SimpleGraphics::drawSprite(imgData, nx, ny, (Image*)boundSurface.getSurface().getRawPointer());
			}
			
			#ifdef USE_OPENGL
			if(actualType == GraphicsInterface::TYPE_OPENGL)
			{
				GLTexture* imgData = (GLTexture*)img.getImage().getRawPointer();
				GLGraphics::drawTexture(nx, ny, imgData);
			}
			#endif

			#ifdef USE_DIRECTX
			if(actualType == GraphicsInterface::TYPE_DIRECTX)
			{
				DXTexture* imgData = (DXTexture*)img.getImage().getRawPointer();
				DXGraphics::drawTexture(nx, ny, imgData);
			}
			#endif
		}
	}

	void GraphicsInterface::drawSprite(const ImageInterface& img, int x1, int y1, int width, int height)
	{
		int actualType = getType();
		if(!boundSurface.isValid())
			return; //Even though opengl does not need a bound surface, return as an error.

		if(img.isValid())
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
				Image* imgData = (Image*)img.getImage().getRawPointer();
				SimpleGraphics::drawSprite(imgData, nx, ny, nw, nh, (Image*)boundSurface.getSurface().getRawPointer());
			}
			
			
			#ifdef USE_OPENGL
			if(actualType == GraphicsInterface::TYPE_OPENGL)
			{
				GLTexture* imgData = (GLTexture*)img.getImage().getRawPointer();
				GLGraphics::drawTexture(nx, ny, nw, nh, imgData);
			}
			#endif

			#ifdef USE_DIRECTX
			if(actualType == GraphicsInterface::TYPE_DIRECTX)
			{
				DXTexture* imgData = (DXTexture*)img.getImage().getRawPointer();
				DXGraphics::drawTexture(nx, ny, nw, nh, imgData);
			}
			#endif
		}
	}

	void GraphicsInterface::drawSpritePart(const ImageInterface& img, int x, int y, int imgX, int imgY, int imgW, int imgH)
	{
		int actualType = getType();
		if(!boundSurface.isValid())
			return; //Even though opengl does not need a bound surface, return as an error.

		if(img.isValid())
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
				Image* imgData = (Image*)img.getImage().getRawPointer();
				SimpleGraphics::drawSpritePart(imgData, nx, ny, imgX, imgY, imgW, imgH, (Image*)boundSurface.getSurface().getRawPointer());
			}
			
			#ifdef USE_OPENGL
			if(actualType == GraphicsInterface::TYPE_OPENGL)
			{
				GLTexture* imgData = (GLTexture*)img.getImage().getRawPointer();
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
				DXTexture* imgData = (DXTexture*)img.getImage().getRawPointer();
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
		if(!boundSurface.isValid())
			return; //Even though opengl does not need a bound surface, return as an error.
		
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
			SimpleGraphics::drawText(strBridge, nx, ny, (Image*)boundSurface.getSurface().getRawPointer());
		}
		
		#ifdef USE_OPENGL
		if(actualType == GraphicsInterface::TYPE_OPENGL)
		{
			GLGraphics::drawText(strBridge, nx, ny);
		}
		#endif

		#ifdef USE_DIRECTX
		if(actualType == GraphicsInterface::TYPE_DIRECTX)
		{
			DXGraphics::drawText(strBridge, nx, ny);
		}
		#endif
	}
	
	void GraphicsInterface::drawTextHighlighted(StringBridge strBridge, int x, int y, int highlightStart, int highlightEnd, Color highlightColor)
	{
		int actualType = getType();
		if(!boundSurface.isValid())
			return; //Even though opengl does not need a bound surface, return as an error.
		
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
			SimpleGraphics::drawTextHighlighted(strBridge, nx, ny, highlightStart, highlightEnd, highlightColor, (Image*)boundSurface.getSurface().getRawPointer());
		}
		
		#ifdef USE_OPENGL
		if(actualType == GraphicsInterface::TYPE_OPENGL)
		{
			GLGraphics::drawTextHighlighted(strBridge, nx, ny, highlightStart, highlightEnd, SimpleGraphics::convertColorToVec4f(highlightColor));
		}
		#endif

		#ifdef USE_DIRECTX
		if(actualType == GraphicsInterface::TYPE_DIRECTX)
		{
			DXGraphics::drawTextHighlighted(strBridge, nx, ny, highlightStart, highlightEnd, SimpleGraphics::convertColorToVec4f(highlightColor));
		}
		#endif
	}

	void GraphicsInterface::drawTextHighlighted(StringBridge strBridge, int x, int y, int highlightStart, int highlightEnd, Vec4f highlightColor)
	{
		int actualType = getType();
		if(!boundSurface.isValid())
			return; //Even though opengl does not need a bound surface, return as an error.
		
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
			SimpleGraphics::drawTextHighlighted(strBridge, nx, ny, highlightStart, highlightEnd, SimpleGraphics::convertVec4fToColor(highlightColor), (Image*)boundSurface.getSurface().getRawPointer());
		}
		
		#ifdef USE_OPENGL
		if(actualType == GraphicsInterface::TYPE_OPENGL)
		{
			GLGraphics::drawTextHighlighted(strBridge, nx, ny, highlightStart, highlightEnd, highlightColor);
		}
		#endif

		#ifdef USE_DIRECTX
		if(actualType == GraphicsInterface::TYPE_DIRECTX)
		{
			DXGraphics::drawTextHighlighted(strBridge, nx, ny, highlightStart, highlightEnd, highlightColor);
		}
		#endif
	}

	void GraphicsInterface::drawTextLimits(StringBridge strBridge, int x, int y, int maxWidth, int maxHeight, char wrapMode)
	{
		int actualType = getType();
		if(!boundSurface.isValid())
			return; //Even though opengl does not need a bound surface, return as an error.

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
			SimpleGraphics::drawTextLimits(strBridge, nx, ny, maxWidth, maxHeight, wrapMode, (Image*)boundSurface.getSurface().getRawPointer());
		}
		
		#ifdef USE_OPENGL
		if(actualType == GraphicsInterface::TYPE_OPENGL)
		{
			GLGraphics::drawTextLimits(strBridge, nx, ny, maxWidth, maxHeight, wrapMode);
		}
		#endif

		#ifdef USE_DIRECTX
		if(actualType == GraphicsInterface::TYPE_DIRECTX)
		{
			DXGraphics::drawTextLimits(strBridge, nx, ny, maxWidth, maxHeight, wrapMode);
		}
		#endif
	}

	void GraphicsInterface::drawTextLimitsHighlighted(StringBridge strBridge, int x, int y, int maxWidth, int maxHeight, char wrapMode, int highlightStart, int highlightEnd, Color highlightColor)
	{
		int actualType = getType();
		if(!boundSurface.isValid())
			return; //Even though opengl does not need a bound surface, return as an error.

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
			SimpleGraphics::drawTextLimitsHighlighted(strBridge, nx, ny, maxWidth, maxHeight, wrapMode, highlightStart, highlightEnd, highlightColor, (Image*)boundSurface.getSurface().getRawPointer());
		}
		
		#ifdef USE_OPENGL
		if(actualType == GraphicsInterface::TYPE_OPENGL)
		{
			GLGraphics::drawTextLimitsHighlighted(strBridge, nx, ny, maxWidth, maxHeight, wrapMode, highlightStart, highlightEnd, SimpleGraphics::convertColorToVec4f(highlightColor));
		}
		#endif

		#ifdef USE_DIRECTX
		if(actualType == GraphicsInterface::TYPE_DIRECTX)
		{
			DXGraphics::drawTextLimitsHighlighted(strBridge, nx, ny, maxWidth, maxHeight, wrapMode, highlightStart, highlightEnd, SimpleGraphics::convertColorToVec4f(highlightColor));
		}
		#endif
	}

	void GraphicsInterface::drawTextLimitsHighlighted(StringBridge strBridge, int x, int y, int maxWidth, int maxHeight, char wrapMode, int highlightStart, int highlightEnd, Vec4f highlightColor)
	{
		int actualType = getType();
		if(!boundSurface.isValid())
			return; //Even though opengl does not need a bound surface, return as an error.

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
			SimpleGraphics::drawTextLimitsHighlighted(strBridge, nx, ny, maxWidth, maxHeight, wrapMode, highlightStart, highlightEnd, SimpleGraphics::convertVec4fToColor(highlightColor), (Image*)boundSurface.getSurface().getRawPointer());
		}
		
		#ifdef USE_OPENGL
		if(actualType == GraphicsInterface::TYPE_OPENGL)
		{
			GLGraphics::drawTextLimitsHighlighted(strBridge, nx, ny, maxWidth, maxHeight, wrapMode, highlightStart, highlightEnd, highlightColor);
		}
		#endif

		#ifdef USE_DIRECTX
		if(actualType == GraphicsInterface::TYPE_DIRECTX)
		{
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

	void GraphicsInterface::drawSurface(const SurfaceInterface& img, int x, int y)
	{
		int actualType = getType();
		if(!boundSurface.isValid())
			return; //Even though opengl does not need a bound surface, return as an error.
		if(!img.isValid())
			return;
		
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
			Image* tempSurf = (Image*)boundSurface.getSurface().getRawPointer();
			SimpleGraphics::drawSprite(tempSurf, nx, ny, (Image*)boundSurface.getSurface().getRawPointer());
		}
		
		#ifdef USE_OPENGL
		if(actualType == GraphicsInterface::TYPE_OPENGL)
		{
			GLSurface* tempSurf = (GLSurface*)boundSurface.getSurface().getRawPointer();
			GLGraphics::drawSurface(nx, ny, nx+tempSurf->getWidth(), ny+tempSurf->getHeight(), tempSurf);
		}
		#endif

		#ifdef USE_DIRECTX
		if(actualType == GraphicsInterface::TYPE_DIRECTX)
		{
			DXSurface* tempSurf = (DXSurface*)boundSurface.getSurface().getRawPointer();
			DXGraphics::drawSurface(nx, ny, nx+tempSurf->getWidth(), ny+tempSurf->getHeight(), tempSurf);
		}
		#endif
	}
	void GraphicsInterface::drawSurface(const SurfaceInterface& img, int x1, int y1, int width, int height)
	{
		int actualType = getType();
		if(!boundSurface.isValid())
			return; //Even though opengl does not need a bound surface, return as an error.
		if(!img.isValid())
			return;

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
			Image* tempSurf = (Image*)img.getSurface().getRawPointer();
			SimpleGraphics::drawSprite(tempSurf, nx1, ny1, nw, nh, (Image*)boundSurface.getSurface().getRawPointer());
		}
		
		#ifdef USE_OPENGL
		if(actualType == GraphicsInterface::TYPE_OPENGL)
		{
			GLSurface* tempSurf = (GLSurface*)img.getSurface().getRawPointer();
			GLGraphics::drawSurface(nx1, ny1, nw, nh, tempSurf);
		}
		#endif

		#ifdef USE_DIRECTX
		if(actualType == GraphicsInterface::TYPE_DIRECTX)
		{
			DXSurface* tempSurf = (DXSurface*)img.getSurface().getRawPointer();
			DXGraphics::drawSurface(nx1, ny1, nw, nh, tempSurf);
		}
		#endif
	}

	void GraphicsInterface::drawToScreen()
	{
		int actualType = getType();
		if(!boundSurface.isValid())
			return; //Even though opengl does not need a bound surface, return as an error.

		if(actualType == GraphicsInterface::TYPE_SOFTWARE)
		{
		}
		
		#ifdef USE_OPENGL
		if(actualType == GraphicsInterface::TYPE_OPENGL)
		{
			if(boundSurface.getSurface().getRawPointer() != nullptr)
			{
				((GLSurface*)boundSurface.getSurface().getRawPointer())->unbind();
				int width = boundSurface.getWidth();
				int height = boundSurface.getHeight();
				GLGraphics::drawSurface(0, 0, width, height, (GLSurface*)boundSurface.getSurface().getRawPointer());
			}
		}
		#endif

		#ifdef USE_DIRECTX
		if(actualType == GraphicsInterface::TYPE_DIRECTX)
		{
			if(boundSurface.getSurface().getRawPointer() != nullptr)
			{
				((DXSurface*)boundSurface.getSurface().getRawPointer())->unbind();
				int width = boundSurface.getWidth();
				int height = boundSurface.getHeight();
				DXGraphics::drawSurface(0, 0, width, height, (DXSurface*)boundSurface.getSurface().getRawPointer());
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


	SurfaceInterface SurfaceInterface::createSoftwareSurface(int width, int height)
	{
		return SurfaceInterface(width, height, GraphicsInterface::TYPE_SOFTWARE);
	}

	SurfaceInterface SurfaceInterface::createGLSurface(int width, int height)
	{
		return SurfaceInterface(width, height, GraphicsInterface::TYPE_OPENGL);
	}

	SurfaceInterface SurfaceInterface::createDXSurface(int width, int height)
	{
		return SurfaceInterface(width, height, GraphicsInterface::TYPE_DIRECTX);
	}

	SurfaceInterface::SurfaceInterface(int width, int height, unsigned char type)
	{
		if(type == GraphicsInterface::TYPE_SOFTWARE)
		{
			surface = SmartMemory<Image>::createDeleteOnLast(new Image(width, height));
		}
		
		#ifdef USE_OPENGL
		if(type == GraphicsInterface::TYPE_OPENGL)
		{
			surface = SmartMemory<GLSurface>::createDeleteOnLast(new GLSurface(width, height));
		}
		#endif

		#ifdef USE_DIRECTX
		if(type == GraphicsInterface::TYPE_DIRECTX)
		{
			surface = SmartMemory<DXSurface>::createDeleteOnLast(new DXSurface(width, height));
		}
		#endif
		this->type = type;
	}

	SurfaceInterface::SurfaceInterface()
	{
	}
	SurfaceInterface::~SurfaceInterface()
	{
		GraphicsInterface::unbindDeletedSurface(*this);
	}
	
	int SurfaceInterface::getType() const
	{
		return type;
	}
	
	bool SurfaceInterface::isValid() const
	{
		return surface.isValid();
	}

	int SurfaceInterface::getWidth() const
	{
		if(isValid())
		{
			if(type == GraphicsInterface::TYPE_SOFTWARE)
				return ((SmartMemory<Image>)surface).getRawPointer()->getWidth();
			
			#ifdef USE_OPENGL
			if(type == GraphicsInterface::TYPE_OPENGL)
				return ((SmartMemory<GLSurface>)surface).getRawPointer()->getWidth();
			#endif

			#ifdef USE_DIRECTX
			if(type == GraphicsInterface::TYPE_DIRECTX)
				return ((SmartMemory<DXSurface>)surface).getRawPointer()->getWidth();
			#endif
		}
		return 0;
	}

	int SurfaceInterface::getHeight() const
	{
		if(isValid())
		{
			if(type == GraphicsInterface::TYPE_SOFTWARE)
				return ((SmartMemory<Image>)surface).getRawPointer()->getHeight();

			#ifdef USE_OPENGL
			if(type == GraphicsInterface::TYPE_OPENGL)
				return ((SmartMemory<GLSurface>)surface).getRawPointer()->getHeight();
			#endif

			#ifdef USE_DIRECTX
			if(type == GraphicsInterface::TYPE_DIRECTX)
				return ((SmartMemory<DXSurface>)surface).getRawPointer()->getHeight();
			#endif
		}
		return 0;
	}

	GenericSmartMemory SurfaceInterface::getSurface() const
	{
		return surface;
	}
	
	bool SurfaceInterface::operator==(const SurfaceInterface& other) const
	{
		return surface == other.surface;
	}
	bool SurfaceInterface::operator!=(const SurfaceInterface& other) const
	{
		return surface != other.surface;
	}
	


	ImageInterface ImageInterface::createSoftwareImage(const File& f)
	{
		return ImageInterface(f, GraphicsInterface::TYPE_SOFTWARE);
	}
	ImageInterface ImageInterface::createSoftwareImage(const GenericSmartMemory& ref)
	{
		return ImageInterface(ref, GraphicsInterface::TYPE_SOFTWARE);
	}

	ImageInterface ImageInterface::createGLImage(const File& f)
	{
		return ImageInterface(f, GraphicsInterface::TYPE_OPENGL);
	}
	ImageInterface ImageInterface::createGLImage(const GenericSmartMemory& ref)
	{
		return ImageInterface(ref, GraphicsInterface::TYPE_OPENGL);
	}

	ImageInterface ImageInterface::createDXImage(const File& f)
	{
		return ImageInterface(f, GraphicsInterface::TYPE_DIRECTX);
	}
	ImageInterface ImageInterface::createDXImage(const GenericSmartMemory& ref)
	{
		return ImageInterface(ref, GraphicsInterface::TYPE_DIRECTX);
	}

	ImageInterface::ImageInterface(const GenericSmartMemory& mem, unsigned char type)
	{
		image = mem;
		this->type = type;
	}
	ImageInterface::ImageInterface(const File& file, unsigned char type)
	{
		if(type == GraphicsInterface::TYPE_SOFTWARE)
		{
			int amountImg = 0;
			Image** loadedImgArray = Image::loadImage(file, &amountImg);

			if(amountImg > 0)
				image = SmartMemory<Image>::createDeleteOnLast(loadedImgArray[0]);

			delete loadedImgArray;
		}
		
		#ifdef USE_OPENGL
		if(type == GraphicsInterface::TYPE_OPENGL)
		{
			image = SmartMemory<GLTexture>::createDeleteOnLast(new GLTexture(file));
		}
		#endif

		#ifdef USE_DIRECTX
		if(type == GraphicsInterface::TYPE_DIRECTX)
		{
			image = SmartMemory<DXTexture>::createDeleteOnLast(new DXTexture(file));
		}
		#endif
		this->type = type;
	}
	
	ImageInterface::ImageInterface()
	{
	}

	
	ImageInterface::~ImageInterface()
	{
		type = -1;
	}

	GenericSmartMemory ImageInterface::getImage() const
	{
		return image;
	}

	int ImageInterface::getType() const
	{
		return type;
	}
	
	bool ImageInterface::isValid() const
	{
		return image.isValid();
	}

	int ImageInterface::getWidth() const
	{
		if(type == GraphicsInterface::TYPE_SOFTWARE)
		{
			return ((SmartMemory<Image>)image)->getWidth();
		}
		
		#ifdef USE_OPENGL
		if(type == GraphicsInterface::TYPE_OPENGL)
		{
			return ((SmartMemory<GLTexture>)image)->getWidth();
		}
		#endif

		#ifdef USE_DIRECTX
		if(type == GraphicsInterface::TYPE_DIRECTX)
		{
			return ((SmartMemory<DXTexture>)image)->getWidth();
		}
		#endif

		return 0;
	}
	int ImageInterface::getHeight() const
	{
		if(type == GraphicsInterface::TYPE_SOFTWARE)
		{
			return ((SmartMemory<Image>)image)->getHeight();
		}
		
		#ifdef USE_OPENGL
		if(type == GraphicsInterface::TYPE_OPENGL)
		{
			return ((SmartMemory<GLTexture>)image)->getHeight();
		}
		#endif

		#ifdef USE_DIRECTX
		if(type == GraphicsInterface::TYPE_DIRECTX)
		{
			return ((SmartMemory<DXTexture>)image)->getHeight();
		}
		#endif

		return 0;
	}
	
	bool ImageInterface::operator==(const ImageInterface& other) const
	{
		return image == other.image;
	}
	bool ImageInterface::operator!=(const ImageInterface& other) const
	{
		return image != other.image;
	}

	SpriteInterface SpriteInterface::createSoftwareSprite(const File& f)
	{
		return SpriteInterface(f, GraphicsInterface::TYPE_SOFTWARE);
	}

	SpriteInterface SpriteInterface::createGLSprite(const File& f)
	{
		return SpriteInterface(f, GraphicsInterface::TYPE_OPENGL);
	}

	SpriteInterface SpriteInterface::createDXSprite(const File& f)
	{
		return SpriteInterface(f, GraphicsInterface::TYPE_DIRECTX);
	}

	SpriteInterface::SpriteInterface(const File& file, unsigned char type)
	{
		if(type == GraphicsInterface::TYPE_SOFTWARE)
		{
			sprite = SmartMemory<Sprite>::createDeleteOnLast(new Sprite());
			((Sprite*)sprite.getRawPointer())->loadImage(file);
			if(((Sprite*)sprite.getRawPointer())->getSize() == 0)
				sprite = GenericSmartMemory(); //invalid. failed to load
		}
		
		#ifdef USE_OPENGL
		if(type == GraphicsInterface::TYPE_OPENGL)
		{
			sprite = SmartMemory<GLSprite>::createDeleteOnLast(new GLSprite());
			((GLSprite*)sprite.getRawPointer())->loadImage(file);
			if(((GLSprite*)sprite.getRawPointer())->getSize() == 0)
				sprite = GenericSmartMemory(); //invalid. failed to load
		}
		#endif

		#ifdef USE_DIRECTX
		if(type == GraphicsInterface::TYPE_DIRECTX)
		{
			sprite = SmartMemory<DXSprite>::createDeleteOnLast(new DXSprite());
			((DXSprite*)sprite.getRawPointer())->loadImage(file);
			if(((DXSprite*)sprite.getRawPointer())->getSize() == 0)
				sprite = GenericSmartMemory(); //invalid. failed to load
		}
		#endif
		this->type = type;
	}
	
	
	SpriteInterface::SpriteInterface()
	{
	}
	SpriteInterface::~SpriteInterface()
	{
	}

	GenericSmartMemory SpriteInterface::getSprite() const
	{
		return sprite;
	}

	int SpriteInterface::getType() const
	{
		return type;
	}
	
	bool SpriteInterface::isValid() const
	{
		return sprite.isValid();
	}

	ImageInterface SpriteInterface::getImage(size_t index) const
	{
		if(!isValid())
			return ImageInterface();
		
		if(index >= this->getSize())
			return ImageInterface();
		
		if(type == GraphicsInterface::TYPE_SOFTWARE)
		{
			Sprite* spr = (Sprite*)sprite.getRawPointer();
			return ImageInterface::createSoftwareImage(spr->getImage(index));
		}
		
		#ifdef USE_OPENGL
		if(type == GraphicsInterface::TYPE_OPENGL)
		{
			GLSprite* spr = (GLSprite*)sprite.getRawPointer();
			return ImageInterface::createGLImage(spr->getTexture(index));
		}
		#endif

		#ifdef USE_DIRECTX
		if(type == GraphicsInterface::TYPE_DIRECTX)
		{
			DXSprite* spr = (DXSprite*)sprite.getRawPointer();
			return ImageInterface::createDXImage(SmartMemory<DXTexture>::createNoDelete(spr->getTexture(index)));
		}
		#endif

		return ImageInterface();
	}

	int SpriteInterface::getDelayTime(size_t index) const
	{
		if(!isValid())
			return -1;
		
		if(type == GraphicsInterface::TYPE_SOFTWARE)
		{
			return ((Sprite*)sprite.getRawPointer())->getDelayTime(index);
		}
		
		#ifdef USE_OPENGL
		if(type == GraphicsInterface::TYPE_OPENGL)
		{
			return ((GLSprite*)sprite.getRawPointer())->getDelayTime(index);
		}
		#endif
		
		#ifdef USE_DIRECTX
		if(type == GraphicsInterface::TYPE_DIRECTX)
		{
			return ((DXSprite*)sprite.getRawPointer)->getDelayTime(index);
		}
		#endif

		return -1;
	}

	size_t SpriteInterface::getSize() const
	{
		if(!isValid())
			return 0;
		
		if(type == GraphicsInterface::TYPE_SOFTWARE)
		{
			return ((Sprite*)sprite.getRawPointer())->getSize();
		}
		
		#ifdef USE_OPENGL
		if(type == GraphicsInterface::TYPE_OPENGL)
		{
			return ((GLSprite*)sprite.getRawPointer())->getSize();
		}
		#endif

		#ifdef USE_DIRECTX
		if(type == GraphicsInterface::TYPE_DIRECTX)
		{
			return ((DXSprite*)sprite.getRawPointer())->getSize();
		}
		#endif

		return 0;
	}
	
	bool SpriteInterface::shouldLoop() const
	{
		if(!isValid())
			return false;
		
		if(type == GraphicsInterface::TYPE_SOFTWARE)
		{
			return ((Sprite*)sprite.getRawPointer())->shouldLoop();
		}
		
		#ifdef USE_OPENGL
		if(type == GraphicsInterface::TYPE_OPENGL)
		{
			return ((GLSprite*)sprite.getRawPointer())->shouldLoop();
		}
		#endif

		#ifdef USE_DIRECTX
		if(type == GraphicsInterface::TYPE_DIRECTX)
		{
			return ((DXSprite*)sprite.getRawPointer())->shouldLoop();
		}
		#endif

		return false;
	}
	
	bool SpriteInterface::load(const File& f, bool shouldClear)
	{
		bool isFine = false;
		if(type == GraphicsInterface::TYPE_SOFTWARE)
		{
			if(!isValid())
				sprite = SmartMemory<Sprite>::createDeleteOnLast(new Sprite());

			isFine = ((Sprite*)sprite.getRawPointer())->loadImage(f, shouldClear);
			if(isFine == false && shouldClear == true)
				sprite = GenericSmartMemory(); //invalid. failed to load
		}
		
		#ifdef USE_OPENGL
		if(type == GraphicsInterface::TYPE_OPENGL)
		{
			if(!isValid())
				sprite = SmartMemory<GLSprite>::createDeleteOnLast(new GLSprite());

			isFine = ((GLSprite*)sprite.getRawPointer())->loadImage(f, shouldClear);
			if(isFine == false && shouldClear == true)
				sprite = GenericSmartMemory(); //invalid. failed to load
		}
		#endif

		#ifdef USE_DIRECTX
		if(type == GraphicsInterface::TYPE_DIRECTX)
		{
			if(!isValid())
				sprite = SmartMemory<DXSprite>::createDeleteOnLast(new DXSprite());
			
			isFine = ((DXSprite*)sprite.getRawPointer())->loadImage(file, shouldClear);
			if(isFine == false && shouldClear == true)
				sprite = GenericSmartMemory(); //invalid. failed to load
		}
		#endif
		return isFine;
	}
	
	bool SpriteInterface::operator==(const SpriteInterface& other) const
	{
		return sprite == other.sprite;
	}
	bool SpriteInterface::operator!=(const SpriteInterface& other) const
	{
		return sprite != other.sprite;
	}

	FontInterface FontInterface::createSoftwareFont(const File& f)
	{
		return FontInterface(f, GraphicsInterface::TYPE_SOFTWARE);
	}

	FontInterface FontInterface::createGLFont(const File& f)
	{
		return FontInterface(f, GraphicsInterface::TYPE_OPENGL);
	}

	FontInterface FontInterface::createDXFont(const File& f)
	{
		return FontInterface(f, GraphicsInterface::TYPE_DIRECTX);
	}

	
	FontInterface FontInterface::createFromFont(const SmartMemory<Font>& f, unsigned char type)
	{
		return FontInterface(f, type);
	}

	FontInterface::FontInterface(const File& file, unsigned char type)
	{
		if(type == GraphicsInterface::TYPE_SOFTWARE)
		{
			font = SmartMemory<BitmapFont>::createDeleteOnLast(new BitmapFont(file));
		}
		
		#ifdef USE_OPENGL
		if(type == GraphicsInterface::TYPE_OPENGL)
		{
			font = SmartMemory<GLFont>::createDeleteOnLast(new GLFont(file));
		}
		#endif

		#ifdef USE_DIRECTX
		if(type == GraphicsInterface::TYPE_DIRECTX)
		{
			font = SmartMemory<DXFont>::createDeleteOnLast(new DXFont(file));
		}
		#endif

		this->type = type;
	}

	FontInterface::FontInterface(const SmartMemory<Font>& f, unsigned char type)
	{
		font = f;
		this->type = type;
	}

	
	FontInterface::FontInterface()
	{
	}
	FontInterface::~FontInterface()
	{
	}
	
	SmartMemory<Font> FontInterface::getFont() const
	{
		return font;
	}

	int FontInterface::getType() const
	{
		return type;
	}
	
	bool FontInterface::isValid() const
	{
		return font.isValid();
	}

	bool FontInterface::operator==(const FontInterface& other) const
	{
		return font == other.font;
	}
	bool FontInterface::operator!=(const FontInterface& other) const
	{
		return font != other.font;
	}
};