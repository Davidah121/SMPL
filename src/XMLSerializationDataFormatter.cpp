#include "XMLSerializationDataFormatter.h"

namespace smpl
{
    void XMLDataFormatter::writeStart(SerializedStreamable& output, const int formatType, const TypeInfo& type, const std::string& varName, int64_t elements)
    {
        std::string title;
        size_t IDThing = stackStuff.size();
        if(stackStuff.size() > 0)
            IDThing = stackStuff.get().second;
        
        if(!varName.empty())
            title = varName;
        else
            title = type.getName() + "-" + StringTools::toString(IDThing);
        title = "<"+title+">";
        output.write(title.c_str(), title.size());
        if(stackStuff.size() > 0)
            IDThing = stackStuff.get().second++;
        stackStuff.add({title, 0});
    }
    void XMLDataFormatter::writeRaw(SerializedStreamable& output, const TypeInfo& type, const void* buffer)
    {
        //hmmm. Should've passed the size of the buffer through as well considering that type may not denote the total size.
        //Ideally, just use the other tools to write out arrays
        std::string converted = StringTools::base64Encode((unsigned char*)buffer, type.size, true);
        output.write(converted.c_str(), converted.size());
    }
    void XMLDataFormatter::writeString(SerializedStreamable& output, const TypeInfo& charType, const void* buffer, size_t elements)
    {
        //skip formating stuff for now - TODO: REPLACE LINE BREAKS AND '<' '>' WITH VALID XML ESCAPE SEQUENCES
        output.write(buffer, elements);
    }
    void XMLDataFormatter::writeNumber(SerializedStreamable& output, const TypeInfo& type, const void* buffer)
    {
        std::string data;
        if(type == TypeInfo::get<char>())
            data = StringTools::toString(*(char*)buffer);
        else if(type == TypeInfo::get<unsigned char>())
            data = StringTools::toString(*(unsigned char*)buffer);
        else if(type == TypeInfo::get<int16_t>())
            data = StringTools::toString(*(int16_t*)buffer);
        else if(type == TypeInfo::get<uint16_t>())
            data = StringTools::toString(*(uint16_t*)buffer);
        else if(type == TypeInfo::get<int32_t>())
            data = StringTools::toString(*(int32_t*)buffer);
        else if(type == TypeInfo::get<uint32_t>())
            data = StringTools::toString(*(uint32_t*)buffer);
        else if(type == TypeInfo::get<int64_t>())
            data = StringTools::toString(*(int64_t*)buffer);
        else if(type == TypeInfo::get<uint64_t>())
            data = StringTools::toString(*(uint64_t*)buffer);
        else if(type == TypeInfo::get<float>())
            data = StringTools::toString(*(float*)buffer);
        else if(type == TypeInfo::get<double>())
            data = StringTools::toString(*(double*)buffer);
        output.write(data.c_str(), data.size());
    }
    void XMLDataFormatter::writeEnd(SerializedStreamable& output)
    {
        if(stackStuff.size() == 0)
            return; //THROW AN EXCEPTION DUMMY
        std::pair<std::string, size_t> pair = stackStuff.get();
        std::string endTag = "</" + pair.first + ">";
        output.write(endTag.c_str(), endTag.size());
        stackStuff.pop();
    }

    int64_t XMLDataFormatter::readStart(SerializedStreamable& input, const int formatType, const TypeInfo& type, const std::string& varName)
    {

    }
    void XMLDataFormatter::readRaw(SerializedStreamable& input, const TypeInfo& type, void* buffer)
    {

    }
    void XMLDataFormatter::readString(SerializedStreamable& input, const TypeInfo& type, std::vector<char>& buffer)
    {

    }
    void XMLDataFormatter::readNumber(SerializedStreamable& input, const TypeInfo& type, void* buffer)
    {
        std::string data;
        char tempBuff[4096];
        while(true)
        {
            memset(tempBuff, 0, 4096);
            input.peek(tempBuff, 4096);
            int i=0;
            for(i=0; i<4096; i++)
            {
                if(tempBuff[i] == '<')
                    break;
                data += tempBuff[i];
            }
            input.read(tempBuff, i);
            if(i < 4096)
                break;
        }

        if(type == TypeInfo::get<char>())
            *(char*)buffer = std::stoi(data);
        else if(type == TypeInfo::get<unsigned char>())
            *(unsigned char*)buffer = std::stoi(data);
        else if(type == TypeInfo::get<int16_t>())
            *(int16_t*)buffer = std::stoi(data);
        else if(type == TypeInfo::get<uint16_t>())
            *(uint16_t*)buffer = std::stoi(data);
        else if(type == TypeInfo::get<int32_t>())
            *(int32_t*)buffer = std::stoi(data);
        else if(type == TypeInfo::get<uint32_t>())
            *(uint32_t*)buffer = std::stoul(data);
        else if(type == TypeInfo::get<int64_t>())
            *(int64_t*)buffer = std::stoll(data);
        else if(type == TypeInfo::get<uint64_t>())
            *(uint64_t*)buffer = std::stoull(data);
        else if(type == TypeInfo::get<float>())
            *(float*)buffer = std::stof(data);
        else if(type == TypeInfo::get<double>())
            *(double*)buffer = std::stod(data);
    }
    void XMLDataFormatter::readEnd(SerializedStreamable& input)
    {

    }
}