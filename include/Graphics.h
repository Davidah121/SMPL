#pragma once
#include "Image.h"
#include "Font.h"

class Graphics
{
public:
	static void setImage(Image* img);
	static Image* getImage();

	//Drawing functions
	static void clearImage(Image* surf = nullptr);
	static void drawRect(int x, int y, int x2, int y2, bool outline, Image* surf = nullptr);
	static void drawLine(int x, int y, int x2, int y2, Image* surf = nullptr);
	static void drawCircle(int x, int y, int radius, bool outline, Image* surf = nullptr);

	static void drawSprite(Image* img, int x, int y, Image* surf = nullptr);
	static void drawSpritePart(Image* img, int x, int y, int imgX, int imgY, int imgW, int imgH, Image* surf = nullptr);

	static void drawText(std::string str, int x, int y, Image* surf = nullptr);

	static void setColor(Color c);
	static Color getColor();

	static void setFont(Font* f);
	static Font* getFont();

private:
	static Image* activeImage;
	static Font* activeFont;
	static Color activeColor;
};

