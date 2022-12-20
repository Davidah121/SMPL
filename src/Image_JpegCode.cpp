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

namespace glib
{

	struct huffThing
	{
		int value;
		BinarySet codeVal;
	};

	int eobRun = 0;
	int dri = 0;

	std::vector<Matrix> quantizationTables;
	std::vector<BinaryTree<HuffmanNode>*> huffmanTreesDC;
	std::vector<BinaryTree<HuffmanNode>*> huffmanTreesAC;

	std::vector<unsigned char> compressedData;
	std::vector<unsigned short> componentData;

	std::vector<unsigned short> samplingRate;

	Matrix getLuminanceQuantizationMatrix(int qualityLevel)
	{
		Matrix qualityMatrix = Matrix(8,8);
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

	Matrix getChrominanceQuantizationMatrix(int qualityLevel)
	{
		//LERP BETWEEN JPEG quality and Photoshop
		Matrix qualityMatrix = Matrix(8,8);

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

	void progressiveProcessEnd(Image* img, Vec3f** imgMat)
	{
		int xMult = ((samplingRate[0]>>4) & 0xF);
		int yMult = ((samplingRate[0]) & 0xF);
		
		int addWidth = 8 / xMult;
		int addHeight = 8 / yMult;
		
		int offX = 0;
		int offY = 0;

		Matrix q1 = Matrix(8,8);
		Matrix q2 = Matrix(8,8);
		Matrix q3 = Matrix(8,8);

		Matrix yMat = Matrix(8,8);
		Matrix cbMat = Matrix(8,8);
		Matrix crMat = Matrix(8,8);

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
					
					yMat[oY2][oX2] = imgMat[y+y2][x+x2].x;
					cbMat[oY2][oX2] = imgMat[y+y2][x+x2].y;
					crMat[oY2][oX2] = imgMat[y+y2][x+x2].z;
				}

				for(int y2=0; y2<8; y2++)
				{
					for(int x2=0; x2<8; x2++)
					{
						yMat[y2][x2] *= quantizationTables[0].get(x2, y2);
						cbMat[y2][x2] *= quantizationTables[1].get(x2, y2);
						crMat[y2][x2] *= quantizationTables[1].get(x2, y2);
					}
				}

				MathExt::FCT8x8(yMat, &q1, true);
				MathExt::FCT8x8(cbMat, &q2, true);
				MathExt::FCT8x8(crMat, &q3, true);

				for(int y2=0; y2<8; y2++)
				{
					int chromaY = y2/yMult;

					for(int x2=0; x2<8; x2++)
					{
						int chromaX = x2/xMult;
						Color c = {0, 0, 0, 255};
						
						c.red = (unsigned char)MathExt::clamp(MathExt::round(q1[y2][x2]+128), 0.0, 255.0);
						c.green = (unsigned char)MathExt::clamp(MathExt::round(q2[chromaY+offY][chromaX+offX]+128), 0.0, 255.0);
						c.blue = (unsigned char)MathExt::clamp(MathExt::round(q3[chromaY+offY][chromaX+offX]+128), 0.0, 255.0);
						
						img->setPixel(x+x2, y+y2, c);
					}
				}

				offX = (offX + addWidth) % 8;
			}
			offY = (offY + addHeight) % 8;
		}
	}

	void progressiveProcessFirst(Image* img, Vec3f** imgMat, int selectionStart, int selectionEnd, int predictionForSuccessive)
	{
		//some notes: for each repeated matrix value, add the components together
		//progressive for 8x8 test1 consist of 2 passes over the matrix where the 1st pass is half the quality of the original
		//for other images, this can span over multiple passes.

		int startX = 0;
		int startY = 0;
		BinarySet binSet = BinarySet();
		int binNum = 0;
		binSet.setBitOrder(BinarySet::RMSB);
		binSet.setValues(compressedData.data(), compressedData.size());

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

		// int preShift = predictionForSuccessive >> 4; //Not used
		int postShift = predictionForSuccessive & 0xF;

		// int fillSize = selectionEnd - selectionStart + 1; //Not used

		Vec3f blockAdd = Vec3f();

		int addWidth = 8;
		int addHeight = 8;

		int currX = 0;
		int currY = 0;

		std::vector<unsigned short> nComponentData = std::vector<unsigned short>();

		if(componentData.size() == 3)
		{
			//y, cb, cr
			addWidth = 8 * ((samplingRate[0]>>4) & 0xF);
			addHeight = 8 * ((samplingRate[0]) & 0xF);

			nComponentData.push_back(componentData[0]);

			if(addWidth>8)
				nComponentData.push_back(componentData[0]);
			if(addHeight>8)
				nComponentData.push_back(componentData[0]);
			if(addWidth>8 && addHeight>8)
				nComponentData.push_back(componentData[0]);
			
			nComponentData.push_back(componentData[1]);
			nComponentData.push_back(componentData[2]);
		}
		else
		{
			//just y or cb or cr
			addWidth = 8 * ((samplingRate[0]>>4) & 0xF);
			addHeight = 8 * ((samplingRate[0]) & 0xF);

			if(componentData[0]>>8 == 1)
			{
				addWidth = 8;
				addHeight = 8;
			}
			nComponentData.push_back(componentData[0]);
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

				BinaryTree<HuffmanNode>* dcTree = huffmanTreesDC[treeIndex1];
				BinaryTree<HuffmanNode>* acTree = huffmanTreesAC[treeIndex2];

				if(comNum!=1)
				{
					currX = 0;
					currY = 0;
				}

				int k = selectionStart;

				while(k <= selectionEnd)
				{
					if(dcMode)
					{
						BinaryTreeNode<HuffmanNode>* node = traverseTree(&binSet, dcTree, &binNum);
						if(node==nullptr)
						{
							StringTools::println("Error traversing tree DC FIRST");
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
							dcValue += (int)blockAdd.x;
							blockAdd.x = dcValue;
						}
						else if(comNum==2)
						{
							dcValue += (int)blockAdd.y;
							blockAdd.y = dcValue;
						}
						else
						{
							dcValue += (int)blockAdd.z;
							blockAdd.z = dcValue;
						}
						
						dcValue = dcValue << postShift;

						if(comNum==1)
						{
							imgMat[startY+currY][startX+currX].x = dcValue;
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

							imgMat[startY][startX].y = dcValue;
							imgMat[startY][startX+addX].y = dcValue;
							imgMat[startY+addY][startX+addX].y = dcValue;
							imgMat[startY+addY][startX].y = dcValue;
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

							imgMat[startY][startX].z = dcValue;
							imgMat[startY][startX+addX].z = dcValue;
							imgMat[startY+addY][startX+addX].z = dcValue;
							imgMat[startY+addY][startX].z = dcValue;
						}

						binNum += category;
						
						k++;
					}
					else
					{
						if(eobRun>0)
						{
							k = selectionEnd+1;
							eobRun--;
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
								eobRun = 1 << r;
								if(r>0)
								{
									eobRun += binSet.getBits(binNum, binNum+r, true);
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
							
							acValue = acValue << postShift;

							
							int addY = k/8;
							int addX = k%8;

							if(comNum==1)
							{
								imgMat[startY+currY+addY][startX+currX+addX].x = acValue;
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

								imgMat[startY+addY][startX+addX].y = acValue;
								imgMat[startY+addY][startX+addX+oAddX].y = acValue;
								imgMat[startY+addY+oAddY][startX+addX+oAddX].y = acValue;
								imgMat[startY+addY+oAddY][startX+addX].y = acValue;
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

								imgMat[startY+addY][startX+addX].z = acValue;
								imgMat[startY+addY][startX+addX+oAddX].z = acValue;
								imgMat[startY+addY+oAddY][startX+addX+oAddX].z = acValue;
								imgMat[startY+addY+oAddY][startX+addX].z = acValue;
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

	void progressiveProcessRefine(Image* img, Vec3f** imgMat, int selectionStart, int selectionEnd, int predictionForSuccessive)
	{
		int startX = 0;
		int startY = 0;
		BinarySet binSet = BinarySet();
		int binNum = 0;
		binSet.setBitOrder(BinarySet::RMSB);
		binSet.setValues(compressedData.data(), compressedData.size());

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

		// int preShift = predictionForSuccessive >> 4; //Not used
		int postShift = predictionForSuccessive & 0xF;

		// int fillSize = selectionEnd - selectionStart + 1; //Not used
		// int k = selectionStart; //Not used
		binNum = 0;

		int p1 = 1 << postShift;
		int m1 = (-1) << postShift;

		int addWidth = 8;
		int addHeight = 8;

		int currX = 0;
		int currY = 0;

		std::vector<unsigned short> nComponentData = std::vector<unsigned short>();

		if(componentData.size() == 3)
		{
			//y, cb, cr
			addWidth = 8 * ((samplingRate[0]>>4) & 0xF);
			addHeight = 8 * ((samplingRate[0]) & 0xF);

			nComponentData.push_back(componentData[0]);

			if(addWidth>8)
				nComponentData.push_back(componentData[0]);
			if(addHeight>8)
				nComponentData.push_back(componentData[0]);
			if(addWidth>8 && addHeight>8)
				nComponentData.push_back(componentData[0]);
			
			nComponentData.push_back(componentData[1]);
			nComponentData.push_back(componentData[2]);
		}
		else
		{
			//just y or cb or cr
			addWidth = 8 * ((samplingRate[0]>>4) & 0xF);
			addHeight = 8 * ((samplingRate[0]) & 0xF);

			if(componentData[0]>>8 == 1)
			{
				addWidth = 8;
				addHeight = 8;
			}
			nComponentData.push_back(componentData[0]);
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
				BinaryTree<HuffmanNode>* acTree = huffmanTreesAC[treeIndex2];

				if(comNum!=1)
				{
					currX = 0;
					currY = 0;
				}

				int k = selectionStart;
				while(k <= selectionEnd)
				{
					if(dcMode)
					{
						int val = binSet.getBit(binNum) << postShift;
						binNum++;

						if(comNum==1)
						{
							int oldV = (int)imgMat[startY+currY][startX+currX].x;
							oldV |= val;
							imgMat[startY+currY][startX+currX].x = oldV;
						}
						else if(comNum==2)
						{
							int oldV = (int)imgMat[startY][startX].y;
							oldV |= val;
							imgMat[startY][startX].y = oldV;
							imgMat[startY][startX+oAddX].y = oldV;
							imgMat[startY+oAddY][startX+oAddX].y = oldV;
							imgMat[startY+oAddY][startX].y = oldV;
						}
						else
						{
							int oldV = (int)imgMat[startY][startX].z;
							oldV |= val;
							imgMat[startY][startX].z = oldV;
							imgMat[startY][startX+oAddX].z = oldV;
							imgMat[startY+oAddY][startX+oAddX].z = oldV;
							imgMat[startY+oAddY][startX].z = oldV;
						}

						k++;
					}
					else
					{
						if(eobRun == 0)
						{
							while(k <= selectionEnd)
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
										eobRun = 1 << r;
										if(r>0)
										{
											eobRun += binSet.getBits(binNum, binNum+r, true);
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
										coef = (int)imgMat[startY+addY+currY][startX+addX+currX].x;
									else if(comNum==2)
										coef = (int)imgMat[startY+addY][startX+addX].y;
									else
										coef = (int)imgMat[startY+addY][startX+addX].z;

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
												imgMat[startY+addY+currY][startX+addX+currX].x = coef;
											else if(comNum==2)
											{
												imgMat[startY+addY][startX+addX].y = coef;
												imgMat[startY+addY][startX+addX+oAddX].y = coef;
												imgMat[startY+addY+oAddY][startX+addX+oAddX].y = coef;
												imgMat[startY+addY+oAddY][startX+addX].y = coef;
											}
											else
											{
												imgMat[startY+addY][startX+addX].z = coef;
												imgMat[startY+addY][startX+addX+oAddX].z = coef;
												imgMat[startY+addY+oAddY][startX+addX+oAddX].z = coef;
												imgMat[startY+addY+oAddY][startX+addX].z = coef;
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
										imgMat[startY+addY+currY][startX+addX+currX].x = acValue;
									}
									else if(comNum==2)
									{
										imgMat[startY+addY][startX+addX].y = acValue;
										imgMat[startY+addY][startX+addX+oAddX].y = acValue;
										imgMat[startY+addY+oAddY][startX+addX+oAddX].y = acValue;
										imgMat[startY+addY+oAddY][startX+addX].y = acValue;
									}
									else
									{
										imgMat[startY+addY][startX+addX].z = acValue;
										imgMat[startY+addY][startX+addX+oAddX].z = acValue;
										imgMat[startY+addY+oAddY][startX+addX+oAddX].z = acValue;
										imgMat[startY+addY+oAddY][startX+addX].z = acValue;
									}
								}

								k++;
							}
						}
						
						if(eobRun>0)
						{
							while(k<=selectionEnd)
							{
								int coef = 0;
								int addY = k/8;
								int addX = k%8;

								if(comNum==1)
									coef = (int)imgMat[startY+addY+currY][startX+addX+currX].x;
								else if(comNum==2)
									coef = (int)imgMat[startY+addY][startX+addX].y;
								else
									coef = (int)imgMat[startY+addY][startX+addX].z;

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
											imgMat[startY+addY+currY][startX+addX+currX].x = coef;
										else if(comNum==2)
										{
											imgMat[startY+addY][startX+addX].y = coef;
											imgMat[startY+addY][startX+addX+oAddX].y = coef;
											imgMat[startY+addY+oAddY][startX+addX+oAddX].y = coef;
											imgMat[startY+addY+oAddY][startX+addX].y = coef;
										}
										else
										{
											imgMat[startY+addY][startX+addX].z = coef;
											imgMat[startY+addY][startX+addX+oAddX].z = coef;
											imgMat[startY+addY+oAddY][startX+addX+oAddX].z = coef;
											imgMat[startY+addY+oAddY][startX+addX].z = coef;
										}
									}
									binNum++;
								}
								k++;
							}
							eobRun--;
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

	void baselineProcess(Image* img)
	{
		// time_t totalTime = 0; //Not used
		if(img!=nullptr)
		{
			//process image data
			BinarySet binSet = BinarySet();
			binSet.setBitOrder(BinarySet::RMSB);
			binSet.setValues(compressedData.data(), compressedData.size());

			//grab a single grid and process that first
			// int matFilled = 0; //Not used
			Vec3f blockAdd = Vec3f();
			Matrix q = Matrix(8,8);
			Matrix m = Matrix(8,8);

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

			addWidth = 8 * ((samplingRate[0]>>4) & 0xF);
			addHeight = 8 * ((samplingRate[0]) & 0xF);

			int yVals = (samplingRate[0]>>4) * (samplingRate[0] & 0x0F);
			
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
					int treeIndex1 = (componentData.at(order[i]) >> 4) & 0x0F;
					int treeIndex2 = componentData.at(order[i]) & 0x0F;
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
					
					BinaryTree<HuffmanNode>* dcTree = huffmanTreesDC[treeIndex1];
					BinaryTree<HuffmanNode>* acTree = huffmanTreesAC[treeIndex2];

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
						m[r][c] = decompData[y] * quantizationTables[quanTable][r][c];
					}

					MathExt::FCT8x8(m, &q, true);

					for(int y=0; y<8; y++)
					{
						for(int x=0; x<8; x++)
						{
							double value = MathExt::round(q[y][x] + 128);

							if(order[i]==0)
							{
								Color c = img->getPixel(actualX+x+currX, actualY+y+currY);
								c.red = (unsigned char) MathExt::clamp(value, 0.0, 255.0);
								img->setPixel(actualX+x+currX, actualY+y+currY, c);
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

								Color c = img->getPixel(actualX+x1, actualY+y1);
								Color c1 = img->getPixel(actualX+x2, actualY+y1);
								Color c2 = img->getPixel(actualX+x2, actualY+y2);
								Color c3 = img->getPixel(actualX+x1, actualY+y2);
								if(order[i]==1)
								{
									c.green = (unsigned char) MathExt::clamp(value, 0.0, 255.0);

									c1.green = c.green;
									c2.green = c.green;
									c3.green = c.green;
								}
								else
								{
									c.blue = (unsigned char) MathExt::clamp(value, 0.0, 255.0);

									c1.blue = c.blue;
									c2.blue = c.blue;
									c3.blue = c.blue;
								}

								img->setPixel(actualX+x1, actualY+y1, c);
								img->setPixel(actualX+x2, actualY+y1, c1);
								img->setPixel(actualX+x2, actualY+y2, c2);
								img->setPixel(actualX+x1, actualY+y2, c3);
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

	void Image::saveJPG(File file, int quality, int subsampleMode)
	{
		//save using jpeg quantization matrix
		// Matrix lum = getLuminanceQuantizationMatrix(quality);
		// Matrix chrom = getChrominanceQuantizationMatrix(quality);
	}
	
	Image** Image::loadJPG(std::vector<unsigned char> fileData, int* amountOfImages)
	{
		//magic number = 0xFF 0xD8

		Image** imgArr = nullptr;
		Image* img = nullptr;
		Vec3f** imgMat = nullptr;

		size_t index = 0;
		quantizationTables = std::vector<Matrix>(4);
		huffmanTreesDC = std::vector<BinaryTree<HuffmanNode>*>(4);
		huffmanTreesAC = std::vector<BinaryTree<HuffmanNode>*>(4);

		compressedData = std::vector<unsigned char>();
		componentData = std::vector<unsigned short>();

		samplingRate = std::vector<unsigned short>();

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
				//StringTools::println("Found start of baseline frame");
				frameType = 0;
				// unsigned short size = (((unsigned short)fileData[index])<<8) + fileData[index+1];
				index+=2;
				
				// unsigned char precision = fileData[index];
				index++;

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
					samplingRate.push_back((h1<<4) + v1);
					
					index+=3;
				}
				
				if(img==nullptr)
					img = new Image(width, height);
			}
			else if(header == 0xFFC1)
			{
				//StringTools::println("Found start of extended sequential frame");
				frameType = 1;
				// unsigned short size = (((unsigned short)fileData[index])<<8) + fileData[index+1];
				index+=2;
				
				// unsigned char precision = fileData[index];
				index++;

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
					samplingRate.push_back((h1<<4) + v1);
					
					index+=3;
				}

				if(img==nullptr)
					img = new Image(width, height);
			}
			else if(header == 0xFFC2)
			{
				//StringTools::println("Found Start of progressive Frame");
				frameType = 2;
				// unsigned short size = (((unsigned short)fileData[index])<<8) + fileData[index+1];
				index+=2;
				
				// unsigned char precision = fileData[index];
				index++;

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
					samplingRate.push_back((h1<<4) + v1);
					
					index+=3;
				}

				if(img==nullptr)
				{
					img = new Image(width, height);

					int addW = (width%8 > 0)? (8-width%8) : 0;
					int addH = (height%8 > 0)? (8-height%8) : 0;

					int width8 = width + addW + 8;
					int height8 = height + addH + 8;
					
					imgMat = new Vec3f*[height8];
					for(int i=0; i<height8; i++)
					{
						imgMat[i] = new Vec3f[width8];
					}
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
						if(huffmanTreesDC[treeNum] != nullptr)
						{
							delete huffmanTreesDC[treeNum];
						}
						huffmanTreesDC[treeNum] = tree;
					}
					else
					{
						if(huffmanTreesAC[treeNum] != nullptr)
						{
							delete huffmanTreesAC[treeNum];
						}
						huffmanTreesAC[treeNum] = tree;
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

					Matrix m = Matrix(8,8);

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
					
					quantizationTables[tableNum] = m;
				}

				index = finalLoc;
			}
			else if(header == 0xFFDA)
			{
				//Start of Scan
				// StringTools::println("START OF SCAN");
				// unsigned short size = (((unsigned short)fileData[index])<<8) + fileData[index+1];
				index+=2;

				int count = fileData[index];
				index++;

				componentData.clear();

				for(int i=0; i<count; i++)
				{
					unsigned short comData = (((unsigned short)fileData[index])<<8) + fileData[index+1];
					index+=2;
					componentData.push_back(comData);
				}

				selectStart = fileData[index];
				selectEnd = fileData[index+1];
				predictionForSuccessive = fileData[index+2];
				index+=3;
				
				//read until 0xFFxx is found where xx is not 00
				// int startInd = index; //Not used
				int nBytes = 0;
				compressedData.clear();
				processImage=true;

				while(true)
				{
					if(fileData[index]!=0xFF)
					{
						compressedData.push_back( fileData[index] );
						index++;
						nBytes++;
					}
					else
					{
						if(fileData[index+1]==0x00)
						{
							compressedData.push_back( 0xFF );
							index+=2;
							nBytes+=2;
						}
						else if(fileData[index+1] >= 0xD0 && fileData[index+1] <= 0xD7)
						{
							compressedData.push_back( 0xFF );
							compressedData.push_back( fileData[index+1] );
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
				dri = (((unsigned short)fileData[index])<<8) + fileData[index+1];
				index += 2;
				// StringTools::println("DRI");
			}
			else if(header >= 0xFFD0 && header <= 0xFFD7)
			{
				//RSTn - Restart
				//DRI must be defined
				
				// StringTools::println("RST");
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
					baselineProcess(img);
				}
				else if(frameType==2)
				{
					if(predictionForSuccessive>>4 == 0)
					{
						progressiveProcessFirst(img, imgMat, selectStart, selectEnd, predictionForSuccessive);
					}
					else
					{
						progressiveProcessRefine(img, imgMat, selectStart, selectEnd, predictionForSuccessive);
					}
				}
			}

		}

		if(frameType==2)
		{
			//convert imgMat using dct to get final image
			progressiveProcessEnd(img, imgMat);
			if(imgMat!=nullptr)
			{
				for(int i=0; i<height8; i++)
				{
					if(imgMat[i]!=nullptr)
						delete[] imgMat[i];
				}
				delete[] imgMat;
			}
		}

		//convert from ycbcr to rgb and store into image

		//chroma averaging if necessary
		if(((samplingRate[0]>>4)&0x0F) != 1)
		{
			//subsample in the x
			for(int y=0; y<img->getHeight(); y++)
			{
				for(int x=1; x<img->getWidth()-1; x++)
				{
					Color c = img->getPixel(x-1,y);
					Color c2 = img->getPixel(x+1,y);
					Color f = img->getPixel(x,y);

					f.green = (unsigned char)MathExt::round(((double)c.green+(double)c2.green)/2.0);
					f.blue = (unsigned char)MathExt::round(((double)c.blue+(double)c2.blue)/2.0);

					img->setPixel(x,y,f);
				}
			}
		}
		if((samplingRate[0]&0x0F) != 1)
		{
			//subsample in the y
			for(int y=1; y<img->getHeight()-1; y++)
			{
				for(int x=0; x<img->getWidth(); x++)
				{
					Color c = img->getPixel(x,y-1);
					Color c2 = img->getPixel(x,y+1);
					Color f = img->getPixel(x,y);

					f.green = (unsigned char)MathExt::round(((double)c.green+(double)c2.green)/2.0);
					f.blue = (unsigned char)MathExt::round(((double)c.blue+(double)c2.blue)/2.0);

					img->setPixel(x,y,f);
				}
			}
		}

		Color* startC = img->getPixels();
		Color* endC = startC + (img->getWidth()*img->getHeight());

		while(startC<endC)
		{
			Color cVec = ColorSpaceConverter::convert(*startC, ColorSpaceConverter::YCBCR_TO_RGB);
			cVec.alpha = 255;
			
			*startC = cVec;
			startC++;
		}
		

		imgArr = new Image*[1]{img};
		if(amountOfImages!=nullptr)
		{
			*amountOfImages = 1;
		}

		for(size_t i=0; i<huffmanTreesDC.size(); i++)
		{
			if(huffmanTreesDC[i]!=nullptr)
				delete huffmanTreesDC[i];
		}
		for(size_t i=0; i<huffmanTreesAC.size(); i++)
		{
			if(huffmanTreesAC[i]!=nullptr)
				delete huffmanTreesAC[i];
		}

		return imgArr;
	}

} //NAMESPACE glib END