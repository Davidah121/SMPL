#include "NewGui.h"

namespace smpl
{
    const RootClass GuiContent::globalClass = CREATE_ROOT_CLASS(GuiContent, &GuiItem::globalClass);
    const RootClass* GuiContent::getClass()
	{
		return &GuiContent::globalClass;
	}

    GuiContent::GuiContent() : GuiItem()
    {
        type = TYPE_CONTENT;
    }
    GuiContent::~GuiContent()
    {

    }
    void GuiContent::loadDataFromXML(SimpleHashMap<std::string, std::string>& attribs, SmartMemory<GuiManager> manager)
    {
        GuiItem::loadDataFromXML(attribs, manager);
    }
}