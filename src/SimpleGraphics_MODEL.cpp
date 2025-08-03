#include "InternalGraphicsHeader.h"

namespace smpl
{
	
	void formatToString(StringStream& stream, const PolyCriticalPoint& v, const std::string& options)
	{
		stream.write('(');
		formatToString(stream, v.xValue, "%.3f");
		stream.write(" | ");
		formatToString(stream, v.direction, "%d");
		stream.write(")");
	}
	void SimpleGraphics::drawPolygon(Vec2f* points, int size, Image* surf)
	{
		Image* otherImg;
		if (surf == nullptr)
			return;
		else
			otherImg = surf;

		if (otherImg != nullptr)
		{
			if(otherImg->getWidth()<=0 || otherImg->getHeight()<=0)
			{
				return;
			}
			//similar in nature to the vector graphic version
			//always connects the last point to the first

			std::vector<Vec2f> actualPoints = std::vector<Vec2f>();
			for(int i=0; i<size; i++)
			{
				actualPoints.push_back(points[i]);
			}
			actualPoints.push_back(points[0]);
			
			int minY = (int)points[0].y;
			int maxY = (int)points[0].y;

			for(int i=0; i<size; i++)
			{
				if((int)points[i].y < minY)
				{
					minY = (int)points[i].y;
				}
				if((int)points[i].y > maxY)
				{
					maxY = (int)points[i].y;
				}
			}

			minY = MathExt::clamp(minY, 0, otherImg->getHeight()-1);
			maxY = MathExt::clamp(maxY, 0, otherImg->getHeight()-1);

			int yDis = 1+maxY-minY;

			std::vector<std::vector<PolyCriticalPoint>> scanLines = std::vector<std::vector<PolyCriticalPoint>>(yDis);
			std::vector<std::vector<std::pair<double, double>>> horizontalLinesFound = std::vector<std::vector<std::pair<double, double>>>(yDis);

			for(int i=0; i<size; i++)
			{
				//Ax + By + C = 0
				//A=(y1-y2)
				//B=(x2-x1)
				//C=(x1*y2 - x2*y1)
				double A = (actualPoints[i].y - actualPoints[i+1].y);
				double B = (actualPoints[i+1].x - actualPoints[i].x);
				double C = (actualPoints[i].x*actualPoints[i+1].y - actualPoints[i+1].x*actualPoints[i].y);
				bool posDir = true;

				//AFAIK, if the y is going up in some way, the direction is positive
				//otherwise, it is negative.
				if(A<=0)
				{
					posDir = true;
				}
				else
				{
					posDir = false;
				}
				
				if(A!=0)
				{
					//vertical line or some other valid line
					int thisMinY = (int)MathExt::round( MathExt::min( actualPoints[i].y, actualPoints[i+1].y));
					int thisMaxY = (int)MathExt::round( MathExt::max( actualPoints[i].y, actualPoints[i+1].y));

					thisMinY = MathExt::clamp(thisMinY, minY, maxY);
					thisMaxY = MathExt::clamp(thisMaxY, minY, maxY);

					for(int j=thisMinY; j<=thisMaxY; j++)
					{
						double xValue = -(B*j + C) / A;
						xValue = MathExt::clamp<double>(xValue, 0, otherImg->getWidth());
						
						scanLines[j-minY].push_back({xValue,posDir});
					}
				}
				else
				{
					//horizontal line or invalid
					//do nothing
					int thisMinY = (int)MathExt::round( MathExt::min( actualPoints[i].y, actualPoints[i+1].y));
					double thisMinX = MathExt::min( actualPoints[i].x, actualPoints[i+1].x);
					double thisMaxX = MathExt::max( actualPoints[i].x, actualPoints[i+1].x);
					horizontalLinesFound[thisMinY-minY].push_back({thisMinX,thisMaxX});
				}
			}

			//sort the points on the scanlines
			//#pragma omp parallel for
			for(int i=0; i<yDis; i++)
			{
				if(scanLines[i].size()>0)
				{
					Sort::insertionSort<PolyCriticalPoint>(scanLines[i].data(), scanLines[i].size(), [](PolyCriticalPoint a, PolyCriticalPoint b) -> bool {return a.xValue<b.xValue;});
					
					//may need to insert a points to handle horizontal line cases
					{
						std::vector<PolyCriticalPoint> tempLines;
						int j=0;
						while(j < scanLines[i].size())
						{
							if(j == scanLines[i].size()-1)
							{
								tempLines.push_back(scanLines[i][j]);
								j++;
								break;
							}
							
							bool isConnected = false;
							for(std::pair<double, double> p : horizontalLinesFound[i])
							{
								if(p.first == scanLines[i][j].xValue && p.second == scanLines[i][j+1].xValue)
								{
									//this is part of a horizontal line. do extra stuff
									isConnected = true;
								}
							}
							
							if(isConnected)
							{
								//do extra stuff
								if(scanLines[i][j].direction != scanLines[i][j+1].direction)
								{
									tempLines.push_back(scanLines[i][j]);
								}
								j++;
							}
							else
							{
								//add single point
								tempLines.push_back(scanLines[i][j]);
								j++;
							}
						}
						scanLines[i] = tempLines;
					}
					
					//rule, can not be the same if you are filling to it.
					//different for even-odd and non-zero
					std::vector<PolyCriticalPoint> newScanLine = std::vector<PolyCriticalPoint>();

					if(fillRule == SimpleGraphics::FILL_EVEN_ODD)
					{
						for(int j=1; j<scanLines[i].size(); j+=2)
						{
							//check if its apart of a horizontal line. IE, both xValues at this y value are apart of the points exactly
							if(scanLines[i][j].xValue != scanLines[i][j-1].xValue)
							{
								newScanLine.push_back(scanLines[i][j-1]);
								newScanLine.push_back(scanLines[i][j]);
							}
							else
							{
								if(scanLines[i][j].direction != scanLines[i][j-1].direction)
								{
									newScanLine.push_back(scanLines[i][j-1]);
									newScanLine.push_back(scanLines[i][j]);
								}
							}
						}
					}
					else
					{
						newScanLine.push_back(scanLines[i][0]);
						for(int j=1; j<scanLines[i].size(); j++)
						{
							if(scanLines[i][j].xValue != scanLines[i][j-1].xValue)
							{
								newScanLine.push_back(scanLines[i][j]);
							}
							else
							{
								if(scanLines[i][j].direction != scanLines[i][j-1].direction)
								{
									newScanLine.push_back(scanLines[i][j]);
								}
							}
						}
					}

					scanLines[i] = newScanLine;
				}
			}

			if(fillRule==SimpleGraphics::FILL_EVEN_ODD)
			{
				//even-odd rule. Fill between even to odd indicies
				//not odd to even indicies
				LARGE_ENOUGH_CLAUSE(yDis)
				#pragma omp parallel for
				for(int j=0; j<yDis; j++)
				{
					int actualSize = scanLines[j].size();
					if(actualSize%2 == 1)
					{
						actualSize-=2;
					}
					else
					{
						actualSize-=1;
					}
					

					for(int i=0; i<actualSize; i+=2)
					{
						//fill between spots
						double fracStartX, fracEndX;
						int startX = MathExt::floor(scanLines[j][i].xValue);
						int endX = MathExt::ceil(scanLines[j][i+1].xValue);

						int fillX = startX;
						do
						{
							SimpleGraphics::drawPixel(fillX, j+minY, SimpleGraphics::activeColor, otherImg);
							fillX++;
						} while (fillX<=endX);

						// fracStartX = 1.0-MathExt::frac(scanLines[j][i].xValue);
						// fracEndX = MathExt::frac(scanLines[j][i+1].xValue);
						// if(fracEndX <= 0.0)
						// 	fracEndX = 1.0;
						
						// fracStartX = 1;
						// fracEndX = 1;
						// Color startBlend = activeColor;
						// Color endBlend = activeColor;
						// startBlend.alpha = fracStartX*255;
						// endBlend.alpha = fracEndX*255;
						
						// SimpleGraphics::drawPixel(startX, j+minY, startBlend, otherImg);
						// if(fracStartX >= 1.0)
						// 	SimpleGraphics::drawPixel(startX, j+minY, SimpleGraphics::activeColor, otherImg);
						
						// int fillX = startX+1;
						// while(fillX <= endX-1)
						// {
						// 	SimpleGraphics::drawPixel(fillX, j+minY, SimpleGraphics::activeColor, otherImg);
						// 	fillX++;
						// }
						// SimpleGraphics::drawPixel(endX, j+minY, endBlend, otherImg);
					}
				}
				RESET_LARGE_ENOUGH_CLAUSE()
			}
			else
			{
				//non-zero rule. Fill when positive only. Not when zero or
				//less.
				LARGE_ENOUGH_CLAUSE(yDis)
				#pragma omp parallel for
				for(int j=0; j<yDis; j++)
				{
					int passCounter = 0;
					for(int i=0; i<scanLines[j].size()-1; i++)
					{
						//fill between spots
						int startX = MathExt::floor(scanLines[j][i].xValue);
						int endX = MathExt::ceil(scanLines[j][i+1].xValue);
						if(scanLines[j][i].direction == true)
						{
							//positive y direction
							passCounter++;
						}
						else
						{
							//negative y direction
							passCounter--;
						}
						
						if(passCounter!=0)
						{
							int fillX = startX;

							do
							{
								SimpleGraphics::drawPixel(fillX, j+minY, SimpleGraphics::activeColor, otherImg);
								fillX++;
							} while (fillX<=endX);
							
						}
					}
				}
				RESET_LARGE_ENOUGH_CLAUSE()
			}

		}

	}
	
