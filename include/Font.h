#pragma once
#include "BuildOptions.h"
#include "StandardTypes.h"
#include "Image.h"
#include "SimpleSerialization.h"
#include "MathExt.h"
#include "GeneralExceptions.h"
#include "Shape.h"
#include "StringBridge.h"
#include "SmartMemory.h"

namespace smpl
{

	struct FontCharInfo
	{
		uint32_t unicodeValue = 0; 	//The actual unicode value of interest
		uint32_t x = 0;				//The x value in the image if it has an image associated with it. Will be zero otherwise
		uint32_t y = 0; 			//The y value in the image if it has an image associated with it. Will be zero otherwise
		uint32_t width = 0;			//The width of the glyph
		uint32_t height = 0;		//The height of the glyph
		int horizAdv = 0;			//The amount to move horizontally after inserting this character. Width is already included and should not be added with this.
		int xOffset = 0;			//The x offset used to position the character when rendering
		int yOffset = 0;			//The y offset used to position the character when rendering
		size_t reserved = 0;		//A reserved value allowing for specific internal implementation.
	};

	struct FontCharBoxInfo
	{
		size_t charIndex;			//The index of the character in the string (redundant)
		int rowStartPosition;		//The y value of the row its on. Offsets exist in the bounding box where applicable. This value does not have that. (A little redundant)
		Box2D boundingBox;			//The actual bounding box of the character
	};
	
	struct FontCharInfoHash
	{
		using is_transparent = void;
		size_t operator()(const FontCharInfo& info) const
		{
			return rapidhash_wrapper(info.unicodeValue);
		}
		size_t operator()(const uint32_t& unicodeValue) const
		{
			return rapidhash_wrapper(unicodeValue);
		}
	};
	struct FontCharInfoEquals
	{
		using is_transparent = void;
		bool operator()(const FontCharInfo& info, const FontCharInfo& info2) const
		{
			return info.unicodeValue == info2.unicodeValue;
		}
		bool operator()(const FontCharInfo& info, const uint32_t& unicodeValue) const
		{
			return info.unicodeValue == unicodeValue;
		}
		bool operator()(const uint32_t& unicodeValue, const FontCharInfo& info) const
		{
			return info.unicodeValue == unicodeValue;
		}
	};

	class DLL_OPTION Font
	{
	public:
		static const char NO_WRAP = 0;
		static const char CHARACTER_WRAP = 1;
		static const char WORD_WRAP = 2;
		
		/**
		 * @brief Creates a Font Object.
		 * 		This class should never be directly instantiated.
		 * 		It is designed to be an Abstract class that will be extended to other classes.
		 */
		Font();
		Font(const Font& other) = default;
		Font& operator=(const Font& other) = default;
		Font(Font&& other) = default;
		Font& operator=(Font&& other) = default;
		

		/**
		 * @brief
		 * 		Destroys a Font Object.
		 */
		virtual ~Font();
		
		/**
		 * @brief Gets the character information for rendering.
		 * 		The values are adjusted based on the current font size vs the original.
		 * 		Assumes the image for each character, or a character sheet, is also scaled.
		 * 		If index is invalid and USE_EXCEPTIONS is defined, throws an OutOfBoundsError
		 * @param c
		 * 		The character you are searching for.
		 * @return FontCharInfo
		 * 		Returns a valid FontCharInfo struct if successful.
		 * 		Otherwise, returns an empty structure where all values are 0.
		 */
		FontCharInfo getFontCharInfo(uint32_t c);

		/**
		 * @brief A virtual function to return the image that contains the character glyph.
		 * 		The default implementation returns a nullptr.
		 * @param index
		 * 		The index of the character wanted.
		 * @return Image*
		 * 		Always returns a nullptr.
		 */
		virtual SmartMemory<Image> getImage(const FontCharInfo& fci);
		
		/**
		 * @brief Sets the size of the font.
		 * 		This is used for rendering.
		 *		This value is with respect to px (pixels) and not pt or em or anything else.
		 *			Note that while it varies from font to font, the final scaled amount may not result
		 *			in any particular character at the set size though the assumption is that the height of box that surrounds
		 *			all possible characters is adjusted down to the desired font size.
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
		 * 		It is adjusted by the current font size vs the original font size.
		 * @return int
		 */
		int getVerticalAdvance();

