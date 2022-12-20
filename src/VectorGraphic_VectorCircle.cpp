#include "VectorGraphic.h"
#include "SimpleGraphics.h"
#include "Sort.h"
#include "StringTools.h"
#include "BezierCurve.h"
#include "ColorNameConverter.h"

namespace glib
{
		
	#pragma region VectorCircle

	const Class VectorCircle::globalClass = Class("VectorCircle", {&VectorShape::globalClass});

	VectorCircle::VectorCircle() : VectorShape()
	{
		setClass(globalClass);
	}

	VectorCircle::~VectorCircle()
	{
	}

	void VectorCircle::testDraw(Image* buffer, int globalWidth, int globalHeight)
	{
		//first, calc bounding box
		double halfStroke = getStrokeWidth()/2;
		int minX = MathExt::clamp((int)(cx-radius-halfStroke), 0, globalWidth);
		int maxX = MathExt::clamp((int)(cx+radius+halfStroke), 0, globalWidth);
		int minY = MathExt::clamp((int)(cy-radius-halfStroke), 0, globalHeight);
		int maxY = MathExt::clamp((int)(cy+radius+halfStroke), 0, globalHeight);

		double insideRad = MathExt::sqrt(MathExt::sqr(radius-halfStroke));
		double insideRad2 = MathExt::sqrt(MathExt::sqr(radius-halfStroke+1));
		double outsideRad = MathExt::sqrt(MathExt::sqr(radius+halfStroke));
		double outsideRad2 = MathExt::sqrt(MathExt::sqr(radius+halfStroke+1));

		for(int y=minY; y<=maxY; y++)
		{
			for(int x=minX; x<=maxX; x++)
			{
				//measure distance
				double dis = MathExt::sqrt(MathExt::sqr(cx-x) + MathExt::sqr(cy-y));

				if(dis < outsideRad2)
				{
					if(getStrokeWidth() == 0)
					{
						if(dis <= insideRad)
						{
							buffer->drawPixel(x, y, getFillColor()); // fully filled inside circle
						}
						else
						{
							//normal anti-aliasing
							double percentFilled = outsideRad2 - dis;
							Color c = getFillColor();
							c.alpha = (unsigned char)(c.alpha * percentFilled);

							buffer->drawPixel(x, y, c);
						}
					}
					else
					{
						if(dis <= insideRad)
						{
							buffer->drawPixel(x, y, getFillColor()); // fully filled inside circle
						}
						else
						{
							//potentially inside stroke
							if(dis <= outsideRad)
							{
								//inside stroke or inside edge of stroke
								if(dis >= insideRad2)
								{
									buffer->drawPixel(x, y, getStrokeColor());
								}
								else
								{
									double percentFilled = (dis - insideRad2);
									Color c = getStrokeColor();
									c.alpha = (unsigned char)(c.alpha * percentFilled);

									buffer->drawPixel(x, y, getFillColor());
									buffer->drawPixel(x, y, c);
								}
							}
							else
							{
								
								double percentFilled = (outsideRad2 - dis);
								Color c = getStrokeColor();
								c.alpha = (unsigned char)(c.alpha * percentFilled);

								buffer->drawPixel(x, y, c);
							}
						}
					}
				}
			}
		}
	}

