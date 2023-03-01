#pragma once
#include <vector>
#include <string>
#include "Tree.h"
#include "BinaryTree.h"
#include "BinarySet.h"
#include "FrequencyTable.h"

#include "GeneralExceptions.h"

namespace glib
{

	struct HuffmanNode
	{
		int frequency = 0;
		int value = 0;
	};

	struct HuffmanCodeData
	{
		int codeLength = 0;
		int codeVal = 0;
		int value = 0;
	};

	struct lengthPair
	{
		bool literal;
		int v1;	//if literal, this is the value else it is length
		int v2;	//backwards distance if not literal; otherwise 0
	};

	struct streamSavedData
	{
		int type;
		union unknown
		{
			unsigned char bytes[16]; //16 bytes to do whatever may be neeeded
		};
		union rle
		{
			unsigned char byte;
			unsigned char length;
		};
		union lzw
		{
			//Using Linear LZW approach so each step should be constant
			Tree<int>* dictionaryTree;
			TreeNode<int>* lastTreeNode;
			int count;
		};
		union lzss
		{

		};
		union deflate
		{

		};
	};

	class StreamCompressionRLE
	{
	public:
		static const bool TYPE_COMPRESSION = false;
		static const bool TYPE_DECOMPRESSION = true;
		
		/**
		 * @brief Construct a new Stream Compression object for RunLengthEncoding
		 * 		Can be created to compress or decompress data as a stream instead of all at once.
		 * 
		 * @param mode 
		 * 		Valid values are:		
		 * 			TYPE_COMPRESSION
		 * 			TYPE_DECOMPRESSION 
		 */
		StreamCompressionRLE(bool mode);

		/**
		 * @brief Destroy the Stream Compression object for RunLengthEncoding
		 * 		This clears the saved buffer.
		 * 
		 */
		~StreamCompressionRLE();
		
		/**
		 * @brief Adds new data to be compressed or decompressed.
		 * 		The size of the buffer may or may not change depending on the value input and
		 * 		the mode set.
		 * 
		 * @param data 
		 * @param length 
		 */
		void addData(unsigned char* data, int length);

		/**
		 * @brief Gets the Buffer of bytes saved by the object. This represents the compressed
		 * 		data or decompressed data so far. It can be read from here and stored in another
		 * 		place as necessary.
		 * 
		 * @return std::vector<unsigned char>& 
		 */
		std::vector<unsigned char>& getBuffer();

		/**
		 * @brief Clears the internal buffer of bytes saved by the object. That buffer represents
		 * 		the compressed data or decompressed data so far. This can be cleared to maintain
		 * 		space as necessary. It should be cleared after reading the buffer or at least copying
		 * 		the buffer as to not lose data.
		 * 
		 */
		void clearBuffer();

		/**
		 * @brief Returns the size in bytes of the internal buffer saved by the object.
		 * 
		 * @return size_t 
		 */
		size_t size();

	private:
		void addDataCompression(unsigned char* data, int length);
		void addDataDecompression(unsigned char* data, int length);

		bool mode = TYPE_COMPRESSION;
		unsigned char byte = 0;
		unsigned char run = 0;
		std::vector<unsigned char> buffer = std::vector<unsigned char>();
		
	};

	class StreamCompressionLZW
	{
	public:
		static const bool TYPE_COMPRESSION = false;
		static const bool TYPE_DECOMPRESSION = true;

		/**
		 * @brief Construct a new Stream Compression object for LZW compression or decompression.
		 * 		Mode changes whether to use it for compression or decompression.
		 * 		CodeSize controls the size of the base dictionary needed by LZW.
		 * 
		 * @param mode 
		 * @param codeSize 
		 */
		StreamCompressionLZW(bool mode, int codeSize);

		/**
		 * @brief Destroy the Stream Compression object for LZW
		 * 		This clears the saved buffer.
		 */
		~StreamCompressionLZW();
		
		/**
		 * @brief Adds data to be either compressed or decompressed based on the mode of the object.
		 * 		The buffer may or may not change based on the data added and mode set.
		 * 
		 * @param data 
		 * @param length 
		 */
		void addData(unsigned char* data, int length);