	void SimpleGraphics::drawModel(Model* model, Image* texture, Image* surf)
	{
		Image* otherImg;
		if (surf == nullptr)
			return;
		else
			otherImg = surf;

		if (otherImg != nullptr)
		{
			if(otherImg->getWidth()<=0 || otherImg->getHeight()<=0)
			{
				return;
			}

			std::vector<VertexFormat> modelVertFormat = model->getVertexFormatInfomation();

			int startOfPosition = -1; //must be >= 0
			int startOfTexture = -1; //Can be -1 but must be >= 0 for textures to work
			
			for(int i=0; i<modelVertFormat.size(); i++)
			{
				if(modelVertFormat[i].usage == Model::USAGE_POSITION)
				{
					if(modelVertFormat[i].type == Model::TYPE_VEC2)
					{
						startOfPosition = i;
					}
				}

				if(modelVertFormat[i].usage == Model::USAGE_TEXTURE)
				{
					if(modelVertFormat[i].type == Model::TYPE_VEC2)
					{
						startOfTexture = i;
					}
				}
			}

			int amtVert = model->size();

            bool order = false;
			if(startOfPosition<0)
			{
				return;
			}

			switch (model->getModelFormat())
			{
			case Model::POINTS:
				//draw pixel at point x,y
				for(int i=0; i<amtVert; i++)
				{
					std::vector<std::vector<int>> vertInfo = model->getVertex(i);
					float* positionData = (float*)vertInfo[startOfPosition].data();

					Vec2f pos = Vec2f( positionData[0], positionData[1] );
					
					SimpleGraphics::drawPixel(pos.x, pos.y, SimpleGraphics::activeColor, surf);
				}
				break;
			case Model::LINES:
				//draw lines at point x,y to point x2,y2
				for(int i=1; i<amtVert; i+=2)
				{
					std::vector<std::vector<int>> vertInfo = model->getVertex(i-1);
					std::vector<std::vector<int>> vertInfo2 = model->getVertex(i);

					float* positionData1 = (float*)vertInfo[startOfPosition].data();
					float* positionData2 = (float*)vertInfo2[startOfPosition].data();

					Vec2f pos = Vec2f( positionData1[0], positionData1[1] );
					Vec2f pos2 = Vec2f( positionData2[0], positionData2[1] );

					SimpleGraphics::drawLine((int)pos.x, (int)pos.y, (int)pos2.x, (int)pos2.y, surf);
				}
				break;
            case Model::LINE_STRIP:
                //draw lines at point x,y to point x2,y2
				for(int i=1; i<amtVert; i++)
				{
					std::vector<std::vector<int>> vertInfo = model->getVertex(i-1);
					std::vector<std::vector<int>> vertInfo2 = model->getVertex(i);

					float* positionData1 = (float*)vertInfo[startOfPosition].data();
					float* positionData2 = (float*)vertInfo2[startOfPosition].data();

					Vec2f pos = Vec2f( positionData1[0], positionData1[1] );
					Vec2f pos2 = Vec2f( positionData2[0], positionData2[1] );

					SimpleGraphics::drawLine((int)pos.x, (int)pos.y, (int)pos2.x, (int)pos2.y, surf);
				}
				break;
            case Model::LINE_LOOP:
                //draw lines at point x,y to point x2,y2
				for(int i=1; i<amtVert+1; i++)
				{
					std::vector<std::vector<int>> vertInfo = model->getVertex(i-1);
					std::vector<std::vector<int>> vertInfo2;
					if( i == amtVert)
						vertInfo2 = model->getVertex(0);
					else
						vertInfo2 = model->getVertex(i);

					float* positionData1 = (float*)vertInfo[startOfPosition].data();
					float* positionData2 = (float*)vertInfo2[startOfPosition].data();

					Vec2f pos = Vec2f( positionData1[0], positionData1[1] );
					Vec2f pos2 = Vec2f( positionData2[0], positionData2[1] );

					SimpleGraphics::drawLine((int)pos.x, (int)pos.y, (int)pos2.x, (int)pos2.y, surf);
				}
				break;
			case Model::TRIANGLES:
				//draw triangles at point x,y to point x2,y2 to point x3,y3
				for(int i=2; i<amtVert; i+=3)
				{
					std::vector<std::vector<int>> vertInfo = model->getVertex(i-2);
					std::vector<std::vector<int>> vertInfo2 = model->getVertex(i-1);
					std::vector<std::vector<int>> vertInfo3 = model->getVertex(i);

					float* positionData1 = (float*)vertInfo[startOfPosition].data();
					float* positionData2 = (float*)vertInfo2[startOfPosition].data();
					float* positionData3 = (float*)vertInfo3[startOfPosition].data();

					if(startOfTexture<0)
					{
						Vec2f pos = Vec2f( positionData1[0], positionData1[1] );
						Vec2f pos2 = Vec2f( positionData2[0], positionData2[1] );
						Vec2f pos3 = Vec2f( positionData3[0], positionData3[1] );
						
						SimpleGraphics::drawTriangle((int)pos.x, (int)pos.y, (int)pos2.x, (int)pos2.y, (int)pos3.x, (int)pos3.y, false, surf);
					}
					else
					{
						float* textureData1 = (float*)vertInfo[startOfTexture].data();
						float* textureData2 = (float*)vertInfo2[startOfTexture].data();
						float* textureData3 = (float*)vertInfo3[startOfTexture].data();
						
						Vec4f pos = Vec4f( positionData1[0], positionData1[1], textureData1[0], textureData1[1] );
						Vec4f pos2 = Vec4f( positionData2[0], positionData2[1], textureData2[0], textureData2[1] );
						Vec4f pos3 = Vec4f( positionData3[0], positionData3[1], textureData3[0], textureData3[1] );

						SimpleGraphics::drawTexturedTriangle(pos, pos2, pos3, texture, surf);
					}
				}
				break;
			case Model::TRIANGLE_FAN:
				//draw triangles using first point to point x2,y2 to point x3,y3
				
				for(int i=2; i<amtVert; i+=1)
				{
					std::vector<std::vector<int>> vertInfo = model->getVertex(0);
					std::vector<std::vector<int>> vertInfo2 = model->getVertex(i-1);
					std::vector<std::vector<int>> vertInfo3 = model->getVertex(i);

					float* positionData1 = (float*)vertInfo[startOfPosition].data();
					float* positionData2 = (float*)vertInfo2[startOfPosition].data();
					float* positionData3 = (float*)vertInfo3[startOfPosition].data();
					
					if(startOfTexture<0)
					{
						Vec2f pos = Vec2f( positionData1[0], positionData1[1] );
						Vec2f pos2 = Vec2f( positionData2[0], positionData2[1] );
						Vec2f pos3 = Vec2f( positionData3[0], positionData3[1] );
						
						SimpleGraphics::drawTriangle((int)pos.x, (int)pos.y, (int)pos2.x, (int)pos2.y, (int)pos3.x, (int)pos3.y, false, surf);
					}
					else
					{
						float* textureData1 = (float*)vertInfo[startOfTexture].data();
						float* textureData2 = (float*)vertInfo2[startOfTexture].data();
						float* textureData3 = (float*)vertInfo3[startOfTexture].data();
						
						Vec4f pos = Vec4f( positionData1[0], positionData1[1], textureData1[0], textureData1[1] );
						Vec4f pos2 = Vec4f( positionData2[0], positionData2[1], textureData2[0], textureData2[1] );
						Vec4f pos3 = Vec4f( positionData3[0], positionData3[1], textureData3[0], textureData3[1] );

						SimpleGraphics::drawTexturedTriangle(pos, pos2, pos3, texture, surf);
					}
				}
				break;
            case Model::TRIANGLE_STRIP:
                for(int i=2; i<amtVert; i++)
                {
					std::vector<std::vector<int>> vertInfo, vertInfo2, vertInfo3;

					if(!order)
					{
						vertInfo = model->getVertex(i-2);
						vertInfo2 = model->getVertex(i-1);
					}
					else
					{
						vertInfo = model->getVertex(i-1);
						vertInfo2 = model->getVertex(i-2);
					}

					vertInfo3 = model->getVertex(i);
					
					float* positionData1 = (float*)vertInfo[startOfPosition].data();
					float* positionData2 = (float*)vertInfo2[startOfPosition].data();
					float* positionData3 = (float*)vertInfo3[startOfPosition].data();

					if(startOfTexture<0)
					{
						Vec2f pos = Vec2f( positionData1[0], positionData1[1] );
						Vec2f pos2 = Vec2f( positionData2[0], positionData2[1] );
						Vec2f pos3 = Vec2f( positionData3[0], positionData3[1] );
						
						SimpleGraphics::drawTriangle((int)pos.x, (int)pos.y, (int)pos2.x, (int)pos2.y, (int)pos3.x, (int)pos3.y, false, surf);
					}
					else
					{
						float* textureData1 = (float*)vertInfo[startOfTexture].data();
						float* textureData2 = (float*)vertInfo2[startOfTexture].data();
						float* textureData3 = (float*)vertInfo3[startOfTexture].data();
						
						Vec4f pos = Vec4f( positionData1[0], positionData1[1], textureData1[0], textureData1[1] );
						Vec4f pos2 = Vec4f( positionData2[0], positionData2[1], textureData2[0], textureData2[1] );
						Vec4f pos3 = Vec4f( positionData3[0], positionData3[1], textureData3[0], textureData3[1] );

						SimpleGraphics::drawTexturedTriangle(pos, pos2, pos3, texture, surf);
					}
                    order = !order;
                }

                break;
			case Model::QUADS:
				//draw 2 triangles using 4 points
				for(int i=3; i<amtVert; i+=4)
				{
					std::vector<std::vector<int>> vertInfo, vertInfo2, vertInfo3, vertInfo4;

					vertInfo = model->getVertex(i-3);
					vertInfo2 = model->getVertex(i-2);
					vertInfo3 = model->getVertex(i-1);
					vertInfo4 = model->getVertex(i);
					
					
					float* positionData1 = (float*)vertInfo[startOfPosition].data();
					float* positionData2 = (float*)vertInfo2[startOfPosition].data();
					float* positionData3 = (float*)vertInfo3[startOfPosition].data();
					float* positionData4 = (float*)vertInfo4[startOfPosition].data();
					
					if(startOfTexture<0)
					{
						Vec2f pos = Vec2f( positionData1[0], positionData1[1] );
						Vec2f pos2 = Vec2f( positionData2[0], positionData2[1] );
						Vec2f pos3 = Vec2f( positionData3[0], positionData3[1] );
						
						SimpleGraphics::drawTriangle((int)pos.x, (int)pos.y, (int)pos2.x, (int)pos2.y, (int)pos3.x, (int)pos3.y, false, surf);

						pos = Vec2f( positionData1[0], positionData1[1] );
						pos2 = Vec2f( positionData3[0], positionData3[1] );
						pos3 = Vec2f( positionData4[0], positionData4[1] );
						
						SimpleGraphics::drawTriangle((int)pos.x, (int)pos.y, (int)pos2.x, (int)pos2.y, (int)pos3.x, (int)pos3.y, false, surf);
					}
					else
					{
						float* textureData1 = (float*)vertInfo[startOfTexture].data();
						float* textureData2 = (float*)vertInfo2[startOfTexture].data();
						float* textureData3 = (float*)vertInfo3[startOfTexture].data();
						float* textureData4 = (float*)vertInfo4[startOfTexture].data();

						Vec4f pos = Vec4f( positionData1[0], positionData1[1], textureData1[0], textureData1[1] );
						Vec4f pos2 = Vec4f( positionData2[0], positionData2[1], textureData2[0], textureData2[1] );
						Vec4f pos3 = Vec4f( positionData3[0], positionData3[1], textureData3[0], textureData3[1] );

						SimpleGraphics::drawTexturedTriangle(pos, pos2, pos3, texture, surf);

						pos = Vec4f( positionData1[0], positionData1[1], textureData1[0], textureData1[1] );
						pos2 = Vec4f( positionData3[0], positionData3[1], textureData3[0], textureData3[1] );
						pos3 = Vec4f( positionData4[0], positionData4[1], textureData4[0], textureData4[1] );

						SimpleGraphics::drawTexturedTriangle(pos, pos2, pos3, texture, surf);
					}
				}
				break;
            case Model::QUAD_STRIP:
                //draw 2 triangles using 4 points

				for(int i=3; i<amtVert; i+=2)
				{
					std::vector<std::vector<int>> vertInfo, vertInfo2, vertInfo3, vertInfo4;

					if(!order)
					{
						vertInfo = model->getVertex(i-3);
						vertInfo2 = model->getVertex(i-2);
						vertInfo3 = model->getVertex(i-1);
						vertInfo4 = model->getVertex(i);
					}
                    else
					{
                        vertInfo = model->getVertex(i-1);
						vertInfo2 = model->getVertex(i-2);
						vertInfo3 = model->getVertex(i-3);
						vertInfo4 = model->getVertex(i);
                    }

					float* positionData1 = (float*)vertInfo[startOfPosition].data();
					float* positionData2 = (float*)vertInfo2[startOfPosition].data();
					float* positionData3 = (float*)vertInfo3[startOfPosition].data();
					float* positionData4 = (float*)vertInfo4[startOfPosition].data();
					
					if(startOfTexture<0)
					{
						Vec2f pos = Vec2f( positionData1[0], positionData1[1] );
						Vec2f pos2 = Vec2f( positionData2[0], positionData2[1] );
						Vec2f pos3 = Vec2f( positionData3[0], positionData3[1] );
						
						SimpleGraphics::drawTriangle((int)pos.x, (int)pos.y, (int)pos2.x, (int)pos2.y, (int)pos3.x, (int)pos3.y, false, surf);

						pos = Vec2f( positionData1[0], positionData1[1] );
						pos2 = Vec2f( positionData3[0], positionData3[1] );
						pos3 = Vec2f( positionData4[0], positionData4[1] );
						
						SimpleGraphics::drawTriangle((int)pos.x, (int)pos.y, (int)pos2.x, (int)pos2.y, (int)pos3.x, (int)pos3.y, false, surf);
					}
					else
					{
						float* textureData1 = (float*)vertInfo[startOfTexture].data();
						float* textureData2 = (float*)vertInfo2[startOfTexture].data();
						float* textureData3 = (float*)vertInfo3[startOfTexture].data();
						float* textureData4 = (float*)vertInfo4[startOfTexture].data();

						Vec4f pos = Vec4f( positionData1[0], positionData1[1], textureData1[0], textureData1[1] );
						Vec4f pos2 = Vec4f( positionData2[0], positionData2[1], textureData2[0], textureData2[1] );
						Vec4f pos3 = Vec4f( positionData3[0], positionData3[1], textureData3[0], textureData3[1] );

						SimpleGraphics::drawTexturedTriangle(pos, pos2, pos3, texture, surf);

						pos = Vec4f( positionData1[0], positionData1[1], textureData1[0], textureData1[1] );
						pos2 = Vec4f( positionData3[0], positionData3[1], textureData3[0], textureData3[1] );
						pos3 = Vec4f( positionData4[0], positionData4[1], textureData4[0], textureData4[1] );

						SimpleGraphics::drawTexturedTriangle(pos, pos2, pos3, texture, surf);
					}

                    order = !order;
				}
                break;
			default:
				break;
			}
		}
	}

} //NAMESPACE glib END