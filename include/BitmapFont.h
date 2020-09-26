#pragma once
#include "Font.h"
#include "Image.h"

class BitmapFont : public Font
{
public:
	BitmapFont(std::string filename);
	~BitmapFont();

	//Object and Class Stuff
	const Class* getClass();
	static const Class* myClass;
	
	Image* getImage(int index);
private:
	Image* img = nullptr;
};

