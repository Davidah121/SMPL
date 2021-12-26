#pragma once
#include <vector>
#include "Object.h"

//NOTE THAT SOME IMPLEMENTATION MAY BE BROKEN DUE TO SWAPING TO TEMPLATES
//FOR ADDING BITS.

namespace glib
{

	class BinarySet : public Object
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
		
		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;

		//CONSTANTS
		
		///LEFT MOST SIGNIFICANT BIT
		static const bool LMSB = true;
		
		///RIGHT MOST SIGNIFICANT BIT
		static const bool RMSB = false;

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
		 */
		template<typename T>
		void add(T v, int amountOfBits = 128, int offset = 0)
		{
			int sizeInBits = sizeof(T) * 8;

			int actualAmount = amountOfBits;
			if (amountOfBits > sizeInBits)
			{
				actualAmount = sizeInBits;
			}

			if(actualAmount < 0)
				actualAmount = 0;

			int end = sizeInBits;
			int actualOffset = offset;
			if(offset < 0)
				offset = 0;
			
			if (offset + actualAmount < end)
			{
				end = offset+actualAmount;
			}

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
		void add(char* v, int size);

		/**
		 * @brief Adds an array of unsigned char to the set.
		 * 		Is affected by bit add order.
		 * @param v
		 * 		The pointer to the array of unsigned char
		 * @param size
		 * 		The amount of items in the array.
		 */
		void add(unsigned char* v, int size);

		/**
		 * @brief Adds another BinarySet to the current set.
		 * 		Is affected by bit add order.
		 * @param other
		 * 		The BinarySet object to add.
		 */
		void add(BinarySet& other);

		/**
		 * @brief Set an array of char values to used as the BinarySet data.
		 * 		These values will be copied into the set.
		 * 		If v is a nullptr, it clears the set.
		 * @param v
		 * 		The pointer to the array of char
		 * @param size
		 * 		The amount of items in the array.
		 */
		void setValues(char* v, int size);

		/**
		 * @brief Set an array of unsigned char values to used as the BinarySet data.
		 * 		These values will be copied into the set.
		 * 		If v is a nullptr, it clears the set.
		 * @param v
		 * 		The pointer to the array of unsigned char
		 * @param size
		 * 		The amount of items in the array.
		 */
		void setValues(unsigned char* v, int size);

		/**
		 * @brief Gets a bit from the set.
		 * @param index
		 * 		The index of the bit to get.
		 * @return bool
		 * 		Returns the value of the bit.
		 * 		If the index was invalid, the value 0 is returned.
		 */
		bool getBit(int index);

		/**
		 * @brief Gets a collection of bits from the set.
		 * 
		 * @param indexStart
		 * 		The start index of the bits to get.
		 * @param indexEnd
		 * 		The end index of the bits to get.
		 * @param lmsb
		 * 		Controls whether to set the bits in the number from LMSB or RMSB
		 * @return int
		 * 		Returns the value of the bits as a number.
		 */
		int getBits(int indexStart, int indexEnd, bool lmsb = false);

		/**
		 * @brief Sets a bit at the specified location
		 * 
		 * @param value
		 * 		The value to set the bit to.
		 * @param index
		 * 		The index of the bit.
		 */
		void setBit(bool value, int index);
		
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
		 * @return int
		 */
		int size();

		/**
		 * @brief Clears the set of all bits and the memory it used.
		 */
		void clear();

		/**
		 * @brief Sets the bit order to either LMSB or RMSB.
		 * 		This adjusts how the function getBit will work but does not
		 * 		change how the data is stored or how data is added to it.
		 * @param bitOrder
		 * 		LMSB or RMSB constants should be used.
		 */
		void setBitOrder(bool bitOrder);

		/**
		 * @brief Sets the add bit order to either LMSB or RMSB.
		 * 		This adjusts how the function add will work but does not
		 * 		change how the data is stored or how data is accessed.
		 * @param bitOrder
		 * 		LMSB or RMSB constants should be used.
		 */
		void setAddBitOrder(bool order);

	private:
		std::vector<unsigned char> set = std::vector<unsigned char>();
		bool MSB = true;
		bool addBitOrder = LMSB;
		long bitNumber = 0;
	};

}  //NAMESPACE glib END