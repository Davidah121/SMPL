#pragma once
#include "MathExt.h"

#include "ext/GLSingleton.h"
#include "ext/GLSprite.h"
#include "ext/GLModel.h"
#include "BitmapFont.h"

namespace glib
{
    class GLFont : public Font
    {
    public:
        /**
		 * @brief Creates a GLBitmapFont Object.
		 * 		GLBitmapFont extends from the Font class and as the name suggest, it uses an Image to
		 * 		store and display characters.
		 * 		Two file types are supported. .ft and .fnt
		 * 		.ft is a custom format for font that uses a relatively simple format to store information about each character.
		 * 		.fnt is a format used by BitmapFontGenerator located here: https://www.angelcode.com/products/bmfont
		 * 			Note that only the xml file format is supported for .fnt
		 * @param filename
		 * 		The name of the file to load for font data.
		 */
		GLFont(File file);

		/**
		 * @brief Destroys a GLBitmapFont Object.
		 */
		~GLFont();
        
        //Object and Class Stuff
		const Class* getClass();
		static const Class myClass;

        /**
		 * @brief Get the texture where the desired character is stored.
		 * 		The texture can contain other characters than just the desired character.
		 * 		If the index is out of range and USE_EXCEPTIONS is defined, an OutOfBounds Error is thrown.
		 * 		If not, a nullptr is returned.
		 * @param index
		 * 		The desired character.
		 * @return Image*
		 * 		A nullptr is returned if no texture was found. Otherwise, a valid GLTexture* is returned.
		 */
		GLTexture* getTexture(int index);

		/**
		 * @brief Gets the Model for the specified character.
		 * 		This model will be the width and height of the character and will
		 * 		also have the correct texture coordinates.
		 * 		If nullptr is returned, the font does not have data for the character.
		 * 
		 * @param index 
		 * 		The desired character.
		 * @return GLModel* 
		 */
		GLModel* getModel(int index);
		
    private:

        void convertBitmapFont(BitmapFont& p);
		GLSprite img = GLSprite();
		std::vector<GLModel*> models = std::vector<GLModel*>();

		std::vector<int> imgPage = std::vector<int>();

    };
}
