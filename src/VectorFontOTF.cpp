#include "VectorFont.h"
#include "StringTools.h"

namespace glib
{
	enum TagNames
	{
		head = 0x68656164, //*//
		hhea = 0x68686561, //*//
		cmap = 0x636D6170, //*//
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
		uint32_t tag;
		uint32_t checksum;
		uint32_t offset;
		uint32_t length;
	};

	struct TableDirectoryOTF //size of 12 bytes plus list
	{
		uint32_t sfntVersion;
		uint16_t numTables;
		uint16_t searchRange;
		uint16_t entrySelector;
		uint16_t rangeShift;
		std::vector<TableRecordOTF> tableRecords;
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

	struct cmapOTF //size of 4 bytes plus list
	{
		uint16_t version;
		uint16_t numTables;
		std::vector<EncodingRecordOTF> encodingRecords;
	};

	void readTableRecord(std::vector<unsigned char>& fBytes, int offset, TableRecordOTF& tr, TableDirectoryOTF& o)
	{
		std::memcpy(&tr, &fBytes.data()[offset], sizeof(TableRecordOTF));
		tr.checksum = StringTools::byteSwap(tr.checksum);
		tr.length = StringTools::byteSwap(tr.length);
		tr.offset = StringTools::byteSwap(tr.offset);
		
		o.tableRecords.push_back(tr);
	}

	void readTableDir(std::vector<unsigned char>& fBytes, TableDirectoryOTF& o)
	{
		std::memcpy(&o, &fBytes.data()[0], 12);
		o.entrySelector = StringTools::byteSwap(o.entrySelector);
		o.numTables = StringTools::byteSwap(o.numTables);
		o.rangeShift = StringTools::byteSwap(o.rangeShift);
		o.searchRange = StringTools::byteSwap(o.searchRange);
		o.sfntVersion = StringTools::byteSwap(o.sfntVersion);

		int offset = 12;
		for(int i=0; i<o.numTables; i++)
		{
			TableRecordOTF nTableRecord;
			readTableRecord(fBytes, offset, nTableRecord, o);
			offset += sizeof(TableRecordOTF);
		}
	}

	void readHead(std::vector<unsigned char>& fBytes, int offset, int length, headOTF& h)
	{
		std::memcpy(&h, &fBytes.data()[offset], length);
		h.majorVersion = StringTools::byteSwap(h.majorVersion);
		h.minorVersion = StringTools::byteSwap(h.minorVersion);
		h.fontRevision = StringTools::byteSwap(h.fontRevision);
		h.checksumAdjustment = StringTools::byteSwap(h.checksumAdjustment);
		h.magicNumber = StringTools::byteSwap(h.magicNumber);
		h.flags = StringTools::byteSwap(h.flags);
		h.unitsPerEm = StringTools::byteSwap(h.unitsPerEm);
		h.created = StringTools::byteSwap(h.created);
		h.modified = StringTools::byteSwap(h.modified);
		h.xMin = StringTools::byteSwap(h.xMin);
		h.yMin = StringTools::byteSwap(h.yMin);
		h.xMax = StringTools::byteSwap(h.xMax);
		h.yMax = StringTools::byteSwap(h.yMax);
		h.macStyle = StringTools::byteSwap(h.macStyle);
		h.lowestRecPPEM = StringTools::byteSwap(h.lowestRecPPEM);
		h.fontDirectionHint = StringTools::byteSwap(h.fontDirectionHint);
		h.indexToLocFormat = StringTools::byteSwap(h.indexToLocFormat);
		h.glyphDataFormat = StringTools::byteSwap(h.glyphDataFormat);
	}

	void readHHead(std::vector<unsigned char>& fBytes, int offset, int length, hheadOTF& hh)
	{
		std::memcpy(&hh, &fBytes.data()[offset], length);
		hh.majorVersion = StringTools::byteSwap(hh.majorVersion);
		hh.minorVersion = StringTools::byteSwap(hh.minorVersion);
		hh.ascender = StringTools::byteSwap(hh.ascender);
		hh.descender = StringTools::byteSwap(hh.descender);
		hh.lineGap = StringTools::byteSwap(hh.lineGap);
		hh.advanceWidthMax = StringTools::byteSwap(hh.advanceWidthMax);
		hh.minLeftSideBearing = StringTools::byteSwap(hh.minLeftSideBearing);
		hh.minRightSideBearing = StringTools::byteSwap(hh.minRightSideBearing);
		hh.xMaxExtent = StringTools::byteSwap(hh.xMaxExtent);
		hh.caretSlopeRise = StringTools::byteSwap(hh.caretSlopeRise);
		hh.caretSlopeRun = StringTools::byteSwap(hh.caretSlopeRun);
		hh.caretOffset = StringTools::byteSwap(hh.caretOffset);
		hh.metricDataFormat = StringTools::byteSwap(hh.metricDataFormat);
		hh.numberOfHMetrics = StringTools::byteSwap(hh.numberOfHMetrics);
	}

	void readCMAP(std::vector<unsigned char>& fBytes, int offset, int length, cmapOTF& cm)
	{
		std::memcpy(&cm, &fBytes.data()[offset], 4);
		cm.version = StringTools::byteSwap(cm.version);
		cm.numTables = StringTools::byteSwap(cm.numTables);

		int secondOffset = 4;
		for(int i=0; i<cm.numTables; i++)
		{
			EncodingRecordOTF er;
			std::memcpy(&er, &fBytes.data()[offset + secondOffset], sizeof(EncodingRecordOTF));
			secondOffset += sizeof(EncodingRecordOTF);

			er.platformID = StringTools::byteSwap(er.platformID);
			er.encodingID = StringTools::byteSwap(er.encodingID);
			er.subtableOffset = StringTools::byteSwap(er.subtableOffset);
			
			cm.encodingRecords.push_back(er);
		}

	}

	bool VectorFont::loadOTFFont(File file)
	{
		//REQUIRED: cmap, head, hhea, hmtx, maxp, name, OS/2, post
		//TTF: glyf, loca (Others are optional tables)
		//CFF: CFF, CFF2, VORG (If not ttf)
		//SVG: SVG
		//Others (May show up): BASE, GDEF, GPOS, GSUB, JSTF, MATH
		//Others2 (May show up): avar, cvar, fvar, gvar, HVAR, MVAR, STAT, VVAR

		//should only be one of each.
		SimpleFile f = SimpleFile(file, SimpleFile::READ | SimpleFile::ASCII);
		if(f.isOpen())
		{
			std::vector<unsigned char> fileBytes = f.readFullFileAsBytes();
			f.close();

			headOTF header;
			hheadOTF secondHeader;
			cmapOTF characterMap;
			TableDirectoryOTF tableDir;
			
			//read table directory at the start of the file.
			readTableDir(fileBytes, tableDir);

			for(TableRecordOTF& tr : tableDir.tableRecords)
			{
				if(tr.tag == TagNames::head)
				{
					readHead(fileBytes, tr.offset, tr.length, header);
				}
				else if(tr.tag == TagNames::hhea)
				{
					readHHead(fileBytes, tr.offset, tr.length, secondHeader);
				}
			}
		}
		else
		{
			StringTools::println("ERROR OPENING FILE");
			return false;
		}

		return false;
	}
};