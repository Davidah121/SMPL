#include "NewGui.h"
#include "Input.h"
#include "GraphicsExt.h"
#include "System.h"

namespace smpl
{
    GuiSprite::GuiSprite() : GuiContent()
    {
        setFocusable(false);
    }

    GuiSprite::~GuiSprite()
    {

    }

    void GuiSprite::setSprite(SmartMemory<SpriteInterface> spr)
    {
        this->spr = spr;
    }

    void GuiSprite::setColor(Color c)
    {
        imgColor = c;
    }

    void GuiSprite::setShouldScale(bool s)
    {
        isScalable = s;
    }

    void GuiSprite::setShouldMaintainAspectRatio(bool ar)
    {
        shouldMaintainAspectRatio = ar;
    }

    void GuiSprite::setShouldAnimate(bool a)
    {
        autoAnimate = a;
    }

    void GuiSprite::setShouldLoop(bool l)
    {
        shouldLoop = l;
    }

    void GuiSprite::setSpriteIndex(int i)
    {
        spriteIndex = i;
        if(spr.getPointer() != nullptr)
        {
            size_t maxSize = spr.getRawPointer()->getSize();
            if(spriteIndex > maxSize)
                spriteIndex = maxSize;
        }
    }

    void GuiSprite::setMinWidth(int v)
    {
        minWidth = v;
    }
    void GuiSprite::setMaxWidth(int v)
    {
        maxWidth = v;
    }
    void GuiSprite::setMinHeight(int v)
    {
        minHeight = v;
    }
    void GuiSprite::setMaxHeight(int v)
    {
        maxHeight = v;
    }

    int GuiSprite::getMinWidth()
    {
        return minWidth;
    }
    int GuiSprite::getMaxWidth()
    {
        return maxWidth;
    }
    int GuiSprite::getMinHeight()
    {
        return minHeight;
    }
    int GuiSprite::getMaxHeight()
    {
        return maxHeight;
    }

    void GuiSprite::layoutUpdate(int offX, int offY, int maximumWidth, int maximumHeight)
    {
        //update width and height of text. Do wrapping if necessary
        x = offX;
        y = offY;

        if(spr.getPointer() != nullptr)
        {
            ImageInterface shallowCopy = spr.getRawPointer()->getImage(spriteIndex);
            width = shallowCopy.getWidth();
            height = shallowCopy.getHeight();

            if(isScalable)
            {
                double allowedMaxWidth, allowedMaxHeight;

                if(maxWidth < 0)
                    allowedMaxWidth = MathExt::clamp(maximumWidth, minWidth, maximumWidth);
                else
                    allowedMaxWidth = MathExt::clamp(maximumWidth, minWidth, maxWidth);

                if(maxHeight < 0)
                    allowedMaxHeight = MathExt::clamp(maximumHeight, minHeight, maximumHeight);
                else
                    allowedMaxHeight = MathExt::clamp(maximumHeight, minHeight, maxHeight);

                if(shouldMaintainAspectRatio)
                {
                    double scaleValue = MathExt::min(allowedMaxWidth / width, allowedMaxHeight / height);
                    
                    scaledWidth = width*scaleValue;
                    scaledHeight = height*scaleValue;
                }
                else
                {
                    double xScale = allowedMaxWidth / width;
                    double yScale =  allowedMaxHeight / height;

                    scaledWidth = width*xScale;
                    scaledHeight = height*yScale;
                }

                width = scaledWidth;
                height = scaledHeight;
            }
        }
        else
        {
            width = 0;
            height = 0;
        }
    }

