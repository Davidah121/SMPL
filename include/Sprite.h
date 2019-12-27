#pragma once
#include"Image.h"
#include <string>
#include "Object.h"

class Sprite : public Object
{
public:
	Sprite();
	~Sprite();

	//Object and Class Stuff
	const Class* getClass();
	static const Class* myClass;

	Image* getImage(int index);
	int getSize();

	void addImage(Image* p);
	void removeImage(int index);

	void loadImage(std::string filename);
	void loadImage(std::wstring filename);

private:
	std::vector<Image*> images = std::vector<Image*>();
};

