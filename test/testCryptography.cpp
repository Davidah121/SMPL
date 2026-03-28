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

TEST_CASE("Test SHA2Hash 256", "[SHA2Streamable]")
{
	SECTION("EMPTY HASH")
	{
		//empty hash = e3b0c442 98fc1c14 9afbf4c8 996fb924 27ae41e4 649b934c a495991b 7852b855
		smpl::SHA2Streamable streamableHash = smpl::SHA2Streamable(smpl::SHA2Streamable::SHA_256);
		streamableHash.endData();
		std::vector<unsigned int> digest = streamableHash.getOutputHash();
		REQUIRE(digest.size() == 8);
		REQUIRE(digest[0] == 0xe3b0c442);
		REQUIRE(digest[1] == 0x98fc1c14);
		REQUIRE(digest[2] == 0x9afbf4c8);
		REQUIRE(digest[3] == 0x996fb924);
		REQUIRE(digest[4] == 0x27ae41e4);
		REQUIRE(digest[5] == 0x649b934c);
		REQUIRE(digest[6] == 0xa495991b);
		REQUIRE(digest[7] == 0x7852b855);
	}
	SECTION("TEST HASH: \"The quick brown fox jumps over the lazy dog\"")
	{
		//hash = d7a8fbb3 07d78094 69ca9abc b0082e4f 8d5651e4 6d3cdb76 2d02d0bf 37c9e592
		smpl::SHA2Streamable streamableHash = smpl::SHA2Streamable(smpl::SHA2Streamable::SHA_256);
		std::string testString = "The quick brown fox jumps over the lazy dog";
		streamableHash.addData((unsigned char*)testString.data(), testString.size());
		streamableHash.endData();
		std::vector<unsigned int> digest = streamableHash.getOutputHash();
		REQUIRE(digest.size() == 8);
		REQUIRE(digest[0] == 0xd7a8fbb3);
		REQUIRE(digest[1] == 0x07d78094);
		REQUIRE(digest[2] == 0x69ca9abc);
		REQUIRE(digest[3] == 0xb0082e4f);
		REQUIRE(digest[4] == 0x8d5651e4);
		REQUIRE(digest[5] == 0x6d3cdb76);
		REQUIRE(digest[6] == 0x2d02d0bf);
		REQUIRE(digest[7] == 0x37c9e592);
	}
	SECTION("LARGE HASH")
	{
		//hash = 61bd6d8b d9ba75ad b55d9ba2 6c4b5167 a49c7bc9 46826acc b32e09a6 231925a8
		smpl::SHA2Streamable streamableHash = smpl::SHA2Streamable(smpl::SHA2Streamable::SHA_256);
		std::string testString = "This tool calculates an MD5 checksum of the given input data in your browser. The MD5 hashing algorithm is a one-way cryptographic function that accepts a message of any length as input and returns as output a fixed-length digest value to be used for authenticating the original message. This utility works just like the md5sum command line tool. It outputs a 32-byte MD5 hex string that is computed from the given input. It also has a multi-line option that lets you calculate MD5 of each input line individually. Coming soon, you'll also be able to change the output base. Hashabulous!";
		streamableHash.addData((unsigned char*)testString.data(), testString.size());
		streamableHash.endData();
		std::vector<unsigned int> digest = streamableHash.getOutputHash();
		REQUIRE(digest.size() == 8);
		REQUIRE(digest[0] == 0x61bd6d8b);
		REQUIRE(digest[1] == 0xd9ba75ad);
		REQUIRE(digest[2] == 0xb55d9ba2);
		REQUIRE(digest[3] == 0x6c4b5167);
		REQUIRE(digest[4] == 0xa49c7bc9);
		REQUIRE(digest[5] == 0x46826acc);
		REQUIRE(digest[6] == 0xb32e09a6);
		REQUIRE(digest[7] == 0x231925a8);
	}
}

