#pragma once

#include "Font.h"
#include "VectorSprite.h"
#include "CachedDataList.h"

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
        static const Class globalClass;

        /**
         * @brief Loads a vector font from a valid file
         *      Valid formats: .svg | .ttf | .otf
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

        /**
         * @brief Gets the image of the font character at the specified index.
         *      The image pointer should not be deleted after use as the class will delete it when no longer needed.
         *      A new cached image may be redrawn when this method is called.
         * 
         * @param index 
         * @return Image* 
         */
        Image* getImage(size_t index);
    private:

        bool loadSVGFont(File file);
        bool loadOTFFont(File file);

        //FontCharInfo should have a direct 1-to-1 mapping to fontSprite to avoid
        //saving the sprite index information and stuff.
        VectorSprite fontSprite = VectorSprite();
        CachedDataList<Image> cachedGlyphs = CachedDataList<Image>(256);
        int baseHorizontalAdvance = 0;
        int baseWidth = 0;
        int baseHeight = 0;
    };

} //NAMESPACE glib END