#include "VectorFont.h"
#include "StringTools.h"

namespace glib
{
	enum TagNames
	{
		head = 0x68656164,
		hhea = 0x68686561,
		cmap = 0x636D6170,
		hmtx = 0x686D7478,
		maxp = 0x6D617870,
		name = 0x6E616D65,
		os2  = 0x4F532F32,
		post = 0x706F7374,
		glyf = 0x676C7966,
		loca = 0x6C6F6361
	};

	struct TableRecordOTF
	{
		uint64_t tag;
		uint32_t checksum;
		uint32_t offset;
		uint32_t length;
	};

	struct headOTF
	{
		uint16_t majorVersion;
		uint16_t minorVersion;
		int32_t fontRevision; //Fixed decimal number
		uint32_t checksumAdjustment;
		uint32_t magicNumber = 0x5F0F3CF5;
		uint16_t flags;
		uint16_t unitsPerEm;
		int64_t created; //LONGDATETIME
		int64_t modified; //LONGDATETIME
		int16_t xMin;
		int16_t yMin;
		int16_t xMax;
		int16_t yMax;
		uint16_t macStyle;
		uint16_t lowestRecPPEM;
		int16_t fontDirectionHint;
		int16_t indexToLocFormat;
		int16_t glyphDataFormat;
	};

	struct hheadOTF
	{
		uint16_t majorVersion;
		uint16_t minorVersion;
		int16_t ascender; //In Font units or whatever it is called
		int16_t descender; //In Font units or whatever it is called
		int16_t lineGap; //In Font units or whatever it is called
		uint16_t advanceWidthMax; //In Font units or whatever it is called
		int16_t minLeftSideBearing;
		int16_t minRightSideBearing;
		int16_t xMaxExtent;
		int16_t caretSlopeRise;
		int16_t caretSlopeRun;
		int16_t caretOffset;
		int16_t reserved[4];
		int16_t metricDataFormat;
		uint16_t numberOfHMetrics;
	};

	struct EncodingRecordOTF
	{
		uint16_t platformID;
		uint16_t encodingID;
		uint32_t subtableOffset;
	};

	struct cmapOTF
	{
		uint16_t version;
		uint16_t numTables;
		std::vector<EncodingRecordOTF> encodingRecords;
	};

	void readTableRecord(std::vector<unsigned char>& bytes)
	{
		//tag, uint32, 
	}

	bool VectorFont::loadOTFFont(File file)
	{
		//REQUIRED: cmap, head, hhea, hmtx, maxp, name, OS/2, post
		//TTF: glyf, loca (Others are optional tables)
		//CFF: CFF, CFF2, VORG (If not ttf)
		//SVG: SVG
		//Others (May show up): BASE, GDEF, GPOS, GSUB, JSTF, MATH
		//Others2 (May show up): avar, cvar, fvar, gvar, HVAR, MVAR, STAT, VVAR

		return false;
	}
};