		/**
		 * @brief Gets the Buffer of bytes saved by the object. This represents the compressed
		 * 		data or decompressed data so far. It can be read from here and stored in another
		 * 		place as necessary.
		 * 		Here, it is a BinarySet since the data does not have to be exactly divisible by 8.
		 * 		The buffer may not have an even number of bytes so it should be written out as bits
		 * 		to preserve what the original data would have been.
		 * 		
		 * 		If mode is TYPE_COMPRESSED, the data is not guareenteed to be divisible by 8.
		 * 		If mode is TYPE_DECOMPRESSED, the data is guareenteed to be divisible by 8.
		 * 
		 * @return BinarySet& 
		 */
		BinarySet& getBuffer();

		/**
		 * @brief Marks the end of the data needed to be compressed. This is required by LZW compression
		 * 		so that it can signal the decompressor when to stop.
		 * 
		 */
		void addEndData();

		/**
		 * @brief Adds a clear dictionary code into the buffer manually. This is not required
		 * 		as the addData function handles it but this can be used to force the dictionary
		 * 		to clear.
		 * 		Useful when concatenate buffers together into one valid stream.
		 * 
		 */
		void addClearDictionary();

		/**
		 * @brief Gets whether the object has been signaled that it has reached the end of the data to compress
		 * 		or decompress. If true, addData() will no longer do anything.
		 * 
		 * @return true 
		 * @return false 
		 */
		bool getEndData();

		/**
		 * @brief Clears the internal buffer used that represents the compressed or decompressed data so far.
		 * 
		 */
		void clearBuffer();

		/**
		 * @brief Returns the size of the internal buffer in bytes.
		 * 		This will always be greater than or equal to the amount of bits actually used.
		 * 		To get the number of bits used, get it from the getBuffer() function.
		 * 
		 * @return size_t 
		 */
		size_t size();
		

	private:
		void addDataCompression(unsigned char* data, int length);
		void addDataDecompression(unsigned char* data, int length);
		void buildBaseDictionaryTree(int codeSize);

		//Compression Stuff
		Tree<int> currentTree = Tree<int>();
		TreeNode<int>* lastTreeNode = nullptr;

		//Decompression Stuff
		std::vector<std::string> baseDictionary = std::vector<std::string>();
		std::vector<std::string> decompressionDictionary = std::vector<std::string>();
		int previousIndex = -1;
		size_t ignoreThis = 0;
		
		//General Stuff
		int count = -1;
		int baseCodeSize = -1;
		int baseBits = 0;
		int currBits = 0;
		int clearDictionaryCode = -1;
		int endOfDataLocation = -1;
		bool isEndOfData = false;
		bool started = false;

		bool mode = TYPE_COMPRESSION;
		BinarySet buffer = BinarySet();
		BinarySet leftovers = BinarySet();
	};

	class Compression
	{
	public:

		struct ExceededExpectedSizeError : public std::exception
		{
			const char* what() const noexcept { return "Error while decompressing data. Exceeded expected size."; }
		};

		struct InvalidDataStreamEndError : public std::exception
		{
			const char* what() const noexcept { return "Error while decompressing data. Invalid end to the data stream."; }
		};
		/**
		 * @brief Compresses data using Run Length Encoding.
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the data pointer is nullptr and USE_EXCEPTIONS is defined, an InvalidDataError is thrown.
		 * 
		 * 		Data is compressed as bytes where the run is specified first, then the data is specified after.
		 * 		Run is a byte in size and the data is a byte in size.
		 * @param data
		 * 		The data to compress
		 * @param size
		 * 		The size of the data.
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the compressed data as a vector so that it can be saved into a file later if desired.
		 */
		static std::vector<unsigned char> compressRLE(unsigned char* data, size_t size);

		/**
		 * @brief Compresses data using Run Length Encoding.
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 
		 * 		Data is compressed as bytes where the run is specified first, then the data is specified after.
		 * 		Run is a byte in size and the data is a byte in size.
		 * @param data
		 * 		The data to compress
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the compressed data as a vector so that it can be saved into a file later if desired.
		 */
		static std::vector<unsigned char> compressRLE(std::vector<unsigned char> data);

