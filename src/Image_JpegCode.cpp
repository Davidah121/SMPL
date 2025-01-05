#include "Image.h"
#include <vector>
#include "SimpleFile.h"
#include "Compression.h"
#include "SimpleGraphics.h"
#include <iostream>
#include "StringTools.h"
#include "Sort.h"
#include "ColorSpaceConverter.h"
#include "System.h"

namespace smpl
{
	struct huffThing
	{
		int value;
		BinarySet codeVal;
	};

	struct DCTChannel
	{
		int16_t y = 0;
		int16_t cb = 0;
		int16_t cr = 0;
	};

	struct quickVec3f
	{
		float x=0;
		float y=0;
		float z=0;

		quickVec3f() {}
		quickVec3f(float v1, float v2, float v3)
		{
			x = v1;
			y = v2;
			z = v3;
		}
	};

	struct JPEGData
	{
		int eobRun = 0;
		int dri = 0;
		int precisionDiv = 255; //8 bit precision
		int precisionSub = 128; //8 bit precision subtraction
		int widthRoundedTo8;
		int heightRoundedTo8;

		std::vector<MatrixF> quantizationTables;
		std::vector<BinaryTree<HuffmanNode>*> huffmanTreesDC;
		std::vector<BinaryTree<HuffmanNode>*> huffmanTreesAC;

		std::vector<unsigned char> compressedData;
		std::vector<unsigned short> componentData;

		std::vector<unsigned short> samplingRate;
	};

	MatrixF getLuminanceQuantizationMatrix(int qualityLevel)
	{
		MatrixF qualityMatrix = MatrixF(8,8);
		double qualityDouble = (double)(MathExt::clamp(qualityLevel, 1, 10)-1)/9.0;

		qualityMatrix[0][0] = MathExt::lerp(16, 2, qualityDouble);     qualityMatrix[0][1] = MathExt::lerp(11, 1.5, qualityDouble);   qualityMatrix[0][2] = MathExt::lerp(10, 1.5, qualityDouble);   qualityMatrix[0][3] = MathExt::lerp(16, 2, qualityDouble);     qualityMatrix[0][4] = MathExt::lerp(24, 3.5, qualityDouble);   qualityMatrix[0][5] = MathExt::lerp(40, 5, qualityDouble);     qualityMatrix[0][6] = MathExt::lerp(51, 6, qualityDouble);     qualityMatrix[0][7] = MathExt::lerp(61, 7.5, qualityDouble); 
		qualityMatrix[1][0] = MathExt::lerp(12, 1.5, qualityDouble);   qualityMatrix[1][1] = MathExt::lerp(12, 1.5, qualityDouble);   qualityMatrix[1][2] = MathExt::lerp(14, 2, qualityDouble);     qualityMatrix[1][3] = MathExt::lerp(19, 2.5, qualityDouble);   qualityMatrix[1][4] = MathExt::lerp(26, 3.5, qualityDouble);   qualityMatrix[1][5] = MathExt::lerp(58, 7, qualityDouble);     qualityMatrix[1][6] = MathExt::lerp(60, 7, qualityDouble);     qualityMatrix[1][7] = MathExt::lerp(55, 7, qualityDouble); 
		qualityMatrix[2][0] = MathExt::lerp(14, 2, qualityDouble);     qualityMatrix[2][1] = MathExt::lerp(13, 2, qualityDouble);     qualityMatrix[2][2] = MathExt::lerp(16, 2, qualityDouble);     qualityMatrix[2][3] = MathExt::lerp(24, 3, qualityDouble);     qualityMatrix[2][4] = MathExt::lerp(40, 5, qualityDouble);     qualityMatrix[2][5] = MathExt::lerp(57, 7, qualityDouble);     qualityMatrix[2][6] = MathExt::lerp(69, 9, qualityDouble);     qualityMatrix[2][7] = MathExt::lerp(56, 8, qualityDouble); 
		qualityMatrix[3][0] = MathExt::lerp(14, 2, qualityDouble);     qualityMatrix[3][1] = MathExt::lerp(17, 2, qualityDouble);     qualityMatrix[3][2] = MathExt::lerp(22, 2.5, qualityDouble);   qualityMatrix[3][3] = MathExt::lerp(29, 4, qualityDouble);     qualityMatrix[3][4] = MathExt::lerp(51, 6.5, qualityDouble);   qualityMatrix[3][5] = MathExt::lerp(87, 10.5, qualityDouble);  qualityMatrix[3][6] = MathExt::lerp(80, 10.5, qualityDouble);  qualityMatrix[3][7] = MathExt::lerp(62, 9.5, qualityDouble); 
		qualityMatrix[4][0] = MathExt::lerp(18, 3, qualityDouble);     qualityMatrix[4][1] = MathExt::lerp(22, 3, qualityDouble);     qualityMatrix[4][2] = MathExt::lerp(37, 4.5, qualityDouble);   qualityMatrix[4][3] = MathExt::lerp(56, 7, qualityDouble);     qualityMatrix[4][4] = MathExt::lerp(68, 9, qualityDouble);     qualityMatrix[4][5] = MathExt::lerp(109, 13.5, qualityDouble); qualityMatrix[4][6] = MathExt::lerp(103, 14, qualityDouble);   qualityMatrix[4][7] = MathExt::lerp(77, 11.5, qualityDouble); 
		qualityMatrix[5][0] = MathExt::lerp(24, 3.5, qualityDouble);   qualityMatrix[5][1] = MathExt::lerp(35, 4.5, qualityDouble);   qualityMatrix[5][2] = MathExt::lerp(55, 7, qualityDouble);     qualityMatrix[5][3] = MathExt::lerp(64, 8.5, qualityDouble);   qualityMatrix[5][4] = MathExt::lerp(81, 10.5, qualityDouble);  qualityMatrix[5][5] = MathExt::lerp(104, 14, qualityDouble);   qualityMatrix[5][6] = MathExt::lerp(113, 15.5, qualityDouble); qualityMatrix[5][7] = MathExt::lerp(92, 13, qualityDouble); 
		qualityMatrix[6][0] = MathExt::lerp(49, 6, qualityDouble);     qualityMatrix[6][1] = MathExt::lerp(64, 7.5, qualityDouble);   qualityMatrix[6][2] = MathExt::lerp(78, 10, qualityDouble);    qualityMatrix[6][3] = MathExt::lerp(87, 11, qualityDouble);    qualityMatrix[6][4] = MathExt::lerp(103, 14, qualityDouble);   qualityMatrix[6][5] = MathExt::lerp(121, 16, qualityDouble);   qualityMatrix[6][6] = MathExt::lerp(120, 16, qualityDouble);   qualityMatrix[6][7] = MathExt::lerp(101, 14, qualityDouble); 
		qualityMatrix[7][0] = MathExt::lerp(72, 8.5, qualityDouble);   qualityMatrix[7][1] = MathExt::lerp(92, 10, qualityDouble);    qualityMatrix[7][2] = MathExt::lerp(95, 12.5, qualityDouble);  qualityMatrix[7][3] = MathExt::lerp(98, 13.5, qualityDouble);  qualityMatrix[7][4] = MathExt::lerp(112, 15, qualityDouble);   qualityMatrix[7][5] = MathExt::lerp(100, 14, qualityDouble);   qualityMatrix[7][6] = MathExt::lerp(103, 14.5, qualityDouble); qualityMatrix[7][7] = MathExt::lerp(99, 14, qualityDouble); 

		return qualityMatrix;
	}

