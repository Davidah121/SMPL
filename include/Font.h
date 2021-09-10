#pragma once
#include "Image.h"
#include "Object.h"
#include "MathExt.h"
#include "GeneralExceptions.h"

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
		/**
		 * @brief Creates a Font Object.
		 * 		This class should never be directly instantiated.
		 * 		It is designed to be an Abstract class that will be extended to other classes.
		 */
		Font();

		/**
		 * @brief
		 * 		Destroys a Font Object.
		 */
		~Font();

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;
		
		/**
		 * @brief Gets the character information for rendering.
		 * 		If index is invalid and USE_EXCEPTIONS is defined, throws an OutOfBoundsError
		 * @param index
		 * 		The index of the character in the array. Not necessarily in order.
		 * 		The index of the character should be obtained from getCharIndex()
		 * @return FontCharInfo
		 * 		Returns a valid FontCharInfo struct if successful.
		 * 		Otherwise, returns an empty structure where all values are 0.
		 */
		FontCharInfo getFontCharInfo(int index);

		/**
		 * @brief Gets the index of the desired character.
		 * 		Takes O(N) time to find the character if the list is unsorted.
		 * 		Takes O(LogN) time to find the character if the list is sorted.
		 * @param c
		 * 		The character to search for.
		 * @return FontCharInfo
		 * 		Returns a valid FontCharInfo struct if successful.
		 * 		Otherwise, returns an empty structure where all values are 0.
		 */
		int getCharIndex(int c);

		/**
		 * @brief A virtual function to return the image that contains the character glyph.
		 * 		The default implementation returns a nullptr.
		 * @param index
		 * 		The index of the character wanted.
		 * @return Image*
		 * 		Always returns a nullptr.
		 */
		virtual Image* getImage(int index);

		/**
		 * @brief Sets the size of the font.
		 * 		This is used for rendering.
		 * @param value
		 * 		The value to set the fontSize to.
		 */
		void setFontSize(int value);

		/**
		 * @brief Returns the size of the font.
		 * @return int
		 */
		int getFontSize();

		/**
		 * @brief Returns the original size of the font.
		 * @return int
		 */
		int getOriginalFontSize();

		/**
		 * @brief Returns the vertical advance of the font.
		 * @return int
		 */
		int getVerticalAdvance();

		/**
		 * @brief Returns the width of the string.
		 * 		This width is the space taken up when drawn.
		 * @param text
		 * 		The string to find the width of.
		 * @return int
		 * 		Returns the width.
		 */
		int getWidthOfString(std::string text);

		/**
		 * @brief Returns the width of the string.
		 * 		This width is the space taken up when drawn.
		 * @param text
		 * 		The string to find the width of.
		 * @return int
		 * 		Returns the width.
		 */
		int getWidthOfString(std::wstring text);

		/**
		 * @brief Returns the raw list of the font characters information.
		 * @return std::vector<FontCharInfo>&
		 */
		std::vector<FontCharInfo>& getListOfFontCharInfo();
		
	protected:
		std::vector<FontCharInfo> charInfoList = std::vector<FontCharInfo>();

		/**
		 * @brief Allows you to sort the list to maintain LogN search time.
		 * @param insertionSort
		 * 		Chooses whether to use insertion sort or merge sort for sorting.
		 * 		Insertion Sort is best used when the list is mostly sorted.
		 */
		void sortList(bool insertionSort);

		/**
		 * @brief Adds character to the list.
		 * 		Sets sorted to false.
		 * 		Should be used instead of modifying the charInfoList directly.
		 * @param a
		 * 		The information for the character.
		 */
		void addChar(FontCharInfo a);

		int maxHorizAdv = 0;
		int ascent = 0;
		int descent = 0;
		int verticalAdv = 0;

		int fontSize = 0;
		int originalFontSize = 0;

	private:
		bool sorted = false;
	};

}  //NAMESPACE glib END
