#pragma once
#include "Image.h"
#include "MathExt.h"

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
    
    static Color convert(Color a, unsigned char type);
    static Vec3f convert(Vec3f a, unsigned char type);
};