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

    void GuiSprite::setSprite(const SpriteInterface& spr)
    {
        this->spr = spr;
    }

    void GuiSprite::setColor(const Color& c)
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
        if(spr.getSprite().isValid())
        {
            size_t maxSize = spr.getSize();
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

        if(spr.getSprite().isValid())
        {
            ImageInterface img = spr.getImage(spriteIndex);
            width = img.getWidth();
            height = img.getHeight();

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
                    double scaleValue = min(allowedMaxWidth / width, allowedMaxHeight / height);
                    
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

    void GuiSprite::update(const SmartMemory<GuiManager>& manager)
    {
        if(spr.getSprite().isValid())
        {
            if(autoAnimate)
            {
                bool wentToNewFrame = false;

                size_t maxSize = spr.getSize();
                int waitTime = spr.getDelayTime(spriteIndex);

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

    void GuiSprite::render(const SmartMemory<GuiManager>& manager)
    {
        //assume that trueX and trueY have been properly updated.
        if(spr.getSprite().isValid())
        {
            GraphicsInterface::setColor(imgColor);
            ImageInterface shallowCopy = spr.getImage(spriteIndex);

            if(isScalable)
            {
                GraphicsInterface::drawSprite(shallowCopy, getTrueX(), getTrueY(), scaledWidth, scaledHeight);
            }
            else
            {
                GraphicsInterface::drawSprite(shallowCopy, getTrueX(), getTrueY());
            }
        }
    }

    void GuiSprite::loadDataFromXML(SimpleHashMap<std::string, std::string>& attribs, const SmartMemory<GuiManager>& manager)
    {
        GuiContent::loadDataFromXML(attribs, manager);
        
        auto pair = attribs.find("src");
        if(pair != attribs.end())
            spr = GuiResourceManager::getResourceManager().getSprite(pair->second);
        attribs.erase(pair);

        pair = attribs.find("color");
        if(pair != attribs.end())
            imgColor = loadColor(pair->second);
        attribs.erase(pair);

        pair = attribs.find("sprite-index");
        if(pair != attribs.end())
        {
            try
            {
                spriteIndex = StringTools::toInt(pair->second);
            }
            catch(...) { }
        }
        attribs.erase(pair);

        pair = attribs.find("scalable");
        if(pair != attribs.end())
            isScalable = (pair->second == "true");
        attribs.erase(pair);

        pair = attribs.find("maintain-aspect-ratio");
        if(pair != attribs.end())
            shouldMaintainAspectRatio = (pair->second == "true");
        attribs.erase(pair);
        
        pair = attribs.find("auto-animate");
        if(pair != attribs.end())
            autoAnimate = (pair->second == "true");
        attribs.erase(pair);
        
        pair = attribs.find("loop");
        if(pair != attribs.end())
            shouldLoop = (pair->second == "true");
        attribs.erase(pair);

        
        pair = attribs.find("min-width");
        if(pair != attribs.end())
            if(loadValueFromAttrib(minWidth, pair->second))
                minWidth = -1;
        attribs.erase(pair);
        pair = attribs.find("min-height");
        if(pair != attribs.end())
            if(loadValueFromAttrib(minHeight, pair->second))
                minHeight = -1;
        attribs.erase(pair);
        pair = attribs.find("max-width");
        if(pair != attribs.end())
            if(loadValueFromAttrib(maxWidth, pair->second))
                maxWidth = -1;
        attribs.erase(pair);
        pair = attribs.find("max-height");
        if(pair != attribs.end())
            if(loadValueFromAttrib(maxHeight, pair->second))
                maxHeight = -1;
        attribs.erase(pair);
        
    }

    SmartMemory<GuiItem> GuiSprite::loadFunction(SimpleHashMap<std::string, std::string>& attributes, const SmartMemory<GuiManager>& manager)
    {
        GuiSprite* ins = new GuiSprite();
        ins->loadDataFromXML(attributes, manager);
        return SmartMemory<GuiItem>(ins);
    }
}