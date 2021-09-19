#pragma once

#include "Font.h"
#include "VectorSprite.h"

namespace glib
{

    class VectorFont : public Font
    {
    public:
        /**
         * @brief Construct a new VectorFont object
         *      A vector font uses vector graphics to display fonts.
         *      This allows for accurate rendering regardless of rotation, scaling, and other transforms.
         * 
         *      (NOTE, NOT COMPLETE)
         * 
         */
        VectorFont();

        /**
         * @brief Destroy the VectorFont object
         * 
         */
        ~VectorFont();

        //Object and Class Stuff
        const Class* getClass();
        static const Class myClass;

        /**
         * @brief Loads a vector font from an .svg file.
         * 
         * @param file 
         * @return true 
         * @return false 
         */
        bool load(File file);

        /**
         * @brief Gets a VectorGraphic at the specified index.
         * 
         * @param index 
         * @return VectorGraphic* 
         */
        VectorGraphic* getGraphic(int index);

        /**
         * @brief Get the VectorSprite used to manage the VectorGraphics.
         * 
         * @return VectorSprite* 
         */
        VectorSprite* getVectorSprite();

    private:
        //FontCharInfo should have a direct 1-to-1 mapping to fontSprite to avoid
        //saving the sprite index information and stuff.
        VectorSprite fontSprite = VectorSprite();
        int baseHorizontalAdvance = 0;
    };

} //NAMESPACE glib END