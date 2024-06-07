#include <catch2/catch_amalgamated.hpp>
#include "BinarySet.h"

//May be incorrect usage here.
TEST_CASE("Testing of the BinarySet class", "[BinarySet]")
{
	std::vector<unsigned char> bytes = {0xF8, 0x37, 0x6A, 0x32, 0x4A, 0x48, 0x54, 0x94};
	std::vector<bool> bits = {1,1,1,1, 1,0,0,0, 0,0,1,1, 0,1,1,1,
							0,1,1,0, 1,0,1,0, 0,0,1,1, 0,0,1,0,
							0,1,0,0, 1,0,1,0, 0,1,0,0, 1,0,0,0,
							0,1,0,1, 0,1,0,0, 1,0,0,1, 0,1,0,0};
	
	SECTION("SET BYTES AND COPY")
	{
		smpl::BinarySet bSet;
		bSet.setBitOrder(smpl::BinarySet::LMSB);
		bSet.setAddBitOrder(smpl::BinarySet::LMSB);
		bSet.setValues(bytes.data(), bytes.size());
		
		REQUIRE(bSet.size() == bits.size());
		for(int i=0; i<bytes.size(); i++)
		{
			int x = bSet.getBits(i*8, (i*8)+8);
			REQUIRE(x == (int)bytes[i]);
		}

		smpl::BinarySet bSet2 = bSet;
		REQUIRE(bSet2.size() == bSet.size());
		for(int i=0; i<bSet.size(); i++)
		{
			REQUIRE(bSet.getBit(i) == bSet2.getBit(i));
		}
	}

	SECTION("ADD & SET BITS")
	{
		smpl::BinarySet bSet;
		bSet.setBitOrder(smpl::BinarySet::LMSB);
		bSet.setAddBitOrder(smpl::BinarySet::LMSB);
		bSet.setValues(bytes.data(), bytes.size());
		
		bSet.add(true);
		REQUIRE(bSet.size() == bits.size()+1);
		REQUIRE(bSet.getByteRef().size() == bytes.size()+1);
		REQUIRE(bSet.getBit(bSet.size()-1) == true);

		bSet.setBit(false, bSet.size()-1);
		REQUIRE(bSet.size() == bits.size()+1);
		REQUIRE(bSet.getByteRef().size() == bytes.size()+1);
		REQUIRE(bSet.getBit(bSet.size()-1) == false);
	}
}