    void GuiSprite::update(SmartMemory<GuiManager> manager)
    {
        if(spr.getPointer() != nullptr)
        {
            if(autoAnimate)
            {
                bool wentToNewFrame = false;

                size_t maxSize = spr.getRawPointer()->getSize();
                int waitTime = spr.getRawPointer()->getDelayTime(spriteIndex);

                size_t currTime = System::getCurrentTimeMillis();
                size_t timePassed = currTime - lastUpdateTime;

                //may have skipped frames so continue until you aren't behind
                while(true)
                {
                    if(timePassed >= waitTime)
                    {
                        wentToNewFrame = true;
                        if(shouldLoop == true)
                        {
                            spriteIndex = (spriteIndex + 1) % maxSize;
                        }
                        else
                        {
                            spriteIndex += 1;
                            if(spriteIndex > maxSize)
                                spriteIndex = maxSize;
                        }
                        timePassed -= waitTime;
                    }
                    else
                    {
                        break;
                    }
                }
                if(wentToNewFrame == true)
                {
                    lastUpdateTime = currTime;
                    setShouldRender();
                }
            }
        }
    }

    void GuiSprite::render(SmartMemory<GuiManager> manager)
    {
        //assume that trueX and trueY have been properly updated.
        if(spr.getPointer() != nullptr)
        {
            GraphicsInterface::setColor(imgColor);
            ImageInterface shallowCopy = spr.getRawPointer()->getImage(spriteIndex);

            if(isScalable)
            {
                GraphicsInterface::drawSprite(&shallowCopy, getTrueX(), getTrueY(), scaledWidth, scaledHeight);
            }
            else
            {
                GraphicsInterface::drawSprite(&shallowCopy, getTrueX(), getTrueY());
            }
        }
    }

    void GuiSprite::loadDataFromXML(SimpleHashMap<std::string, std::string>& attribs, SmartMemory<GuiManager> manager)
    {
        GuiContent::loadDataFromXML(attribs, manager);
        
        auto pair = attribs.get("src");
        if(pair != nullptr)
            spr = GuiResourceManager::getResourceManager().getSprite(pair->second);
        attribs.remove(pair);

        pair = attribs.get("color");
        if(pair != nullptr)
            imgColor = loadColor(pair->second);
        attribs.remove(pair);

        pair = attribs.get("sprite-index");
        if(pair != nullptr)
        {
            try
            {
                spriteIndex = StringTools::toInt(pair->second);
            }
            catch(...) { }
        }
        attribs.remove(pair);

        pair = attribs.get("scalable");
        if(pair != nullptr)
            isScalable = (pair->second == "true");
        attribs.remove(pair);

        pair = attribs.get("maintain-aspect-ratio");
        if(pair != nullptr)
            shouldMaintainAspectRatio = (pair->second == "true");
        attribs.remove(pair);
        
        pair = attribs.get("auto-animate");
        if(pair != nullptr)
            autoAnimate = (pair->second == "true");
        attribs.remove(pair);
        
        pair = attribs.get("loop");
        if(pair != nullptr)
            shouldLoop = (pair->second == "true");
        attribs.remove(pair);

        
        pair = attribs.get("min-width");
        if(pair != nullptr)
            if(loadValueFromAttrib(minWidth, pair->second))
                minWidth = -1;
        attribs.remove(pair);
        pair = attribs.get("min-height");
        if(pair != nullptr)
            if(loadValueFromAttrib(minHeight, pair->second))
                minHeight = -1;
        attribs.remove(pair);
        pair = attribs.get("max-width");
        if(pair != nullptr)
            if(loadValueFromAttrib(maxWidth, pair->second))
                maxWidth = -1;
        attribs.remove(pair);
        pair = attribs.get("max-height");
        if(pair != nullptr)
            if(loadValueFromAttrib(maxHeight, pair->second))
                maxHeight = -1;
        attribs.remove(pair);
        
    }

    SmartMemory<GuiItem> GuiSprite::loadFunction(SimpleHashMap<std::string, std::string>& attributes, SmartMemory<GuiManager> manager)
    {
        GuiSprite* ins = new GuiSprite();
        ins->loadDataFromXML(attributes, manager);
        return SmartMemory<GuiItem>(ins);
    }
}