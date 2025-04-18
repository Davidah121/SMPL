#pragma once
#include "BuildOptions.h"
#include "Image.h"
#include "Shape.h"
#include "Font.h"

namespace smpl
{
	    
    class DLL_OPTION SurfaceInterface
	{
	public:
		static SurfaceInterface* createSoftwareSurface(int width, int height);
		static SurfaceInterface* createGLSurface(int width, int height);
		static SurfaceInterface* createDXSurface(int width, int height);
		
		SurfaceInterface(const SurfaceInterface& other);
		void operator=(const SurfaceInterface& other);
		~SurfaceInterface();

		void* getSurface();
		int getType();

		int getWidth();
		int getHeight();
	private:
		SurfaceInterface(int width, int height, unsigned char type);
		void* surface = nullptr;
		int type = -1;
	};

	class DLL_OPTION ImageInterface
	{
	public:
		static ImageInterface* createSoftwareImage(File f);
		static ImageInterface* createGLImage(File f);
		static ImageInterface* createDXImage(File f);

		ImageInterface(const ImageInterface& other);
		void operator=(const ImageInterface& other);
		void copy(const ImageInterface& other);
		~ImageInterface();

		void* getImage();
		int getType();

		int getWidth();
		int getHeight();
	private:
		friend class SpriteInterface;
		ImageInterface(File file, unsigned char type);
		ImageInterface();

		bool shouldDelete = true;
		void* image = nullptr;
		int type = -1;
	};

	class DLL_OPTION SpriteInterface
	{
	public:
		static SpriteInterface* createSoftwareSprite(File f);
		static SpriteInterface* createGLSprite(File f);
		static SpriteInterface* createDXSprite(File f);

		SpriteInterface(const SpriteInterface& other);
		void operator=(const SpriteInterface& other);
		void copy(const SpriteInterface& other);
		~SpriteInterface();

		void* getSprite();
		int getType();

		/**
		 * @brief Returns a shallow copy of the image stored by the sprite at the specified index.
		 * 		The shallow copy can be safely deleted without affecting the image or sprite but will not
		 * 		be valid when the sprite has been deleted.
		 * 
		 * @param index 
		 * @return ImageInterface 
		 */
		ImageInterface getImage(size_t index);
		int getDelayTime(size_t index);
		size_t getSize();
		bool shouldLoop();
	private:

		SpriteInterface(File file, unsigned char type);
		void* sprite = nullptr;
		int type = -1;
	};

	class DLL_OPTION FontInterface
	{
	public:
		static FontInterface* createSoftwareFont(File f);
		static FontInterface* createGLFont(File f);
		static FontInterface* createDXFont(File f);
		static FontInterface* createFromFont(Font* f, unsigned char type);

		FontInterface(const FontInterface& other);
		void operator=(const FontInterface& other);
		~FontInterface();
		Font* getFont();
		int getType();
	private:
	
		FontInterface(File file, unsigned char type);
		FontInterface(Font* f, unsigned char type);
		Font* font = nullptr;
		int type = -1;
		bool shouldDelete = true;
	};

	class DLL_OPTION GraphicsInterface
	{
	public:
		static const unsigned char TYPE_SOFTWARE = 0;
		static const unsigned char TYPE_OPENGL = 1;
		static const unsigned char TYPE_DIRECTX = 2;
		static const unsigned char TYPE_INVALID = -1;
		static const unsigned char TYPE_DEFAULT = 255;
		
		/**
		 * @brief Creates a SurfaceInterface object that aligns with this
		 * 		Graphics Interface.
		 * 
		 * @param width 
		 * @param height 
		 * @return SurfaceInterface* 
		 */
		static SurfaceInterface* createSurface(int width, int height);

		/**
		 * @brief Create a ImageInterface object that aligns with this
		 * 		Graphics Interface.
		 * 
		 * @param f 
		 * @return ImageInterface* 
		 */
		static ImageInterface* createImage(File f);

		/**
		 * @brief Create a SpriteInterface object that aligns with this
		 * 		Graphics Interface.
		 * 
		 * @param f 
		 * @return SpriteInterface* 
		 */
		static SpriteInterface* createSprite(File f);

		/**
		 * @brief Create a FontInterface object that aligns with this
		 * 		Graphics Interface.
		 * 
		 * @param f 
		 * @return FontInterface* 
		 */
		static FontInterface* createFont(File f);

		static unsigned char getDefaultType();
		static void setDefaultType(unsigned char type);

		/**
		 * @brief Gets the mode for the graphics interface. If the input is TYPE_DEFAULT, the default
		 * 		mode is returned. Otherwise, the input is returned.
		 * 
		 * @param v 
		 * @return unsigned char 
		 */
		static unsigned char getType();
		
		static void setType(unsigned char v);

		/**
		 * @brief Delete memory that this class allocated that has not been deleted yet.
		 * 
		 */
		static void dispose();

		static void setBoundSurface(SurfaceInterface* surface);
		static SurfaceInterface* getBoundSurface();

		static void setColor(Vec4f color);
		static void setColor(Color color);
		static Color getColor();
		static Vec4f getColorVec4f();

		static void setFont(FontInterface* f);
		static FontInterface* getFont();

		static void clear();

		static void drawRect(int x, int y, int width, int height, bool outline);
		static void drawLine(int x, int y, int x2, int y2);
		static void drawCircle(int x, int y, int radius, bool outline);
		static void drawEllipse(int x, int y, int xRad, int yRad, bool outline);

		static void drawSprite(ImageInterface* img, int x, int y);
		static void drawSprite(ImageInterface* img, int x1, int y1, int width, int height);
		static void drawSpritePart(ImageInterface* img, int x, int y, int imgX, int imgY, int imgW, int imgH);

		static void drawText(StringBridge strBridge, int x, int y);
		static void drawTextHighlighted(StringBridge strBridge, int x, int y, int highlightStart, int highlightEnd, Color highlightColor);
		static void drawTextHighlighted(StringBridge strBridge, int x, int y, int highlightStart, int highlightEnd, Vec4f highlightColor);
		static void drawTextLimits(StringBridge strBridge, int x, int y, int maxWidth, int maxHeight, char wrapMode);
		static void drawTextLimitsHighlighted(StringBridge strBridge, int x, int y, int maxWidth, int maxHeight, char wrapMode, int highlightStart, int highlightEnd, Color highlightColor);
		static void drawTextLimitsHighlighted(StringBridge strBridge, int x, int y, int maxWidth, int maxHeight, char wrapMode, int highlightStart, int highlightEnd, Vec4f highlightColor);
		
		static void setClippingRect(Box2D b);
		static Box2D getClippingRect();
		static void resetClippingPlane();

		static void drawSurface(SurfaceInterface* img, int x, int y);
		static void drawSurface(SurfaceInterface* img, int x1, int y1, int width, int height); //TODO

		static void drawToScreen();
		static void setProjection(Mat4f proj);
		static void setOrthoProjection(int width, int height);

		static Vec2f getTranslationFactor();
		static void setTranslationFactor(Vec2f v);
		
		static Vec2f getScalingFactor();
		static void setScalingFactor(Vec2f v);
		static void enableScaling(bool v);
		
	private:

		static unsigned char type;
		static unsigned char defaultType;
		static SurfaceInterface* boundSurface;
		static FontInterface* boundFont;
		static bool ownedFont;
		static Box2D clippingRect;
		static Vec2f scalingFactor;
		static Vec2f translationFactor;
		static bool useScaling;
	};
}