		/**
		 * @brief Decompresses Run Length Encoded data.
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the data pointer is nullptr and USE_EXCEPTIONS is defined, an InvalidDataError is thrown.
		 * 		If the output exceeds the expected size and USE_EXCEPTIONS is defined, an ExceededExpectedSizeError is thrown.
		 * 			If USE_EXCEPTIONS is not defined, the current decompressed data is returned as is.
		 * 
		 * 		The data is expected to have both run and the data as bytes.
		 * 		Run is expected to be first and the data second.
		 * @param data
		 * 		The data to decompress
		 * @param size
		 * 		The size of the data.
		 * @param expectedSize
		 * 		The expected size of the decompressed data. Set this if you know what the data size should be when decompressed.
		 * 		By default, it is set to the max that a vector can store.
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the decompressed data as a vector.
		 */
		static std::vector<unsigned char> decompressRLE(unsigned char* data, size_t size, size_t expectedSize = -1);

		/**
		 * @brief Decompresses Run Length Encoded data.
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the output exceeds the expected size and USE_EXCEPTIONS is defined, an ExceededExpectedSizeError is thrown.
		 * 			If USE_EXCEPTIONS is not defined, the current decompressed data is returned as is.
		 * 
		 * 		The data is expected to have both run and the data as bytes.
		 * 		Run is expected to be first and the data second.
		 * @param data
		 * 		The data to decompress
		 * @param expectedSize
		 * 		The expected size of the decompressed data. Set this if you know what the data size should be when decompressed.
		 * 		By default, it is set to the max that a vector can store.
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the decompressed data as a vector.
		 */
		static std::vector<unsigned char> decompressRLE(std::vector<unsigned char> data, size_t expectedSize = -1);

		struct LZW_ERROR_L317 : public std::exception
		{
			const char* what() const noexcept { return "Error Code L317 when decompressing LZW data."; }
		};

		struct LZW_ERROR_L340 : public std::exception
		{
			const char* what() const noexcept { return "Error Code L340 when decompressing LZW data."; }
		};

		/**
		 * @brief Decompresses LZW (Lempel Ziv Welch) data.
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the data pointer is nullptr and USE_EXCEPTIONS is defined, an InvalidDataError is thrown.
		 * 		If the output exceeds the expected size and USE_EXCEPTIONS is defined, an ExceededExpectedSizeError is thrown.
		 * 			If USE_EXCEPTIONS is not defined, the current decompressed data is returned as is.
		 * @param data
		 * 		The data to decompress
		 * @param size
		 * 		The size of the data.
		 * @param dictionarySize
		 * 		The size of the dictionary used to compress the data. Required to decompress.
		 * @param expectedSize
		 * 		The expected size of the decompressed data. Set this if you know what the data size should be when decompressed.
		 * 		By default, it is set to the max that a vector can store.
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the decompressed data as a vector. The decompressed data will just be the indicies in the dictionary.
		 * 		The original dictionary is still needed to fully decompress the data.
		 */
		static std::vector<unsigned char> decompressLZW(unsigned char* data, size_t size, int dictionarySize, size_t expectedSize = -1);

		/**
		 * @brief Decompresses LZW (Lempel Ziv Welch) data.
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the output exceeds the expected size and USE_EXCEPTIONS is defined, an ExceededExpectedSizeError is thrown.
		 * 			If USE_EXCEPTIONS is not defined, the current decompressed data is returned as is.
		 * @param data
		 * 		The data to decompress
		 * @param dictionarySize
		 * 		The size of the dictionary used to compress the data. Required to decompress.
		 * @param expectedSize
		 * 		The expected size of the decompressed data. Set this if you know what the data size should be when decompressed.
		 * 		By default, it is set to the max that a vector can store.
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the decompressed data as a vector. The decompressed data will just be the indicies in the dictionary.
		 * 		The original dictionary is still needed to fully decompress the data.
		 */
		static std::vector<unsigned char> decompressLZW(std::vector<unsigned char> data, int dictionarySize, size_t expectedSize = -1);

		static std::vector<unsigned char> compressLZW(unsigned char* data, size_t size, int blocks, int codeSize);
		static std::vector<unsigned char> compressLZW(std::vector<unsigned char> data, int blocks, int codeSize);

		/**
		 * @brief Compresses LZW (Lempel Ziv Welch) data.
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the data pointer is nullptr and USE_EXCEPTIONS is defined, an InvalidDataError is thrown.
		 * @param data
		 * 		The data to compress
		 * @param size
		 * 		The size of the data.
		 * @param codeSizePointer
		 * 		A pointer to the code size parameter that should be used to compress the data.
		 * 		If left as a nullptr, the code size paramter will be determined by the function.
		 * 		If the code size is less that or equal to 0, the function will determine the code size and modify the pointer to reflect that.
		 * 		Default is nullptr.
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the compressed data as a vector.
		 */
		static std::vector<unsigned char> compressLZW(unsigned char* data, size_t size, int codeSize);
		
