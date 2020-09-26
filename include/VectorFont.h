#pragma once

#include "Font.h"
#include "VectorSprite.h"

class VectorFont : public Font
{
public:
    VectorFont();
    ~VectorFont();

    //Object and Class Stuff
    const Class* getClass();
    static const Class* myClass;

    bool load(std::string filename);

    Image* getImage(int index);
    VectorSprite* getVectorSprite();

private:
    //FontCharInfo should have a direct 1-to-1 mapping to fontSprite to avoid
    //saving the sprite index information and stuff.
    VectorSprite fontSprite = VectorSprite();
    int baseHorizontalAdvance = 0;
};