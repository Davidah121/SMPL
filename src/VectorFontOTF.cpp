#include "VectorFont.h"
#include "StringTools.h"

namespace smpl
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

	
	struct EncodingRecordOTF
	{
		uint16_t platformID;
		uint16_t encodingID;
		uint32_t subtableOffset;
	};

	//Set alignment to 1 byte. Packs everything together with no additional padding.
	//MSVC syntax but works on GCC and Clang
	#pragma pack(push, 1)
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

	struct maxpOTF
	{
		uint32_t version; //packed major and minor as 2 16-bit numbers
		uint16_t numGlyphs;
		uint16_t maxPoints;
		uint16_t maxContours;
		uint16_t maxCompositePoints;
		uint16_t maxCompositeContours;
		uint16_t maxZones;
		uint16_t maxTwilightPoints;
		uint16_t maxStorage;
		uint16_t maxFunctionDefs;
		uint16_t maxInstructionDef;
		uint16_t maxStackElements;
		uint16_t maxSizeOfInstructions;
		uint16_t maxComponentElements;
		uint16_t maxComponentDepth;
	};

	struct os2OTF
	{
		uint16_t version;
		int16_t xAvgCharWidth;
		uint16_t usWeightClass;
		uint16_t usWidthClass;
		uint16_t fsType;

		int16_t ySubscriptXSize;
		int16_t ySubscriptYSize;

		int16_t ySubscriptXOffset;
		int16_t ySubscriptYOffset;

		int16_t ySuperscriptXSize;
		int16_t ySuperscriptYSize;

		int16_t ySuperscriptXOffset;
		int16_t ySuperscriptYOffset;

		int16_t yStrikeoutSize;
		int16_t yStrikeoutPosition;
		
		int16_t sFamilyClass;
		uint8_t panose[10];
		uint32_t ulUnicodeRange1;
		uint32_t ulUnicodeRange2;
		uint32_t ulUnicodeRange3;
		uint32_t ulUnicodeRange4;

		uint32_t achVendID; //TAG
		uint16_t fsSelection;
		uint16_t usFirstCharIndex;
		uint16_t usLastCharIndex;
		int16_t sTypoAscender;
		int16_t sTypoDescender;
		int16_t sTypoLineGap;
		uint16_t usWinAscent;
		uint16_t usWinDescent;
		//END OF VERSION 0

		uint32_t ulCodePageRange1;
		uint32_t ulCodePageRange2;
		//END OF VERSION 1

		int16_t sxHeight;
		int16_t sCapHeight;
		uint16_t usDefaultChar;
		uint16_t usBreakChar;
		uint16_t usMaxContext;
		//END OF VERSION 2,3,4

		uint16_t usLowerOpticalPointSize;
		uint16_t usUpperOpticalPointSize;
		//END OF VERSION 5
	};

	struct postOTF
	{
		uint32_t version; //packed major and minor as 2 16-bit numbers
		uint32_t italicAngle; //fixed decimal number. split in half so upper half is significant. lower half is decimal.
		int16_t underlinePosition; //font unit
		int16_t underlineThickness; //font unit
		uint32_t isFixedPitch;

		//useless 
		uint32_t minMemType42;
		uint32_t maxMemType42;
		uint32_t minMemType1;
		uint32_t maxMemType1;
	};
	
	//Reset alignment to default behaviour
	#pragma pack(pop)

	
	struct cmapOTF //size of 4 bytes plus list
	{
		uint16_t version;
		uint16_t numTables;
		std::vector<EncodingRecordOTF> encodingRecords;
	};

	struct longHorMetric
	{
		uint16_t advanceWidth;
		int16_t lsb;
	};

	struct htmxOTF
	{
		std::vector<longHorMetric> hMetrics;
		std::vector<int16_t> leftSideBearingsEX;
	};

	struct locaOTF
	{
		std::vector<uint32_t> offsets;
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
		std::memcpy((void*)&o, &fBytes.data()[0], 12);
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
		StringTools::println("%d, %d", sizeof(headOTF), length);
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
		numGroups = StringTools::byteSwap(numGroups);
		actualOffset += 4;

		struct SequentialMapGroup
		{
			uint32_t startCharCode;
			uint32_t endCharCode;
			uint32_t startGlyphID;
		};

		std::vector<SequentialMapGroup> groups = std::vector<SequentialMapGroup>(numGroups);
		for(uint32_t i=0; i<groups.size(); i++)
		{
			std::memcpy(&groups[i], &fBytes.data()[actualOffset], sizeof(SequentialMapGroup));
			groups[i].startCharCode = StringTools::byteSwap(groups[i].startCharCode);
			groups[i].endCharCode = StringTools::byteSwap(groups[i].endCharCode);
			groups[i].startGlyphID = StringTools::byteSwap(groups[i].startGlyphID);
			actualOffset += sizeof(SequentialMapGroup);

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
		numGroups = StringTools::byteSwap(numGroups);
		actualOffset += 4;

		struct SequentialMapGroup
		{
			uint32_t startCharCode;
			uint32_t endCharCode;
			uint32_t startGlyphID;
		};

		std::vector<SequentialMapGroup> groups = std::vector<SequentialMapGroup>(numGroups);
		for(uint32_t i=0; i<groups.size(); i++)
		{
			std::memcpy(&groups[i], &fBytes.data()[actualOffset], sizeof(SequentialMapGroup));
			groups[i].startCharCode = StringTools::byteSwap(groups[i].startCharCode);
			groups[i].endCharCode = StringTools::byteSwap(groups[i].endCharCode);
			groups[i].startGlyphID = StringTools::byteSwap(groups[i].startGlyphID);

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
		std::memcpy((void*)&cm, &fBytes.data()[offset], 4);
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

	void readHMTX(std::vector<unsigned char>& fBytes, int offset, int length, htmxOTF& horizontalMetrics, uint16_t numberOfHMetrics, uint32_t numGlyphs)
	{
		uint16_t additionalMetrics = numGlyphs - numberOfHMetrics;
		if(numGlyphs < numberOfHMetrics)
			additionalMetrics = 0; //just in case
		
		horizontalMetrics.hMetrics = std::vector<longHorMetric>(numberOfHMetrics);
		horizontalMetrics.leftSideBearingsEX = std::vector<int16_t>(additionalMetrics);

		for(int i=0; i<numberOfHMetrics; i++)
		{
			int j = 2*i;
			horizontalMetrics.hMetrics[i].advanceWidth = (((uint16_t)fBytes[j+offset+1]) << 8) + ((uint16_t)fBytes[j+offset]);
			horizontalMetrics.hMetrics[i].lsb = (((int16_t)fBytes[j+offset+1]) << 8) + ((int16_t)fBytes[j+offset]);
		}
		
		int secondOffset = offset + numberOfHMetrics*2;
		for(int i=0; i<additionalMetrics; i++)
		{
			int j = 2*i;
			horizontalMetrics.leftSideBearingsEX[i] = (((int16_t)fBytes[j+secondOffset+1]) << 8) + ((int16_t)fBytes[j+secondOffset]);
		}
	}

	void readMAXP(std::vector<unsigned char>& fBytes, int offset, int length, maxpOTF& profile)
	{
		int helperIndex = offset;
		profile.version = (uint32_t)fBytes[helperIndex+3] + (((uint32_t)fBytes[helperIndex+2]) << 8) + (((uint32_t)fBytes[helperIndex+1]) << 16) + (((uint32_t)fBytes[helperIndex]) << 24);
		helperIndex+=4;

		profile.numGlyphs = (uint16_t)fBytes[helperIndex+1] + (((uint16_t)fBytes[helperIndex] )<< 8);
		helperIndex+=2;

		if(profile.version == 0x00005000)
			return;
		
		profile.maxPoints = (uint16_t)fBytes[helperIndex+1] +  (((uint16_t)fBytes[helperIndex] )<< 8);
		helperIndex+=2;

		profile.maxContours = (uint16_t)fBytes[helperIndex+1] +  (((uint16_t)fBytes[helperIndex] )<< 8);
		helperIndex+=2;

		profile.maxCompositePoints = (uint16_t)fBytes[helperIndex+1] +  (((uint16_t)fBytes[helperIndex] )<< 8);
		helperIndex+=2;

		profile.maxCompositeContours = (uint16_t)fBytes[helperIndex+1] +  (((uint16_t)fBytes[helperIndex] )<< 8);
		helperIndex+=2;

		profile.maxZones = (uint16_t)fBytes[helperIndex+1] +  (((uint16_t)fBytes[helperIndex] )<< 8);
		helperIndex+=2;

		profile.maxTwilightPoints = (uint16_t)fBytes[helperIndex+1] +  (((uint16_t)fBytes[helperIndex] )<< 8);
		helperIndex+=2;

		profile.maxStorage = (uint16_t)fBytes[helperIndex+1] +  (((uint16_t)fBytes[helperIndex] )<< 8);
		helperIndex+=2;

		profile.maxFunctionDefs = (uint16_t)fBytes[helperIndex+1] +  (((uint16_t)fBytes[helperIndex] )<< 8);
		helperIndex+=2;

		profile.maxInstructionDef = (uint16_t)fBytes[helperIndex+1] +  (((uint16_t)fBytes[helperIndex] )<< 8);
		helperIndex+=2;

		profile.maxStackElements = (uint16_t)fBytes[helperIndex+1] +  (((uint16_t)fBytes[helperIndex] )<< 8);
		helperIndex+=2;

		profile.maxSizeOfInstructions = (uint16_t)fBytes[helperIndex+1] +  (((uint16_t)fBytes[helperIndex] )<< 8);
		helperIndex+=2;

		profile.maxComponentElements = (uint16_t)fBytes[helperIndex+1] +  (((uint16_t)fBytes[helperIndex] )<< 8);
		helperIndex+=2;

		profile.maxComponentDepth = (uint16_t)fBytes[helperIndex+1] +  (((uint16_t)fBytes[helperIndex] )<< 8);
		helperIndex+=2;
	}

	void readOS2(std::vector<unsigned char>& fBytes, int offset, int length, os2OTF& os2Table)
	{
		//Since we allocate enough for version 5 of the table, we just fill to the specified length which should not
		//be more than the largest possible table. If it does, just fill to the size of the table. (future proofing)
		int maxLength = (sizeof(os2OTF) < length) ? sizeof(os2OTF) : length;

		//just use memcpy and some byte swaps. Im lazy
		memcpy((unsigned char*)&os2Table, &fBytes[offset], maxLength);

		//now byte swap everything but the array
		uint16_t* dataAsShorts = (uint16_t*)&os2Table;
		
		for(int i=0; i<16; i++)
		{
			dataAsShorts[i] = StringTools::byteSwap(dataAsShorts[i]);
		}
		
		os2Table.ulUnicodeRange1 = StringTools::byteSwap(os2Table.ulUnicodeRange1);
		os2Table.ulUnicodeRange2 = StringTools::byteSwap(os2Table.ulUnicodeRange2);
		os2Table.ulUnicodeRange3 = StringTools::byteSwap(os2Table.ulUnicodeRange3);
		os2Table.ulUnicodeRange4 = StringTools::byteSwap(os2Table.ulUnicodeRange4);

		os2Table.ulCodePageRange1 = StringTools::byteSwap(os2Table.ulCodePageRange1);
		os2Table.ulCodePageRange2 = StringTools::byteSwap(os2Table.ulCodePageRange2);

		dataAsShorts = (uint16_t*)&os2Table.fsSelection;
		for(int i=0; i<8; i++)
		{
			dataAsShorts[i] = StringTools::byteSwap(dataAsShorts[i]);
		}

		dataAsShorts = (uint16_t*)&os2Table.sxHeight;
		for(int i=0; i<7; i++)
		{
			dataAsShorts[i] = StringTools::byteSwap(dataAsShorts[i]);
		}
	}

	void readPost(std::vector<unsigned char>& fBytes, int offset, int length, postOTF& postTable)
	{
		//Version 2 and 2.5 have additional stuff but I'm not going to use those likely.
		//Just mac stuff
		int maxLength = (sizeof(postOTF) < length) ? sizeof(postOTF) : length;

		//just use memcpy and some byte swaps. Im lazy
		memcpy((unsigned char*)&postTable, &fBytes[offset], maxLength);

		//byte swaps by hand.
		postTable.version = StringTools::byteSwap(postTable.version);
		postTable.italicAngle = StringTools::byteSwap(postTable.italicAngle);
		postTable.underlinePosition = StringTools::byteSwap(postTable.underlinePosition);
		postTable.underlineThickness = StringTools::byteSwap(postTable.underlineThickness);
		postTable.isFixedPitch = StringTools::byteSwap(postTable.isFixedPitch);
		postTable.minMemType42 = StringTools::byteSwap(postTable.minMemType42);
		postTable.maxMemType42 = StringTools::byteSwap(postTable.maxMemType42);
		postTable.minMemType1 = StringTools::byteSwap(postTable.minMemType1);
		postTable.maxMemType1 = StringTools::byteSwap(postTable.maxMemType1);
	}

	void readLoca(std::vector<unsigned char>& fBytes, int offset, int length, locaOTF& locaTable, int16_t version, uint16_t n)
	{
		//Based on the wording, may be 2 bytes per value or 4 bytes per value.
		locaTable.offsets = std::vector<uint32_t>(n);

		int startIndex = offset;

		if(version == 0)
		{
			//uint16_t
			for(int i=0; i<n; i++)
			{
				locaTable.offsets[i] = ((uint32_t)fBytes[startIndex+1]) +  (((uint32_t)fBytes[startIndex]) << 8);
				startIndex+=2;
			}
		}
		else
		{
			//uint32_t
			for(int i=0; i<n; i++)
			{
				locaTable.offsets[i] = ((uint32_t)fBytes[startIndex+3]) +  (((uint32_t)fBytes[startIndex+2]) << 8) + (((uint32_t)fBytes[startIndex+1]) << 16) + (((uint32_t)fBytes[startIndex]) << 24);
				startIndex+=4;
			}
		}
	}

	void readGlyf(std::vector<unsigned char>& fBytes, int offset, int length, VectorSprite& mainSprite, locaOTF& locaTable)
	{
		//convert each glyph into a svg and store it in the VectorSprite
		
		//read header, read simple or composite glyph. (just simple for now)
		struct GlyphHeader
		{
			int16_t numberOfContours;
			int16_t xMin;
			int16_t yMin;
			int16_t xMax;
			int16_t yMax;
		};

		struct SimpleGlyphTable
		{
			std::vector<uint16_t> endPtsOfContours;
			uint16_t instructionLength;
			std::vector<uint8_t> instructions;
			// std::vector<uint8_t> flags;
			// std::vector<int16_t> xCoordinates; //could be uint8_t but is contained in int16_t
			// std::vector<int16_t> yCoordinates; //could be uint8_t but is contained in int16_t
		};

		for(int indexInLoc = 0; indexInLoc < locaTable.offsets.size(); indexInLoc++)
		{
			StringTools::println("Index: %d : %u", indexInLoc, locaTable.offsets[indexInLoc]);
			if(indexInLoc == 64)
				break;
			
			if(indexInLoc < locaTable.offsets.size()-1)
			{
				if(locaTable.offsets[indexInLoc] == locaTable.offsets[indexInLoc+1])
				{
					//empty glyph. Add blank graphic
					StringTools::println("%d: EMPTY GRAPH", indexInLoc);

					VectorGraphic* glyphAsSVG = new VectorGraphic(); //Width and Height should be known so fill this out later.
					VectorPath* currentPath = new VectorPath();
					
					currentPath->setFillMethod( VectorPath::NON_ZERO_RULE );
					// currentPath->setFillColor( {255,255,255,255} );
					glyphAsSVG->addShape(currentPath);
					mainSprite.addGraphic(glyphAsSVG);

					continue;
				}
			}

			int currIndex = offset + locaTable.offsets[indexInLoc];

			//readHeader
			GlyphHeader header = GlyphHeader();
			memcpy(&header, &fBytes[currIndex], sizeof(GlyphHeader));

			//do some byte swaps
			uint16_t* headerAsShorts = (uint16_t*)&header;
			for(int i=0; i<5; i++)
			{
				headerAsShorts[i] = StringTools::byteSwap(headerAsShorts[i]);
			}

			// StringTools::println("START INDEX: %d", currIndex);
			// StringTools::println("%d", header.numberOfContours);

			currIndex += sizeof(GlyphHeader);
			
			if(header.numberOfContours >= 0)
			{
				SimpleGlyphTable glyph;
				glyph.endPtsOfContours = std::vector<uint16_t>(header.numberOfContours);

				for(int i=0; i<header.numberOfContours; i++)
				{
					glyph.endPtsOfContours[i] = ((uint16_t)fBytes[currIndex+1]) + (((uint16_t)fBytes[currIndex]) << 8);
					currIndex+=2;
				}

				glyph.instructionLength = ((uint16_t)fBytes[currIndex+1]) + (((uint16_t)fBytes[currIndex]) << 8);
				currIndex+=2;

				if(glyph.instructionLength > 0)
				{
					glyph.instructions = std::vector<uint8_t>(glyph.instructionLength);
					for(int i=0; i<glyph.instructionLength; i++)
					{
						glyph.instructions[i] = fBytes[currIndex];
						currIndex++;
					}

					//ignore for now. Should not need
				}

				if(header.numberOfContours == 0)
				{
					//probably add empty glyph
					// StringTools::println("SIMPLE BUT NO DATA");
					StringTools::println("%d: SIMPLE BUT NO DATA", indexInLoc);

					VectorGraphic* glyphAsSVG = new VectorGraphic(); //Width and Height should be known so fill this out later.
					VectorPath* currentPath = new VectorPath();
					
					currentPath->setFillMethod( VectorPath::NON_ZERO_RULE );
					// currentPath->setFillColor( {255,255,255,255} );
					glyphAsSVG->addShape(currentPath);
					mainSprite.addGraphic(glyphAsSVG);

					continue;
				}

				//read points. They depend on how many points from previous end contour point
				uint16_t size = glyph.endPtsOfContours.back() + 1;
				std::vector<uint8_t> flags = std::vector<uint8_t>(size);
				std::vector<Vec2f> points = std::vector<Vec2f>(size);

				int i = 0;
				while(i < size)
				{
					uint8_t newFlag = fBytes[currIndex];
					currIndex++;
					flags[i] = newFlag;
					i++;
					if(newFlag & 0x08)
					{
						int repeatCount = fBytes[currIndex];
						currIndex++;
						for(int j=0; j<repeatCount; j++)
						{
							flags[i+j] = newFlag;
						}
						i+=repeatCount;
					}
				}

				// StringTools::println("START OF COORDS: %d", currIndex);
				
				int16_t lastPoint = 0;
				for(int i=0; i<size; i++)
				{
					//read x coords
					uint8_t flag = flags[i];
					if(flag & 0x02)
					{
						//read 1 byte
						if(flag & 0x10)
							points[i].x = fBytes[currIndex] + lastPoint;
						else
							points[i].x = -fBytes[currIndex] + lastPoint;
						
						currIndex++;
					}
					else
					{
						//read 2 bytes
						if(flag & 0x10)
						{
							if(i == 0)
								points[i].x = 0;
							else
								points[i].x = points[i-1].x;
						}
						else
						{
							int16_t nPoint = ((int16_t)fBytes[currIndex+1]) + (((int16_t)fBytes[currIndex])<<8);
							int16_t relMove = nPoint + lastPoint;
							points[i].x = relMove;
							currIndex+=2;
						}
					}
					lastPoint = points[i].x;
				}

				//read y coords
				lastPoint = 0;
				for(int i=0; i<size; i++)
				{
					uint8_t flag = flags[i];
					if(flag & 0x04)
					{
						//read 1 byte
						if(flag & 0x20)
							points[i].y = fBytes[currIndex] + lastPoint;
						else
							points[i].y = -fBytes[currIndex] + lastPoint;
						
						currIndex++;
					}
					else
					{
						//read 2 bytes
						if(flag & 0x20)
						{
							if(i == 0)
								points[i].y = 0;
							else
								points[i].y = points[i-1].y;
						}
						else
						{
							int16_t nPoint = ((int16_t)fBytes[currIndex+1]) + (((int16_t)fBytes[currIndex])<<8);
							int16_t relMove = nPoint + lastPoint;
							points[i].y = relMove;
							currIndex+=2;
						}
					}
					lastPoint = points[i].y;
				}

				// if(indexInLoc == 455)
				// {
				// 	for(int i=0; i<size; i++)
				// 	{
				// 		if(flags[i] & 0x01)
				// 		{
				// 			StringTools::println("ON: x=%d y=%d", (int)points[i].x, (int)points[i].y);
				// 		}
				// 		else
				// 		{
				// 			StringTools::println("OFF: x=%d y=%d", (int)points[i].x, (int)points[i].y);
				// 		}
				// 	}
				// }

				//add them to a vector path in an SVG
				VectorGraphic* glyphAsSVG = new VectorGraphic(); //Width and Height should be known so fill this out later.
				VectorPath* currentPath = new VectorPath();

				std::vector<Vec2f> collectionOfPoints;
				int lastOnCurvePoint = 0;
				int lastEndPoint = 0;

				//MS made things complicated since control points can occur right after each other but there are only quadratic curves.
				//Some interpolation is needed along with collecting points.
				i=0;
				int j=0;
				Vec2f startP;

				while(i < size)
				{
					if(indexInLoc == 4)
					{
						StringTools::println("i = %d", i);
					}

					if(i > lastEndPoint || i == 0)
					{
						if(i != 0)
						{
							currentPath->addClosePath();
							if(indexInLoc == 4)
							{
								StringTools::println("Z");
							}
						}
						
						if(indexInLoc == 4)
						{
							StringTools::println("M x=%d, y=%d", (int)points[i].x, (int)points[i].y);
						}

						currentPath->addMoveTo(points[i]);
						startP = points[i];
						lastOnCurvePoint = i;
						i++;

						lastEndPoint = glyph.endPtsOfContours[j];
						j++;
					}
					else
					{
						// StringTools::println("x=%d, y=%d", (int)points[i].x, (int)points[i].y);
						int extraPoints = i - lastOnCurvePoint;

						if(flags[i] & 0x01 || i == lastEndPoint)
						{
							if(extraPoints == 1)
							{
								if(indexInLoc == 4)
								{
									StringTools::println("L x=%d, y=%d", (int)points[i].x, (int)points[i].y);
								}
								currentPath->addLineTo(points[i]);
							}
							else if(extraPoints == 2)
							{
								if(indexInLoc == 4)
								{
									StringTools::println("Q x=%d, y=%d, x2=%d, y2=%d", (int)points[i-1].x, (int)points[i-1].y, (int)points[i].x, (int)points[i].y);
								}
								currentPath->addQuadTo(points[i-1], points[i]);
							}
							else if(extraPoints >=3)
							{
								if((flags[i] & 0x01) == false)
								{
									for(int k=lastOnCurvePoint+1; k<i; k++)
									{
										Vec2f impliedPoint;
										
										// if(k < i-1)
											impliedPoint = (points[k] + points[k+1])/2;
										// else
										// 	impliedPoint = (points[k] + startP)/2;
										
										currentPath->addQuadTo(points[k], impliedPoint);
										if(indexInLoc == 4)
										{
											StringTools::println("Q x=%d, y=%d, x2=%d, y2=%d", (int)points[k].x, (int)points[k].y, (int)impliedPoint.x, (int)impliedPoint.y);
										}
									}
									currentPath->addQuadTo(points[i], startP);
									if(indexInLoc == 4)
									{
										StringTools::println("Q x=%d, y=%d, x2=%d, y2=%d", (int)points[i].x, (int)points[i].y, (int)startP.x, (int)startP.y);
									}
								}
								else
								{
									for(int k=lastOnCurvePoint+1; k<i-1; k++)
									{
										Vec2f impliedPoint = (points[k] + points[k+1])/2;
										currentPath->addQuadTo(points[k], impliedPoint);
										if(indexInLoc == 4)
										{
											StringTools::println("Q x=%d, y=%d, x2=%d, y2=%d", (int)points[k].x, (int)points[k].y, (int)impliedPoint.x, (int)impliedPoint.y);
										}
									}
									currentPath->addQuadTo(points[i-1], points[i]);
									if(indexInLoc == 4)
									{
										StringTools::println("Q x=%d, y=%d, x2=%d, y2=%d", (int)points[i-1].x, (int)points[i-1].y, (int)points[i].x, (int)points[i].y);
									}
								}
							}
							lastOnCurvePoint = i;
						}

						i++;
					}
				}
				
				//Add to path to the graphic, add graphic to sprite.
				//DON'T DELETE HERE. THE SPRITE SHOULD DELETE THEM.
				currentPath->addClosePath();
				StringTools::println("%d: SIMPLE", indexInLoc);
				currentPath->setFillMethod( VectorPath::NON_ZERO_RULE );
				currentPath->setFillColor( {0,0,0,255} );
				glyphAsSVG->addShape(currentPath);
				mainSprite.addGraphic(glyphAsSVG);
			
			}
			else
			{
				//composite. Ignore for now
				StringTools::println("%d: COMPOSITE", indexInLoc);
				
				VectorGraphic* glyphAsSVG = new VectorGraphic(); //Width and Height should be known so fill this out later.
				VectorPath* currentPath = new VectorPath();
				currentPath->setFillMethod( VectorPath::NON_ZERO_RULE );
				currentPath->setFillColor( {0,0,0,255} );
				glyphAsSVG->addShape(currentPath);
				mainSprite.addGraphic(glyphAsSVG);


				struct ComponentGlyphTable
				{
					uint16_t flags;
					uint16_t glyphIndex;
					int32_t arg1; //Im lazy
					int32_t arg2; //Im lazy
				};

				std::vector<ComponentGlyphTable> cgTables;

				while(true)
				{
					ComponentGlyphTable cgTable;

					cgTable.flags = ((uint16_t)fBytes[currIndex+1]) + (((uint16_t)fBytes[currIndex]) << 8);
					currIndex+=2;
					cgTable.glyphIndex = ((uint16_t)fBytes[currIndex+1]) + (((uint16_t)fBytes[currIndex]) << 8);
					currIndex+=2;

					if(cgTable.flags & 0x0001)
					{
						if(cgTable.flags & 0x0002)
						{
							cgTable.arg1 = ((int16_t)fBytes[currIndex+1]) + (((int16_t)fBytes[currIndex]) << 8);
							currIndex+=2;
							cgTable.arg2 = ((int16_t)fBytes[currIndex+1]) + (((int16_t)fBytes[currIndex]) << 8);
							currIndex+=2;
						}
						else
						{
							cgTable.arg1 = ((uint16_t)fBytes[currIndex+1]) + (((uint16_t)fBytes[currIndex]) << 8);
							currIndex+=2;
							cgTable.arg2 = ((uint16_t)fBytes[currIndex+1]) + (((uint16_t)fBytes[currIndex]) << 8);
							currIndex+=2;
						}
					}
					else
					{
						if(cgTable.flags & 0x0002)
						{
							cgTable.arg1 = (char)fBytes[currIndex];
							currIndex+=1;
							cgTable.arg2 = (char)fBytes[currIndex];
							currIndex+=1;
						}
						else
						{
							
							cgTable.arg1 = fBytes[currIndex];
							currIndex+=1;
							cgTable.arg2 = fBytes[currIndex];
							currIndex+=1;
						}
					}

					cgTables.push_back(cgTable);
					if((cgTable.flags & 0x0020) == false)
					{
						break;
					}
				}

				if(cgTables.front().flags & 0x0100)
				{
					int16_t instructionLength = ((uint16_t)fBytes[currIndex+1]) + (((uint16_t)fBytes[currIndex]) << 8);
					currIndex+=2;
					std::vector<uint8_t> instructions = std::vector<uint8_t>(instructionLength);
					for(int i=0; i<instructionLength; i++)
					{
						instructions[i] = fBytes[currIndex];
						currIndex++;
					}
				}

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
			htmxOTF horizontalMetricsTable;
			maxpOTF maximumProfileTable;
			os2OTF os2Table;
			locaOTF locaTable;
			postOTF postTable;

			TableDirectoryOTF tableDir;
			std::unordered_map<uint32_t, uint32_t> characterMapping;
			
			//read table directory at the start of the file.
			readTableDir(fileBytes, tableDir);

			//order to read:
			//		head, hhea, maxp, cmap, hmtx, os2, post, loca, glyf
			// Can read like this to avoid issues: head, hhea, maxp, cmap, os2, post, glyf (In any order)
			// Then in a separate loop: hmtx, loca 

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
				else if(tr.tag == TagNames::maxp)
				{
					readMAXP(fileBytes, tr.offset, tr.length, maximumProfileTable);
				}
				else if(tr.tag == TagNames::name)
				{
					//skipping but could be necessary for certain cases. (normal text editors and stuff)
				}
				else if(tr.tag == TagNames::os2)
				{
					readOS2(fileBytes, tr.offset, tr.length, os2Table);
				}
				else if(tr.tag == TagNames::post)
				{
					readPost(fileBytes, tr.offset, tr.length, postTable);
				}
				
				//may need GSUB
			}

			for(TableRecordOTF& tr : tableDir.tableRecords)
			{
				if(tr.tag == TagNames::loca)
				{
					readLoca(fileBytes, tr.offset, tr.length, locaTable, header.indexToLocFormat, maximumProfileTable.numGlyphs+1);
					// for(int i=0; i<locaTable.offsets.size(); i++)
					// {
					// 	StringTools::println("%d: offset = %u", i, locaTable.offsets[i]);
					// 	if(i == 64)
					// 	{
					// 		break;
					// 	}
					// }
				}
				else if(tr.tag == TagNames::hmtx)
				{
					readHMTX(fileBytes, tr.offset, tr.length, horizontalMetricsTable, secondHeader.numberOfHMetrics, maximumProfileTable.numGlyphs);
				}
			}

			for(TableRecordOTF& tr : tableDir.tableRecords)
			{
				if(tr.tag == TagNames::glyf)
				{
					readGlyf(fileBytes, tr.offset, tr.length, fontSprite, locaTable);
				}
			}

			// return false;
			//draw the first 5 letters starting from 'A' to 'E'

			// for(int i='A'; i<='E'; i++)
			// {
			// 	uint32_t glyphLocation = characterMapping[i];
			// 	StringTools::println("glyphLocation is %u", glyphLocation);
				
			// 	VectorGraphic* glyph = fontSprite.getGraphic(glyphLocation);
			// 	if(glyph == nullptr)
			// 	{
			// 		StringTools::println("ERROR FINDING %c. Its location is %u", i, glyphLocation);
			// 	}
			// 	else
			// 	{
			// 		Image img = Image(1024, 1024);
			// 		img.setAllPixels({255,255,255,255});
			// 		glyph->draw(&img);
					
			// 		std::string fileString = "TTFTest/img_";
			// 		fileString += (char)i;
			// 		img.saveBMP(fileString+".bmp");
			// 	}
			// }

			for(int i=0; i<=28; i++)
			{
				uint32_t glyphLocation = i;
				
				StringTools::println("Location: %u", glyphLocation);
				VectorGraphic* glyph = fontSprite.getGraphic(glyphLocation);

				if(glyph == nullptr)
				{
					StringTools::println("ERROR FINDING %c. Its location is %u", i, glyphLocation);
				}
				else
				{
					glyph->setTransform(MathExt::scale2D(0.5, 0.5));
					Image img = Image(1024, 1024);
					img.setAllPixels({255,255,255,255});
					glyph->draw(&img);
					
					std::string fileString = "TTFTest/img_";
					fileString += std::to_string(i);
					img.saveBMP(fileString+".bmp");
				}
			}

			// return false;

			//Set the FontCharInfo and match glyphs with characters
			//need glyph to be done, hmtx, cmap, head. maybe loca
		}
		else
		{
			StringTools::println("ERROR OPENING FILE");
			return false;
		}

		return false;
	}
};