	MatrixF getChrominanceQuantizationMatrix(int qualityLevel)
	{
		//LERP BETWEEN JPEG quality and Photoshop
		MatrixF qualityMatrix = MatrixF(8,8);

		double qualityDouble = (double)(MathExt::clamp(qualityLevel, 1, 10)-1)/9.0;

		qualityMatrix[0][0] = MathExt::lerp(17, 2, qualityDouble);      qualityMatrix[0][1] = MathExt::lerp(18, 2, qualityDouble);      qualityMatrix[0][2] = MathExt::lerp(24, 3, qualityDouble);      qualityMatrix[0][3] = MathExt::lerp(47, 6, qualityDouble);      qualityMatrix[0][4] = MathExt::lerp(99, 8, qualityDouble);      qualityMatrix[0][5] = MathExt::lerp(99, 9, qualityDouble);      qualityMatrix[0][6] = MathExt::lerp(99, 9, qualityDouble);      qualityMatrix[0][7] = MathExt::lerp(99, 9, qualityDouble); 
		qualityMatrix[1][0] = MathExt::lerp(18, 2, qualityDouble);      qualityMatrix[1][1] = MathExt::lerp(21, 3, qualityDouble);      qualityMatrix[1][2] = MathExt::lerp(26, 4, qualityDouble);      qualityMatrix[1][3] = MathExt::lerp(66, 6, qualityDouble);      qualityMatrix[1][4] = MathExt::lerp(99, 9, qualityDouble);      qualityMatrix[1][5] = MathExt::lerp(99, 9, qualityDouble);      qualityMatrix[1][6] = MathExt::lerp(99, 9, qualityDouble);      qualityMatrix[1][7] = MathExt::lerp(99, 9, qualityDouble); 
		qualityMatrix[2][0] = MathExt::lerp(24, 3, qualityDouble);      qualityMatrix[2][1] = MathExt::lerp(26, 4, qualityDouble);      qualityMatrix[2][2] = MathExt::lerp(56, 5, qualityDouble);      qualityMatrix[2][3] = MathExt::lerp(99, 9, qualityDouble);      qualityMatrix[2][4] = MathExt::lerp(99, 9, qualityDouble);      qualityMatrix[2][5] = MathExt::lerp(99, 9, qualityDouble);      qualityMatrix[2][6] = MathExt::lerp(99, 9, qualityDouble);      qualityMatrix[2][7] = MathExt::lerp(99, 9, qualityDouble); 
		qualityMatrix[3][0] = MathExt::lerp(47, 6, qualityDouble);      qualityMatrix[3][1] = MathExt::lerp(66, 6, qualityDouble);      qualityMatrix[3][2] = MathExt::lerp(99, 9, qualityDouble);      qualityMatrix[3][3] = MathExt::lerp(99, 9, qualityDouble);      qualityMatrix[3][4] = MathExt::lerp(99, 9, qualityDouble);      qualityMatrix[3][5] = MathExt::lerp(99, 9, qualityDouble);      qualityMatrix[3][6] = MathExt::lerp(99, 9, qualityDouble);      qualityMatrix[3][7] = MathExt::lerp(99, 9, qualityDouble); 
		qualityMatrix[4][0] = MathExt::lerp(99, 8, qualityDouble);      qualityMatrix[4][1] = MathExt::lerp(99, 9, qualityDouble);      qualityMatrix[4][2] = MathExt::lerp(99, 9, qualityDouble);      qualityMatrix[4][3] = MathExt::lerp(99, 9, qualityDouble);      qualityMatrix[4][4] = MathExt::lerp(99, 9, qualityDouble);      qualityMatrix[4][5] = MathExt::lerp(99, 9, qualityDouble);      qualityMatrix[4][6] = MathExt::lerp(99, 9, qualityDouble);      qualityMatrix[4][7] = MathExt::lerp(99, 9, qualityDouble); 
		qualityMatrix[5][0] = MathExt::lerp(99, 9, qualityDouble);      qualityMatrix[5][1] = MathExt::lerp(99, 9, qualityDouble);      qualityMatrix[5][2] = MathExt::lerp(99, 9, qualityDouble);      qualityMatrix[5][3] = MathExt::lerp(99, 9, qualityDouble);      qualityMatrix[5][4] = MathExt::lerp(99, 9, qualityDouble);      qualityMatrix[5][5] = MathExt::lerp(99, 9, qualityDouble);      qualityMatrix[5][6] = MathExt::lerp(99, 9, qualityDouble);      qualityMatrix[5][7] = MathExt::lerp(99, 9, qualityDouble); 
		qualityMatrix[6][0] = MathExt::lerp(99, 9, qualityDouble);      qualityMatrix[6][1] = MathExt::lerp(99, 9, qualityDouble);      qualityMatrix[6][2] = MathExt::lerp(99, 9, qualityDouble);      qualityMatrix[6][3] = MathExt::lerp(99, 9, qualityDouble);      qualityMatrix[6][4] = MathExt::lerp(99, 9, qualityDouble);      qualityMatrix[6][5] = MathExt::lerp(99, 9, qualityDouble);      qualityMatrix[6][6] = MathExt::lerp(99, 9, qualityDouble);      qualityMatrix[6][7] = MathExt::lerp(99, 9, qualityDouble); 
		qualityMatrix[7][0] = MathExt::lerp(99, 9, qualityDouble);      qualityMatrix[7][1] = MathExt::lerp(99, 9, qualityDouble);      qualityMatrix[7][2] = MathExt::lerp(99, 9, qualityDouble);      qualityMatrix[7][3] = MathExt::lerp(99, 9, qualityDouble);      qualityMatrix[7][4] = MathExt::lerp(99, 9, qualityDouble);      qualityMatrix[7][5] = MathExt::lerp(99, 9, qualityDouble);      qualityMatrix[7][6] = MathExt::lerp(99, 9, qualityDouble);      qualityMatrix[7][7] = MathExt::lerp(99, 9, qualityDouble); 

		return qualityMatrix;
	}

	BinaryTreeNode<HuffmanNode>* traverseTree(BinarySet* binSet, BinaryTree<HuffmanNode>* tree, int* startLoc)
	{
		BinaryTreeNode<HuffmanNode>* currNode = tree->getRoot();
		while(true)
		{
			if(currNode==nullptr)
			{
				return nullptr;
			}

			if(currNode->leftChild==nullptr && currNode->rightChild==nullptr)
			{
				return currNode;
			}

			if(binSet->getBit(*startLoc))
			{
				currNode = currNode->rightChild;
			}
			else
			{
				currNode = currNode->leftChild;
			}

			(*startLoc)++;
		}
		return nullptr;
	}

	void writeTable(BinaryTreeNode<HuffmanNode>* node, std::vector<huffThing>* data, BinarySet bin)
	{
		if(node!=nullptr)
		{
			if(node->data.frequency>0 && bin.size()>0)
			{
				data->push_back({node->data.value, bin});
			}

			if(node->leftChild!=nullptr)
			{
				BinarySet other = bin;
				other.add(false);

				writeTable(node->leftChild, data, other);
			}

			if(node->rightChild!=nullptr)
			{
				BinarySet other = bin;
				other.add(true);

				writeTable(node->rightChild, data, other);
			}
		}
	}

	int getZigZagPos(int index)
	{
		int pos[64] = {	0x00, 0x01, 0x10, 0x20, 0x11, 0x02, 0x03, 0x12,
						0x21, 0x30, 0x40, 0x31, 0x22, 0x13, 0x04, 0x05,
						0x14, 0x23, 0x32, 0x41, 0x50, 0x60, 0x51, 0x42,
						0x33, 0x24, 0x15, 0x06, 0x07, 0x16, 0x25, 0x34,
						0x43, 0x52, 0x61, 0x70, 0x71, 0x62, 0x53, 0x44,
						0x35, 0x26, 0x17, 0x27, 0x36, 0x45, 0x54, 0x63,
						0x72, 0x73, 0x64, 0x55, 0x46, 0x37, 0x47, 0x56,
						0x65, 0x74, 0x75, 0x66, 0x57, 0x67, 0x76, 0x77 };
		return pos[index];
	}

