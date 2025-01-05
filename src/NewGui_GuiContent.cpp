#include "NewGui.h"

namespace smpl
{
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