		/**
		 * @brief Compresses LZW (Lempel Ziv Welch) data.
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * @param data
		 * 		The data to compress
		 * @param codeSizePointer
		 * 		A pointer to the code size parameter that should be used to compress the data.
		 * 		If left as a nullptr, the code size paramter will be determined by the function.
		 * 		If the code size is less that or equal to 0, the function will determine the code size and modify the pointer to reflect that.
		 * 		Default is nullptr.
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the compressed data as a vector.
		 */
		static std::vector<unsigned char> compressLZW(std::vector<unsigned char> data, int codeSize);

		static void compressLZW(BinarySet* outputData, unsigned char* data, size_t size, int codeSize, bool omitEndCode = false);

		/**
		 * @brief Compresses LZ77 (Lempel Ziv 77) data.
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the data pointer is nullptr and USE_EXCEPTIONS is defined, an InvalidDataError is thrown.
		 * 
		 * 		The output uses triplets with the format
		 * 			(back_distance, copy_length, literal)
		 * 		The maximum buffer size also specifies how many bytes are used for the back_distance and copy_length in each distance by using
		 * 		the log base 2 of the maxBufferSize.
		 * @param data
		 * 		The data to compress
		 * @param size
		 * 		The size of the data.
		 * @param maxBufferSize
		 * 		The maximum allowed backwards distance allowed for compression in the sliding window.
		 * 		Default is 32767
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the compressed data as a vector.
		 */
		static std::vector<unsigned char> compressLZ77(unsigned char* data, size_t size, int maxBufferSize = 0x7FFF);

		/**
		 * @brief Compresses LZ77 (Lempel Ziv 77) data.
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 
		 * 		The output uses triplets with the format
		 * 			(back_distance, copy_length, literal)
		 * 		The maximum buffer size also specifies how many bytes are used for the back_distance and copy_length in each distance by using
		 * 		the log base 2 of the maxBufferSize.
		 * @param data
		 * 		The data to compress
		 * @param maxBufferSize
		 * 		The maximum allowed backwards distance allowed for compression in the sliding window.
		 * 		Default is 32767
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the compressed data as a vector.
		 */
		static std::vector<unsigned char> compressLZ77(std::vector<unsigned char> data, int maxBufferSize = 0x7FFF);

		/**
		 * @brief Decompresses LZ77 (Lempel Ziv 77) data.
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the data pointer is nullptr and USE_EXCEPTIONS is defined, an InvalidDataError is thrown.
		 * 		If the output exceeds the expected size and USE_EXCEPTIONS is defined, an ExceededExpectedSizeError is thrown.
		 * 			If USE_EXCEPTIONS is not defined, the current decompressed data is returned as is.
		 * 
		 * 		The input is expected to use triplets with the format
		 * 			(back_distance, copy_length, literal)
		 * 		The maximum buffer size also specifies how many bytes are used for the back_distance and copy_length in each distance by using
		 * 		the log base 2 of the maxBufferSize.
		 * @param data
		 * 		The data to decompress
		 * @param size
		 * 		The size of the data.
		 * @param maxBufferSize
		 * 		The maximum allowed backwards distance allowed for compression in the sliding window.
		 * 		Default is 32767.
		 * @param expectedSize
		 * 		The expected size of the decompressed data. Set this if you know what the data size should be when decompressed.
		 * 		By default, it is set to the max that a vector can store.
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the decompressed data as a vector.
		 */
		static std::vector<unsigned char> decompressLZ77(unsigned char* data, size_t size, int maxBufferSize = 0x7FFF, size_t expectedSize = -1);

		/**
		 * @brief Decompresses LZ77 (Lempel Ziv 77) data.
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the output exceeds the expected size and USE_EXCEPTIONS is defined, an ExceededExpectedSizeError is thrown.
		 * 			If USE_EXCEPTIONS is not defined, the current decompressed data is returned as is.
		 * 
		 * 		The input is expected to use triplets with the format
		 * 			(back_distance, copy_length, literal)
		 * 		The maximum buffer size also specifies how many bytes are used for the back_distance and copy_length in each distance by using
		 * 		the log base 2 of the maxBufferSize.
		 * @param data
		 * 		The data to decompress
		 * @param maxBufferSize
		 * 		The maximum allowed backwards distance allowed for compression in the sliding window.
		 * 		Default is 32767.
		 * @param expectedSize
		 * 		The expected size of the decompressed data. Set this if you know what the data size should be when decompressed.
		 * 		By default, it is set to the max that a vector can store.
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the decompressed data as a vector.
		 */
		static std::vector<unsigned char> decompressLZ77(std::vector<unsigned char> data, int maxBufferSize = 0x7FFF, size_t expectedSize = -1);