TEST_CASE("Test SHA2Hash 384", "[SHA2Streamable]")
{
	SECTION("EMPTY HASH")
	{
		//empty hash = 38b060a7 51ac9638 4cd9327e b1b1e36a 21fdb711 14be0743 4c0cc7bf 63f6e1da 274edebf e76f65fb d51ad2f1 4898b95b
		smpl::SHA2Streamable streamableHash = smpl::SHA2Streamable(smpl::SHA2Streamable::SHA_384);
		streamableHash.endData();
		std::vector<unsigned int> digest = streamableHash.getOutputHash();
		REQUIRE(digest.size() == 12);
		REQUIRE(digest[0] == 0x38b060a7);
		REQUIRE(digest[1] == 0x51ac9638);
		REQUIRE(digest[2] == 0x4cd9327e);
		REQUIRE(digest[3] == 0xb1b1e36a);
		REQUIRE(digest[4] == 0x21fdb711);
		REQUIRE(digest[5] == 0x14be0743);
		REQUIRE(digest[6] == 0x4c0cc7bf);
		REQUIRE(digest[7] == 0x63f6e1da);
		REQUIRE(digest[8] == 0x274edebf);
		REQUIRE(digest[9] == 0xe76f65fb);
		REQUIRE(digest[10] == 0xd51ad2f1);
		REQUIRE(digest[11] == 0x4898b95b);
	}
	SECTION("TEST HASH: \"The quick brown fox jumps over the lazy dog\"")
	{
		//hash = ca737f10 14a48f4c 0b6dd43c b177b0af d9e51693 67544c49 4011e331 7dbf9a50 9cb1e5dc 1e85a941 bbee3d7f 2afbc9b1
		smpl::SHA2Streamable streamableHash = smpl::SHA2Streamable(smpl::SHA2Streamable::SHA_384);
		std::string testString = "The quick brown fox jumps over the lazy dog";
		streamableHash.addData((unsigned char*)testString.data(), testString.size());
		streamableHash.endData();
		std::vector<unsigned int> digest = streamableHash.getOutputHash();
		REQUIRE(digest.size() == 12);
		REQUIRE(digest[0] == 0xca737f10);
		REQUIRE(digest[1] == 0x14a48f4c);
		REQUIRE(digest[2] == 0x0b6dd43c);
		REQUIRE(digest[3] == 0xb177b0af);
		REQUIRE(digest[4] == 0xd9e51693);
		REQUIRE(digest[5] == 0x67544c49);
		REQUIRE(digest[6] == 0x4011e331);
		REQUIRE(digest[7] == 0x7dbf9a50);
		REQUIRE(digest[8] == 0x9cb1e5dc);
		REQUIRE(digest[9] == 0x1e85a941);
		REQUIRE(digest[10] == 0xbbee3d7f);
		REQUIRE(digest[11] == 0x2afbc9b1);
	}
	SECTION("LARGE HASH")
	{
		//hash = a4292fd7 6eee84d7 ee365b71 fa4e3a16 935c256a 755510c0 63a30cd3 dc766868 0999bc8d 212d889e 83119708 ab840bad
		smpl::SHA2Streamable streamableHash = smpl::SHA2Streamable(smpl::SHA2Streamable::SHA_384);
		std::string testString = "This tool calculates an MD5 checksum of the given input data in your browser. The MD5 hashing algorithm is a one-way cryptographic function that accepts a message of any length as input and returns as output a fixed-length digest value to be used for authenticating the original message. This utility works just like the md5sum command line tool. It outputs a 32-byte MD5 hex string that is computed from the given input. It also has a multi-line option that lets you calculate MD5 of each input line individually. Coming soon, you'll also be able to change the output base. Hashabulous!";
		streamableHash.addData((unsigned char*)testString.data(), testString.size());
		streamableHash.endData();
		std::vector<unsigned int> digest = streamableHash.getOutputHash();
		REQUIRE(digest.size() == 12);
		REQUIRE(digest[0] == 0xa4292fd7);
		REQUIRE(digest[1] == 0x6eee84d7);
		REQUIRE(digest[2] == 0xee365b71);
		REQUIRE(digest[3] == 0xfa4e3a16);
		REQUIRE(digest[4] == 0x935c256a);
		REQUIRE(digest[5] == 0x755510c0);
		REQUIRE(digest[6] == 0x63a30cd3);
		REQUIRE(digest[7] == 0xdc766868);
		REQUIRE(digest[8] == 0x0999bc8d);
		REQUIRE(digest[9] == 0x212d889e);
		REQUIRE(digest[10] == 0x83119708);
		REQUIRE(digest[11] == 0xab840bad);
	}
}