	void progressiveProcessEnd(HiResImage* img, DCTChannel* dctTableValues, JPEGData& jpegData)
	{
		int xMult = ((jpegData.samplingRate[0]>>4) & 0xF);
		int yMult = ((jpegData.samplingRate[0]) & 0xF);
		
		int addWidth = 8 / xMult;
		int addHeight = 8 / yMult;
		
		int offX = 0;
		int offY = 0;

		MatrixF q1 = MatrixF(8,8);
		MatrixF q2 = MatrixF(8,8);
		MatrixF q3 = MatrixF(8,8);

		MatrixF yMat = MatrixF(8,8);
		MatrixF cbMat = MatrixF(8,8);
		MatrixF crMat = MatrixF(8,8);

		//assuming imgMat is correctly setup
		for(int y=0; y<img->getHeight(); y+=8)
		{
			offX = 0;
			for(int x=0; x<img->getWidth(); x+=8)
			{
				for(int k=0; k<64; k++)
				{
					int pos = getZigZagPos(k);
					int y2 = k/8;
					int x2 = k%8;
					int oY2 = pos>>4;
					int oX2 = pos&0x0F;
					
					
					yMat[oY2][oX2] = dctTableValues[(y+y2)*jpegData.widthRoundedTo8 + (x+x2)].y;
					cbMat[oY2][oX2] = dctTableValues[(y+y2)*jpegData.widthRoundedTo8 + (x+x2)].cb;
					crMat[oY2][oX2] = dctTableValues[(y+y2)*jpegData.widthRoundedTo8 + (x+x2)].cr;
				}

				for(int y2=0; y2<8; y2++)
				{
					for(int x2=0; x2<8; x2++)
					{
						yMat[y2][x2] *= jpegData.quantizationTables[0][y2][x2];
						cbMat[y2][x2] *= jpegData.quantizationTables[1][y2][x2];
						crMat[y2][x2] *= jpegData.quantizationTables[1][y2][x2];
					}
				}

				MathExt::FCT8x8(yMat, &q1, true);
				MathExt::FCT8x8(cbMat, &q2, true);
				MathExt::FCT8x8(crMat, &q3, true);

				int yRange = img->getHeight() - y;
				yRange = MathExt::min(yRange, 8);
				int xRange = img->getWidth() - x;
				xRange = MathExt::min(xRange, 8);
				
				for(int y2=0; y2<yRange; y2++)
				{
					int chromaY = y2/yMult;

					for(int x2=0; x2<xRange; x2++)
					{
						int chromaX = x2/xMult;
						Color4f c;
						double v1 = (q1[y2][x2]+jpegData.precisionSub)/jpegData.precisionDiv;
						double v2 = (q2[chromaY+offY][chromaX+offX]+jpegData.precisionSub)/jpegData.precisionDiv;
						double v3 = (q3[chromaY+offY][chromaX+offX]+jpegData.precisionSub)/jpegData.precisionDiv;
						if(v1 < 0)
							v1 = 0;
						if(v2 < 0)
							v2 = 0;
						if(v3 < 0)
							v3 = 0;

						c.red = v1;
						c.green = v2;
						c.blue = v3;
						c.alpha = 1.0;

						img->getPixels()[(y+y2)*img->getWidth() + (x+x2)] = c;
					}
				}

				offX = (offX + addWidth) % 8;
			}
			offY = (offY + addHeight) % 8;
		}
	}

	void progressiveProcessFirst(HiResImage* img, DCTChannel* dctTableValues, JPEGData& jpegData, int selectionStart, int selectionEnd, int predictionForSuccessive)
	{
		//some notes: for each repeated matrix value, add the components together
		//progressive for 8x8 test1 consist of 2 passes over the matrix where the 1st pass is half the quality of the original
		//for other images, this can span over multiple passes.

		int startX = 0;
		int startY = 0;
		BinarySet binSet = BinarySet();
		int binNum = 0;
		binSet.setBitOrder(BinarySet::RMSB);
		binSet.setValues(jpegData.compressedData.data(), jpegData.compressedData.size());

		bool dcMode = false;
		if(selectionStart==0)
		{
			//use dc tree
			dcMode = true;
		}
		else
		{
			//use ac tree
			dcMode = false;
		}

		//the following 4 variables are named the exact same way they are named in the specifications
		int Ss = selectionStart; //Spectral Select Start
		int Se = selectionEnd; //Spectral Select End
		
		int Ah = predictionForSuccessive >> 4; //Successive Approximation High (shift used in the last scan)
		int Al = predictionForSuccessive & 0xF; //Successive Approximation Low (shift to use for this scan)

		DCTChannel blockAdd = DCTChannel();

		int addWidth = 8;
		int addHeight = 8;

		int currX = 0;
		int currY = 0;

		std::vector<unsigned short> nComponentData = std::vector<unsigned short>();

		if(jpegData.componentData.size() == 3)
		{
			//y, cb, cr
			addWidth = 8 * ((jpegData.samplingRate[0]>>4) & 0xF);
			addHeight = 8 * ((jpegData.samplingRate[0]) & 0xF);

			nComponentData.push_back(jpegData.componentData[0]);

			if(addWidth>8)
				nComponentData.push_back(jpegData.componentData[0]);
			if(addHeight>8)
				nComponentData.push_back(jpegData.componentData[0]);
			if(addWidth>8 && addHeight>8)
				nComponentData.push_back(jpegData.componentData[0]);
			
			nComponentData.push_back(jpegData.componentData[1]);
			nComponentData.push_back(jpegData.componentData[2]);
		}
		else
		{
			//just y or cb or cr
			addWidth = 8 * ((jpegData.samplingRate[0]>>4) & 0xF);
			addHeight = 8 * ((jpegData.samplingRate[0]) & 0xF);

			if(jpegData.componentData[0]>>8 == 1)
			{
				addWidth = 8;
				addHeight = 8;
			}
			nComponentData.push_back(jpegData.componentData[0]);
		}

		while(true)
		{
			currX = 0;
			currY = 0;
			for(size_t i=0; i<nComponentData.size(); i++)
			{
				unsigned short comD = nComponentData[i];
				int comNum = comD >> 8;
				int treeIndex1 = (comD >> 4) & 0x0F;
				int treeIndex2 = comD & 0x0F;

				BinaryTree<HuffmanNode>* dcTree = jpegData.huffmanTreesDC[treeIndex1];
				BinaryTree<HuffmanNode>* acTree = jpegData.huffmanTreesAC[treeIndex2];

				if(comNum!=1)
				{
					currX = 0;
					currY = 0;
				}

				int k = Ss;

				while(k <= Se)
				{
					if(dcMode)
					{
						BinaryTreeNode<HuffmanNode>* node = traverseTree(&binSet, dcTree, &binNum);
						if(node==nullptr)
						{
							StringTools::println("Error traversing tree DC PROG FIRST");
							return;
						}

						int category = node->data.value;
						int dcValue = 0;

						if(category>0)
						{
							if(binSet.getBit(binNum)==0)
							{
								//negative
								dcValue = -((1<<category)-1) + binSet.getBits(binNum+1, binNum+category, true);
							}
							else
							{
								dcValue = binSet.getBits(binNum, binNum+category, true);
							}
						}

						if(comNum==1)
						{
							dcValue += (int)blockAdd.y;
							blockAdd.y = dcValue;
						}
						else if(comNum==2)
						{
							dcValue += (int)blockAdd.cb;
							blockAdd.cb = dcValue;
						}
						else
						{
							dcValue += (int)blockAdd.cr;
							blockAdd.cr = dcValue;
						}
						
						dcValue = dcValue << Al;

						if(comNum==1)
						{
							dctTableValues[(startY+currY)*jpegData.widthRoundedTo8 + (startX+currX)].y = dcValue;
							// imgMat[startY+currY][startX+currX].x = dcValue;
						}
						else if(comNum==2)
						{
							int addX = addWidth/2;
							int addY = addHeight/2;
							if(addX != 8)
							{
								addX = 0;
							}
							if(addY != 8)
							{
								addY = 0;
							}
							
							dctTableValues[(startY)*jpegData.widthRoundedTo8 + (startX)].cb = dcValue;
							dctTableValues[(startY)*jpegData.widthRoundedTo8 + (startX+addX)].cb = dcValue;
							dctTableValues[(startY+addY)*jpegData.widthRoundedTo8 + (startX+addX)].cb = dcValue;
							dctTableValues[(startY+addY)*jpegData.widthRoundedTo8 + (startX)].cb = dcValue;
						}
						else
						{
							int addX = addWidth/2;
							int addY = addHeight/2;
							if(addX != 8)
							{
								addX = 0;
							}
							if(addY != 8)
							{
								addY = 0;
							}

							dctTableValues[(startY)*jpegData.widthRoundedTo8 + (startX)].cr = dcValue;
							dctTableValues[(startY)*jpegData.widthRoundedTo8 + (startX+addX)].cr = dcValue;
							dctTableValues[(startY+addY)*jpegData.widthRoundedTo8 + (startX+addX)].cr = dcValue;
							dctTableValues[(startY+addY)*jpegData.widthRoundedTo8 + (startX)].cr = dcValue;
						}

						binNum += category;
						
						k++;
					}
					else
					{
						if(jpegData.eobRun>0)
						{
							k = Se+1;
							jpegData.eobRun--;
							break;
						}

						BinaryTreeNode<HuffmanNode>* node = traverseTree(&binSet, acTree, &binNum);
						if(node==nullptr)
						{
							StringTools::println("Error traversing tree PROGRESSIVE AC FIRST");
							return;
						}

						int acV = node->data.value;
						int r = (acV>>4) & 0x0F;
						int s = acV & 0x0F;

						if(s==0)
						{
							//eob or zrl
							if(r==15)
							{
								//zrl - 16 zeros
								k+=16;
							}
							else
							{
								jpegData.eobRun = 1 << r;
								if(r>0)
								{
									jpegData.eobRun += binSet.getBits(binNum, binNum+r, true);
									binNum += r;
								}
							}
						}
						else
						{
							k+=r;

							int acValue = 0;

							if(binSet.getBit(binNum)==0)
							{
								//negative
								acValue = -((1<<s)-1) + binSet.getBits(binNum+1, binNum+s, true);
							}
							else
							{
								acValue = binSet.getBits(binNum, binNum+s, true);
							}
							binNum+=s;
							
							acValue = acValue << Al;

							
							int addY = k/8;
							int addX = k%8;

							if(comNum==1)
							{
								dctTableValues[(startY+currY+addY)*jpegData.widthRoundedTo8 + (startX+currX+addX)].y = acValue;
								// imgMat[startY+currY+addY][startX+currX+addX].x = acValue;
							}
							else if(comNum==2)
							{
								int oAddX = addWidth/2;
								int oAddY = addHeight/2;
								if(oAddX != 8)
								{
									oAddX = 0;
								}
								if(oAddY != 8)
								{
									oAddY = 0;
								}
								
								dctTableValues[(startY+addY)*jpegData.widthRoundedTo8 + (startX+addX)].cb = acValue;
								dctTableValues[(startY+addY)*jpegData.widthRoundedTo8 + (startX+addX+oAddX)].cb = acValue;
								dctTableValues[(startY+addY+oAddY)*jpegData.widthRoundedTo8 + (startX+addX+oAddX)].cb = acValue;
								dctTableValues[(startY+addY+oAddY)*jpegData.widthRoundedTo8 + (startX+addX)].cb = acValue;
							}
							else
							{
								int oAddX = addWidth/2;
								int oAddY = addHeight/2;
								if(oAddX != 8)
								{
									oAddX = 0;
								}
								if(oAddY != 8)
								{
									oAddY = 0;
								}

								dctTableValues[(startY+addY)*jpegData.widthRoundedTo8 + (startX+addX)].cr = acValue;
								dctTableValues[(startY+addY)*jpegData.widthRoundedTo8 + (startX+addX+oAddX)].cr = acValue;
								dctTableValues[(startY+addY+oAddY)*jpegData.widthRoundedTo8 + (startX+addX+oAddX)].cr = acValue;
								dctTableValues[(startY+addY+oAddY)*jpegData.widthRoundedTo8 + (startX+addX)].cr = acValue;
							}

							k++;
						}
					}
				}
				
				currX += 8;

				if(currX >= addWidth)
				{
					currX = 0;
					currY += 8;
				}
				
			}
			
			startX += addWidth;
			if(startX >= img->getWidth())
			{
				startX = 0;
				startY += addHeight;
			}

			if(startY >= img->getHeight())
			{
				break;
			}
			
		}
	}

