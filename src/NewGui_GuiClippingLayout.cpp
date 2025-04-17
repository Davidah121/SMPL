#include "NewGui.h"

namespace smpl
{
    GuiClippingLayout::GuiClippingLayout() : GuiLayoutFixed()
    {

    }

    GuiClippingLayout::~GuiClippingLayout()
    {

    }
    
    void GuiClippingLayout::layoutUpdate(int offX, int offY, int maximumWidth, int maximumHeight)
    {
        GuiLayoutFixed::layoutUpdate(offX, offY, maximumWidth, maximumHeight);

        int actualMaxW = maximumWidth;
        int actualMaxH = maximumHeight;
        if(maxWidth > 0)
            actualMaxW = maxWidth;
        if(maxHeight > 0)
            actualMaxH = maxHeight;
        
        //insure that width and height have not exceeded the min and max
        width = __min(width, actualMaxW);
        height = __min(height, actualMaxH);
    }

    void GuiClippingLayout::render(SmartMemory<GuiManager> manager)
    {
        //set clipping rectangle
        Box2D oldClipRect = smpl::GraphicsInterface::getClippingRect();
        int left, right, top, bottom;
        if(maxWidth >= 0)
        {
            left = getTrueX();
            right = getTrueX() + maxWidth;
        }
        else
        {
            left = oldClipRect.getLeftBound();
            right = oldClipRect.getRightBound();
        }
        if(maxHeight >= 0)
        {
            top = getTrueY();
            bottom = getTrueY() + maxHeight;
        }
        else
        {
            top = oldClipRect.getTopBound();
            bottom = oldClipRect.getBottomBound();
        }

        Box2D newClipRect = Box2D( __max(oldClipRect.getLeftBound(), left), __max(oldClipRect.getTopBound(), top),
                                __min(oldClipRect.getRightBound(), right), __min(oldClipRect.getBottomBound(), bottom));
        
        GraphicsInterface::setClippingRect(newClipRect);

        //render layout stuff
        GuiLayoutFixed::render(manager);

        //resset clipping rectangle
        GraphicsInterface::setClippingRect(oldClipRect);
    }

    void GuiClippingLayout::loadDataFromXML(SimpleHashMap<std::string, std::string>& attribs, SmartMemory<GuiManager> manager)
    {
        GuiLayoutFixed::loadDataFromXML(attribs, manager);
    }

    SmartMemory<GuiItem> GuiClippingLayout::loadFunction(SimpleHashMap<std::string, std::string>& attributes, SmartMemory<GuiManager> manager)
    {
        GuiClippingLayout* ins = new GuiClippingLayout();
        ins->loadDataFromXML(attributes, manager);
        return SmartMemory<GuiItem>(ins);
    }
}