#pragma once
#include "Image.h"
#include "Object.h"

class Font : public Object
{
public:
	Font(std::string filename);
	~Font();

	//Object and Class Stuff
	const Class* getClass();
	static const Class* myClass;
	
	int getCharX(int c);
	int getCharY(int c);

	int getCharWidth(int c);
	int getCharHeight(int c);

	Image* getImage();
private:

	Image* img = nullptr;

	int getCharIndex(int c);

	std::vector<int> charNum = std::vector<int>();
	std::vector<int> charX = std::vector<int>();
	std::vector<int> charY = std::vector<int>();
	std::vector<int> charWidth = std::vector<int>();
	std::vector<int> charHeight = std::vector<int>();

	
};