	void progressiveProcessRefine(HiResImage* img, DCTChannel* dctTableValues, JPEGData& jpegData, int selectionStart, int selectionEnd, int predictionForSuccessive)
	{
		int startX = 0;
		int startY = 0;
		BinarySet binSet = BinarySet();
		int binNum = 0;
		binSet.setBitOrder(BinarySet::RMSB);
		binSet.setValues(jpegData.compressedData.data(), jpegData.compressedData.size());

		bool dcMode = false;
		if(selectionStart==0)
		{
			//use dc tree
			dcMode = true;
		}
		else
		{
			//use ac tree
			dcMode = false;
		}

		int Ss = selectionStart; //Spectral Select Start
		int Se = selectionEnd; //Spectral Select End
		
		int Ah = predictionForSuccessive >> 4; //Successive Approximation High (shift used in the last scan)
		int Al = predictionForSuccessive & 0xF; //Successive Approximation Low (shift to use for this scan)

		// int fillSize = selectionEnd - selectionStart + 1; //Not used
		// int k = selectionStart; //Not used
		binNum = 0;

		int p1 = 1 << Al;
		int m1 = (-1) << Al;

		int addWidth = 8;
		int addHeight = 8;

		int currX = 0;
		int currY = 0;

		std::vector<unsigned short> nComponentData = std::vector<unsigned short>();

		if(jpegData.componentData.size() == 3)
		{
			//y, cb, cr
			addWidth = 8 * ((jpegData.samplingRate[0]>>4) & 0xF);
			addHeight = 8 * ((jpegData.samplingRate[0]) & 0xF);

			nComponentData.push_back(jpegData.componentData[0]);

			if(addWidth>8)
				nComponentData.push_back(jpegData.componentData[0]);
			if(addHeight>8)
				nComponentData.push_back(jpegData.componentData[0]);
			if(addWidth>8 && addHeight>8)
				nComponentData.push_back(jpegData.componentData[0]);
			
			nComponentData.push_back(jpegData.componentData[1]);
			nComponentData.push_back(jpegData.componentData[2]);
		}
		else
		{
			//just y or cb or cr
			addWidth = 8 * ((jpegData.samplingRate[0]>>4) & 0xF);
			addHeight = 8 * ((jpegData.samplingRate[0]) & 0xF);

			if(jpegData.componentData[0]>>8 == 1)
			{
				addWidth = 8;
				addHeight = 8;
			}
			nComponentData.push_back(jpegData.componentData[0]);
		}

		int oAddX = addWidth/2;
		int oAddY = addHeight/2;
		if(oAddX != 8)
		{
			oAddX = 0;
		}
		if(oAddY != 8)
		{
			oAddY = 0;
		}

		while(true)
		{
			currX = 0;
			currY = 0;
			for(size_t i=0; i<nComponentData.size(); i++)
			{
				unsigned short comD = nComponentData[i];
				int comNum = comD >> 8;
				// int treeIndex1 = (comD >> 4) & 0x0F; //Not used
				int treeIndex2 = comD & 0x0F;

				// BinaryTree<HuffmanNode>* dcTree = huffmanTreesDC[treeIndex1]; //Not used. Odd since it seems like it should be used. Investigate later
				BinaryTree<HuffmanNode>* acTree = jpegData.huffmanTreesAC[treeIndex2];

				if(comNum!=1)
				{
					currX = 0;
					currY = 0;
				}

				int k = Ss;
				while(k <= Se)
				{
					if(dcMode)
					{
						int val = binSet.getBit(binNum) << Al;
						binNum++;

						if(comNum==1)
						{
							int dctTableIndex = (startY+currY)*jpegData.widthRoundedTo8 + (startX+currX);
							int oldV = dctTableValues[dctTableIndex].y | val;
							dctTableValues[dctTableIndex].y = oldV;
						}
						else if(comNum==2)
						{
							int oldV = dctTableValues[(startY)*jpegData.widthRoundedTo8 + (startX)].cb | val;
							dctTableValues[(startY)*jpegData.widthRoundedTo8 + (startX)].cb = oldV;
							dctTableValues[(startY)*jpegData.widthRoundedTo8 + (startX+oAddX)].cb = oldV;
							dctTableValues[(startY+oAddY)*jpegData.widthRoundedTo8 + (startX+oAddX)].cb = oldV;
							dctTableValues[(startY+oAddY)*jpegData.widthRoundedTo8 + (startX)].cb = oldV;
						}
						else
						{
							int oldV = dctTableValues[(startY)*jpegData.widthRoundedTo8 + (startX)].cr | val;
							dctTableValues[(startY)*jpegData.widthRoundedTo8 + (startX)].cr = oldV;
							dctTableValues[(startY)*jpegData.widthRoundedTo8 + (startX+oAddX)].cr = oldV;
							dctTableValues[(startY+oAddY)*jpegData.widthRoundedTo8 + (startX+oAddX)].cr = oldV;
							dctTableValues[(startY+oAddY)*jpegData.widthRoundedTo8 + (startX)].cr = oldV;
						}

						k++;
					}
					else
					{
						if(jpegData.eobRun == 0)
						{
							while(k <= Se)
							{
								BinaryTreeNode<HuffmanNode>* node = traverseTree(&binSet, acTree, &binNum);
								if(node==nullptr)
								{
									StringTools::println("Error traversing tree AC REFINE");
									return;
								}

								int acV = node->data.value;

								int r = (acV>>4) & 0x0F;
								int s = acV & 0x0F;
								int acValue = 0;

								if(s==0)
								{
									//eob or zrl
									if(r!=15)
									{
										jpegData.eobRun = 1 << r;
										if(r>0)
										{
											jpegData.eobRun += binSet.getBits(binNum, binNum+r, true);
											binNum+=r;
										}
										break;
									}
									else
									{
										//StringTools::println("ZRL REFINE");
									}
								}
								else
								{
									acValue = (binSet.getBit(binNum)) ? p1 : m1;
									binNum++;
								}

								while(k<=selectionEnd)
								{
									int coef = 0;
									int addY = k/8;
									int addX = k%8;

									if(comNum==1)
										coef = dctTableValues[(startY+addY+currY)*jpegData.widthRoundedTo8 + (startX+addX+currX)].y;
									else if(comNum==2)
										coef = dctTableValues[(startY+addY)*jpegData.widthRoundedTo8 + (startX+addX)].cb;
									else
										coef = dctTableValues[(startY+addY)*jpegData.widthRoundedTo8 + (startX+addX)].cr;

									bool nonZeroHistory = (coef!=0);
									
									if(nonZeroHistory)
									{
										if(binSet.getBit(binNum))
										{
											if(coef>0)
												coef += p1;
											else
												coef += m1;

											if(comNum==1)
											{
												dctTableValues[(startY+addY+currY)*jpegData.widthRoundedTo8 + (startX+addX+currX)].y = coef;
											}
											else if(comNum==2)
											{
												dctTableValues[(startY+addY)*jpegData.widthRoundedTo8 + (startX+addX)].cb = coef;
												dctTableValues[(startY+addY)*jpegData.widthRoundedTo8 + (startX+addX+oAddX)].cb = coef;
												dctTableValues[(startY+addY+oAddY)*jpegData.widthRoundedTo8 + (startX+addX+oAddX)].cb = coef;
												dctTableValues[(startY+addY+oAddY)*jpegData.widthRoundedTo8 + (startX+addX)].cb = coef;
											}
											else
											{
												dctTableValues[(startY+addY)*jpegData.widthRoundedTo8 + (startX+addX)].cr = coef;
												dctTableValues[(startY+addY)*jpegData.widthRoundedTo8 + (startX+addX+oAddX)].cr = coef;
												dctTableValues[(startY+addY+oAddY)*jpegData.widthRoundedTo8 + (startX+addX+oAddX)].cr = coef;
												dctTableValues[(startY+addY+oAddY)*jpegData.widthRoundedTo8 + (startX+addX)].cr = coef;
											}
										}
										binNum++;
									}
									else
									{
										r--;
										if(r < 0)
										{
											break;
										}
									}

									k++;
								}
								
								if((acValue!=0) && (k<64))
								{
									int addY = k/8;
									int addX = k%8;

									if(comNum==1)
									{
										dctTableValues[(startY+addY+currY)*jpegData.widthRoundedTo8 + (startX+addX+currX)].y = acValue;
									}
									else if(comNum==2)
									{
										dctTableValues[(startY+addY)*jpegData.widthRoundedTo8 + (startX+addX)].cb = acValue;
										dctTableValues[(startY+addY)*jpegData.widthRoundedTo8 + (startX+addX+oAddX)].cb = acValue;
										dctTableValues[(startY+addY+oAddY)*jpegData.widthRoundedTo8 + (startX+addX+oAddX)].cb = acValue;
										dctTableValues[(startY+addY+oAddY)*jpegData.widthRoundedTo8 + (startX+addX)].cb = acValue;
									}
									else
									{
										dctTableValues[(startY+addY)*jpegData.widthRoundedTo8 + (startX+addX)].cr = acValue;
										dctTableValues[(startY+addY)*jpegData.widthRoundedTo8 + (startX+addX+oAddX)].cr = acValue;
										dctTableValues[(startY+addY+oAddY)*jpegData.widthRoundedTo8 + (startX+addX+oAddX)].cr = acValue;
										dctTableValues[(startY+addY+oAddY)*jpegData.widthRoundedTo8 + (startX+addX)].cr = acValue;
									}
								}

								k++;
							}
						}
						
						if(jpegData.eobRun>0)
						{
							while(k<=selectionEnd)
							{
								int coef = 0;
								int addY = k/8;
								int addX = k%8;

								if(comNum==1)
									coef = dctTableValues[(startY+addY+currY)*jpegData.widthRoundedTo8 + (startX+addX+currX)].y;
								else if(comNum==2)
									coef = dctTableValues[(startY+addY)*jpegData.widthRoundedTo8 + (startX+addX)].cb;
								else
									coef = dctTableValues[(startY+addY)*jpegData.widthRoundedTo8 + (startX+addX)].cr;
								
								bool nonZeroHistory = (coef!=0);
								if(nonZeroHistory)
								{
									if(binSet.getBit(binNum))
									{
										if(coef>0)
											coef += p1;
										else
											coef += m1;

										if(comNum==1)
										{
											dctTableValues[(startY+addY+currY)*jpegData.widthRoundedTo8 + (startX+addX+currX)].y = coef;
										}
										else if(comNum==2)
										{
											dctTableValues[(startY+addY)*jpegData.widthRoundedTo8 + (startX+addX)].cb = coef;
											dctTableValues[(startY+addY)*jpegData.widthRoundedTo8 + (startX+addX+oAddX)].cb = coef;
											dctTableValues[(startY+addY+oAddY)*jpegData.widthRoundedTo8 + (startX+addX+oAddX)].cb = coef;
											dctTableValues[(startY+addY+oAddY)*jpegData.widthRoundedTo8 + (startX+addX)].cb = coef;
										}
										else
										{
											dctTableValues[(startY+addY)*jpegData.widthRoundedTo8 + (startX+addX)].cr = coef;
											dctTableValues[(startY+addY)*jpegData.widthRoundedTo8 + (startX+addX+oAddX)].cr = coef;
											dctTableValues[(startY+addY+oAddY)*jpegData.widthRoundedTo8 + (startX+addX+oAddX)].cr = coef;
											dctTableValues[(startY+addY+oAddY)*jpegData.widthRoundedTo8 + (startX+addX)].cr = coef;
										}
									}
									binNum++;
								}
								k++;
							}
							jpegData.eobRun--;
						}
					}
				}

				currX += 8;

				if(currX >= addWidth)
				{
					currX = 0;
					currY += 8;
				}
				
			}
			
			startX += addWidth;
			if(startX >= img->getWidth())
			{
				startX = 0;
				startY += addHeight;
			}

			if(startY >= img->getHeight())
			{
				break;
			}
		}
	}