		/**
		 * @brief Returns a bounding box that surrounds the text based on the given parameters.
		 * 		Note that when a line break or wrap text occurs, it does not carry the whole word to the next line.
		 * 		Wrapped text can be removed by setting max width to a negative value.
		 * 		It is adjusted by the current font size vs the original font size.
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
		Box2D getBoundingBox(const StringBridge& textBridge, unsigned int maxWidth, char wrapMode);

		/**
		 * @brief Gets the bounding of the caret cursor box.
		 * 		The width of the box will always be 1 and may extend outside the allowed bounds specified by maxWidth.
		 * 
		 * @param textBridge 
		 * @param maxWidth 
		 * @param wrapMode 
		 * @param index 
		 * @return Box2D 
		 */
		Box2D getCaretBox(const StringBridge& textBridge, unsigned int maxWidth, char wrapMode, size_t index);

		/**
		 * @brief Get the Select Index at the specified position.
		 * 		Assumes that the start position of the text is at (0,0).
		 * 		Returns the maximum of a size_t for invalid cases.
		 * 
		 * @param text 
		 * @param maxWidth 
		 * 		Used for text wrapping.
		 * @param x 
		 * @param y 
		 * @return size_t 
		 */
		size_t getSelectIndex(const StringBridge& textBridge, unsigned int maxWidth, char wrapMode, int x, int y);

		/**
		 * @brief Gets the selection index if the cursor were moved directly up.
		 * 		When moving up, it moves to the character whose bound is less than or equal to the cursor location.
		 * 
		 * @param textBridge 
		 * @param maxWidth 
		 * @param wrapMode 
		 * @param startSelectIndex 
		 * @return size_t 
		 */
		size_t moveSelectionUp(const StringBridge& textBridge, unsigned int maxWidth, char wrapMode, size_t startSelectIndex);

		/**
		 * @brief Gets the selection index if the cursor were moved directly down.
		 * 		When moving down, it moves to the character whose bound is greater than or equal to the cursor location.
		 * 
		 * @param textBridge 
		 * @param maxWidth 
		 * @param wrapMode 
		 * @param startSelectIndex 
		 * @return size_t 
		 */
		size_t moveSelectionDown(const StringBridge& textBridge, unsigned int maxWidth, char wrapMode, size_t startSelectIndex);
		
		/**
		 * @brief Returns the raw list of the font characters information.
		 * 		It is not adjusted by the set font size. Each FontCharInfo must be adjusted
		 * 		separately.
		 * @return SimpleHashSet<FontCharInfo, FontCharInfoHash, FontCharInfoEquals>&
		 */
		SimpleHashSet<FontCharInfo, FontCharInfoHash, FontCharInfoEquals>& getListOfFontCharInfo();

		/**
		 * @brief Gets the bounding box for each character in the string. Useful for rendering purposes and selection purposes.
		 * 		Features 3 modes of operation. 
		 * 			NO_WRAP
		 * 			CHARACTER_WRAP
		 * 			WORD_WRAP
		 * 		Note that maxWidth is ignored in the NO_WRAP case but required to be > 0 in all other cases.
		 * 			Note that in WORD_WRAP, if the entire word can not fit on a single line, it will switch to CHARACTER_WRAP temporarily.
		 * 		
		 * @param text 
		 * @param maxWidth 
		 * @param wrapMode 
		 * @return std::vector<Box2D> 
		 */
		std::vector<FontCharBoxInfo> getAllCharBoxes(const StringBridge& textBridge, unsigned int maxWidth, char wrapMode);
		
		/**
		 * @brief Converts font sizes in pt to px.
		 *		It is calculated as 1.25 multiplier.
		 * 
		 */
		static int ptToPx();
	protected:
		SimpleHashSet<FontCharInfo, FontCharInfoHash, FontCharInfoEquals> charInfoSet;

		/**
		 * @brief Adds character to the list.
		 * 		Sets sorted to false.
		 * 		Should be used instead of modifying the charInfoList directly.
		 * @param a
		 * 		The information for the character.
		 */
		void addChar(const FontCharInfo& a);

		int maxHorizAdv = 0;
		int ascent = 0;
		int descent = 0;
		int verticalAdv = 0;

		int fontSize = 0;
		int originalFontSize = 0;

	private:
		bool sorted = false;
	};

}  //NAMESPACE smpl END
