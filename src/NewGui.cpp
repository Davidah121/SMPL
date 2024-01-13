#include "NewGui.h"

namespace glib
{
    Color loadColor(std::string data)
    {
        //check if first character is '{'
        if(!data.empty())
        {
            if(data.front() == '{')
            {
                std::vector<std::string> split = StringTools::splitString(data.substr(1, data.size()-2), ',');
                if(split.size() == 3)
                {
                    //parse as rgb
                    try
                    {
                        Color c;
                        c.red = StringTools::toInt(split[0]);
                        c.green = StringTools::toInt(split[1]);
                        c.blue = StringTools::toInt(split[2]);
                        c.alpha = 255;
                        return c;
                    }
                    catch(...)
                    {
                        //problem parsing text. Ignore for now but log errors later
                    }
                    
                }
                else if(split.size() == 4)
                {
                    //parse as rgba
                    try
                    {
                        Color c;
                        c.red = StringTools::toInt(split[0]);
                        c.green = StringTools::toInt(split[1]);
                        c.blue = StringTools::toInt(split[2]);
                        c.alpha = StringTools::toInt(split[3]);
                        return c;
                    }
                    catch(...)
                    {
                        //problem parsing text. Ignore for now but log errors later
                    }
                }
            }
            else
            {
                std::vector<std::string> split = StringTools::splitString(data, ',');
                if(split.size() == 1)
                {
                    //parse as color name
                    return ColorNameConverter::NameToColor(split[0]);
                }
                else if(split.size() == 2)
                {
                    //parse as color name and alpha
                    try
                    {
                        Color c = ColorNameConverter::NameToColor(split[0]);
                        c.alpha = StringTools::toInt(split[1]);
                        return c;
                    }
                    catch(...)
                    {
                        //problem parsing text. Ignore for now but log errors later
                    }
                }
            }
        }
        return {0, 0, 0, 0};
    }

    bool tryToloadValueStuff(double& value, std::string data)
    {
        //check for %
        size_t indexOfPercent = data.find('%');
        if(indexOfPercent != SIZE_MAX)
        {
            //is percent
            try
            {
                value = GuiItem::PERCENTAGE_DIVIDE * (StringTools::toDouble(data.substr(indexOfPercent)) / 100.0);
            }
            catch(...) { }
            
            return true;
        }
        else
        {
            try
            {
                value = StringTools::toDouble(data);
            }
            catch(...) { }
            return false;
        }
        return false;
    }

    bool loadValueFromAttrib(int8_t& value, std::string data)
    {
        double temp = 0;
        bool isPercentage = tryToloadValueStuff(temp, data);
        value = (int8_t)temp;
        return isPercentage;
    }
    bool loadValueFromAttrib(uint8_t& value, std::string data)
    {
        double temp = 0;
        bool isPercentage = tryToloadValueStuff(temp, data);
        value = (uint8_t)temp;
        return isPercentage;
    }
    bool loadValueFromAttrib(int16_t& value, std::string data)
    {
        double temp = 0;
        bool isPercentage = tryToloadValueStuff(temp, data);
        value = (int16_t)temp;
        return isPercentage;
    }
    bool loadValueFromAttrib(uint16_t& value, std::string data)
    {
        double temp = 0;
        bool isPercentage = tryToloadValueStuff(temp, data);
        value = (uint16_t)temp;
        return isPercentage;
    }
    bool loadValueFromAttrib(int32_t& value, std::string data)
    {
        double temp = 0;
        bool isPercentage = tryToloadValueStuff(temp, data);
        value = (int32_t)temp;
        return isPercentage;
    }
    bool loadValueFromAttrib(uint32_t& value, std::string data)
    {
        double temp = 0;
        bool isPercentage = tryToloadValueStuff(temp, data);
        value = (uint32_t)temp;
        return isPercentage;
    }
    bool loadValueFromAttrib(int64_t& value, std::string data)
    {
        double temp = 0;
        bool isPercentage = tryToloadValueStuff(temp, data);
        value = (int64_t)temp;
        return isPercentage;
    }
    bool loadValueFromAttrib(uint64_t& value, std::string data)
    {
        double temp = 0;
        bool isPercentage = tryToloadValueStuff(temp, data);
        value = (uint64_t)temp;
        return isPercentage;
    }

    bool loadValueFromAttrib(double& value, std::string data)
    {
        return tryToloadValueStuff(value, data);
    }

}