		struct LZSS_ERROR : public std::exception
		{
			const char* what() const noexcept { return "Error when decompressing LZSS data."; }
		};

		/**
		 * @brief Compresses data using LZSS (Lempel Ziv Storer Szymanski)
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the data pointer is nullptr and USE_EXCEPTIONS is defined, an InvalidDataError is thrown.
		 * 
		 * 		The output uses 9 bits for a literal or 24 bits for a reference pair
		 * 		The first bit specifies if it is a literal or not.
		 * 		If literal, the next 8 bits are the value.
		 * 		In a referencePair, the backwards distance is 15 bits and the copy length is 8 bits.
		 * @param data
		 * 		The data to compress.
		 * @param size
		 * 		The size of the data.
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the compressed data as a vector.
		 */
		static std::vector<unsigned char> compressLZSS(unsigned char* data, size_t size);

		/**
		 * @brief Compresses data using LZSS (Lempel Ziv Storer Szymanski)
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 
		 * 		The output uses 9 bits for a literal or 24 bits for a reference pair
		 * 		The first bit specifies if it is a literal or not.
		 * 		If literal, the next 8 bits are the value.
		 * 		In a referencePair, the backwards distance is 15 bits and the copy length is 8 bits.
		 * @param data
		 * 		The data to compress
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the compressed data as a vector.
		 */
		static std::vector<unsigned char> compressLZSS(std::vector<unsigned char> data);
		
		/**
		 * @brief Decompresses LZSS (Lempel Ziv Storer Szymanski) data.
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the data pointer is nullptr and USE_EXCEPTIONS is defined, an InvalidDataError is thrown.
		 * 		If the output exceeds the expected size and USE_EXCEPTIONS is defined, an ExceededExpectedSizeError is thrown.
		 * 			If USE_EXCEPTIONS is not defined, the current decompressed data is returned as is.
		 * 
		 * 		The input expects the format to be the same as the compression methods.
		 * 		The first bit specifies if it is a literal or not.
		 * 		If literal, the next 8 bits are the value.
		 * 		In a referencePair, the backwards distance is 15 bits and the copy length is 8 bits.
		 * @param data
		 * 		The data to compress.
		 * @param size
		 * 		The size of the data.
		 * @param expectedSize
		 * 		The expected size of the decompressed data. Set this if you know what the data size should be when decompressed.
		 * 		By default, it is set to the max that a vector can store.
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the decompressed data as a vector.
		 */
		static std::vector<unsigned char> decompressLZSS(unsigned char* data, size_t size, size_t expectedSize = -1);

		/**
		 * @brief Decompresses LZSS (Lempel Ziv Storer Szymanski) data.
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the output exceeds the expected size and USE_EXCEPTIONS is defined, an ExceededExpectedSizeError is thrown.
		 * 			If USE_EXCEPTIONS is not defined, the current decompressed data is returned as is.
		 * 
		 * 		The input expects the format to be the same as the compression methods.
		 * 		The first bit specifies if it is a literal or not.
		 * 		If literal, the next 8 bits are the value.
		 * 		In a referencePair, the backwards distance is 15 bits and the copy length is 8 bits.
		 * @param data
		 * 		The data to compress.
		 * @param expectedSize
		 * 		The expected size of the decompressed data. Set this if you know what the data size should be when decompressed.
		 * 		By default, it is set to the max that a vector can store.
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the decompressed data as a vector.
		 */
		static std::vector<unsigned char> decompressLZSS(std::vector<unsigned char> data, size_t expectedSize = -1);

		struct HUFFMAN_TREE_ERROR : public std::exception
		{
			const char* what() const noexcept { return "Invalid Huffman tree used."; }
		};

