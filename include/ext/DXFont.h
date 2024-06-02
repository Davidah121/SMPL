#pragma once

#ifdef USE_DIRECTX

	#include "MathExt.h"

	#include "ext/DXSingleton.h"
	#include "ext/DXSprite.h"
	#include "ext/DXModel.h"
	#include "BitmapFont.h"

	namespace smpl
	{
		class DXFont : public Font
		{
		public:
			/**
			 * @brief Creates a DXBitmapFont Object.
			 * 		DXBitmapFont extends from the Font class and as the name suggest, it uses an Image to
			 * 		store and display characters.
			 * 		Two file types are supported. .ft and .fnt
			 * 		.ft is a custom format for font that uses a relatively simple format to store information about each character.
			 * 		.fnt is a format used by BitmapFontGenerator located here: https://www.angelcode.com/products/bmfont
			 * 			Note that only the xml file format is supported for .fnt
			 * @param filename
			 * 		The name of the file to load for font data.
			 */
			DXFont(File file);

			/**
			 * @brief Destroys a DXBitmapFont Object.
			 */
			~DXFont();
			
			//Object and RootClass Stuff
			static const RootClass globalClass;
			virtual const RootClass* getClass();

			/**
			 * @brief Get the texture where the desired character is stored.
			 * 		The texture can contain other characters than just the desired character.
			 * 		If the index is out of range and USE_EXCEPTIONS is defined, an OutOfBounds Error is thrown.
			 * 		If not, a nullptr is returned.
			 * @param index
			 * 		The desired character.
			 * @return Image*
			 * 		A nullptr is returned if no texture was found. Otherwise, a valid DXTexture* is returned.
			 */
			DXTexture* getTexture(int index);

			/**
			 * @brief Gets the Model for the specified character.
			 * 		This model will be the width and height of the character and will
			 * 		also have the correct texture coordinates.
			 * 		If nullptr is returned, the font does not have data for the character.
			 * 
			 * @param index 
			 * 		The desired character.
			 * @return DXModel* 
			 */
			DXModel* getModel(int index);
			
		private:

			void convertBitmapFont(BitmapFont& p);
			DXSprite img = DXSprite();
			std::vector<DXModel*> models = std::vector<DXModel*>();

			std::vector<int> imgPage = std::vector<int>();

		};
	}

#endif