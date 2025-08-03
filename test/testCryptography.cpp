#include "Cryptography.h"
#include <catch_amalgamated.hpp>

TEST_CASE("Test MD5Hash", "[MD5Streamable]")
{
	SECTION("EMPTY HASH")
	{
		//empty hash = d41d8cd9 8f00b204 e9800998 ecf8427e
		smpl::MD5Streamable streamableHash = smpl::MD5Streamable();
		streamableHash.endData();
		std::vector<unsigned int> digest = streamableHash.getOutputHash();
		REQUIRE(digest.size() == 4);
		REQUIRE(digest[0] == 0xd41d8cd9);
		REQUIRE(digest[1] == 0x8f00b204);
		REQUIRE(digest[2] == 0xe9800998);
		REQUIRE(digest[3] == 0xecf8427e);
	}
	SECTION("TEST HASH: \"The quick brown fox jumps over the lazy dog\"")
	{
		//hash = 9e107d9d 372bb682 6bd81d35 42a419d6
		smpl::MD5Streamable streamableHash = smpl::MD5Streamable();
		std::string testString = "The quick brown fox jumps over the lazy dog";
		streamableHash.addData((unsigned char*)testString.data(), testString.size());
		streamableHash.endData();
		std::vector<unsigned int> digest = streamableHash.getOutputHash();
		REQUIRE(digest.size() == 4);
		REQUIRE(digest[0] == 0x9e107d9d);
		REQUIRE(digest[1] == 0x372bb682);
		REQUIRE(digest[2] == 0x6bd81d35);
		REQUIRE(digest[3] == 0x42a419d6);
	}
	SECTION("LARGE HASH")
	{
		//hash = 3960b911 54c43565 fb0d162e 849d7bc8
		smpl::MD5Streamable streamableHash = smpl::MD5Streamable();
		std::string testString = "This tool calculates an MD5 checksum of the given input data in your browser. The MD5 hashing algorithm is a one-way cryptographic function that accepts a message of any length as input and returns as output a fixed-length digest value to be used for authenticating the original message. This utility works just like the md5sum command line tool. It outputs a 32-byte MD5 hex string that is computed from the given input. It also has a multi-line option that lets you calculate MD5 of each input line individually. Coming soon, you'll also be able to change the output base. Hashabulous!";
		streamableHash.addData((unsigned char*)testString.data(), testString.size());
		streamableHash.endData();
		std::vector<unsigned int> digest = streamableHash.getOutputHash();
		REQUIRE(digest.size() == 4);
		REQUIRE(digest[0] == 0x3960b911);
		REQUIRE(digest[1] == 0x54c43565);
		REQUIRE(digest[2] == 0xfb0d162e);
		REQUIRE(digest[3] == 0x849d7bc8);
	}
}

TEST_CASE("Test SHA1Hash", "[SHA1Streamable]")
{
	SECTION("EMPTY HASH")
	{
		//empty hash = da39a3ee 5e6b4b0d 3255bfef 95601890 afd80709
		smpl::SHA1Streamable streamableHash = smpl::SHA1Streamable();
		streamableHash.endData();
		std::vector<unsigned int> digest = streamableHash.getOutputHash();
		REQUIRE(digest.size() == 5);
		REQUIRE(digest[0] == 0xda39a3ee);
		REQUIRE(digest[1] == 0x5e6b4b0d);
		REQUIRE(digest[2] == 0x3255bfef);
		REQUIRE(digest[3] == 0x95601890);
		REQUIRE(digest[4] == 0xafd80709);
	}
	SECTION("TEST HASH: \"The quick brown fox jumps over the lazy dog\"")
	{
		//hash = 2fd4e1c6 7a2d28fc ed849ee1 bb76e739 1b93eb12
		smpl::SHA1Streamable streamableHash = smpl::SHA1Streamable();
		std::string testString = "The quick brown fox jumps over the lazy dog";
		streamableHash.addData((unsigned char*)testString.data(), testString.size());
		streamableHash.endData();
		std::vector<unsigned int> digest = streamableHash.getOutputHash();
		REQUIRE(digest.size() == 5);
		REQUIRE(digest[0] == 0x2fd4e1c6);
		REQUIRE(digest[1] == 0x7a2d28fc);
		REQUIRE(digest[2] == 0xed849ee1);
		REQUIRE(digest[3] == 0xbb76e739);
		REQUIRE(digest[4] == 0x1b93eb12);
	}
	SECTION("LARGE HASH")
	{
		//hash = 87620ab4 daae223f 5a9e8aad a7638966 c1f11540
		smpl::SHA1Streamable streamableHash = smpl::SHA1Streamable();
		std::string testString = "This tool calculates an MD5 checksum of the given input data in your browser. The MD5 hashing algorithm is a one-way cryptographic function that accepts a message of any length as input and returns as output a fixed-length digest value to be used for authenticating the original message. This utility works just like the md5sum command line tool. It outputs a 32-byte MD5 hex string that is computed from the given input. It also has a multi-line option that lets you calculate MD5 of each input line individually. Coming soon, you'll also be able to change the output base. Hashabulous!";
		streamableHash.addData((unsigned char*)testString.data(), testString.size());
		streamableHash.endData();
		std::vector<unsigned int> digest = streamableHash.getOutputHash();
		REQUIRE(digest.size() == 5);
		REQUIRE(digest[0] == 0x87620ab4);
		REQUIRE(digest[1] == 0xdaae223f);
		REQUIRE(digest[2] == 0x5a9e8aad);
		REQUIRE(digest[3] == 0xa7638966);
		REQUIRE(digest[4] == 0xc1f11540);
	}
}