		/**
		 * @brief Compresses data using a Huffman tree.
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the data pointer is nullptr and USE_EXCEPTIONS is defined, an InvalidDataError is thrown.
		 * @param data
		 * 		The data to decompress
		 * @param size
		 * 		The size of the data.
		 * @param tree
		 * 		The pointer that will store the huffman tree generated.
		 * 		Must be a valid pointer and should not have any data stored in it.
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the compressed data as a vector and the tree pointer should have the
		 * 		huffman tree used to compress the data.
		 */
		static std::vector<unsigned char> compressHuffman(unsigned char* data, size_t size, BinaryTree<HuffmanNode>* tree);

		/**
		 * @brief Compresses data using a Huffman tree.
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * @param data
		 * 		The data to decompress
		 * @param tree
		 * 		The pointer that will store the huffman tree generated.
		 * 		Must be a valid pointer and should not have any data stored in it.
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the compressed data as a vector and the tree pointer should have the
		 * 		huffman tree used to compress the data.
		 */
		static std::vector<unsigned char> compressHuffman(std::vector<unsigned char> data, BinaryTree<HuffmanNode>* tree);

		/**
		 * @brief Decompresses data using Huffman tree encoding.
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the data pointer is nullptr and USE_EXCEPTIONS is defined, an InvalidDataError is thrown.
		 * 		If the output exceeds the expected size and USE_EXCEPTIONS is defined, an ExceededExpectedSizeError is thrown.
		 * 			If USE_EXCEPTIONS is not defined, the current decompressed data is returned as is.
		 * 		If the tree is invalid for any reason such as being null or not being able to resolve data and USE_EXCEPTIONS
		 * 		is defined, a HUFFMAN_TREE_ERROR is thrown.
		 * 
		 * @param data
		 * 		The data to decompress
		 * @param size
		 * 		The size of the data.
		 * @param tree
		 * 		The huffman tree to use for decompression
		 * @param expectedSize
		 * 		The expected size of the decompressed data. Set this if you know what the data size should be when decompressed.
		 * 		By default, it is set to the max that a vector can store.
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the decompressed data as a vector.
		 */
		static std::vector<unsigned char> decompressHuffman(unsigned char* data, size_t size, BinaryTree<HuffmanNode>* tree, size_t expectedSize = -1);

		/**
		 * @brief Decompresses data using Huffman tree encoding.
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the output exceeds the expected size and USE_EXCEPTIONS is defined, an ExceededExpectedSizeError is thrown.
		 * 			If USE_EXCEPTIONS is not defined, the current decompressed data is returned as is.
		 * 		If the tree is invalid for any reason such as being null or not being able to resolve data and USE_EXCEPTIONS
		 * 		is defined, a HUFFMAN_TREE_ERROR is thrown.
		 * 
		 * @param data
		 * 		The data to decompress
		 * @param tree
		 * 		The huffman tree to use for decompression
		 * @param expectedSize
		 * 		The expected size of the decompressed data. Set this if you know what the data size should be when decompressed.
		 * 		By default, it is set to the max that a vector can store.
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the decompressed data as a vector.
		 */
		static std::vector<unsigned char> decompressHuffman(std::vector<unsigned char> data, BinaryTree<HuffmanNode>* tree, size_t expectedSize = -1);

		struct HUFFMAN_CANONICAL_ERROR : public std::exception
		{
			const char* what() const noexcept { return "Error creating Cannonical Huffman Tree."; }
		};

		/**
		 * @brief Creates a huffman tree from the specified data array.
		 * 		A length limited huffman tree can be created this way as well if desired.
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the data pointer is nullptr and USE_EXCEPTIONS is defined, an InvalidDataError is thrown.
		 * 		
		 * @param data
		 * 		The data to create a huffman tree from. It should be a numerical data type as
		 * 		the data will be cast to ints to find the tree.
		 * @param size
		 * 		The size of the data.
		 * @param maxCodeLength
		 * 		The maximum code length allowed for the tree.
		 * 		A value less than 0 will create a huffman tree that is not length limited.
		 * @return BinaryTree<HuffmanNode>
		 * 		Returns a valid huffman tree if successful. Otherwise, returns a nullptr.
		 */
		template<typename T>
		static BinaryTree<HuffmanNode>* buildHuffmanTree(T* data, size_t size, int maxCodeLength = -1);

