#pragma once
#include "Font.h"
#include "Sprite.h"
#include "GeneralExceptions.h"

namespace glib
{
	
	class BitmapFont : public Font
	{
	public:
		BitmapFont(std::string filename);
		~BitmapFont();

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;

		struct InvalidFileFormat : public std::exception
		{
			const char* what() noexcept { return "File format can not be read."; }
		};

		
		Image* getImage(int index);
	private:
		void loadFT(std::string filename);
		void loadFNT(std::string filename);
		Sprite img = Sprite();

		std::vector<int> imgPage = std::vector<int>();
	};

}  //NAMESPACE glib END