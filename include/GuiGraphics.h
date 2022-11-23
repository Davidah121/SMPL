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
		void copy(const GuiImageInterface& other);
		~GuiImageInterface();

		void* getImage();
		int getType();

		int getWidth();
		int getHeight();
	private:
		friend class GuiSpriteInterface;
		GuiImageInterface(File file, unsigned char type);
		GuiImageInterface();

		bool shouldDelete = true;
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
		void copy(const GuiSpriteInterface& other);
		~GuiSpriteInterface();

		void* getSprite();
		int getType();

		/**
		 * @brief Returns a shallow copy of the image stored by the sprite at the specified index.
		 * 		The shallow copy can be safely deleted without affecting the image or sprite but will not
		 * 		be valid when the sprite has been deleted.
		 * 
		 * @param index 
		 * @return GuiImageInterface 
		 */
		GuiImageInterface getImage(int index);
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
		static const unsigned char TYPE_DEFAULT = 255;
		
		/**
		 * @brief Creates a GuiSurfaceInterface object that aligns with this
		 * 		Graphics Interface.
		 * 
		 * @param width 
		 * @param height 
		 * @return GuiSurfaceInterface* 
		 */
		static GuiSurfaceInterface* createSurface(int width, int height, unsigned char type = TYPE_DEFAULT);

		/**
		 * @brief Create a GuiImageInterface object that aligns with this
		 * 		Graphics Interface.
		 * 
		 * @param f 
		 * @return GuiImageInterface* 
		 */
		static GuiImageInterface* createImage(File f, unsigned char type = TYPE_DEFAULT);

		/**
		 * @brief Create a GuiSpriteInterface object that aligns with this
		 * 		Graphics Interface.
		 * 
		 * @param f 
		 * @return GuiSpriteInterface* 
		 */
		static GuiSpriteInterface* createSprite(File f, unsigned char type = TYPE_DEFAULT);

		/**
		 * @brief Create a GuiFontInterface object that aligns with this
		 * 		Graphics Interface.
		 * 
		 * @param f 
		 * @return GuiFontInterface* 
		 */
		static GuiFontInterface* createFont(File f, unsigned char type = TYPE_DEFAULT);

		static unsigned char getDefaultType();
		static void setDefaultType(unsigned char type);

		/**
		 * @brief Gets the mode for the graphics interface. If the input is TYPE_DEFAULT, the default
		 * 		mode is returned. Otherwise, the input is returned.
		 * 
		 * @param v 
		 * @return unsigned char 
		 */
		static unsigned char getType(unsigned char v);

		/**
		 * @brief Delete memory that this class allocated that has not been deleted yet.
		 * 
		 */
		static void dispose();

		static void setBoundSurface(GuiSurfaceInterface* surface);
		static GuiSurfaceInterface* getBoundSurface();

		static void setColor(Vec4f color, unsigned char type = TYPE_DEFAULT);
		static void setColor(Color color, unsigned char type = TYPE_DEFAULT);
		static Color getColor(unsigned char type = TYPE_DEFAULT);
		static Vec4f getColorVec4f(unsigned char type = TYPE_DEFAULT);

		static void setFont(GuiFontInterface* f, unsigned char type = TYPE_DEFAULT);
		static GuiFontInterface* getFont(unsigned char type = TYPE_DEFAULT);

		static void clear(unsigned char type = TYPE_DEFAULT);

		static void drawRect(int x, int y, int x2, int y2, bool outline, unsigned char type = TYPE_DEFAULT);
		static void drawLine(int x, int y, int x2, int y2, unsigned char type = TYPE_DEFAULT);
		static void drawCircle(int x, int y, int radius, bool outline, unsigned char type = TYPE_DEFAULT);
		static void drawEllipse(int x, int y, int xRad, int yRad, bool outline, unsigned char type = TYPE_DEFAULT);

		static void drawSprite(GuiImageInterface* img, int x, int y, unsigned char type = TYPE_DEFAULT);
		static void drawSprite(GuiImageInterface* img, int x1, int y1, int x2, int y2, unsigned char type = TYPE_DEFAULT);
		static void drawSpritePart(GuiImageInterface* img, int x, int y, int imgX, int imgY, int imgW, int imgH, unsigned char type = TYPE_DEFAULT);

		static void drawText(std::string str, int x, int y, unsigned char type = TYPE_DEFAULT);
		static void drawText(std::wstring str, int x, int y, unsigned char type = TYPE_DEFAULT);

		static void drawTextLimits(std::wstring str, int x, int y, int maxWidth, int maxHeight, bool useLineBreak, unsigned char type = TYPE_DEFAULT);
		static void drawTextLimits(std::string str, int x, int y, int maxWidth, int maxHeight, bool useLineBreak, unsigned char type = TYPE_DEFAULT);
		
		static void drawTextLimitsHighlighted(std::wstring str, int x, int y, int maxWidth, int maxHeight, bool useLineBreak, int highlightStart, int highlightEnd, Color highlightColor, unsigned char type = TYPE_DEFAULT);
		static void drawTextLimitsHighlighted(std::string str, int x, int y, int maxWidth, int maxHeight, bool useLineBreak, int highlightStart, int highlightEnd, Color highlightColor, unsigned char type = TYPE_DEFAULT);
		
		static void drawTextLimitsHighlighted(std::wstring str, int x, int y, int maxWidth, int maxHeight, bool useLineBreak, int highlightStart, int highlightEnd, Vec4f highlightColor, unsigned char type = TYPE_DEFAULT);
		static void drawTextLimitsHighlighted(std::string str, int x, int y, int maxWidth, int maxHeight, bool useLineBreak, int highlightStart, int highlightEnd, Vec4f highlightColor, unsigned char type = TYPE_DEFAULT);
		
		static void setClippingRect(Box2D b, unsigned char type = TYPE_DEFAULT);
		static Box2D getClippingRect();
		static void resetClippingPlane(unsigned char type = TYPE_DEFAULT);

		static void drawSurface(GuiSurfaceInterface* img, int x, int y, unsigned char type = TYPE_DEFAULT);
		static void drawSurface(GuiSurfaceInterface* img, int x1, int y1, int x2, int y2, unsigned char type = TYPE_DEFAULT);

		static void drawToScreen(unsigned char type = TYPE_DEFAULT);
		static void setProjection(Mat4f proj, unsigned char type = TYPE_DEFAULT);
		static void setOrthoProjection(int width, int height, unsigned char type = TYPE_DEFAULT);
		static void setScalingFactor(Vec2f v);
		static void enableScaling(bool v);
		
	private:

		static unsigned char type;
		static GuiSurfaceInterface* boundSurface;
		static GuiFontInterface* boundFont;
		static bool ownedFont;
		static Box2D clippingRect;
		static Vec2f scalingFactor;
		static bool useScaling;
	};
}