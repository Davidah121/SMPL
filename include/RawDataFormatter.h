#pragma once
#include <iostream>
#include <vector>
#include "SimpleSerialization.h"

class RawDataFormatter : public DataFormatter
{
public:
    RawDataFormatter(){}
    ~RawDataFormatter(){}
    virtual void writeStart(SerializedStreamable& output, const int formatType, const TypeInfo& type, const std::string& varName, int64_t elements)
    {
        if(formatType == DataFormatter::FORMAT_ARRAY)
        {
            output.write((void*)&elements, sizeof(int64_t));
        }
    }
    virtual void writeRaw(SerializedStreamable& output, const TypeInfo& type, const void* buffer)
    {
        //no idea what it is. raw format so write out the bytes raw
        if(buffer == nullptr)
            throw std::runtime_error("Failed to write raw data to Streamable output. Buffer is nullptr.");
        
        if(!output.write(buffer, type.size))
            throw std::runtime_error("Failed to write raw data to Streamable output. Streamable reported failed write.");
    }
    virtual void writeString(SerializedStreamable& output, const TypeInfo& type, const void* buffer, size_t elements)
    {
        //assume you got passed the raw data and not just std::string or some container
        if(buffer == nullptr)
            throw std::runtime_error("Failed to write string data to Streamable output. Buffer is nullptr.");

        size_t totalSize = type.size*elements;
        bool okay = output.write(buffer, totalSize);
        if(!okay)
            throw std::runtime_error("Failed to write string data to Streamable output. Streamable reported failed write.");
        
        //must be null terminated.
        bool isNullTerminated = true;
        for(size_t i=totalSize-type.size; i<totalSize; i++)
        {
            if(((char*)buffer)[i] != 0)
            {
                isNullTerminated = false;
                break;
            }
        }
        if(isNullTerminated == false)
        {
            //add null terminator?
            for(int i=0; i<type.size; i++)
                output.write(0, 1);
        }
    }
    virtual void writeNumber(SerializedStreamable& output, const TypeInfo& type, const void* buffer)
    {
        if(buffer == nullptr)
            throw std::runtime_error("Failed to write numeric data to Streamable output. Buffer is nullptr.");
        
        if(!output.write(buffer, type.size))
            throw std::runtime_error("Failed to write numeric data to Streamable output. Streamable reported failed write.");
    }
    virtual void writeEnd(SerializedStreamable& output)
    {
        //do nothing
    }

    virtual int64_t readStart(SerializedStreamable& input, const int formatType, const TypeInfo& type, const std::string& varName)
    {
        if(formatType == DataFormatter::FORMAT_ARRAY)
        {
            int64_t elements = 0;
            if(!input.read((void*)&elements, sizeof(int64_t)))
                return -1; //not strictly a bad thing. Don't throw exception
            return elements;
        }
        return 1;
    }

    virtual void readRaw(SerializedStreamable& input, const TypeInfo& type, void* buffer)
    {
        if(buffer == nullptr)
            throw std::runtime_error("Failed to read raw data to Streamable input. Buffer is nullptr.");
        
        if(!input.read(buffer, type.size))
            throw std::runtime_error("Failed to read raw data to Streamable input. Streamable reported failed read.");
    }

    virtual void readString(SerializedStreamable& input, const TypeInfo& type, std::vector<char>& buffer)
    {
        buffer.clear();
        //read till null char
        std::vector<char> tempBuff = std::vector<char>(type.size);
        while(input.peek(tempBuff.data(), tempBuff.size()))
        {
            input.read(tempBuff.data(), tempBuff.size()); //officially read
            bool allZero = true;
            for(size_t i=0; i < tempBuff.size(); i++)
            {
                if(tempBuff[i]!=0)
                    allZero = false;
                buffer.push_back(tempBuff[i]);
            }
            if(allZero)
                return;
        }
        
        //throw exception
        throw std::runtime_error("Failed to read string data to Streamable input. Unable to find null character and/or input reported failed read.");

    }

    virtual void readNumber(SerializedStreamable& input, const TypeInfo& type, void* buffer)
    {
        if(buffer == nullptr)
            throw std::runtime_error("Failed to read numeric data to Streamable input. Buffer is nullptr.");
        
        if(!input.read(buffer, type.size))
            throw std::runtime_error("Failed to read numeric data to Streamable input. Streamable reported failed read.");
    }

    virtual void readEnd(SerializedStreamable& input)
    {
        //do nothing
    }
};