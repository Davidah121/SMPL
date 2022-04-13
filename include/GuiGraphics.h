#pragma once

#include "Image.h"
#include "Shape.h"
#include "Font.h"

namespace glib
{
	    
    class GuiSurfaceInterface
	{
	public:
		static GuiSurfaceInterface* createSoftwareSurface(int width, int height);
		static GuiSurfaceInterface* createGLSurface(int width, int height);
		
		GuiSurfaceInterface(const GuiSurfaceInterface& other);
		void operator=(const GuiSurfaceInterface& other);
		~GuiSurfaceInterface();

		void* getSurface();
		int getType();

		int getWidth();
		int getHeight();
	private:
		GuiSurfaceInterface(int width, int height, unsigned char type);
		void* surface = nullptr;
		int type = -1;
	};

	class GuiImageInterface
	{
	public:
		static GuiImageInterface* createSoftwareImage(File f);
		static GuiImageInterface* createGLImage(File f);

		GuiImageInterface(const GuiImageInterface& other);
		void operator=(const GuiImageInterface& other);
		~GuiImageInterface();

		void* getImage();
		int getType();

		int getWidth();
		int getHeight();
	private:
		friend class GuiSpriteInterface;
		GuiImageInterface(File file, unsigned char type);
		GuiImageInterface();

		void* image = nullptr;
		int type = -1;
	};

	class GuiSpriteInterface
	{
	public:
		static GuiSpriteInterface* createSoftwareSprite(File f);
		static GuiSpriteInterface* createGLSprite(File f);

		GuiSpriteInterface(const GuiSpriteInterface& other);
		void operator=(const GuiSpriteInterface& other);
		~GuiSpriteInterface();

		void* getSprite();
		int getType();

		GuiImageInterface* getImage(int index);
		int getDelayTime(int index);
		int getSize();
		bool shouldLoop();
	private:

		GuiSpriteInterface(File file, unsigned char type);
		void* sprite = nullptr;
		int type = -1;
	};

	class GuiFontInterface
	{
	public:
		static GuiFontInterface* createSoftwareFont(File f);
		static GuiFontInterface* createGLFont(File f);
		static GuiFontInterface* createFromFont(Font* f, unsigned char type);

		GuiFontInterface(const GuiFontInterface& other);
		void operator=(const GuiFontInterface& other);
		~GuiFontInterface();
		Font* getFont();
		int getType();
	private:
	
		GuiFontInterface(File file, unsigned char type);
		GuiFontInterface(Font* f, unsigned char type);
		Font* font = nullptr;
		int type = -1;
		bool shouldDelete = true;
	};

	class GuiGraphicsInterface
	{
	public:
		static const unsigned char TYPE_SOFTWARE = 0;
		static const unsigned char TYPE_OPENGL = 1;
		static const unsigned char TYPE_INVALID = -1;
		
		GuiGraphicsInterface();
		GuiGraphicsInterface(unsigned char v);
		~GuiGraphicsInterface();

		/**
		 * @brief Creates a GuiSurfaceInterface object that aligns with this
		 * 		Graphics Interface.
		 * 
		 * @param width 
		 * @param height 
		 * @return GuiSurfaceInterface* 
		 */
		GuiSurfaceInterface* createSurface(int width, int height);

		/**
		 * @brief Create a GuiImageInterface object that aligns with this
		 * 		Graphics Interface.
		 * 
		 * @param f 
		 * @return GuiImageInterface* 
		 */
		GuiImageInterface* createImage(File f);

		/**
		 * @brief Create a GuiSpriteInterface object that aligns with this
		 * 		Graphics Interface.
		 * 
		 * @param f 
		 * @return GuiSpriteInterface* 
		 */
		GuiSpriteInterface* createSprite(File f);

		/**
		 * @brief Create a GuiFontInterface object that aligns with this
		 * 		Graphics Interface.
		 * 
		 * @param f 
		 * @return GuiFontInterface* 
		 */
		GuiFontInterface* createFont(File f);

		unsigned char getType();

		void setBoundSurface(GuiSurfaceInterface* surface);
		GuiSurfaceInterface* getBoundSurface();

		void setColor(Vec4f color);
		void setColor(Color color);
		Color getColor();
		Vec4f getColorVec4f();

		void setFont(GuiFontInterface* f);
		GuiFontInterface* getFont();

		void clear();

		void drawRect(int x, int y, int x2, int y2, bool outline);
		void drawLine(int x, int y, int x2, int y2);
		void drawCircle(int x, int y, int radius, bool outline);

		void drawSprite(GuiImageInterface* img, int x, int y);
		void drawSprite(GuiImageInterface* img, int x1, int y1, int x2, int y2);
		void drawSpritePart(GuiImageInterface* img, int x, int y, int imgX, int imgY, int imgW, int imgH);

		void drawText(std::string str, int x, int y);
		void drawText(std::wstring str, int x, int y);

		void drawTextLimits(std::wstring str, int x, int y, int maxWidth, int maxHeight, bool useLineBreak);
		void drawTextLimits(std::string str, int x, int y, int maxWidth, int maxHeight, bool useLineBreak);
		
		void drawTextLimitsHighlighted(std::wstring str, int x, int y, int maxWidth, int maxHeight, bool useLineBreak, int highlightStart, int highlightEnd, Color highlightColor);
		void drawTextLimitsHighlighted(std::string str, int x, int y, int maxWidth, int maxHeight, bool useLineBreak, int highlightStart, int highlightEnd, Color highlightColor);
		
		void drawTextLimitsHighlighted(std::wstring str, int x, int y, int maxWidth, int maxHeight, bool useLineBreak, int highlightStart, int highlightEnd, Vec4f highlightColor);
		void drawTextLimitsHighlighted(std::string str, int x, int y, int maxWidth, int maxHeight, bool useLineBreak, int highlightStart, int highlightEnd, Vec4f highlightColor);
		
		void setClippingRect(Box2D b);
		Box2D getClippingRect();
		void resetClippingPlane();

		void drawSurface(GuiSurfaceInterface* img, int x, int y);
		void drawSurface(GuiSurfaceInterface* img, int x1, int y1, int x2, int y2);

		void drawToScreen();
		void setProjection(Mat4f proj);
		void setOrthoProjection(int width, int height);
		
	private:
		unsigned char type = TYPE_INVALID;
		GuiSurfaceInterface* boundSurface = nullptr;
		Box2D clippingRect = Box2D(0, 0, 65535, 65535);
	};
}