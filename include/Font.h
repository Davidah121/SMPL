#pragma once
#include "Image.h"
#include "Object.h"
#include "MathExt.h"

namespace glib
{

	struct FontCharInfo
	{
		int unicodeValue;
		int x;
		int y;
		int width;
		int height;
		int horizAdv;
		int xOffset;
		int yOffset;
	};

	class Font : public Object
	{
	public:
		Font();
		~Font();

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;
		
		FontCharInfo getFontCharInfo(int index);

		virtual Image* getImage(int index);

		void setFontSize(int value);
		int getFontSize();

		int getVerticalAdvance();

		void setFontTransform(Mat4f mat);
		Mat4f getFontTransform();

		int getWidthOfString(std::string text);
		int getWidthOfString(std::wstring text);

		std::vector<FontCharInfo> getListOfFontCharInfo();
		
		int getCharIndex(int c);
	protected:
		std::vector<FontCharInfo> charInfoList = std::vector<FontCharInfo>();

		int maxHorizAdv = 0;
		int ascent = 0;
		int descent = 0;
		int verticalAdv = 0;

		int fontSize = 0;
		Mat4f fontTransform = Mat4f::getIdentity();

	private:
	};

}  //NAMESPACE glib END