TEST_CASE("Test SHA2Hash 224", "[SHA2Streamable]")
{
	SECTION("EMPTY HASH")
	{
		//empty hash = d14a028c 2a3a2bc9 476102bb 288234c4 15a2b01f 828ea62a c5b3e42f
		smpl::SHA2Streamable streamableHash = smpl::SHA2Streamable(smpl::SHA2Streamable::SHA_224);
		streamableHash.endData();
		std::vector<unsigned int> digest = streamableHash.getOutputHash();
		REQUIRE(digest.size() == 7);
		REQUIRE(digest[0] == 0xd14a028c);
		REQUIRE(digest[1] == 0x2a3a2bc9);
		REQUIRE(digest[2] == 0x476102bb);
		REQUIRE(digest[3] == 0x288234c4);
		REQUIRE(digest[4] == 0x15a2b01f);
		REQUIRE(digest[5] == 0x828ea62a);
		REQUIRE(digest[6] == 0xc5b3e42f);
	}
	SECTION("TEST HASH: \"The quick brown fox jumps over the lazy dog\"")
	{
		//hash = 730e109b d7a8a32b 1cb9d9a0 9aa2325d 2430587d dbc0c38b ad911525
		smpl::SHA2Streamable streamableHash = smpl::SHA2Streamable(smpl::SHA2Streamable::SHA_224);
		std::string testString = "The quick brown fox jumps over the lazy dog";
		streamableHash.addData((unsigned char*)testString.data(), testString.size());
		streamableHash.endData();
		std::vector<unsigned int> digest = streamableHash.getOutputHash();
		REQUIRE(digest.size() == 7);
		REQUIRE(digest[0] == 0x730e109b);
		REQUIRE(digest[1] == 0xd7a8a32b);
		REQUIRE(digest[2] == 0x1cb9d9a0);
		REQUIRE(digest[3] == 0x9aa2325d);
		REQUIRE(digest[4] == 0x2430587d);
		REQUIRE(digest[5] == 0xdbc0c38b);
		REQUIRE(digest[6] == 0xad911525);
	}
	SECTION("LARGE HASH")
	{
		//hash = d040afce 63202785 0eb892eb d70fd192 09e7bc90 45f81a9d aff8278b
		smpl::SHA2Streamable streamableHash = smpl::SHA2Streamable(smpl::SHA2Streamable::SHA_224);
		std::string testString = "This tool calculates an MD5 checksum of the given input data in your browser. The MD5 hashing algorithm is a one-way cryptographic function that accepts a message of any length as input and returns as output a fixed-length digest value to be used for authenticating the original message. This utility works just like the md5sum command line tool. It outputs a 32-byte MD5 hex string that is computed from the given input. It also has a multi-line option that lets you calculate MD5 of each input line individually. Coming soon, you'll also be able to change the output base. Hashabulous!";
		streamableHash.addData((unsigned char*)testString.data(), testString.size());
		streamableHash.endData();
		std::vector<unsigned int> digest = streamableHash.getOutputHash();
		REQUIRE(digest.size() == 7);
		REQUIRE(digest[0] == 0xd040afce);
		REQUIRE(digest[1] == 0x63202785);
		REQUIRE(digest[2] == 0x0eb892eb);
		REQUIRE(digest[3] == 0xd70fd192);
		REQUIRE(digest[4] == 0x09e7bc90);
		REQUIRE(digest[5] == 0x45f81a9d);
		REQUIRE(digest[6] == 0xaff8278b);
	}
}