#include "NewGui.h"

namespace smpl
{
    GuiCanvas::GuiCanvas() : GuiLayoutFixed()
    {
        setFocusable(false);
    }
    
    GuiCanvas::GuiCanvas(int width, int height, int graphicsInterfaceMode) : GuiLayoutFixed()
    {
        surf = GraphicsInterface::createSurface(width, height);
    }

    GuiCanvas::~GuiCanvas()
    {

    }

    void GuiCanvas::addChild(const SmartMemory<GuiItem>& child)
    {
        //can only have one child.
        //More efficient than GuiLayoutList::removeChild() since we are removing all.
        while(children.size() > 0)
        {
            GuiItem* rawP = children.back().getPointer();
            if(rawP != nullptr)
            {
                rawP->setParent(nullptr);
            }
            children.pop_back();
        }
    
        //does checking for nullptr or self inside this function.
        GuiLayoutFixed::addChild(child);
    }

    void GuiCanvas::update(const SmartMemory<GuiManager>& manager)
    {
        //update the children
        GuiLayout::update(manager);
    }

    void GuiCanvas::render(const SmartMemory<GuiManager>& manager)
    {
        if(surf.getSurface().isValid())
        {
            //set the surface
            SurfaceInterface oldSurface = GraphicsInterface::getBoundSurface();
            Box2D oldClipRect = GraphicsInterface::getClippingRect();
            Vec2f oldTranslationFactor = GraphicsInterface::getTranslationFactor();
            
            GraphicsInterface::setBoundSurface(surf);
            GraphicsInterface::setColor(clearColor);
            GraphicsInterface::clear();
            GraphicsInterface::resetClippingPlane(); //lazy approach
            GraphicsInterface::setTranslationFactor(Vec2f(-x, -y));

            //render
            GuiLayoutFixed::render(manager);

            //draw this surface on top of the old one
            //reset the surface to what it was before
            GraphicsInterface::setBoundSurface(oldSurface);
            GraphicsInterface::setClippingRect(oldClipRect);
            GraphicsInterface::setTranslationFactor(oldTranslationFactor);
            GraphicsInterface::setColor(drawColor);
            GraphicsInterface::drawSurface(surf, x, y);
        }
    }

    void GuiCanvas::setSurfaceInterface(const SurfaceInterface& s)
    {
        surf = s;
    }

    Color GuiCanvas::getClearColor()
    {
        return clearColor;
    }

    Color GuiCanvas::getDrawColor()
    {
        return drawColor;
    }
    
    void GuiCanvas::setClearColor(const Color& c)
    {
        clearColor = c;
    }

    void GuiCanvas::setDrawColor(const Color& c)
    {
        drawColor = c;
    }
    
    void GuiCanvas::loadDataFromXML(SimpleHashMap<std::string, std::string>& attribs, const SmartMemory<GuiManager>& manager)
    {
        
    }
    SmartMemory<GuiItem> GuiCanvas::loadFunction(SimpleHashMap<std::string, std::string>& attributes, const SmartMemory<GuiManager>& manager)
    {
        return nullptr;
    }
}