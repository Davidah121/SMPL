#pragma once
#include "BuildOptions.h"
#include "Font.h"
#include "Sprite.h"
#include "GeneralExceptions.h"

namespace smpl
{
	
	class DLL_OPTION BitmapFont : public Font
	{
	public:
		/**
		 * @brief Creates a BitmapFont Object.
		 * 		BitmapFont extends from the Font class and as the name suggest, it uses an Image to
		 * 		store and display characters.
		 * 		Two file types are supported. .ft and .fnt
		 * 		.ft is a custom format for font that uses a relatively simple format to store information about each character.
		 * 		.fnt is a format used by BitmapFontGenerator located here: https://www.angelcode.com/products/bmfont
		 * 			Note that only the xml file format is supported for .fnt
		 * @param filename
		 * 		The name of the file to load for font data.
		 */
		BitmapFont(File file);

		/**
		 * @brief Destroys a BitmapFont Object.
		 */
		~BitmapFont();

		struct InvalidFileFormat : public std::exception
		{
			const char* what() const noexcept { return "File format can not be read."; }
		};

		/**
		 * @brief Get the image where the desired character is stored.
		 * 		The image can contain other characters than just the desired character.
		 * 		If the index is out of range and USE_EXCEPTIONS is defined, an OutOfBounds Error is thrown.
		 * 		If not, a nullptr is returned.
		 * @param index
		 * 		The desired character.
		 * @return Image*
		 * 		A nullptr is returned if no image was found. Otherwise, a valid Image* is returned.
		 */
		Image* getImage(size_t index);
		
		/**
		 * @brief Gets the internal Sprite used to store the image data for the characters.
		 * 
		 * @return Sprite* 
		 */
		Sprite* getSprite();
		
		/**
		 * @brief Gets the internal data specifying what image in the sprite a character is located in.
		 * 
		 * @return std::vector<size_t> 
		 */
		std::vector<size_t> getImagePages();

	private:
		void loadFT(File file);
		void loadFNT(File file);

		void init(File file);
		Sprite img = Sprite();

		std::vector<size_t> imgPage = std::vector<size_t>();

		SERIALIZE_SUPER_CLASS(Font)
		SERIALIZE_CLASS()
	};

}  //NAMESPACE smpl END