	void baselineProcess(HiResImage* img, JPEGData& jpegData)
	{
		// time_t totalTime = 0; //Not used
		if(img!=nullptr)
		{
			//process image data
			BinarySet binSet = BinarySet();
			binSet.setBitOrder(BinarySet::RMSB);
			binSet.setValues(jpegData.compressedData.data(), jpegData.compressedData.size());

			//grab a single grid and process that first
			// int matFilled = 0; //Not used
			quickVec3f blockAdd = quickVec3f();
			MatrixF q = MatrixF(8,8);
			MatrixF m = MatrixF(8,8);

			int actualX = 0;
			int actualY = 0;
			// int addVal = 0; //Not used
			int bitNum = 0;
			bool valid = true;

			int* order;
			int addWidth = 8;
			int addHeight = 8;

			int currX = 0;
			int currY = 0;

			addWidth = 8 * ((jpegData.samplingRate[0]>>4) & 0xF);
			addHeight = 8 * ((jpegData.samplingRate[0]) & 0xF);

			int yVals = (jpegData.samplingRate[0]>>4) * (jpegData.samplingRate[0] & 0x0F);
			
			order = new int[yVals+2];
			for(int i=0; i<yVals; i++)
			{
				order[i] = 0;
			}
			order[yVals] = 1;
			order[yVals+1] = 2;

			while(true)
			{
				//first grab the dc element
				currX = 0;
				currY = 0;

				for(int i=0; i<yVals+2; i++)
				{	
					int treeIndex1 = (jpegData.componentData.at(order[i]) >> 4) & 0x0F;
					int treeIndex2 = jpegData.componentData.at(order[i]) & 0x0F;
					int quanTable = 0;

					if(order[i]==0)
						quanTable = 0;
					else
						quanTable = 1;

					if(order[i]!=0)
					{
						currX = 0;
						currY = 0;
					}
					
					BinaryTree<HuffmanNode>* dcTree = jpegData.huffmanTreesDC[treeIndex1];
					BinaryTree<HuffmanNode>* acTree = jpegData.huffmanTreesAC[treeIndex2];

					if(dcTree==nullptr || acTree==nullptr)
					{
						StringTools::println("INVALID");
						valid = false;
						return;
					}
					
					BinaryTreeNode<HuffmanNode>* currNode = dcTree->getRoot();

					std::vector<int> decompData = std::vector<int>();
					bool setDC = true;
					while(decompData.size() < 64)
					{
						if(setDC)
							currNode = traverseTree(&binSet, dcTree, &bitNum);
						else
							currNode = traverseTree(&binSet, acTree, &bitNum);

						if(currNode==nullptr)
						{
							if(setDC)
								StringTools::println("ERROR TRAVERSING TREE BASELINE DC");
							else
								StringTools::println("ERROR TRAVERSING TREE BASELINE AC");
							
							return;
						}

						if(setDC)
						{
							int category = currNode->data.value;
							int dcValue = 0;
							if(category>0)
							{
								if(binSet.getBit(bitNum)==0)
								{
									//negative
									dcValue = -( (1<<(category)) - 1) + binSet.getBits(bitNum+1, bitNum+category, true);
								}
								else
								{
									//positive
									dcValue = binSet.getBits(bitNum, bitNum+category, true);
								}
							}

							if(order[i]==0)
							{
								dcValue += (int)blockAdd.x;
								blockAdd.x = dcValue;
							}
							else if(order[i]==1)
							{
								dcValue += (int)blockAdd.y;
								blockAdd.y = dcValue;
							}
							else
							{
								dcValue += (int)blockAdd.z;
								blockAdd.z = dcValue;
							}

							decompData.push_back(dcValue);
								
							setDC = false;
							bitNum += category;
						}
						else
						{
							//category here represents the (RRRR,SSSS) data where RRRR is the leading zeroes and SSSS is the category
							int acV = currNode->data.value;
							int leadingZeros = (acV >> 4) & 0x0F;
							int category = acV & 0x0F;

							if(leadingZeros == 0 && category==0)
							{
								//end of matrix. push back remaining zeros
								int fillSize = 64 - decompData.size();
								for(int j=0; j<fillSize; j++)
								{
									decompData.push_back(0);
								}
								break;
							}

							for(int j=0; j<leadingZeros; j++)
							{
								decompData.push_back(0);
							}

							int acValue = 0;
							if(category>0)
							{
								if(binSet.getBit(bitNum)==0)
								{
									//negative
									acValue = -( (1<<(category)) - 1) + binSet.getBits(bitNum+1, bitNum+category, true);
								}
								else
								{
									//positive
									acValue = binSet.getBits(bitNum, bitNum+category, true);
								}
							}

							decompData.push_back(acValue);

							bitNum += category;
						}
					}

					int r=0;
					int c=0;
					// bool zig=false; //Not used
					// bool backwards = false; //Not used
					for(int y=0; y<64; y++)
					{
						int zigPos = getZigZagPos(y);
						r = zigPos>>4;
						c = zigPos & 0x0F;
						m[r][c] = decompData[y] * jpegData.quantizationTables[quanTable][r][c];
					}

					MathExt::FCT8x8(m, &q, true);

					for(int y=0; y<8; y++)
					{
						for(int x=0; x<8; x++)
						{
							double value = (q[y][x] + jpegData.precisionSub) / jpegData.precisionDiv;
							if(value < 0)
								value = 0;
							int queryX = actualX+x+currX;
							int queryY = actualY+y+currY;
							
							
							if(order[i]==0)
							{
								if(queryX < img->getWidth() && queryY < img->getHeight())
								{
									img->getPixels()[queryY*img->getWidth() + queryX].red = value;
								}
								
							}
							else
							{
								int xMult = addWidth/8;
								int yMult = addHeight/8;

								int x1 = x;
								int x2 = x;
								int y1 = y;
								int y2 = y;

								if(xMult==2)
								{
									x1 = 2*x;
									x2 = x1+1;
								}

								if(yMult==2)
								{
									y1 = 2*y;
									y2 = y1+1;
								}

								Color4f c0, c1, c2, c3;
								
								if(actualX+x1 < img->getWidth())
								{
									if(actualY+y1 < img->getHeight())
										c0 = img->getPixels()[(actualY+y1)*img->getWidth() + actualX+x1];
									
									if(actualY+y2 < img->getHeight())
										c3 = img->getPixels()[(actualY+y2)*img->getWidth() + actualX+x1];
								}
								if(actualX+x2 < img->getWidth())
								{
									if(actualY+y1 < img->getHeight())
										c1 = img->getPixels()[(actualY+y1)*img->getWidth() + actualX+x2];
									
									if(actualY+y2 < img->getHeight())
										c2 = img->getPixels()[(actualY+y2)*img->getWidth() + actualX+x2];
								}

								if(order[i]==1)
								{
									c0.green = value;
									c1.green = c0.green;
									c2.green = c0.green;
									c3.green = c0.green;
								}
								else
								{
									c0.blue = value;
									c1.blue = c0.blue;
									c2.blue = c0.blue;
									c3.blue = c0.blue;
								}

								if(actualX+x1 < img->getWidth())
								{
									if(actualY+y1 < img->getHeight())
										img->getPixels()[(actualY+y1)*img->getWidth() + actualX+x1] = c0;
									
									if(actualY+y2 < img->getHeight())
										img->getPixels()[(actualY+y2)*img->getWidth() + actualX+x1] = c3;
								}
								if(actualX+x2 < img->getWidth())
								{
									if(actualY+y1 < img->getHeight())
										img->getPixels()[(actualY+y1)*img->getWidth() + actualX+x2] = c1;
									
									if(actualY+y2 < img->getHeight())
										img->getPixels()[(actualY+y2)*img->getWidth() + actualX+x2] = c2;
								}
							}
						}
					}

					currX += 8;

					if(currX >= addWidth)
					{
						currX = 0;
						currY += 8;
					}
					
				}
				

				actualX+=addWidth;
				if(actualX>=img->getWidth())
				{
					actualX=0;
					actualY+=addHeight;
				}

				if(actualY>=img->getHeight())
				{
					break;
				}
			}

			delete[] order;
		}
	}