TEST_CASE("Test SHA2Hash 512", "[SHA2Streamable]")
{
	SECTION("EMPTY HASH")
	{
		//empty hash = cf83e135 7eefb8bd f1542850 d66d8007 d620e405 0b5715dc 83f4a921 d36ce9ce 47d0d13c 5d85f2b0 ff8318d2 877eec2f 63b931bd 47417a81 a538327a f927da3e
		smpl::SHA2Streamable streamableHash = smpl::SHA2Streamable(smpl::SHA2Streamable::SHA_512);
		streamableHash.endData();
		std::vector<unsigned int> digest = streamableHash.getOutputHash();
		REQUIRE(digest.size() == 16);
		REQUIRE(digest[0] == 0xcf83e135);
		REQUIRE(digest[1] == 0x7eefb8bd);
		REQUIRE(digest[2] == 0xf1542850);
		REQUIRE(digest[3] == 0xd66d8007);
		REQUIRE(digest[4] == 0xd620e405);
		REQUIRE(digest[5] == 0x0b5715dc);
		REQUIRE(digest[6] == 0x83f4a921);
		REQUIRE(digest[7] == 0xd36ce9ce);
		REQUIRE(digest[8] == 0x47d0d13c);
		REQUIRE(digest[9] == 0x5d85f2b0);
		REQUIRE(digest[10] == 0xff8318d2);
		REQUIRE(digest[11] == 0x877eec2f);
		REQUIRE(digest[12] == 0x63b931bd);
		REQUIRE(digest[13] == 0x47417a81);
		REQUIRE(digest[14] == 0xa538327a);
		REQUIRE(digest[15] == 0xf927da3e);
	}
	SECTION("TEST HASH: \"The quick brown fox jumps over the lazy dog\"")
	{
		//hash = 07e547d9 586f6a73 f73fbac0 435ed769 51218fb7 d0c8d788 a309d785 436bbb64 2e93a252 a954f239 12547d1e 8a3b5ed6 e1bfd709 7821233f a0538f3d b854fee6
		smpl::SHA2Streamable streamableHash = smpl::SHA2Streamable(smpl::SHA2Streamable::SHA_512);
		std::string testString = "The quick brown fox jumps over the lazy dog";
		streamableHash.addData((unsigned char*)testString.data(), testString.size());
		streamableHash.endData();
		std::vector<unsigned int> digest = streamableHash.getOutputHash();
		REQUIRE(digest.size() == 16);
		REQUIRE(digest[0] == 0x07e547d9);
		REQUIRE(digest[1] == 0x586f6a73);
		REQUIRE(digest[2] == 0xf73fbac0);
		REQUIRE(digest[3] == 0x435ed769);
		REQUIRE(digest[4] == 0x51218fb7);
		REQUIRE(digest[5] == 0xd0c8d788);
		REQUIRE(digest[6] == 0xa309d785);
		REQUIRE(digest[7] == 0x436bbb64);
		REQUIRE(digest[8] == 0x2e93a252);
		REQUIRE(digest[9] == 0xa954f239);
		REQUIRE(digest[10] == 0x12547d1e);
		REQUIRE(digest[11] == 0x8a3b5ed6);
		REQUIRE(digest[12] == 0xe1bfd709);
		REQUIRE(digest[13] == 0x7821233f);
		REQUIRE(digest[14] == 0xa0538f3d);
		REQUIRE(digest[15] == 0xb854fee6);
	}
	SECTION("LARGE HASH")
	{
		//hash = 0e0d14ce aeae8b59 7542ec36 8c0128b9 2d1c8a93 d50d5dd2 dfeb7e24 a04ae324 d148c0f3 67cc4c26 f64a99d6 7b64023a 7882e043 db56b4b2 044ad779 4680fc57
		smpl::SHA2Streamable streamableHash = smpl::SHA2Streamable(smpl::SHA2Streamable::SHA_512);
		std::string testString = "This tool calculates an MD5 checksum of the given input data in your browser. The MD5 hashing algorithm is a one-way cryptographic function that accepts a message of any length as input and returns as output a fixed-length digest value to be used for authenticating the original message. This utility works just like the md5sum command line tool. It outputs a 32-byte MD5 hex string that is computed from the given input. It also has a multi-line option that lets you calculate MD5 of each input line individually. Coming soon, you'll also be able to change the output base. Hashabulous!";
		streamableHash.addData((unsigned char*)testString.data(), testString.size());
		streamableHash.endData();
		std::vector<unsigned int> digest = streamableHash.getOutputHash();
		REQUIRE(digest.size() == 16);
		REQUIRE(digest[0] == 0x0e0d14ce);
		REQUIRE(digest[1] == 0xaeae8b59);
		REQUIRE(digest[2] == 0x7542ec36);
		REQUIRE(digest[3] == 0x8c0128b9);
		REQUIRE(digest[4] == 0x2d1c8a93);
		REQUIRE(digest[5] == 0xd50d5dd2);
		REQUIRE(digest[6] == 0xdfeb7e24);
		REQUIRE(digest[7] == 0xa04ae324);
		REQUIRE(digest[8] == 0xd148c0f3);
		REQUIRE(digest[9] == 0x67cc4c26);
		REQUIRE(digest[10] == 0xf64a99d6);
		REQUIRE(digest[11] == 0x7b64023a);
		REQUIRE(digest[12] == 0x7882e043);
		REQUIRE(digest[13] == 0xdb56b4b2);
		REQUIRE(digest[14] == 0x044ad779);
		REQUIRE(digest[15] == 0x4680fc57);
	}
}