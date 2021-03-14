#include "ColorSpaceConverter.h"

Color ColorSpaceConverter::convert(Color a, int type)
{
    Vec3f c = Vec3f((double)a.red, (double)a.green, (double)a.blue);
    switch (type)
    {
    case LAB_TO_RGB:
        c.x = 100*(c.x/255);
        c.y -= 128;
        c.z -= 128;
        break;
    default:
        break;
    }
    
    Vec3f finalColor = ColorSpaceConverter::convert(c, type);
    switch (type)
    {
    case RGB_TO_LAB:
        finalColor.x = 255*(finalColor.x/100);
        finalColor.y += 128;
        finalColor.z += 128;
        break;
    default:
        break;
    }

    Color out;
    out.alpha = a.alpha;
    out.red = (unsigned char)MathExt::clamp( MathExt::round(finalColor.x), 0.0, 255.0);
    out.green = (unsigned char)MathExt::clamp( MathExt::round(finalColor.y), 0.0, 255.0);
    out.blue = (unsigned char)MathExt::clamp( MathExt::round(finalColor.z), 0.0, 255.0);

    return out;
}

Vec3f ColorSpaceConverter::convert(Vec3f c, int type)
{
    Vec3f finalColor = Vec3f();
    Vec3f o = Vec3f();

    //assume srgb
    switch(type)
    {
        case RGB_TO_XYZ:
            c /= 255.0;
            if(c.x <= 0.04045)
                c.x /= 12.92;
            else
                c.x = MathExt::pow((c.x + 0.055) / 1.055, 2.4);

            if(c.y <= 0.04045)
                c.y /= 12.92;
            else
                c.y = MathExt::pow((c.y + 0.055) / 1.055, 2.4);
                
            if(c.z <= 0.04045)
                c.z /= 12.92;
            else
                c.z = MathExt::pow((c.z + 0.055) / 1.055, 2.4);
            
            c *= 100;

            finalColor.x = c.x*0.4124 + c.y*0.3576 + c.z*0.1805;
            finalColor.y = c.x*0.2126 + c.y*0.7152 + c.z*0.0722;
            finalColor.z = c.x*0.0193 + c.y*0.1192 + c.z*0.9505;
            break;
        case XYZ_TO_RGB:
            c /= 100.0;

            finalColor.x = c.x*3.2406 + c.y*(-1.5372) + c.z*(-0.4986);
            finalColor.y = c.x*(-0.9689) + c.y*1.8758 + c.z*0.0415;
            finalColor.z = c.x*0.0557 + c.y*(-0.2040) + c.z*1.0570;

            if(finalColor.x <= 0.0031308)
                finalColor.x *= 12.92;
            else
                finalColor.x = (1.055 * MathExt::pow(finalColor.x, 1.0/2.4)) - 0.055;

            if(finalColor.y <= 0.0031308)
                finalColor.y *= 12.92;
            else
                finalColor.y = (1.055 * MathExt::pow(finalColor.y, 1.0/2.4)) - 0.055;
                
            if(finalColor.z <= 0.0031308)
                finalColor.z *= 12.92;
            else
                finalColor.z = (1.055 * MathExt::pow(finalColor.z, 1.0/2.4)) - 0.055;
            
            finalColor *= 255;
            break;
        case RGB_TO_YCBCR:
            finalColor.x = 0 + (0.299*c.x) + (0.587*c.y) + (0.114*c.z);
            finalColor.y = 128 - (0.168736*c.x) - (0.331264*c.y) + (0.5*c.z);
            finalColor.z = 128 + (0.5*c.x) - (0.418688*c.y) - (0.081312*c.z);
            break;
        case YCBCR_TO_RGB:
            finalColor.x = c.x + 1.402 * (c.z - 128);
            finalColor.y = c.x - 0.344136 * (c.y - 128) - 0.714136*(c.z - 128);
            finalColor.z = c.x + 1.772*(c.y - 128);
            break;
        case RGB_TO_LAB:
            o = convert(c, RGB_TO_XYZ);
            o.x /= 95.0489;
            o.y /= 100;
            o.z /= 108.8840;

            if( o.x > 0.008856)
                o.x = MathExt::cubeRoot(o.x);
            else
                o.x = (7.787 * o.x) + (16.0/116.0);
            
            if( o.y > 0.008856)
                o.y = MathExt::cubeRoot(o.y);
            else
                o.y = (7.787 * o.y) + (16.0/116.0);

            if( o.z > 0.008856)
                o.z = MathExt::cubeRoot(o.z);
            else
                o.z = (7.787 * o.z) + (16.0/116.0);

            finalColor.x = (116 * o.y) - 16;
            finalColor.y = 500 * (o.x - o.y);
            finalColor.z = 200 * (o.y - o.z);
            break;

        case LAB_TO_RGB:
            o.y = (c.x + 16) / 116;
            o.x = c.y / 500 + o.y;
            o.z = o.y - c.z / 200;

            if( MathExt::cube(o.x) > 0.008856)
                o.x = MathExt::cube(o.x);
            else
                o.x = (o.x - 16 / 116) / 7.787;
            
            if( o.y > 0.008856)
                o.y = MathExt::cube(o.y);
            else
                o.y = (o.y - 16 / 116) / 7.787;

            if( o.z > 0.008856)
                o.z = MathExt::cube(o.z);
            else
                o.z = (o.z - 16 / 116) / 7.787;

            o.x *= 95.0489;
            o.y *= 100;
            o.z *= 108.8840;
            finalColor = convert(o, XYZ_TO_RGB);
            break;
        default:
            break;
    }

    return finalColor;
}