	void HiResImage::saveJPG(File file, int quality, int subsampleMode)
	{
		//save using jpeg quantization matrix
		// MatrixF lum = getLuminanceQuantizationMatrix(quality);
		// MatrixF chrom = getChrominanceQuantizationMatrix(quality);
	}
	
	HiResImage** HiResImage::loadJPG(std::vector<unsigned char> fileData, int* amountOfImages)
	{
		//magic number = 0xFF 0xD8

		HiResImage** imgArr = nullptr;
		HiResImage* img = nullptr;
		DCTChannel* dctValueTable = nullptr;
		JPEGData jpegData;


		size_t index = 0;
		jpegData.quantizationTables = std::vector<MatrixF>(4);
		jpegData.huffmanTreesDC = std::vector<BinaryTree<HuffmanNode>*>(4);
		jpegData.huffmanTreesAC = std::vector<BinaryTree<HuffmanNode>*>(4);

		jpegData.compressedData = std::vector<unsigned char>();
		jpegData.componentData = std::vector<unsigned short>();

		jpegData.samplingRate = std::vector<unsigned short>();

		int frameType = 0;
		int selectStart = 0;
		int selectEnd = 63;
		int predictionForSuccessive = 0;

		bool processImage = false;

		// int width8 = 0; //Not used
		int height8 = 0;

		// int chromaX = 1; //Not used
		// int chromaY = 1; //Not used

		while(index < fileData.size())
		{
			unsigned short header = (((unsigned short)fileData[index])<<8) + fileData[index+1];
			index += 2;
			
			if(header == 0xFFD8)
			{
				//start of image
			}
			else if(header == 0xFFE0)
			{
				//default header
				unsigned short size = (((unsigned short)fileData[index])<<8) + fileData[index+1];
				index+=size;

				//skipping this header because their is very little useful in it
				//list of the data in the header: length, identifier, version, units, xDensity, yDensity, xThumbnail, yThumbnail, thumbnailData
			}
			else if(header == 0xFFC0)
			{
				// StringTools::println("BASELINE");
				frameType = 0;
				unsigned short size = (((unsigned short)fileData[index])<<8) + fileData[index+1];
				index+=2;
				
				unsigned char precision = fileData[index];
				index++;

				//required to deal with higher bit precision.
				jpegData.precisionDiv = (1<<precision)-1; //The division value needed to get a value in [0-1]. (usually 255)
				jpegData.precisionSub = (1<<(precision-1)); //The subtraction value at the mid point. (usually 128)

				unsigned short height = (((unsigned short)fileData[index])<<8) + fileData[index+1];
				index+=2;

				unsigned short width = (((unsigned short)fileData[index])<<8) + fileData[index+1];
				index+=2;

				unsigned char components = fileData[index];
				index++;
				
				for(int i=0; i<components; i++)
				{
					// unsigned char id = fileData[index];
					unsigned char sampling = fileData[index+1];
					// unsigned char tableNum = fileData[index+2];

					//adjust sampling cause it is setup dumb
					int h1 = (sampling >> 4) & 0x0F;
					int v1 = sampling & 0x0F;
					if(h1==3)
					{
						h1 = 2;
					}
					if(v1==3)
					{
						v1 = 2;
					}
					jpegData.samplingRate.push_back((h1<<4) + v1);
					
					index+=3;
				}
				
				if(img==nullptr)
					img = new HiResImage(width, height);
			}
			else if(header == 0xFFC1)
			{
				// StringTools::println("EXT SEQ");
				frameType = 1;
				unsigned short size = (((unsigned short)fileData[index])<<8) + fileData[index+1];
				index+=2;
				
				unsigned char precision = fileData[index];
				index++;

				//required to deal with higher bit precision.
				jpegData.precisionDiv = (1<<precision)-1; //The division value needed to get a value in [0-1]. (usually 255)
				jpegData.precisionSub = (1<<(precision-1)); //The subtraction value at the mid point. (usually 128)

				unsigned short height = (((unsigned short)fileData[index])<<8) + fileData[index+1];
				index+=2;

				unsigned short width = (((unsigned short)fileData[index])<<8) + fileData[index+1];
				index+=2;

				unsigned char components = fileData[index];
				index++;
				
				for(int i=0; i<components; i++)
				{
					// unsigned char id = fileData[index];
					unsigned char sampling = fileData[index+1];
					// unsigned char tableNum = fileData[index+2];

					//adjust sampling cause it is setup dumb
					int h1 = (sampling >> 4) & 0x0F;
					int v1 = sampling & 0x0F;
					if(h1==3)
					{
						h1 = 2;
					}
					if(v1==3)
					{
						v1 = 2;
					}
					jpegData.samplingRate.push_back((h1<<4) + v1);
					
					index+=3;
				}

				if(img==nullptr)
					img = new HiResImage(width, height);
			}
			else if(header == 0xFFC2)
			{
				// StringTools::println("PROGRESSIVE");
				frameType = 2;
				unsigned short size = (((unsigned short)fileData[index])<<8) + fileData[index+1];
				index+=2;
				
				unsigned char precision = fileData[index];
				index++;

				//required to deal with higher bit precision.
				jpegData.precisionDiv = (1<<precision)-1; //The division value needed to get a value in [0-1]. (usually 255)
				jpegData.precisionSub = (1<<(precision-1)); //The subtraction value at the mid point. (usually 128)

				unsigned short height = (((unsigned short)fileData[index])<<8) + fileData[index+1];
				index+=2;

				unsigned short width = (((unsigned short)fileData[index])<<8) + fileData[index+1];
				index+=2;

				unsigned char components = fileData[index];
				index++;
				
				for(int i=0; i<components; i++)
				{
					// unsigned char id = fileData[index];
					unsigned char sampling = fileData[index+1];
					// unsigned char tableNum = fileData[index+2];

					//adjust sampling cause it is setup dumb
					int h1 = (sampling >> 4) & 0x0F;
					int v1 = sampling & 0x0F;
					if(h1==3)
					{
						h1 = 2;
					}
					if(v1==3)
					{
						v1 = 2;
					}
					jpegData.samplingRate.push_back((h1<<4) + v1);
					
					index+=3;
				}

				if(img==nullptr)
				{
					img = new HiResImage(width, height);

					int addW = (width%8 > 0)? (8-width%8) : 0;
					int addH = (height%8 > 0)? (8-height%8) : 0;

					int width8 = width + addW + 8;
					int height8 = height + addH + 8;
					
					jpegData.widthRoundedTo8 = width8;
					jpegData.heightRoundedTo8 = height8;
					
					dctValueTable = new DCTChannel[width8*height8];
				}
			}
			else if(header == 0xFFC4)
			{
				//define huffman table
				unsigned short size = (((unsigned short)fileData[index])<<8) + fileData[index+1];
				
				size_t finalLoc = index+size;
				index+=2;

				while(index < finalLoc)
				{
					int treeNum = fileData[index] & 0x0F;
					int type = (fileData[index] >> 4) & 0x01;
					index++;

					std::vector<int> codeLengths = std::vector<int>(17);
					std::vector<int> dataVals = std::vector<int>();

					int count = 0;
					for(int i=0; i<16; i++)
					{
						codeLengths[i+1] = (int)fileData[index];
						count += fileData[index];
						index++;
					}
					
					for(int i=0; i<count; i++)
					{
						dataVals.push_back( (int)fileData[index] );
						index++;
					}

					BinaryTree<HuffmanNode>* tree = Compression::buildCanonicalHuffmanTree(dataVals.data(), dataVals.size(), codeLengths.data(), codeLengths.size(), true, true);

					if(type==0)
					{
						if(jpegData.huffmanTreesDC[treeNum] != nullptr)
						{
							delete jpegData.huffmanTreesDC[treeNum];
						}
						jpegData.huffmanTreesDC[treeNum] = tree;
					}
					else
					{
						if(jpegData.huffmanTreesAC[treeNum] != nullptr)
						{
							delete jpegData.huffmanTreesAC[treeNum];
						}
						jpegData.huffmanTreesAC[treeNum] = tree;
					}
				}

				index = finalLoc;
			}
			else if(header == 0xFFDB)
			{
				//define quantization table
				unsigned short size = (((unsigned short)fileData[index])<<8) + fileData[index+1];
				size_t finalLoc = index+size;

				index+=2;

				while(index < finalLoc)
				{
					int tableNum = fileData[index] & 0x0F;
					int precision = (fileData[index] >> 4) & 0x0F;
					index++;

					if(tableNum>3)
					{
						//invalid
						StringTools::println("INVALID QUANTIZATION TABLE");
						if(amountOfImages!=nullptr)
							*amountOfImages = -1;
						return nullptr;
					}

					MatrixF m = MatrixF(8,8);

					int r = 0;
					int c = 0;
					for(int y=0; y<64; y++)
					{
						int zigPos = getZigZagPos(y);
						r = zigPos>>4;
						c = zigPos & 0x0F;

						if(precision==0)
						{
							m[r][c] = fileData[index];
							index++;
						}
						else
						{
							m[r][c] = ((unsigned short)fileData[index]<<8) + fileData[index+1];
							index+=2;
						}
					}
					
					jpegData.quantizationTables[tableNum] = m;
				}

				index = finalLoc;
			}
			else if(header == 0xFFDA)
			{
				//Start of Scan
				// StringTools::println("START OF SCAN");
				unsigned short size = (((unsigned short)fileData[index])<<8) + fileData[index+1];
				index+=2;

				int count = fileData[index];
				index++;

				jpegData.componentData.clear();

				for(int i=0; i<count; i++)
				{
					unsigned short comData = (((unsigned short)fileData[index])<<8) + fileData[index+1];
					index+=2;
					jpegData.componentData.push_back(comData);
				}

				selectStart = fileData[index];
				selectEnd = fileData[index+1];
				predictionForSuccessive = fileData[index+2];
				index+=3;
				
				//read until 0xFFxx is found where xx is not 00
				// int startInd = index; //Not used
				int nBytes = 0;
				jpegData.compressedData.clear();
				processImage=true;

				while(true)
				{
					if(fileData[index]!=0xFF)
					{
						jpegData.compressedData.push_back( fileData[index] );
						index++;
						nBytes++;
					}
					else
					{
						if(fileData[index+1]==0x00)
						{
							jpegData.compressedData.push_back( 0xFF );
							index+=2;
							nBytes+=2;
						}
						else if(fileData[index+1] >= 0xD0 && fileData[index+1] <= 0xD7)
						{
							jpegData.compressedData.push_back( 0xFF );
							jpegData.compressedData.push_back( fileData[index+1] );
							index+=2;
							nBytes+=2;
						}
						else 
						{
							break;
						}
					}
				}
			}
			else if(header == 0xFFDD)
			{
				//DRI - Define Restart Interval
				//could be useful later
				index += 2;
				jpegData.dri = (((unsigned short)fileData[index])<<8) + fileData[index+1];
				index += 2;
				StringTools::println("DRI NOT SUPPORTED YET");

				if(imgArr != nullptr)
					delete[] imgArr;
				
				if(img != nullptr)
					delete img;
				
				for(size_t i=0; i<jpegData.huffmanTreesDC.size(); i++)
				{
					if(jpegData.huffmanTreesDC[i]!=nullptr)
						delete jpegData.huffmanTreesDC[i];
				}
				for(size_t i=0; i<jpegData.huffmanTreesAC.size(); i++)
				{
					if(jpegData.huffmanTreesAC[i]!=nullptr)
						delete jpegData.huffmanTreesAC[i];
				}
				if(amountOfImages!=nullptr)
					*amountOfImages = -1;
				return nullptr;

			}
			else if(header >= 0xFFD0 && header <= 0xFFD7)
			{
				//RSTn - Restart
				//DRI must be defined
				// finish remaining eobrun
				// set eobrun to 0
				// jpegData.eobRun = 0;
				// Not sure how to deal with this yet so assume that it can not be done.
				StringTools::println("RST");
			}
			else if(header == 0xFF01)
			{
				//IDK but has no size data specified
			}
			else if(header == 0xFFD9)
			{
				//end of image
				break;
			}
			else
			{
				//unknown header or header that will not be processed
				switch (header)
				{
				case 0xFFC3:
					StringTools::println("Lossless sequential JPEGs are not supported");
					if(amountOfImages!=nullptr)
						*amountOfImages = -1;
					return nullptr;
					break;
				case 0xFFC5:
					StringTools::println("Differential sequential JPEGs are not supported");
					if(amountOfImages!=nullptr)
						*amountOfImages = -1;
					return nullptr;
					break;
				case 0xFFC6:
					StringTools::println("Differential progressive JPEGs are not supported");
					if(amountOfImages!=nullptr)
						*amountOfImages = -1;
					return nullptr;
					break;
				case 0xFFC7:
					StringTools::println("Differential Lossless sequential JPEGs are not supported");
					if(amountOfImages!=nullptr)
						*amountOfImages = -1;
					return nullptr;
					break;
				case 0xFFC9:
					StringTools::println("Extended sequential Arithmetic coded JPEGs are not supported");
					if(amountOfImages!=nullptr)
						*amountOfImages = -1;
					return nullptr;
					break;
				case 0xFFCA:
					StringTools::println("Progressive Arithmetic coded JPEGs are not supported");
					if(amountOfImages!=nullptr)
						*amountOfImages = -1;
					return nullptr;
					break;
				case 0xFFCB:
					StringTools::println("Lossless Arithmetic coded JPEGs are not supported");
					if(amountOfImages!=nullptr)
						*amountOfImages = -1;
					return nullptr;
					break;
				case 0xFFCC:
					StringTools::println("Arithmetic coded JPEGs are not supported");
					if(amountOfImages!=nullptr)
						*amountOfImages = -1;
					return nullptr;
					break;
				default:
					break;
				}
				unsigned short size = (((unsigned short)fileData[index])<<8) + fileData[index+1];
				// StringTools::println("Unknown header %x, size %x", header, size);
				index+=size;
			}

			if(processImage)
			{
				processImage = false;

				if(frameType==0 || frameType==1)
				{
					baselineProcess(img, jpegData);
				}
				else if(frameType==2)
				{
					if(predictionForSuccessive>>4 == 0)
					{
						progressiveProcessFirst(img, dctValueTable, jpegData, selectStart, selectEnd, predictionForSuccessive);
					}
					else
					{
						progressiveProcessRefine(img, dctValueTable, jpegData, selectStart, selectEnd, predictionForSuccessive);
					}
				}
			}

		}

		if(frameType==2)
		{
			//convert imgMat using dct to get final image
			progressiveProcessEnd(img, dctValueTable, jpegData);
			if(dctValueTable!=nullptr)
			{
				delete[] dctValueTable;
			}
		}

		//convert from ycbcr to rgb and store into image

		//chroma averaging if necessary
		if(((jpegData.samplingRate[0]>>4)&0x0F) != 1)
		{
			//subsample in the x
			for(int y=0; y<img->getHeight(); y++)
			{
				for(int x=1; x<img->getWidth()-1; x++)
				{
					Color4f c = img->pixels[y*img->getWidth() + x-1];
					Color4f c2 = img->pixels[y*img->getWidth() + x+1];
					Color4f f = img->pixels[y*img->getWidth() + x];

					f.green = ((double)c.green + (double)c2.green)/2.0;
					f.blue = ((double)c.blue + (double)c2.blue)/2.0;

					img->pixels[y*img->getWidth() + x] = f;
				}
			}
		}
		if((jpegData.samplingRate[0]&0x0F) != 1)
		{
			//subsample in the y
			for(int y=1; y<img->getHeight()-1; y++)
			{
				for(int x=0; x<img->getWidth(); x++)
				{
					Color4f c = img->pixels[(y-1)*img->getWidth() + x];
					Color4f c2 = img->pixels[(y+1)*img->getWidth() + x];
					Color4f f = img->pixels[y*img->getWidth() + x];

					f.green = ((double)c.green + (double)c2.green)/2.0;
					f.blue = ((double)c.blue + (double)c2.blue)/2.0;

					img->pixels[y*img->getWidth() + x] = f;
				}
			}
		}

		Color4f* startC = img->getPixels();
		Color4f* endC = startC + (img->getWidth()*img->getHeight());

		while(startC<endC)
		{
			//do it manually
			//Values already have 16 bit precision but as floats in range [0-1] so this still preserves the precision
			quickVec3f cVec = quickVec3f(255*(float)startC->red, 255*(float)startC->green, 255*(float)startC->blue);
			Color4f finalC;
			
			//Also convert to [0-1]
			//Must clamp to prevent overflow or underflow. Internally, these values are just uint16_t which don't do well with negative values.
			finalC.red = MathExt::clamp((cVec.x + 1.402 * (cVec.z - 128)) / 255.0, 0.0, 1.0);
			finalC.green = MathExt::clamp((cVec.x - 0.344136*(cVec.y - 128) - 0.714136*(cVec.z - 128)) / 255.0, 0.0, 1.0);
			finalC.blue = MathExt::clamp((cVec.x + 1.772*(cVec.y - 128)) / 255.0, 0.0, 1.0);
			finalC.alpha = 1.0;

			*startC = finalC;
			startC++;
		}
		

		imgArr = new HiResImage*[1]{img};
		if(amountOfImages!=nullptr)
		{
			*amountOfImages = 1;
		}

		for(size_t i=0; i<jpegData.huffmanTreesDC.size(); i++)
		{
			if(jpegData.huffmanTreesDC[i]!=nullptr)
				delete jpegData.huffmanTreesDC[i];
		}
		for(size_t i=0; i<jpegData.huffmanTreesAC.size(); i++)
		{
			if(jpegData.huffmanTreesAC[i]!=nullptr)
				delete jpegData.huffmanTreesAC[i];
		}

		return imgArr;
	}

} //NAMESPACE glib END