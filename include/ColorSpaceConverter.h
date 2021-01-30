#pragma once
#include "Image.h"
#include "MathExt.h"

class ColorSpaceConverter
{
public:
    static const int RGB_TO_YCACB = 0;
    static const int YCACB_TO_RGB = 1;

    static const int RGB_TO_LAB = 2;
    static const int LAB_TO_RGB = 3;

    static const int RGB_TO_HSV = 4;
    static const int HSV_TO_RGB = 5;

    static const int RGB_TO_HSL = 6;
    static const int HSL_TO_RGB = 7;

    static const int RGB_TO_XYZ = 8;
    static const int XYZ_TO_RGB = 9;
    
    static Color convert(Color a, int type);
    static Vec3f convert(Vec3f a, int type);
};