#include "ext/GLTexture.h"

#ifdef USE_OPENGL
	namespace smpl
	{
		GLTexture::GLTexture()
		{

		}

		GLTexture::GLTexture(const File& f, bool includeAlpha, GLTextureParams params)
		{
			if(!GLSingleton::getInit())
			{
				return;
			}
			loadImage(f, includeAlpha, params);
		}

		GLTexture::GLTexture(Image* img, bool includeAlpha, GLTextureParams params)
		{
			if(!GLSingleton::getInit())
			{
				return;
			}
			setImage(img, includeAlpha, params);
		}
		
		void GLTexture::copyTexture(const GLTexture& o)
		{
			if(!GLSingleton::getInit())
			{
				return;
			}
			if(o.textureID == 0)
				return; //throw exception
			
			glGenTextures(1, &textureID);
			glBindTexture(GL_TEXTURE_2D, textureID);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, edgeBehavior);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, edgeBehavior);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterType);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterType);
			
			width = o.width;
			height = o.height;
			
			if(includesAlpha)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
			else
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

			glBindTexture(GL_TEXTURE_2D, 0);

			glCopyImageSubData(o.textureID, GL_TEXTURE_2D, 0, 0, 0, 0,
							   textureID, GL_TEXTURE_2D, 0, 0, 0, 0,
							   width, height, 1);
		}

		GLTexture::~GLTexture()
		{
			if(textureID != 0)
				glDeleteTextures(1, &textureID);

			textureID = 0;
		}

		void GLTexture::loadImage(const File& f, bool includeAlpha, GLTextureParams params)
		{
			int imgSize = 0;
			Image** imgPointer = Image::loadImage(f, &imgSize);

			if(imgSize > 0)
			{
				setImage(imgPointer[0], includeAlpha, params);
			}

			for(int i=0; i<imgSize; i++)
			{
				delete imgPointer[i];
			}
			delete[] imgPointer;
		}

		void GLTexture::setImage(Image* img, bool includeAlpha, GLTextureParams params)
		{
			edgeBehavior = params.edgeBehavior;
			filterType = params.filterType;
			includesAlpha = includeAlpha;

			if(img==nullptr)
				return;
			
			glGenTextures(1, &textureID);
			glBindTexture(GL_TEXTURE_2D, textureID);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, edgeBehavior);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, edgeBehavior);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterType);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterType);
			
			width = img->getWidth();
			height = img->getHeight();
			Color* data = img->getPixels();
			
			if(includeAlpha)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			}

			glBindTexture(GL_TEXTURE_2D, 0);
		}

		void GLTexture::bind(int location)
		{
			if(textureID == 0)
				return; //throw exception

			int textureLocation = MathExt::clamp(GL_TEXTURE0 + location, GL_TEXTURE0, GL_MAX_TEXTURE_UNITS);
			
			glActiveTexture(textureLocation);
			glBindTexture(GL_TEXTURE_2D, textureID);
		}

		int GLTexture::getWidth()
		{
			return width;
		}

		int GLTexture::getHeight()
		{
			return height;
		}

		
		Image* GLTexture::toImage()
		{
			if(textureID == 0)
				return nullptr;

			Image* imgPointer = new Image(width, height);
			
			glActiveTexture(0);
			glBindTexture(GL_TEXTURE_2D, textureID);
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgPointer->getPixels());
			glBindTexture(GL_TEXTURE_2D, 0);
			return imgPointer;
		}

	}
	
#endif