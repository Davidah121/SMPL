#pragma once
#include <vector>
namespace glib
{
    class Cryptography
    {
    public:
        /**
         * @brief Generates a SHA1 Hash. The return value is a 160 bit number (20 bytes).
         * 
         * @param bytes 
         * @param size 
         * @return std::vector<uint32_t> 
         */
        static std::vector<uint32_t> SHA1(unsigned char* bytes, size_t size);

        /**
		 * @brief Generates a checksum using ADLER32.
		 * 		If the size of data is 0 and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the data pointer is nullptr and USE_EXCEPTIONS is defined, an InvalidDataError is thrown.
		 * @param data
		 * 		The data to generate the checksum from
		 * @param size
		 * 		The size of the data.
		 * @return unsigned int
		 * 		Returns a valid checksum if successful. Otherwise, 0 is returned.
		 * 		Note that 0 can be a valid checksum return.
		 */
		static unsigned int adler32(unsigned char* data, size_t size);

		/**
		 * @brief Generates a checksum using ADLER32.
		 * 		If the size of data is 0 and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * @param data
		 * 		The data to generate the checksum from
		 * @return unsigned int
		 * 		Returns a valid checksum if successful. Otherwise, 0 is returned.
		 * 		Note that 0 can be a valid checksum return.
		 */
		static unsigned int adler32(std::vector<unsigned char> data);

		static const unsigned char CRC_8 = 0;
		static const unsigned char CRC_16 = 1;
		static const unsigned char CRC_32 = 2;
		
		/**
		 * @brief Generates a checksum using CRC.
		 * 		If the size of data is 0 and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the data pointer is nullptr and USE_EXCEPTIONS is defined, an InvalidDataError is thrown.
		 * 		Three CRC methods are available:
		 * 			CRC_8
		 * 			CRC_16
		 * 			CRC_32
		 * @param data
		 * 		The data to generate the checksum from
		 * @param size
		 * 		The size of the data.
		 * @param type
		 * 		The type of CRC to use.
		 * 		DEFAULT is CRC_32
		 * @return unsigned int
		 * 		Returns a valid checksum if successful. Otherwise, 0 is returned.
		 * 		Note that 0 can be a valid checksum return.
		 */
		static unsigned int crc(unsigned char* data, size_t size, unsigned char type = CRC_32);

		/**
		 * @brief Generates a checksum using CRC.
		 * 		If the size of data is 0 and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the data pointer is nullptr and USE_EXCEPTIONS is defined, an InvalidDataError is thrown.
		 * 		Three CRC methods are available:
		 * 			CRC_8
		 * 			CRC_16
		 * 			CRC_32
		 * @param data
		 * 		The data to generate the checksum from
		 * @param size
		 * 		The size of the data.
		 * @param type
		 * 		The type of CRC to use.
		 * 		DEFAULT is CRC_32
		 * @return unsigned int
		 * 		Returns a valid checksum if successful. Otherwise, 0 is returned.
		 * 		Note that 0 can be a valid checksum return.
		 */
		static unsigned int crc(std::vector<unsigned char> data, unsigned char type = CRC_32);

    private:

    };
}