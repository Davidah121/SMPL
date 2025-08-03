#include "DataFormatter.h"
#include "SimpleXml.h"
#include "Stack.h"
namespace smpl
{
    class XMLDataFormatter : public DataFormatter
    {
    public:
        /**
         * @brief Format and write the start of the data out in XML format.
         *      This denotes an entry tag where the varName is used as the tag's name or if empty, the typename followed by a number to 
         *          differentiate between duplicate types with no available variable name. 
         *      The separator between the number and type is '-' as that character is allowed in tag names but not allowed in C++ class names.
         *      WriteEnd must follow this otherwise the resulting XML will be malformed
         * @param output 
         * @param formatType 
         *      Intended to inform the formatter what type of information is being written to allow better/cleaner output.
         *      valid formats:
         *          FORMAT_DATA - writes out just data. This data is stored as follows:
         *                  <tagname>DATA</tagname>
         *          FORMAT_ARRAY - writes out an array of data. This is written out the same way as FORMAT_OBJECT as xml has no distinction between the 2.
         *          FORMAT_OBJECT - writes out an object. This is stored as follows: NOTE that object data will be another object/data/array type
         *                  <tagname>
         *                      OBJECT DATA
         *                  </tagname>
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
         *      This will store data as Base64
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
         *      Will be converted to UTF-8 and all symbols that are invalid in XML ('<' '>' '\n') will be converted to
         *      a XML escape sequence that can be converted back and forth. 
         *          NOTE: Not the class std::string or any container but the NULL terminated string itself.
         *              Does not have to be null terminated specifically.
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
         *      All values will be written out as a decimal capable value
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
         *      This will specifically write out the end of the tag.
         * 
         * @param output 
         */
        virtual void writeEnd(SerializedStreamable& output) = 0;

        /**
         * @brief Reads in the start of formatted data based on the format type, variable type, and the optional variable name.
         *      This denotes an entry tag where the varName is used as the tag's name or if empty, the typename followed by a number to 
         *          differentiate between duplicate types with no available variable name. 
         *      The separator between the number and type is '-' as that character is allowed in tag names but not allowed in C++ class names.
         *      ReadEnd must follow this otherwise the reading process will fail
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
         *      The data is in Base64 and will be converted back to raw bytes automatically.
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
         *      The string is in UTF-8 format and all escape sequences will be automatically converted.
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
         *      Reads in the number which is written out as plain text in decimal.
         *          The type determines how the data is intended to be converted.
         *              (Whether or not to use std::stoi or std::stoull or std::stod or etc.)
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
         *      Required as it reads the ending tag. 
         *          Internally, it just pops the stack as end tags are stripped off after parsing XML data
         * @param input 
         */
        virtual void readEnd(SerializedStreamable& input) = 0;
    private:
        Stack<std::pair<std::string, size_t>> stackStuff;
    };
}