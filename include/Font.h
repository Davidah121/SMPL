#pragma once
#include "Image.h"
#include "Object.h"
#include "MathExt.h"

struct FontCharInfo
{
	int unicodeValue;
	int x;
	int y;
	int width;
	int height;
	int horizAdv;
};

class Font : public Object
{
public:
	Font();
	~Font();

	//Object and Class Stuff
	const Class* getClass();
	static const Class* myClass;
	
	FontCharInfo getFontCharInfo(int index);

	Image* getImage(int index);

	void setFontSize(int value);
	int getFontSize();

	void setFontTransform(Mat4f mat);
	Mat4f getFontTransform();

	std::vector<FontCharInfo> getListOfFontCharInfo();
	
	int getCharIndex(int c);
protected:
	std::vector<FontCharInfo> charInfoList = std::vector<FontCharInfo>();

    int maxHorizAdv = 0;
    int ascent = 0;
    int descent = 0;

	int fontSize = 0;
	Mat4f fontTransform = Mat4f::getIdentity();

private:
};

