#pragma once
#include"Image.h"
#include <string>
#include "Object.h"

namespace glib
{

	class Sprite : public Object
	{
	public:
		Sprite();
		~Sprite();
		Sprite(const Sprite& o);
		void operator=(const Sprite& o);

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;

		Image* getImage(int index);
		int getDelayTime(int index);
		int getSize();

		void addImage(Image* p, int microSecondsDelay = 0);
		void setDelayTime(int index, int microSecondsDelay);
		void removeImage(int index);

		void loadImage(std::string filename);
		void loadImage(std::wstring filename);

		void dispose();

	private:
		bool ownership = false;
		bool loops = true;
		std::vector<Image*> images = std::vector<Image*>();
		std::vector<int> delayTimeForFrame = std::vector<int>();
	};

} //NAMESPACE glib END