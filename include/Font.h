#pragma once
#include "Image.h"
#include "Object.h"
#include "MathExt.h"
#include "GeneralExceptions.h"
#include "Shape.h"

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

		/**
		 * @brief Copies all font information from another Font object.
		 * 		Does not copy information specific to a particular implementation
		 * 		of the Font class.
		 * 
		 * @param f 
		 */
		void copyFont(Font& f);

		//Object and Class Stuff
		static const Class globalClass;
		
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
		 * @brief Returns a bounding box that surrounds the text based on the given parameters.
		 * 		Note that when a line break or wrap text occurs, it does not carry the whole word to the next line.
		 * 		Wrapped text can be removed by setting max width to a negative value.
		 * 
		 * @param text 
		 * @param maxWidth 
		 * 		Sets a maximum width up to 2^31 - 1. A negative value means no maximum width.
		 * 		If the string exceeds the maximum width, it moves to a new line.
		 * @param maxHeight 
		 * 		Sets a maximum height up to 2^31 - 1. A negative value means no maximum height.
		 * 		If the string exceeds the maximum height, it stops.
		 * @return Box2D 
		 */
		Box2D getBoundingBox(std::string text, int maxWidth, int maxHeight);

		/**
		 * @brief Returns a bounding box that surrounds the text based on the given parameters.
		 * 		Note that when a line break or wrap text occurs, it does not carry the whole word to the next line.
		 * 		Wrapped text can be removed by setting max width to a negative value.
		 * 
		 * @param text 
		 * @param maxWidth 
		 * 		Sets a maximum width up to 2^31 - 1. A negative value means no maximum width.
		 * 		If the string exceeds the maximum width, it moves to a new line.
		 * @param maxHeight 
		 * 		Sets a maximum height up to 2^31 - 1. A negative value means no maximum height.
		 * 		If the string exceeds the maximum height, it stops.
		 * @return Box2D 
		 */
		Box2D getBoundingBox(std::wstring text, int maxWidth, int maxHeight);

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
