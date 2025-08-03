#pragma once
#include "BuildOptions.h"
#include <iostream>
#include <vector>
#include <exception>
#include "File.h"

namespace smpl
{
	class DLL_OPTION MD5Streamable
	{
	public:
		MD5Streamable();
		~MD5Streamable();

		void addData(unsigned char* buffer, size_t s);
		void endData();

		std::vector<unsigned int> getOutputHash();
	private:
		void computeChunk(unsigned int* buffer);
		
		size_t totalSizeAccumulated = 0;
		bool endFlagSet = false;
		std::vector<unsigned char> leftoverBuffer;
		std::vector<unsigned int> digest;
	};

	class DLL_OPTION SHA1Streamable
	{
	public:
		SHA1Streamable();
		~SHA1Streamable();

		void addData(unsigned char* buffer, size_t s);
		void endData();

		std::vector<unsigned int> getOutputHash();
	private:
		void computeChunk(unsigned int* buffer);
		
		size_t totalSizeAccumulated = 0;
		bool endFlagSet = false;
		std::vector<unsigned char> leftoverBuffer;
		std::vector<unsigned int> digest;
	};

	class DLL_OPTION SHA2Streamable
	{
	public:
		static const unsigned char SHA_224 = 0;
		static const unsigned char SHA_256 = 1;
		static const unsigned char SHA_384 = 2;
		static const unsigned char SHA_512 = 3;
		
		SHA2Streamable(unsigned char type);
		~SHA2Streamable();

		void addData(unsigned char* buffer, size_t s);
		void endData();

		std::vector<unsigned int> getOutputHash();
	private:
		void computeChunk256(unsigned int* buffer);
		void computeChunk512(uint64_t* buffer);
		
		size_t totalSizeAccumulated = 0;
		bool endFlagSet = false;
		unsigned char type = SHA_224;
		std::vector<unsigned char> leftoverBuffer;
		std::vector<unsigned int> digest;
		std::vector<uint64_t> digest64;
	};

    class DLL_OPTION Cryptography
    {
    public:
	
        /**
         * @brief Generates a MD5 Hash.
		 * 		 Internally uses the MD5Stream class to compute the hash.
         * 
         * @param bytes 
         * @param size 
         * @return std::vector<uint32_t> 
         */
        static std::vector<uint32_t> MD5(unsigned char* bytes, size_t size);

		/**
		 * @brief Computes the MD5 Hash for a file. 
		 * 		Internally uses the MD5Stream class to compute the hash.
		 * 
		 * @param filename 
		 * @return std::vector<uint32_t> 
		 */
		static std::vector<uint32_t> MD5File(File filename);

        /**
         * @brief Generates a SHA1 Hash.
		 * 		Internally uses the SHA1Stream class to compute the hash.
         * 
         * @param bytes 
         * @param size 
         * @return std::vector<uint32_t> 
         */
        static std::vector<uint32_t> SHA1(unsigned char* bytes, size_t size);

		/**
		 * @brief Computes the SHA1 Hash for a file.
		 * 		Internally uses the SHA1Stream class to compute the hash.
		 * 
		 * @param filename 
		 * @return std::vector<uint32_t> 
		 */
		static std::vector<uint32_t> SHA1File(File filename);

		
        /**
         * @brief Generates a SHA2 Hash.
		 * 		Internally uses the SHA2Stream class to compute the hash.
		 * 
         * @param type
		 * 		The type of SHA2 hash to use. Valid options are:
		 * 			SHA2Streamable::SHA_224
		 * 			SHA2Streamable::SHA_256
		 * 			SHA2Streamable::SHA_384
		 * 			SHA2Streamable::SHA_512
         * @param bytes 
         * @param size 
         * @return std::vector<uint32_t> 
         */
        static std::vector<uint32_t> SHA2(unsigned char type, unsigned char* bytes, size_t size);

		/**
		 * @brief Computes the SHA2 Hash for a file.
		 * 		Internally uses the SHA2Stream class to compute the hash.
		 * 
         * @param type
		 * 		The type of SHA2 hash to use. Valid options are:
		 * 			SHA2Streamable::SHA_224
		 * 			SHA2Streamable::SHA_256
		 * 			SHA2Streamable::SHA_384
		 * 			SHA2Streamable::SHA_512
		 * @param filename 
		 * @return std::vector<uint32_t> 
		 */
		static std::vector<uint32_t> SHA2File(unsigned char type, File filename);

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