		/**
		 * @brief Create a Canonical Huffman Tree based on the parameters given.
		 * 		The data can be given where each value in the dataValue array has a unique code length given in the codeLength array
		 * 		at the same spot or they can be separate where the codeLengths array tells how many codes have length specified by its
		 * 		index. Both representations are used in different data formats.
		 * 		
		 * 		If the size of data is 0 or less or there is not enough data to map code lengths to the data,
		 * 		and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the data pointer is nullptr and USE_EXCEPTIONS is defined, an InvalidDataError is thrown.
		 * 		
		 * @param dataValue
		 * 		The data values used.
		 * @param sizeOfData
		 * 		The size of the dataValue array.
		 * @param codeLengths
		 * 		The code lengths for the data.
		 * @param sizeOfCodeLengths
		 * 		The size of the codeLength array.
		 * @param separateCodes
		 * 		Whether or not the code lengths have a one to one mapping.
		 * 		If false, codeLength[i] must specify the length of dataValue[i] and the sizes must match.
		 * 		Default value is true
		 * @param rmsb
		 * 		Determines whether the code values will be in LMSB or RMSB order.
		 * 		Default is false | which is LMSB
		 */
		static BinaryTree<HuffmanNode>* buildCanonicalHuffmanTree(int* dataValue, size_t sizeOfData, int* codeLength, size_t sizeOfCodeLengths, bool separateCodes = true, bool rmsb = false);

		/**
		 * @brief Compresses data using DELFATE
		 * 
		 * 		If the size of data is 0 or blocks is 0 and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the data pointer is nullptr and USE_EXCEPTIONS is defined, an InvalidDataError is thrown.
		 * 		
		 * 		This method will use multiple threads to compress to maintain reasonable speed unless the amount of blocks is 1.
		 * 		Custom huffman trees are supported.
		 * 		Uses a zlib header.
		 * @param data
		 * 		The data to decompress
		 * @param size
		 * 		The size of the data.
		 * @param blocks
		 * 		The amount of blocks to separate the data into for compression.
		 * 		Adding more blocks allows multiple threads to compress them individually but can reduce the compression ratio.
		 * @param compressionLevel
		 * 		Changing the compression level adjusts the maximum backwards distance allowed.
		 * 		The lowest backwards distance is 256 where compressionLevel is set to 0
		 * 		The maximum distance is 32768 where compressionLevel is set to 7
		 * 		Default value is 7
		 * @param customTable
		 * 		Chooses whether to use a custom huffman table for compression.
		 * 		Slower but results in a unique table for each block and better compression.
		 * 		Default value is false
		 * @return std::vector<unsigned char>
		 * 		If successful, returns a valid deflate stream using a zlib header as a std::vector<unsigned char>.
		 * 		Otherwise, returns an empty vector.
		 */
		static std::vector<unsigned char> compressDeflate(unsigned char* data, size_t size, int blocks, int compressionLevel = 7, bool customTable = false);
		static void compressDeflate(BinarySet* outputData, unsigned char* data, size_t size, int blocks, int compressionLevel = 7, bool customTable = false);
		
		/**
		 * @brief Compresses data using DELFATE
		 * 
		 * 		If the size of data is 0 or blocks is 0 and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		
		 * 		This method will use multiple threads to compress to maintain reasonable speed unless the amount of blocks is 1.
		 * 		Custom huffman trees are supported.
		 * 		Uses a zlib header.
		 * @param data
		 * 		The data to decompress
		 * @param blocks
		 * 		The amount of blocks to separate the data into for compression.
		 * 		Adding more blocks allows multiple threads to compress them individually but can reduce the compression ratio.
		 * @param compressionLevel
		 * 		Changing the compression level adjusts the maximum backwards distance allowed.
		 * 		The lowest backwards distance is 256 where compressionLevel is set to 0
		 * 		The maximum distance is 32768 where compressionLevel is set to 7
		 * 		Default value is 7
		 * @param customTable
		 * 		Chooses whether to use a custom huffman table for compression.
		 * 		Slower but results in a unique table for each block and better compression.
		 * 		Default value is false
		 * @return std::vector<unsigned char>
		 * 		If successful, returns a valid deflate stream using a zlib header as a std::vector<unsigned char>.
		 * 		Otherwise, returns an empty vector.
		 */
		static std::vector<unsigned char> compressDeflate(std::vector<unsigned char> data, int blocks, int compressionLevel = 7, bool customTable = false);

		struct DEFLATE_INVALID_MODE : public std::exception
		{
			const char* what() const noexcept { return "Error when decompressing deflate block. Invalid compression mode found."; }
		};
		
