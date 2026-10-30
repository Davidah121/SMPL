#pragma once
#include "BuildOptions.h"
#include "StandardTypes.h"
#include "SimpleSerialization.h"
#include <vector>
#include <cstddef>

//NOTE THAT SOME IMPLEMENTATION MAY BE BROKEN DUE TO SWAPING TO TEMPLATES
//FOR ADDING BITS.

namespace smpl
{

	class DLL_OPTION BinarySet : public SerializedObject
	{
		
	public:
		/**
		 * @brief Contructs a BinarySet object.
		 * 		A BinarySet is a set of bits where each bit can be accessed as a bool.
		 * 		Internally, it uses a std::vector<unsigned char> to store data where 8
		 * 		bits are in a single byte. 
		 * 		Useful for bit processing.
		 */
		BinarySet();

		/**
		 * @brief Destroys a BinarySet object clearing the memory used by it.
		 */
		~BinarySet();

		//CONSTANTS
		
		///RIGHT MOST SIGNIFICANT BYTE
		static const bool LittleEndian = true;
		static const bool RMSB = true;
		
		///LEFT MOST SIGNIFICANT BYTE
		static const bool BigEndian = false;
		static const bool LMSB = false;

		/**
		 * @brief Adds a bit to the set.
		 * @param v
		 * 		The value of the bit to add. True = 1 | False = 0
		 */
		void add(bool v);

		/**
		 * @brief Adds a template type to the set.
		 * 		Is affected by the add bit order
		 * @param v
		 * 		The data to add to the set.
		 * @param amountOfBits
		 * 		The amount of bits to add from the data.
		 * 		Can be less than the size of the data but not greater than that size.
		 * @param offset
		 * 		The offset from the start of the bits in the data to start adding from.
		 * @param addBitOrder
		 *		Controls the order that bits are added to the data. 
		 *			In most cases, the left most bit in a byte is more significant as this is default.
		 *			In some cases, it may be useful to swap the order for files.
		 */
		template<typename T>
		void add(const T& v, int amountOfBits = 128, int offset = 0, bool addBitOrder = LMSB)
		{
			int sizeInBits = sizeof(T) * 8;

			int actualAmount = amountOfBits;
			if (amountOfBits > sizeInBits)
				actualAmount = sizeInBits;
			

			if(actualAmount < 0)
				actualAmount = 0;

			int end = sizeInBits;
			if(offset < 0)
				offset = 0;
			
			if (offset + actualAmount < end)
				end = offset+actualAmount;

			for (int i = offset; i < end; i++)
			{
				int h = i;
				if(addBitOrder==RMSB)
				{
					h = (actualAmount-1) - h;
				}
				this->add(((v >> h) & 0x01) == 1);
			}
		}

	
		/**
		 * @brief Adds an array of char to the set.
		 * 		Is affected by bit add order.
		 * @param v
		 * 		The pointer to the array of char
		 * @param size
		 * 		The amount of items in the array.
		 */
		void add(char* v, size_t size, bool addBitOrder = LMSB);

		/**
		 * @brief Adds an array of unsigned char to the set.
		 * 		Is affected by bit add order.
		 * @param v
		 * 		The pointer to the array of unsigned char
		 * @param size
		 * 		The amount of items in the array.
		 */
		void add(unsigned char* v, size_t size, bool addBitOrder = LMSB);

		/**
		 * @brief Adds another BinarySet to the current set.
		 * 		Is affected by bit add order.
		 * @param other
		 * 		The BinarySet object to add.
		 */
		void add(BinarySet& other, bool addBitOrder = LMSB);

		/**
		 * @brief Set an array of char values to used as the BinarySet data.
		 * 		These values will be copied into the set.
		 * 		If v is a nullptr, it clears the set.
		 * @param v
		 * 		The pointer to the array of char
		 * @param size
		 * 		The amount of items in the array.
		 * @param addBitOrder
		 *		Allows swapping of the order of the bits per each byte.
		 *			Typically ordered as |7-6-5-4-3-2-1-0|
		 *			Can be ordered as |0-1-2-3-4-5-6-7| which some compression methods utilize
		 */
		void setValues(char* v, size_t size, bool addBitOrder = LMSB);

