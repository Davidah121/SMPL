#pragma once
#include "SerializedStreamable.h"
#include "TypeInfo.h"

class DataFormatter
{
public:
    const static int FORMAT_DATA = 0;
    const static int FORMAT_ARRAY = 1;
    const static int FORMAT_OBJECT = 2;
    
    /**
     * @brief Format and write the start of the data out to the Streamable datatype.
     *      Note that nothing may be written out if a particular DataFormatter does not need it.
     * 
     * @param output 
     * @param formatType 
     *      Intended to inform the formatter what type of information is being written to allow better/cleaner output.
     *      valid formats:
     *          FORMAT_DATA - writes out just data. May be interpretted as a data pair
     *          FORMAT_ARRAY - writes out an array of data.
     *          FORMAT_OBJECT - writes out an object. FORMAT_ARRAY and FORMAT_OBJECT are not the same but may output similarly.
     * @param type 
     *      TypeInfo that represents the actual type of data being written.
     * @param varName 
     *      Optional. If included, variable name can be output with the type.
     *          For certain DataFormatters, it can be searched for to ensure no errors on loading when changing class definitions.
     * @param elements
     *      Information about how many elements are present for the data to be written.
     *          This is only useful for FORMAT_ARRAY as all other types must have only 1 element.
     *          For arrays, this value can be 0 or more.
     */
    virtual void writeStart(SerializedStreamable& output, const int formatType, const TypeInfo& type, const std::string& varName, int64_t elements) = 0;

    /**
     * @brief Formats and Writes out raw information.
     *      This specifies that the data can't be stored as a string or number directly and must use a format that can
     *      safely store any byte value for any amount of bytes.
     *          I.E. store it directly as bytes or as base64 if storing it as raw bytes is not an option.
     * 
     *      Throws exception if it fails.
     * 
     * @param output 
     * @param type 
     *      TypeInfo that represents the actual type of data being written.
     * @param buffer 
     *      Void pointer to the start of the data to be written.
     */
    virtual void writeRaw(SerializedStreamable& output, const TypeInfo& type, const void* buffer) = 0;

    /**
     * @brief Formats and Writes out a string.
     *      Specifies that the input buffer is to be interpreted as a raw string.
     *          NOTE: Not the class std::string or any container but the NULL terminated string itself.
     *              Does not have to be null terminated specifically.
     *          NOTE: Can be any type of string but depending on the formatter, it may be converted to a UTF-8 string or not formatted at all.
     * 
     *      Throws exception if it fails.
     * @param output 
     * @param type 
     *      TypeInfo that represents the actual type of data being written.
     *          Specifically the type without pointer.
     *          EXAMPLE: char not char*
     * @param buffer 
     *      Void pointer to the start of the data to be written.
     * @param elements 
     *      Total number of elements.
     *          Not necessarily equal to the size in bytes of the string
     */
    virtual void writeString(SerializedStreamable& output, const TypeInfo& charType, const void* buffer, size_t elements) = 0;

    /**
     * @brief Formats and Writes out a number.
     *      The number can be either a floating point number, signed integer type, or unsigned integer type.
     *      The type determines what it will be interpretted as.
     *          NOTE: Some formats may store it as a string while some may store as raw bytes.
     *              The size of the data may not be stored with the data so its possible that transferring to a different system
     *              could cause issues when deserializing if not handled properly.
     * 
     *      Throws exception if it fails.
     * @param output 
     * @param type 
     *      TypeInfo that represents the actual type of data being written.
     * @param buffer 
     *      Void pointer to the start of the data to be written.
     */
    virtual void writeNumber(SerializedStreamable& output, const TypeInfo& type, const void* buffer) = 0;

    /**
     * @brief Writes out the end of the data.
     *      This marks the end of whatever format type was currently being written whether it was
     *          FORMAT_DATA, FORMAT_ARRAY, FORMAT_OBJECT
     *      This does not necessarily write out data if it isn't needed by the formatter but should be assumed that
     *          any particular formatter uses it for something and it is required.
     * 
     * @param output 
     */
    virtual void writeEnd(SerializedStreamable& output) = 0;

    /**
     * @brief Reads in the start of formatted data based on the format type, variable type, and the optional variable name.
     *      This can be used to search for a specific variable or specific type to load if the data isn't in order and the formatter
     *      supports that action.
     *      It is up to individual DataFormatters to properly manage what has been read and hasn't and ensure the order is correct.
     *      
     *      The return value is the total number of elements to read/load. For data and objects, its always going to be 1 element. For arrays, it can be 0.
     *      Must return a value greater than or equal to 0 if it was sucessful. If unsucessful, a negative value is returned.
     *          Assume that the variable can't be properly loaded as it may not exist in this data.
     * @param input 
     * @param formatType 
     *      Intended to inform the formatter what type of information is being read to allow the correct data to be read.
     *      valid formats:
     *          FORMAT_DATA - reads in just data. May be interpretted as a data pair
     *          FORMAT_ARRAY - reads in an array of data.
     *          FORMAT_OBJECT - reads in an object. FORMAT_ARRAY and FORMAT_OBJECT are not the same but may output similarly.
     * @param type 
     *      TypeInfo that represents the actual type of data being read.
     * @param varName 
     *      Optional. If included, variable name can be searched for with the type.
     * @return int64_t
     */
    virtual int64_t readStart(SerializedStreamable& input, const int formatType, const TypeInfo& type, const std::string& varName) = 0;

    /**
     * @brief Reads in raw formatted data.
     * 
     *      Throws exception if it fails.
     * @param input 
     * @param type 
     *      TypeInfo that represents the actual type of data being written.
     * @param buffer 
     *      Void pointer to the start of the data to be written.
     */
    virtual void readRaw(SerializedStreamable& input, const TypeInfo& type, void* buffer) = 0;

    /**
     * @brief Reads in a formatted string.
     *      The string may be in UTF-8, UTF-16, ASCII, or just raw bytes. 
     *      A DataFormatter must properly convert the string to the desired type that was given.
     * 
     *      Throws exception if it fails.
     * @param input 
     * @param type 
     *      TypeInfo that represents the actual type of data being written.
     *          Specifically the type without pointer.
     *          EXAMPLE: char not char*
     * @param buffer 
     *      A vector so you can the buffer is guaranteed to be the correct size
     */
    virtual void readString(SerializedStreamable& input, const TypeInfo& type, std::vector<char>& buffer) = 0;

    /**
     * @brief Reads in a formatted number.
     *      The number may be a floating point stored as a string, integer or unsigned integer stored as a string,
     *          or just raw bytes.
     *      A DataFormatter must properly convert the number (even if its a string) to the desired type that was given.
     * 
     *      Throws exception if it fails.
     * @param input 
     * @param type 
     *      TypeInfo that represents the actual type of data being written.
     * @param buffer 
     *      Void pointer to the start of the data to be written.
     */
    virtual void readNumber(SerializedStreamable& input, const TypeInfo& type, void* buffer) = 0;

    /**
     * @brief Reads the end mark of the data.
     *      While this may not actually do anything for some DataFormatters, it is required for others so assume
     *      that this must be called.
     *      If readStart() failed, this shouldn't be called as it could cause DataFormatters to improperly read data causing more errors.
     * 
     * @param input 
     */
    virtual void readEnd(SerializedStreamable& input) = 0;
    
};