		/**
		 * @brief Decompresses a DEFLATE data stream.
		 * 
		 * 		If the size of data is 0 or blocks is 0 and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the data pointer is nullptr and USE_EXCEPTIONS is defined, an InvalidDataError is thrown.
		 * 		
		 * 		Assumes that the data uses a ZLIB header. GZIP is not supported.
		 * @param data
		 * 		The data to decompress
		 * @param size
		 * 		The size of the data.
		 * @param expectedSize
		 * 		The expected size of the decompressed data. Set this if you know what the data size should be when decompressed.
		 * 		By default, it is set to the max that a vector can store.
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the decompressed data. Otherwise, an empty vector is returned.
		 */
		static std::vector<unsigned char> decompressDeflate(unsigned char* data, size_t size, size_t expectedSize = -1);

		/**
		 * @brief Decompresses a DEFLATE data stream.
		 * 
		 * 		If the size of data is 0 or blocks is 0 and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the data pointer is nullptr and USE_EXCEPTIONS is defined, an InvalidDataError is thrown.
		 * 		
		 * 		Assumes that the data uses a ZLIB header. GZIP is not supported.
		 * @param data
		 * 		The data to decompress
		 * @param size
		 * 		The size of the data.
		 * @param expectedSize
		 * 		The expected size of the decompressed data. Set this if you know what the data size should be when decompressed.
		 * 		By default, it is set to the max that a vector can store.
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the decompressed data. Otherwise, an empty vector is returned.
		 */
		static std::vector<unsigned char> decompressDeflate(std::vector<unsigned char> data, size_t expectedSize = -1);

		/**
		 * @brief Experimental Arithmetic compression.
		 * 		Should not be used currently.
		 */
		static double compressArithmetic(std::vector<unsigned char> data, std::vector<double>& percentages);

		/**
		 * @brief Experimental Arithmetic decompression.
		 * 		Should not be used currently.
		 */
		static std::vector<unsigned char> decompressArithmetic(double data, size_t messageSize, std::vector<double> percentages);

	private:

		static BinaryTree<HuffmanNode>* buildHuffmanTreeSubFunc(FrequencyTable<int>* freqTable);
		static BinaryTree<HuffmanNode>* buildLimitedHuffmanTreeSubFunc(FrequencyTable<int>* freqTable, int maxCodeLength);
		
		static void fillHuffmanTable(BinaryTreeNode<HuffmanNode>* treeNode, unsigned int* table, int length, int code);

		static BinaryTree<HuffmanNode>* buildDeflateDefaultTree();
		static BinaryTree<HuffmanNode>** buildDynamicDeflateTree(BinarySet* data, int* currentLocation);

		static void getCopyLengthInformation(int code, int* baseValue, int* extraBits);
		static void getBackDistanceInformation(int code, int* baseValue, int* extraBits);

		static void compressDeflateSubFunction(unsigned char* data, int size, std::vector<lengthPair>* outputData, int compressionLevel = 7);
		static void compressDeflateSubFunction2(std::vector<lengthPair>* block, BinarySet* output, bool dynamic, bool lastBlock);
		
		// static void buildCanonicalHuffTreeFromHuffTreeSubFunc(BinaryTreeNode<HuffmanNode>* tree);
	};

	template<typename T>
	BinaryTree<HuffmanNode>* Compression::buildHuffmanTree(T* data, size_t size, int maxCodeLength)
	{
		if(size <= 0)
		{
			#ifdef USE_EXCEPTIONS
			throw InvalidSizeError();
			#endif
			return nullptr;
		}

		if(data == nullptr)
		{
			#ifdef USE_EXCEPTIONS
			throw InvalidDataError();
			#endif
			return nullptr;
		}

		//First pass is to fill the frequency table
		FrequencyTable<int> freqTable = FrequencyTable<int>();
		for (int i = 0; i < size; i++)
		{
			freqTable.add((int)data[i]);
		}

		if(maxCodeLength<=0)
		{
			BinaryTree<HuffmanNode>* retVal = buildHuffmanTreeSubFunc(&freqTable);
			return retVal;
		}
		else
		{
			BinaryTree<HuffmanNode>* retVal = buildLimitedHuffmanTreeSubFunc(&freqTable, maxCodeLength);
			return retVal;
		}
	}

}  //NAMESPACE glib END