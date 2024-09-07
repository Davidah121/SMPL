#pragma once
#include "BuildOptions.h"
#include <iostream>
#include <vector>

namespace smpl
{
    class DLL_OPTION Cryptography
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
		 * @brief Generates a SHA1 Hash using stream of bytes. This method is continuous and does not require all data to be in memory first.
		 * 		The function needs a buffer of bytes, the number of bytes in that buffer, the ongoing hash value, and whether this is the end of the stream of data.
		 * 		The function returns how many bytes were processed. The data left over should be included in the next pass.
		 * 		
		 * 		On the first function call, hashValue must be empty. On successive calls, the hashValue should not be modified by outside sources and should be 5 elements.
		 * 		On the last call, end must be set and totalSize must be set. 
		 * 
		 * @param bytes 
		 * @param size 
		 * @param hashValue 
		 * @param end
		 * @param totalSize
		 * @return size_t 
		 */
		static size_t SHA1Stream(unsigned char* bytes, size_t size, std::vector<uint32_t>& hashValue, bool end, size_t totalSize);

		/**
		 * @brief Computes the SHA1 Hash for a file. This method uses the SHA1Stream function to compute the hash.
		 * 
		 * @param filename 
		 * @return std::vector<uint32_t> 
		 */
		static std::vector<uint32_t> SHA1(std::string filename);

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