		/**
		 * @brief Set an array of unsigned char values to used as the BinarySet data.
		 * 		These values will be copied into the set.
		 * 		If v is a nullptr, it clears the set.
		 * @param v
		 * 		The pointer to the array of unsigned char
		 * @param size
		 * 		The amount of items in the array.
		 * @param addBitOrder
		 *		Allows swapping of the order of the bits per each byte.
		 *			Typically ordered as |7-6-5-4-3-2-1-0|
		 *			Can be ordered as |0-1-2-3-4-5-6-7| which some compression methods utilize
		 */
		void setValues(unsigned char* v, size_t size, bool addBitOrder = LMSB);

		/**
		 * @brief Gets a bit from the set.
		 * @param index
		 * 		The index of the bit to get.
		 * @return bool
		 * 		Returns the value of the bit.
		 * 		If the index was invalid, the value 0 is returned.
		 */
		bool getBit(size_t index);

		/**
		 * @brief Gets a collection of bits from the set.
		 *		Reads in data such that the first bit read is the least significant bit.
		 *		Note the final result is affected by the flags set for the function
		 *		A maximum of 64 bits can be read at a time with this function.
		 *		This value will need to be type-punned to the appropriate type desired most likely
		 * 
		 * @param indexStart
		 * 		The start index of the bits to get.
		 * @param indexEnd
		 * 		The end index of the bits to get.
		 * @param byteOrder
		 *		The stored order of the internal bytes. This determines which byte is more significant meaning are the bytes to the right or left more significant.
		 *			LittleEndian starts reading from the right most byte as that is the 0th byte
		 *			BigEndian starts reading from the left most byte as that is the 0th byte
		 * @return uint64_t
		 * 		Returns the value of the bits as a number.
		 */
		uint64_t getBits(size_t indexStart, size_t indexEnd, bool byteOrder);

		/**
		 * @brief Gets a collection of bits from the set and fills out the output value in reversed order.
		 *		Reads in data such that the first bit read is the most significant bit.
		 *		In some cases when reading binary data, binary data may be expected to be read in such that the first bit is the most significant instead of being the least significant.
		 * 
		 * @param indexStart 
		 * @param indexEnd 
		 * @param byteOrder 
		 * @return uint64_t 
		 */
		uint64_t getBitsReversed(size_t indexStart, size_t indexEnd, bool byteOrder);

		/**
		 * @brief Sets a bit at the specified location
		 * 
		 * @param value
		 * 		The value to set the bit to.
		 * @param index
		 * 		The index of the bit.
		 */
		void setBit(bool value, size_t index);
		
		/**
		 * @brief Returns the binary set data as bytes.
		 * 		Internally, it just returns the set it uses.
		 * 		It does not do any transformations on the data.
		 * @return std::vector<unsigned char>
		 */
		std::vector<unsigned char> toBytes();

		/**
		 * @brief Gets the Reference to the internal set used to store
		 * 		data. Best used when the data is just being read as it avoids
		 * 		additional copies.
		 * 
		 * @return std::vector<unsigned char>& 
		 */
		std::vector<unsigned char>& getByteRef();

		/**
		 * @brief Returns the number of bits in the set.
		 * @return size_t
		 */
		size_t size();

		/**
		 * @brief Clears the set of all bits and the memory it used.
		 */
		void clear();

		/**
		 * @brief Set the Number Of Bits stored.
		 * 		This is useful if the data was set to be an array of bytes but you know the
		 * 		exact number of bits it actually stores. (Ex: an array of 4 bytes but only 13 bits are used)
		 * 
		 * 		This is a work around and is here for performance reasons only.
		 */
		void setNumberOfBits(size_t n);

	private:
	
		std::vector<unsigned char> set = std::vector<unsigned char>();
		size_t bitNumber = 0;

	SERIALIZE_CLASS(bitNumber, set)
	};

}  //NAMESPACE smpl END