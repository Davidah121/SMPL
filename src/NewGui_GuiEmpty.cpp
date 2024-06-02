#include "NewGui.h"
namespace smpl
{
    const RootClass GuiEmpty::globalClass = CREATE_ROOT_CLASS(GuiEmpty, &GuiItem::globalClass);
    const RootClass* GuiEmpty::getClass()
	{
		return &GuiEmpty::globalClass;
	}

    GuiEmpty::GuiEmpty() : GuiItem()
    {

    }
    GuiEmpty::~GuiEmpty()
    {

    }

    void GuiEmpty::layoutUpdate(int offX, int offY, int maximumWidth, int maximumHeight)
    {
        
    }

    void GuiEmpty::update(SmartMemory<GuiManager> manager)
    {

    }

    void GuiEmpty::render(SmartMemory<GuiManager> manager)
    {

    }

    void GuiEmpty::loadDataFromXML(SimpleHashMap<std::string, std::string>& attribs, SmartMemory<GuiManager> manager)
    {
        //just call parent load data function. Empty element with no traits
        GuiItem::loadDataFromXML(attribs, manager);
    }

    SmartMemory<GuiItem> GuiEmpty::loadFunction(SimpleHashMap<std::string, std::string>& attributes, SmartMemory<GuiManager> manager)
    {
        GuiEmpty* ins = new GuiEmpty();
        ins->loadDataFromXML(attributes, manager);
        return SmartMemory<GuiItem>(ins);
    }
}