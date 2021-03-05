#include "Image.h"
#include <vector>
#include "SimpleFile.h"
#include "Compression.h"
#include "Graphics.h"
#include <iostream>
#include "StringTools.h"
#include "Sort.h"
#include "ColorSpaceConverter.h"

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

	//assuming imgMat is correctly setup
	for(int y=0; y<img->getHeight(); y+=8)
	{
		offX = 0;
		for(int x=0; x<img->getWidth(); x+=8)
		{
			Matrix yMat = Matrix(8,8);
			Matrix cbMat = Matrix(8,8);
			Matrix crMat = Matrix(8,8);

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


			Matrix q1 = MathExt::cosineTransform2D(yMat, true);
			Matrix q2 = MathExt::cosineTransform2D(cbMat, true);
			Matrix q3 = MathExt::cosineTransform2D(crMat, true);

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

	int preShift = predictionForSuccessive >> 4;
	int postShift = predictionForSuccessive & 0xF;

	int fillSize = selectionEnd - selectionStart + 1;

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
		for(int i=0; i<nComponentData.size(); i++)
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
						dcValue += blockAdd.x;
						blockAdd.x = dcValue;
					}
					else if(comNum==2)
					{
						dcValue += blockAdd.y;
						blockAdd.y = dcValue;
					}
					else
					{
						dcValue += blockAdd.z;
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

	int preShift = predictionForSuccessive >> 4;
	int postShift = predictionForSuccessive & 0xF;

	int fillSize = selectionEnd - selectionStart + 1;
	int k = selectionStart;
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
		for(int i=0; i<nComponentData.size(); i++)
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
									coef = imgMat[startY+addY+currY][startX+addX+currX].x;
								else if(comNum==2)
									coef = imgMat[startY+addY][startX+addX].y;
								else
									coef = imgMat[startY+addY][startX+addX].z;

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
								coef = imgMat[startY+addY+currY][startX+addX+currX].x;
							else if(comNum==2)
								coef = imgMat[startY+addY][startX+addX].y;
							else
								coef = imgMat[startY+addY][startX+addX].z;

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
	if(img!=nullptr)
	{
		//process image data
		BinarySet binSet = BinarySet();
		binSet.setBitOrder(BinarySet::RMSB);
		binSet.setValues(compressedData.data(), compressedData.size());

		//grab a single grid and process that first
		int matFilled = 0;
		Vec3f blockAdd = Vec3f();

		int actualX = 0;
		int actualY = 0;
		int addVal = 0;
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
							dcValue += blockAdd.x;
							blockAdd.x = dcValue;
						}
						else if(order[i]==1)
						{
							dcValue += blockAdd.y;
							blockAdd.y = dcValue;
						}
						else
						{
							dcValue += blockAdd.z;
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

				Matrix m = Matrix(8,8);
				int r=0;
				int c=0;
				bool zig=false;
				bool backwards = false;
				for(int y=0; y<64; y++)
				{
					int zigPos = getZigZagPos(y);
					r = zigPos>>4;
					c = zigPos & 0x0F;
					m[r][c] = decompData[y] * quantizationTables[quanTable][r][c];
				}

				Matrix q = MathExt::cosineTransform2D(m, true);

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

void Image::saveJPG(std::string filename)
{
    
}

Image** Image::loadJPG(std::vector<unsigned char> fileData, int* amountOfImages)
{
	//magic number = 0xFF 0xD8

	Image** imgArr = nullptr;
	Image* img = nullptr;
	Vec3f** imgMat = nullptr;

	int index = 0;
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

	int width8 = 0;
	int height8 = 0;

	int chromaX = 1;
	int chromaY = 1;

	while(index < fileData.size())
	{
		unsigned short header = (((unsigned short)fileData[index])<<8) + fileData[index+1];
		index += 2;

		//StringTools::println("header %x", header);
		
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
			unsigned short size = (((unsigned short)fileData[index])<<8) + fileData[index+1];
			index+=2;
			
			unsigned char precision = fileData[index];
			index++;

			unsigned short height = (((unsigned short)fileData[index])<<8) + fileData[index+1];
			index+=2;

			unsigned short width = (((unsigned short)fileData[index])<<8) + fileData[index+1];
			index+=2;

			unsigned char components = fileData[index];
			index++;
			
			for(int i=0; i<components; i++)
			{
				unsigned char id = fileData[index];
				unsigned char sampling = fileData[index+1];
				unsigned char tableNum = fileData[index+2];

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
			unsigned short size = (((unsigned short)fileData[index])<<8) + fileData[index+1];
			index+=2;
			
			unsigned char precision = fileData[index];
			index++;

			unsigned short height = (((unsigned short)fileData[index])<<8) + fileData[index+1];
			index+=2;

			unsigned short width = (((unsigned short)fileData[index])<<8) + fileData[index+1];
			index+=2;

			unsigned char components = fileData[index];
			index++;
			
			for(int i=0; i<components; i++)
			{
				unsigned char id = fileData[index];
				unsigned char sampling = fileData[index+1];
				unsigned char tableNum = fileData[index+2];

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
			unsigned short size = (((unsigned short)fileData[index])<<8) + fileData[index+1];
			index+=2;
			
			unsigned char precision = fileData[index];
			index++;

			unsigned short height = (((unsigned short)fileData[index])<<8) + fileData[index+1];
			index+=2;

			unsigned short width = (((unsigned short)fileData[index])<<8) + fileData[index+1];
			index+=2;

			unsigned char components = fileData[index];
			index++;
			
			for(int i=0; i<components; i++)
			{
				unsigned char id = fileData[index];
				unsigned char sampling = fileData[index+1];
				unsigned char tableNum = fileData[index+2];

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
			
			int finalLoc = index+size;
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
			int finalLoc = index+size;

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
			unsigned short size = (((unsigned short)fileData[index])<<8) + fileData[index+1];
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
			int startInd = index;
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
			for(int x=0; x<img->getWidth()-1; x++)
			{
				if(x%2 == 1)
				{
					Color c = img->getPixel(x,y);
					Color c2 = img->getPixel(x+1,y);
					Color f = {c.red, 0, 0, 255};

					f.green = (unsigned char)(((int)c.green+(int)c2.green)/2);
					f.blue = (unsigned char)(((int)c.blue+(int)c2.blue)/2);

					img->setPixel(x,y,f);
				}
			}
		}
	}
	if((samplingRate[0]&0x0F) != 1)
	{
		//subsample in the y
		for(int y=0; y<img->getHeight()-1; y++)
		{
			if(y%2 == 0)
			{
				for(int x=0; x<img->getWidth(); x++)
				{
					Color c = img->getPixel(x,y);
					Color c2 = img->getPixel(x,y+1);
					Color f = {c.red, 0, 0, 255};

					f.green = (unsigned char)(((int)c.green+(int)c2.green)/2);
					f.blue = (unsigned char)(((int)c.blue+(int)c2.blue)/2);

					img->setPixel(x,y,f);
				}
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

	for(int i=0; i<huffmanTreesDC.size(); i++)
	{
		if(huffmanTreesDC[i]!=nullptr)
			delete huffmanTreesDC[i];
	}
	for(int i=0; i<huffmanTreesAC.size(); i++)
	{
		if(huffmanTreesAC[i]!=nullptr)
			delete huffmanTreesAC[i];
	}

	return imgArr;
}

