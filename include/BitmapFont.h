#pragma once
#include "Font.h"
#include "Sprite.h"

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
	void loadFT(std::string filename);
	void loadFNT(std::string filename);
	Sprite img = Sprite();

	std::vector<int> imgPage = std::vector<int>();
};