	void VectorCircle::draw(Image* buffer, int globalWidth, int globalHeight)
	{
		if(Mat3f::getIdentity() != getTransform())
		{
			//Note that this function draws a path version of the shape
			drawTransformed(buffer, globalWidth, globalHeight);
			return;
		}

		testDraw(buffer, globalWidth, globalHeight);
		return;

		//multiply everything by 1 + AA_LEVEL then calculate.
		//At end, use sample data to adjust transparency.

		if(VectorGraphic::getAALevel()==0)
		{
			#pragma region OLD_CODE

				//first, calc bounding box
				double tempRad = radius;

				int y1 = (int)MathExt::floor(cy-tempRad);
				int y2 = (int)MathExt::floor(cy+tempRad-1);

				y1 = MathExt::clamp(y1, 0, buffer->getHeight());
				y2 = MathExt::clamp(y2, 0, buffer->getHeight());

				double radSqr = MathExt::sqr(tempRad);

				for(int j=y1; j<=y2; j++)
				{
					double startX, endX;
					
					startX = -MathExt::sqrt( radSqr - MathExt::sqr(j-cy+0.5) ) + cx;
					endX = MathExt::sqrt( radSqr - MathExt::sqr(j-cy+0.5) ) + cx;

					for(int i=(int)MathExt::round(startX); i<(int)MathExt::round(endX); i++)
					{
						buffer->drawPixel(i, j, getFillColor());
					}
				}

			#pragma endregion
		}
		else
		{
			#pragma region NEW_CODE
				
				int AAadjustment = VectorGraphic::getAALevel() + 1;
				//first, calc bounding box
				double tempRad = radius*AAadjustment;
				double nCX = cx*AAadjustment;
				double nCY = cy*AAadjustment;

				int y1 = (int)MathExt::floor(nCY-tempRad);
				int y2 = (int)MathExt::floor(nCY+tempRad - 1);

				int maxXDis = buffer->getWidth() * AAadjustment;
				int maxYDis = buffer->getHeight() * AAadjustment;
				
				y1 = MathExt::clamp(y1, 0, maxYDis);
				y2 = MathExt::clamp(y2, 0, maxYDis);

				double radSqr = MathExt::sqr(tempRad);
				int i = 0;
				std::vector<Vec2f> AAscanLines = std::vector<Vec2f>(1+y2-y1);
				for(int j=y1; j<=y2; j++)
				{
					Vec2f scanLineData;
					
					scanLineData.x = -MathExt::sqrt( radSqr - MathExt::sqr(j-nCY+0.5) ) + nCX;
					scanLineData.y = MathExt::sqrt( radSqr - MathExt::sqr(j-nCY+0.5) ) + nCX;

					AAscanLines[i] = scanLineData;
					i++;
				}

				int newY1 = y1/AAadjustment;
				int newY2 = y2/AAadjustment;

				i=0;
				double percentageInc = 1.0/MathExt::sqr(AAadjustment);

				for(int j=newY1; j<=newY2; j++)
				{
					std::vector<double> scanLinePercentages = std::vector<double>(maxXDis/AAadjustment);

					for(int k=0; k<AAadjustment; k++)
					{
						double startX = AAscanLines[i+k].x;
						double endX = AAscanLines[i+k].y;

						for(int tempX = (int)MathExt::round(startX); tempX < (int)MathExt::round(endX); tempX++)
						{
							int location = tempX / AAadjustment;
							scanLinePercentages[location] += percentageInc;
						}
					}

					for(int tX = 0; tX<scanLinePercentages.size(); tX++)
					{
						if(scanLinePercentages[tX]>0)
						{
							double tempPercentage = MathExt::clamp(scanLinePercentages[tX], 0.0, 1.0);
							Color col = getFillColor();
							col.alpha = (unsigned char)(tempPercentage*col.alpha);
							
							buffer->drawPixel(tX, j, col);
						}
					}
					
					i+=AAadjustment;
				}

			#pragma endregion
		}
	}

