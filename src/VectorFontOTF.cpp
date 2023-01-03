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
		tr.tag = StringTools::byteSwap(tr.tag);
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

	void cmapFormat0(std::vector<unsigned char>& fBytes, int offset, int length, EncodingRecordOTF& er, std::unordered_map<uint32_t, uint32_t>& characterMapping)
	{
		//Make sure that the format says 0
		int actualOffset = er.subtableOffset + offset;
		uint16_t format = 0;
		std::memcpy(&format, &fBytes.data()[actualOffset], 2);
		format = StringTools::byteSwap(format);

		if(format != 0)
			return; //Error
		
		//4 bytes since we don't need the length, and language.
		actualOffset+=6;

		//read array of mappings
		for(int i=0; i<256; i++)
		{
			characterMapping[i] = fBytes[actualOffset+i];
		}
	}

	void cmapFormat4(std::vector<unsigned char>& fBytes, int offset, int length, EncodingRecordOTF& er, std::unordered_map<uint32_t, uint32_t>& characterMapping)
	{
		//Make sure that the format says 4
		int actualOffset = er.subtableOffset + offset;
		uint16_t format = 0;
		std::memcpy(&format, &fBytes.data()[actualOffset], 2);
		format = StringTools::byteSwap(format);

		if(format != 4)
			return; //Error
		
		uint16_t lengthOfStruct = 0;
		std::memcpy(&lengthOfStruct, &fBytes.data()[actualOffset+2], 2);
		lengthOfStruct = StringTools::byteSwap(lengthOfStruct);

		int endOfStruct = offset+er.subtableOffset+lengthOfStruct;

		//4 bytes since we don't need the length, and language.
		actualOffset+=6;

		uint16_t segCountX2, searchRange, entrySelector, rangeShift;
		std::memcpy(&segCountX2, &fBytes.data()[actualOffset], 2);
		std::memcpy(&searchRange, &fBytes.data()[actualOffset+2], 2);
		std::memcpy(&entrySelector, &fBytes.data()[actualOffset+4], 2);
		std::memcpy(&rangeShift, &fBytes.data()[actualOffset+6], 2);
		segCountX2 = StringTools::byteSwap(segCountX2);
		searchRange = StringTools::byteSwap(searchRange);
		entrySelector = StringTools::byteSwap(entrySelector);
		rangeShift = StringTools::byteSwap(rangeShift);

		actualOffset += 8;
		
		uint16_t segCount = segCountX2/2;
		std::vector<uint16_t> endCodes = std::vector<uint16_t>(segCount);
		std::vector<uint16_t> startCodes = std::vector<uint16_t>(segCount);
		std::vector<int16_t> idDelta = std::vector<int16_t>(segCount);
		std::vector<uint16_t> idRangeOffsets = std::vector<uint16_t>(segCount);

		std::memcpy(endCodes.data(), &fBytes.data()[actualOffset], segCountX2);
		actualOffset+=segCountX2 + 2; //extra reserved padding
		std::memcpy(startCodes.data(), &fBytes.data()[actualOffset], segCountX2);
		actualOffset+=segCountX2;
		std::memcpy(idDelta.data(), &fBytes.data()[actualOffset], segCountX2);
		actualOffset+=segCountX2;

		int idRangeOfLocation = actualOffset;
		int glyphIDLocation = actualOffset+segCountX2;
		
		std::memcpy(idRangeOffsets.data(), &fBytes.data()[actualOffset], segCountX2);
		actualOffset+=segCountX2;
		
		int lengthOfGlyphIDArray = (endOfStruct - actualOffset)/2;
		std::vector<uint16_t> glyphIDArray = std::vector<uint16_t>(lengthOfGlyphIDArray);
		
		std::memcpy(glyphIDArray.data(), &fBytes.data()[actualOffset], lengthOfGlyphIDArray*2);
		actualOffset += lengthOfGlyphIDArray*2; //Not needed since we won't be reading in this function anymore.

		//ByteSwap
		for(int i=0; i<segCount; i++)
		{
			endCodes[i] = StringTools::byteSwap(endCodes[i]);
			startCodes[i] = StringTools::byteSwap(startCodes[i]);
			idDelta[i] = StringTools::byteSwap(idDelta[i]);
			idRangeOffsets[i] = StringTools::byteSwap(idRangeOffsets[i]);

			// StringTools::println("Values at %d: %u, %u, %d, %u", i, endCodes[i], startCodes[i], idDelta[i], idRangeOffsets[i]);
		}

		for(int i=0; i<glyphIDArray.size(); i++)
		{
			glyphIDArray[i] = StringTools::byteSwap(glyphIDArray[i]);
		}

		if(endCodes.size() > 1)
		{
			for(int i=0; i<segCount; i++)
			{
				int totalRange = endCodes[i] - startCodes[i];
				for(int j=0; j<=totalRange; j++)
				{
					int index = -1;
					int c = j + startCodes[i] + idDelta[i];
					if(idRangeOffsets[i] != 0)
					{
						//Microsoft did weird things here.
						//The idRangeOffset specifies how much to move from its current location in the file to find the desired index.
						//Since we are using data structures around the data, we need to simulate that behavior and deduce the index in the array instead.

						int refIndex = idRangeOffsets[i] + (c-startCodes[i])*2; //How many shorts to move by.
						int finalLocation = ((refIndex + idRangeOfLocation+(i*2)) - glyphIDLocation)/2; //Divide the final thing by 2 to get the correct index.

						if(finalLocation < glyphIDArray.size() && finalLocation >= 0)
						{
							index = glyphIDArray[finalLocation];
						}
					}
					else
					{
						index = c;
					}
					characterMapping[startCodes[i] + j] = index;
				}
			}
		}
	}

	void cmapFormat6(std::vector<unsigned char>& fBytes, int offset, int length, EncodingRecordOTF& er, std::unordered_map<uint32_t, uint32_t>& characterMapping)
	{
		//Make sure that the format says 6
		int actualOffset = er.subtableOffset + offset;
		uint16_t format = 0;
		uint16_t firstCode = 0;
		uint16_t entryCount = 0;

		std::memcpy(&format, &fBytes.data()[actualOffset], 2);
		format = StringTools::byteSwap(format);

		if(format != 6)
			return; //Error
		
		actualOffset += 6; //move past format bytes and skip length and language bytes.

		std::memcpy(&firstCode, &fBytes.data()[actualOffset], 2);
		std::memcpy(&entryCount, &fBytes.data()[actualOffset+2], 2);
		actualOffset+=4;

		firstCode = StringTools::byteSwap(firstCode);
		entryCount = StringTools::byteSwap(entryCount);

		std::vector<uint16_t> glyphIDArray = std::vector<uint16_t>(entryCount);
		std::memcpy(glyphIDArray.data(), &fBytes.data()[actualOffset], entryCount*2);

		for(int i=0; i<entryCount; i++)
		{
			characterMapping[firstCode+i] = StringTools::byteSwap(glyphIDArray[i]);
		}
	}

	void cmapFormat12(std::vector<unsigned char>& fBytes, int offset, int length, EncodingRecordOTF& er, std::unordered_map<uint32_t, uint32_t>& characterMapping)
	{
		//Make sure that the format says 12
		int actualOffset = er.subtableOffset + offset;
		uint16_t format = 0;
		uint32_t numGroups = 0;

		std::memcpy(&format, &fBytes.data()[actualOffset], 2);
		format = StringTools::byteSwap(format);

		if(format != 12)
			return; //Error
		
		actualOffset += 4; //Move past format bytes and skip reserved bytes as well.
		actualOffset += 8; //Skip length and language bytes.
		
		std::memcpy(&numGroups, &fBytes.data()[actualOffset], 4);

		struct SequentialMapGroup
		{
			uint32_t startCharCode;
			uint32_t endCharCode;
			uint32_t startGlyphID;
		};

		std::vector<SequentialMapGroup> groups = std::vector<SequentialMapGroup>(numGroups);
		for(uint32_t i=0; i<groups.size(); i++)
		{
			for(uint32_t j=groups[i].startCharCode; j<=groups[i].endCharCode; j++)
			{
				characterMapping[j] = groups[i].startGlyphID + (j-groups[i].startCharCode);
			}
		}
	}

	void cmapFormat13(std::vector<unsigned char>& fBytes, int offset, int length, EncodingRecordOTF& er, std::unordered_map<uint32_t, uint32_t>& characterMapping)
	{
		//Make sure that the format says 13
		int actualOffset = er.subtableOffset + offset;
		uint16_t format = 0;
		uint32_t numGroups = 0;

		std::memcpy(&format, &fBytes.data()[actualOffset], 2);
		format = StringTools::byteSwap(format);

		if(format != 13)
			return; //Error
		
		actualOffset += 4; //Move past format bytes and skip reserved bytes as well.
		actualOffset += 8; //Skip length and language bytes.
		
		std::memcpy(&numGroups, &fBytes.data()[actualOffset], 4);

		struct SequentialMapGroup
		{
			uint32_t startCharCode;
			uint32_t endCharCode;
			uint32_t startGlyphID;
		};

		std::vector<SequentialMapGroup> groups = std::vector<SequentialMapGroup>(numGroups);
		for(uint32_t i=0; i<groups.size(); i++)
		{
			for(uint32_t j=groups[i].startCharCode; j<=groups[i].endCharCode; j++)
			{
				characterMapping[j] = groups[i].startGlyphID;
			}
		}
	}

	void cmapFormat14(std::vector<unsigned char>& fBytes, int offset, int length, EncodingRecordOTF& er, std::unordered_map<uint32_t, uint32_t>& characterMapping)
	{
		//Make sure that the format says 14
		int actualOffset = er.subtableOffset + offset;
		int startOfTable = er.subtableOffset + offset;
		uint16_t format = 0;
		uint32_t numVarSelectors = 0;

		std::memcpy(&format, &fBytes.data()[actualOffset], 2);
		format = StringTools::byteSwap(format);

		if(format != 14)
			return; //Error

		actualOffset += 6; //Skip length as it is not needed.

		std::memcpy(&numVarSelectors, &fBytes.data()[actualOffset], 4);
		numVarSelectors = StringTools::byteSwap(numVarSelectors);
		actualOffset += 4;

		struct VariationSelector
		{
			uint32_t varSelector;
			uint32_t defaultUVSOffset;
			uint32_t nonDefaultUVSOffset;
		};

		std::vector<VariationSelector> varSelectors = std::vector<VariationSelector>(numVarSelectors);
		for(int i=0; i<numVarSelectors; i++)
		{
			VariationSelector v;
			v.varSelector = (((uint32_t)fBytes[actualOffset]) << 16) + (((uint32_t)fBytes[actualOffset+1]) << 8) + (((uint32_t)fBytes[actualOffset+2]));
			v.defaultUVSOffset = (((uint32_t)fBytes[actualOffset+3]) << 24) + (((uint32_t)fBytes[actualOffset+4]) << 16) + (((uint32_t)fBytes[actualOffset+5]) << 8) + (((uint32_t)fBytes[actualOffset+6]));
			v.nonDefaultUVSOffset = (((uint32_t)fBytes[actualOffset+7]) << 24) + (((uint32_t)fBytes[actualOffset+8]) << 16) + (((uint32_t)fBytes[actualOffset+9]) << 8) + (((uint32_t)fBytes[actualOffset+10]));
			actualOffset += 11;
		}

		//TODO - Do the Variation Selector stuff with the default and non default tables.
		// for(int i=0; i<varSelectors.size(); i++)
		// {
		// 	v.
		// }
	} 

	void readCMAP(std::vector<unsigned char>& fBytes, int offset, int length, cmapOTF& cm, std::unordered_map<uint32_t, uint32_t>& characterMapping)
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
		
		//handle mappings.
		//Not doing all of them. Just 0, 4, 6, 12, 13, 14.
		//Not dealing with platformID or encodingID unless it causes an issue.
		for(int i=0; i<cm.encodingRecords.size(); i++)
		{
			uint16_t format = 0;
			std::memcpy(&format, &fBytes.data()[offset + cm.encodingRecords[i].subtableOffset], 2);
			format = StringTools::byteSwap(format);

			StringTools::println("Format: %d", format);
			if(format == 0)
			{
				cmapFormat0(fBytes, offset, length, cm.encodingRecords[i], characterMapping);
			}
			else if(format == 4)
			{
				cmapFormat4(fBytes, offset, length, cm.encodingRecords[i], characterMapping);
			}
			else if(format == 6)
			{
				cmapFormat6(fBytes, offset, length, cm.encodingRecords[i], characterMapping);
			}
			else if(format == 12)
			{
				cmapFormat12(fBytes, offset, length, cm.encodingRecords[i], characterMapping);
			}
			else if(format == 13)
			{
				cmapFormat13(fBytes, offset, length, cm.encodingRecords[i], characterMapping);
			}
			else if(format == 14)
			{
				//Not implemented
				cmapFormat14(fBytes, offset, length, cm.encodingRecords[i], characterMapping);
			}
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
			cmapOTF characterMapHeader;
			TableDirectoryOTF tableDir;
			std::unordered_map<uint32_t, uint32_t> characterMapping;
			
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
				else if(tr.tag == TagNames::cmap)
				{
					readCMAP(fileBytes, tr.offset, tr.length, characterMapHeader, characterMapping);
				}
				else if(tr.tag == TagNames::hmtx)
				{

				}
				else if(tr.tag == TagNames::maxp)
				{

				}
				else if(tr.tag == TagNames::name)
				{

				}
				else if(tr.tag == TagNames::os2)
				{

				}
				else if(tr.tag == TagNames::post)
				{

				}
				else if(tr.tag == TagNames::glyf)
				{

				}
				else if(tr.tag == TagNames::loca)
				{

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