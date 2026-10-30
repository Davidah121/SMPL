#include "NewGui.h"
namespace smpl
{
	GuiSelfRegisteringLoader<GuiEmpty> GuiEmpty::singleton("Empty");
    GuiEmpty::GuiEmpty() : GuiItem()
    {

    }
    GuiEmpty::~GuiEmpty()
    {

    }

    void GuiEmpty::layoutUpdate(int offX, int offY, int maximumWidth, int maximumHeight)
    {
        
    }

    void GuiEmpty::update(const SmartMemory<GuiManager>& manager)
    {

    }

    void GuiEmpty::render(const SmartMemory<GuiManager>& manager)
    {

    }

    void GuiEmpty::loadDataFromXML(SimpleHashMap<std::string, std::string>& attribs, const SmartMemory<GuiManager>& manager)
    {
        //just call parent load data function. Empty element with no traits
        GuiItem::loadDataFromXML(attribs, manager);
    }

    SmartMemory<GuiItem> GuiEmpty::loadFunction(SimpleHashMap<std::string, std::string>& attributes, const SmartMemory<GuiManager>& manager)
    {
        GuiEmpty* ins = new GuiEmpty();
        ins->loadDataFromXML(attributes, manager);
        return SmartMemory<GuiItem>(ins);
    }
}