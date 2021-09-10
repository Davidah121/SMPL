#pragma once
#include "Image.h"
#include "MathExt.h"

namespace glib
{

    class ColorSpaceConverter
    {
    public:
        static const unsigned char RGB_TO_YCBCR = 0;
        static const unsigned char YCBCR_TO_RGB = 1;

        static const unsigned char RGB_TO_LAB = 2;
        static const unsigned char LAB_TO_RGB = 3;

        static const unsigned char RGB_TO_HSV = 4;
        static const unsigned char HSV_TO_RGB = 5;

        static const unsigned char RGB_TO_HSL = 6;
        static const unsigned char HSL_TO_RGB = 7;

        static const unsigned char RGB_TO_XYZ = 8;
        static const unsigned char XYZ_TO_RGB = 9;
        
        /**
         * @brief Converts a Color to a different color space specifed by the
         *      available constants.
         * @param a
         *      The color to convert.
         * @param type
         *      The type of conversion to do.
         * @return Color
         *      Returns the converted color. Clipping may have occured due to Color
         *      using bytes for each color channel.
         */
        static Color convert(Color a, unsigned char type);

        /**
         * @brief Converts a Color to a different color space specifed by the
         *      available constants.
         * @param a
         *      The color to convert as a Vec3f.
         * @param type
         *      The type of conversion to do.
         * @return Vec3f
         *      Returns the converted color.
         */
        static Vec3f convert(Vec3f a, unsigned char type);
    };

}  //NAMESPACE glib END