	void VectorCircle::drawStroke(Image* buffer, int globalWidth, int globalHeight)
	{
		if(Mat3f::getIdentity() != getTransform())
		{
			//A interesting note for transforms is that it does not have to
			//change into a path. An Ellipse could also be used in some situations
			//and translation does not require a change. Skew, and in some situations,
			//rotation and scale require a change
			
			//Note that this function draws a path version of the shape
			drawStrokeTransformed(buffer, globalWidth, globalHeight);
			return;
		}

		//first, calc bounding box
		double tempRad = radius + MathExt::abs(getStrokeWidth()/2);
		double tempRad2 = radius - MathExt::abs(getStrokeWidth()/2);
		
		int x1 = (int)MathExt::round(cx-tempRad);
		int x2 = (int)MathExt::round(cx+tempRad);
		int y1 = (int)MathExt::round(cy-tempRad);
		int y2 = (int)MathExt::round(cy+tempRad);

		x1 = MathExt::clamp(x1, 0, buffer->getWidth());
		y1 = MathExt::clamp(y1, 0, buffer->getHeight());
		x2 = MathExt::clamp(x2, 0, buffer->getWidth());
		y2 = MathExt::clamp(y2, 0, buffer->getHeight());

		double radSqr = MathExt::sqr(tempRad);
		double radSqr2 = MathExt::sqr(tempRad2);

		for(int j=y1; j<=y2; j++)
		{
			double startX, endX;
			double startX2, endX2;

			double sqrtVar1, sqrtVar2;

			sqrtVar1 = radSqr - MathExt::sqr(j-cy+0.5);
			sqrtVar2 = radSqr2 - MathExt::sqr(j-cy+0.5);

			startX = -MathExt::sqrt( sqrtVar1 ) + cx;
			endX = MathExt::sqrt( sqrtVar1 ) + cx;

			startX2 = -MathExt::sqrt( sqrtVar2 ) + cx;
			endX2 = MathExt::sqrt( sqrtVar2 ) + cx;

			if( sqrtVar1 >= 0 && sqrtVar2 >= 0)
			{
				//Fill from startX to startX2.
				if(startX>startX2)
				{
					double temp = startX2;
					startX2 = startX;
					startX = temp;
				}

				for(int i=(int)MathExt::round(startX); i<=(int)MathExt::round(startX2); i++)
				{
					buffer->drawPixel(i, j, getStrokeColor());
				}

				//Fill from endX to endX2.
				if(endX>endX2)
				{
					double temp = endX2;
					endX2 = endX;
					endX = temp;
				}

				for(int i=(int)MathExt::round(endX)-1; i<(int)MathExt::round(endX2); i++)
				{
					buffer->drawPixel(i, j, getStrokeColor());
				}
			}
			else
			{
				if(startX>endX)
				{
					double temp = endX;
					endX = startX;
					startX = temp;
				}

				for(int i=(int)MathExt::round(startX)-1; i<=(int)MathExt::round(endX); i++)
				{
					buffer->drawPixel(i, j, getStrokeColor());
				}
			}

		}
	}

	void VectorCircle::setX(double x)
	{
		this->cx = x;
	}

	double VectorCircle::getX()
	{
		return cx;
	}

	void VectorCircle::setY(double y)
	{
		this->cy = y;
	}

	double VectorCircle::getY()
	{
		return cy;
	}

	void VectorCircle::setRadius(double r)
	{
		this->radius = r;
	}

	double VectorCircle::getRadius()
	{
		return this->radius;
	}

	void VectorCircle::applyTransform()
	{
		//Do nothing. Object transformed to VectorPath then drawn.
	}

	void VectorCircle::drawTransformed(Image* buffer, int globalWidth, int globalHeight)
	{
		//convert to path then transform
		VectorPath p = VectorPath();

		p.setFillColor(getFillColor());
		p.setFillMethod(getFillMethod());
		p.setStrokeWidth(getStrokeWidth());
		p.setStrokeColor(getStrokeColor());
		p.setTransform(getTransform());
		p.setLineCap(getLineCap());
		p.setLineJoin(getLineJoin());

		//
		p.addMoveTo(cx-radius, cy);
		p.addArcTo(radius, radius, 0, false, false, cx+radius, cy);
		p.addArcTo(radius, radius, 0, false, false, cx-radius, cy);
		p.addClosePath();

		p.draw(buffer, globalWidth, globalHeight);
	}

	void VectorCircle::drawStrokeTransformed(Image* buffer, int globalWidth, int globalHeight)
	{
		//convert to path then transform
		VectorPath p = VectorPath();

		p.setFillColor(getFillColor());
		p.setFillMethod(getFillMethod());
		p.setStrokeWidth(getStrokeWidth());
		p.setStrokeColor(getStrokeColor());
		p.setTransform(getTransform());
		p.setLineCap(getLineCap());
		p.setLineJoin(getLineJoin());

		//
		p.addMoveTo(cx-radius, cy);
		p.addArcTo(radius, radius, 0, false, false, cx+radius, cy);
		p.addArcTo(radius, radius, 0, false, false, cx-radius, cy);
		p.addClosePath();

		p.drawStroke(buffer, globalWidth, globalHeight);
	}

	#pragma endregion

} //NAMESPACE glib END