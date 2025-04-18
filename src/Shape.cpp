#include "Shape.h"
#include "StringTools.h"

namespace smpl
{
	
	#pragma region SHAPE

	Shape::Shape()
	{
	}

	Shape::~Shape()
	{
	}

	void Shape::setPosition(Vec3f posPointer)
	{
		position = posPointer;
		onTransformChanged();
	}

	Vec3f Shape::getPosition()
	{
		return position;
	}

	void Shape::setScale(Vec3f posScale)
	{
		scale = posScale;
		onTransformChanged();
	}

	Vec3f Shape::getScale()
	{
		return scale;
	}

	void Shape::setRotation(Vec3f posRotation)
	{
		rotation = posRotation;
		onTransformChanged();
	}

	Vec3f Shape::getRotation()
	{
		return rotation;
	}

	void Shape::onTransformChanged()
	{
		
	}

	float Shape::generateBoundingRadius()
	{
		return 0;
	}

	#pragma endregion

	#pragma region POINT2D

	Point2D::Point2D()
	{
	}

	Point2D::Point2D(float x, float y)
	{
		position.x = x;
		position.y = y;
	}
	Point2D::Point2D(Vec2f pos)
	{
		position.x = pos.x;
		position.y = pos.y;
	}

	Point2D::~Point2D()
	{
	}

	float Point2D::generateBoundingRadius()
	{
		return 0;
	}

	void Point2D::onTransformChanged()
	{

	}

	#pragma endregion

	#pragma region Box2D

	Box2D::Box2D()
	{
	}

	Box2D::Box2D(float leftBound, float topBound, float rightBound, float bottomBound)
	{
		baseTopLeft.x = leftBound;
		baseTopLeft.y = topBound;
		baseBottomRight.x = rightBound;
		baseBottomRight.y = bottomBound;

		topLeft = baseTopLeft;
		bottomRight = baseBottomRight;
	}

	Box2D::~Box2D()
	{
	}

	void Box2D::setLeftBound(float lb)
	{
		baseTopLeft.x = lb;
		onTransformChanged();
	}

	float Box2D::getLeftBound()
	{
		return topLeft.x;
	}

	void Box2D::setTopBound(float tb)
	{
		baseTopLeft.y = tb;
		onTransformChanged();
	}

	float Box2D::getTopBound()
	{
		return topLeft.y;
	}

	void Box2D::setRightBound(float rb)
	{
		baseBottomRight.x = rb;
		onTransformChanged();
	}

	float Box2D::getRightBound()
	{
		return bottomRight.x;
	}

	void Box2D::setBottomBound(float bb)
	{
		baseBottomRight.y = bb;
		onTransformChanged();
	}

	float Box2D::getBottomBound()
	{
		return bottomRight.y;
	}
	
	float Box2D::getWidth()
	{
		return bottomRight.x - topLeft.x;
	}
	
	float Box2D::getHeight()
	{
		return bottomRight.y - topLeft.y;
	}

	void Box2D::onTransformChanged()
	{
		topLeft.x = baseTopLeft.x * getScale().x;
		topLeft.y = baseTopLeft.y * getScale().y;
		bottomRight.x = baseBottomRight.x * getScale().x;
		bottomRight.y = baseBottomRight.y * getScale().y;

		topLeft += getPosition().toVec2f();
		bottomRight += getPosition().toVec2f();
	}	

	float Box2D::generateBoundingRadius()
	{
		Vec2f midPoint = (topLeft + bottomRight)/2.0;
		Vec2f toTopLeft = topLeft - midPoint;
		
		return toTopLeft.getLength();
	}

	
	bool Box2D::operator==(Box2D other)
	{
		return (topLeft == other.topLeft && bottomRight == other.bottomRight);
	}

	bool Box2D::operator!=(Box2D other)
	{
		return !operator==(other);
	}

	#pragma endregion

	#pragma region CIRCLE

	Circle::Circle()
	{
	}

	Circle::Circle(float rad)
	{
		radius = rad;
		baseRadius = rad;
	}

	Circle::~Circle()
	{
	}

	float Circle::getRadius()
	{
		return radius;
	}

	void Circle::setRadius(float rad)
	{
		baseRadius = rad;
		onTransformChanged();
	}

	void Circle::onTransformChanged()
	{
		radius = baseRadius*getScale().x;
	}

	float Circle::generateBoundingRadius()
	{
		return radius;
	}

	#pragma endregion

	#pragma region ELLIPSE

	Ellipse::Ellipse()
	{
	}

	Ellipse::Ellipse(float xRad, float yRad)
	{
		xRadius = xRad;
		yRadius = yRad;

		baseXRadius = xRad;
		baseYRadius = yRad;
	}

	Ellipse::~Ellipse()
	{

	}

	float Ellipse::getXRadius()
	{
		return xRadius;
	}

	void Ellipse::setXRadius(float rad)
	{
		baseXRadius = rad;
		onTransformChanged();
	}

	float Ellipse::getYRadius()
	{
		return yRadius;
	}

	void Ellipse::setYRadius(float rad)
	{
		baseYRadius = rad;
		onTransformChanged();
	}

	void Ellipse::onTransformChanged()
	{
		xRadius = baseXRadius*getScale().x;
		yRadius = baseYRadius*getScale().y;
	}

	float Ellipse::generateBoundingRadius()
	{
		return MathExt::max(xRadius, yRadius);
	}

	#pragma endregion

	#pragma region LINE2D

	Line2D::Line2D()
	{
	}

	Line2D::Line2D(float x1, float y1, float x2, float y2)
	{
		baseLine = Line(x1, y1, x2, y2);
		l = Line(x1, y1, x2, y2);
	}

	Line2D::Line2D(Vec2f p1, Vec2f p2)
	{
		baseLine = Line(p1, p2);
		l = Line(p1, p2);
	}

	Line2D::~Line2D()
	{
	}

	void Line2D::setPoint1(Vec2f p)
	{
		baseLine = Line(p, baseLine.getPoint2());
		onTransformChanged();
	}

	void Line2D::setPoint1(float x, float y)
	{
		baseLine = Line(Vec2f(x,y), baseLine.getPoint2());
		onTransformChanged();
	}

	void Line2D::setPoint2(Vec2f p)
	{
		baseLine = Line(baseLine.getPoint1(), p);
		onTransformChanged();
	}

	void Line2D::setPoint2(float x, float y)
	{
		baseLine = Line(baseLine.getPoint1(), Vec2f(x,y));
		onTransformChanged();
	}

	Vec2f Line2D::getPoint1()
	{
		return l.getPoint1();
	}

	Vec2f Line2D::getPoint2()
	{
		return l.getPoint2();
	}

	Line Line2D::getLine()
	{
		return l;
	}

	void Line2D::onTransformChanged()
	{
		Vec2f p1 = baseLine.getPoint1();
		Vec2f p2 = baseLine.getPoint2();

		//Move to origin
		Vec2f midPoint = (p1+p2)/2;

		p1 -= midPoint;
		p2 -= midPoint;

		//Scale
		p1.x *= getScale().x;
		p1.y *= getScale().y;

		p2.x *= getScale().x;
		p2.y *= getScale().y;

		//Rotate
		Mat3f rot = MathExt::rotation2D(getRotation().x);

		Vec3f rotatedP1 = rot*Vec3f(p1);
		Vec3f rotatedP2 = rot*Vec3f(p2);

		//Translate
		p1 = (rotatedP1 + getPosition()).toVec2f();
		p2 = (rotatedP2 + getPosition()).toVec2f();
		
		//Undo move to origin
		Vec2f nMidPoint = getPosition().toVec2f() + Vec2f( midPoint.x*getScale().x, midPoint.y*getScale().y );

		p1 += nMidPoint;
		p2 += nMidPoint;

		l = Line(p1, p2);
	}

	float Line2D::generateBoundingRadius()
	{
		Vec2f dir = l.getToPoint()/2;
		return dir.getLength();
	}

	#pragma endregion

	#pragma region TRIANGLE2D

	Triangle2D::Triangle2D()
	{
	}

	Triangle2D::Triangle2D(Vec2f p1, Vec2f p2, Vec2f p3)
	{
		v1 = p1;
		v2 = p2;
		v3 = p3;
	}

	Triangle2D::Triangle2D(float x1, float y1, float x2, float y2, float x3, float y3)
	{
		v1.x = x1;
		v1.y = y1;
		v2.x = x2;
		v2.y = y2;
		v3.x = x3;
		v3.y = y3;
	}

	Triangle2D::~Triangle2D()
	{
	}

	void Triangle2D::setVertex1(Vec2f p)
	{
		v1 = p;
	}

	void Triangle2D::setVertex1(float x, float y)
	{
		v1.x = x;
		v1.y = y;
	}

	void Triangle2D::setVertex2(Vec2f p)
	{
		v2 = p;
	}

	void Triangle2D::setVertex2(float x, float y)
	{
		v2.x = x;
		v2.y = y;
	}

	void Triangle2D::setVertex3(Vec2f p)
	{
		v3 = p;
	}

	void Triangle2D::setVertex3(float x, float y)
	{
		v3.x = x;
		v3.y = y;
	}

	Vec2f Triangle2D::getVertex1()
	{
		Vec2f nPos = Vec2f(getPosition().x, getPosition().y);
		return v1 + nPos;
	}

	Vec2f Triangle2D::getVertex2()
	{
		Vec2f nPos = Vec2f(getPosition().x, getPosition().y);
		return v2 + nPos;
	}

	Vec2f Triangle2D::getVertex3()
	{
		Vec2f nPos = Vec2f(getPosition().x, getPosition().y);
		return v3 + nPos;
	}

	Vec2f Triangle2D::getCenterPosition()
	{
		Vec2f nPos = Vec2f(getPosition().x, getPosition().y);
		Vec2f centerPos = (v1+v2+v3)/3;

		return centerPos+nPos;
	}

	void Triangle2D::onTransformChanged()
	{
		
	}

	float Triangle2D::generateBoundingRadius()
	{
		return 0;
	}

	#pragma endregion

	#pragma region TRIANGLE2D_MODEL

	Triangle2DModel::Triangle2DModel()
	{

	}
	Triangle2DModel::~Triangle2DModel()
	{

	}

	void Triangle2DModel::add(Triangle2D tri)
	{
		triangleList.push_back(tri);
	}
	Triangle2D Triangle2DModel::get(size_t index)
	{
		if(index < triangleList.size())
		{
			return triangleList[index];
		}
		return Triangle2D();
	}
	
	std::vector<Triangle2D>& Triangle2DModel::getTriangles()
	{
		return triangleList;
	}

	size_t Triangle2DModel::size()
	{
		return triangleList.size();
	}

	#pragma endregion

	#pragma region POLYGON2D

	Polygon2D::Polygon2D()
	{
	}

	Polygon2D::~Polygon2D()
	{
	}

	void Polygon2D::addVertex(Vec2f p)
	{
		centerPoint *= (int)points.size();
		centerPoint += p;
		points.push_back(p);
		centerPoint /= (int)points.size();

		checkConcave();
	}

	Vec2f Polygon2D::getVertex(size_t index)
	{
		if(index < points.size())
		{
			Vec2f finalPoint = points[index];
			finalPoint.x += getPosition().x;
			finalPoint.y += getPosition().y;
			return finalPoint;
		}
		return Vec2f();
	}

	Vec2f Polygon2D::getCenterPoint()
	{
		Vec2f finalPoint = centerPoint;
		finalPoint.x += getPosition().x;
		finalPoint.y += getPosition().y;
		return finalPoint;
	}

	void Polygon2D::clear()
	{
		points.clear();
	}

	size_t Polygon2D::size()
	{
		return points.size();
	}

	bool Polygon2D::getConvex()
	{
		return isConvex;
	}

	void Polygon2D::checkConcave()
	{
		if(points.size()>3)
		{
			float lastDirVal = MathExt::dirToPoint( points[1].x, points[1].y, points[2].x, points[2].y);
			int lastQuadrant = (int)(lastDirVal / (PI/2));
			bool dir = lastQuadrant <= 2; //going counter-clockwise

			for(size_t i=3; i<points.size()+1; i++)
			{
				Vec2f thisToVec;
				if(i != points.size())
					thisToVec = points[i] - points[i-1];
				else
					thisToVec = points[0] - points[i-1];
				
				float thisDirVal = MathExt::dirToPoint(0, 0, thisToVec.x, thisToVec.y);
				bool thisDir = false;

				if(dir)
				{
					float dirDis = 0;
					if(lastDirVal > thisDirVal)
					{
						dirDis = thisDirVal + (2*PI - lastDirVal);
					}
					else
					{
						dirDis = thisDirVal - lastDirVal;
					}

					thisDir = (dirDis < PI) ? dir : !dir;
				}
				else
				{
					float dirDis = 0;
					if(lastDirVal < thisDirVal)
					{
						dirDis = lastDirVal + (2*PI - thisDirVal);
					}
					else
					{
						dirDis = lastDirVal - thisDirVal;
					}

					thisDir = (dirDis < PI) ? dir : !dir;
				}

				lastDirVal = thisDirVal;
			}
		}
		else
		{
			isConvex = true;
		}
	}

	Polygon2D Polygon2D::approximateCircle(float radius, int n)
	{
		if(n <= 3)
			return Polygon2D();
		
		Polygon2D out;
		float du = 2*PI / n;
		for(int i=0; i<=n; i++)
		{
			float x = radius * MathExt::cos(du*i);
			float y = radius * MathExt::sin(du*i);

			out.addVertex( Vec2f(x,y) );
		}

		return out;
	}

	Polygon2D Polygon2D::approximateEllipse(float xRadius, float yRadius, int n)
	{
		if(n <= 3)
			return Polygon2D();
		
		Polygon2D out;
		float du = 2*PI / n;
		for(int i=0; i<=n; i++)
		{
			float x = xRadius * MathExt::cos(du*i);
			float y = yRadius * MathExt::sin(du*i);

			out.addVertex( Vec2f(x,y) );
		}

		return out;
	}

	Polygon2D Polygon2D::approximateArc(float xRadius, float yRadius, float startAngle, float endAngle, int n)
	{
		if(n <= 3)
			return Polygon2D();
		
		Polygon2D out;
		out.addVertex( Vec2f(0,0) );
		float du = (endAngle - startAngle) / n;
		for(int i=0; i<=n; i++)
		{
			float x = xRadius * MathExt::cos(startAngle + du*i);
			float y = yRadius * MathExt::sin(startAngle + du*i);

			out.addVertex( Vec2f(x,y) );
		}

		return out;
	}

	void Polygon2D::onTransformChanged()
	{
		
	}

	float Polygon2D::generateBoundingRadius()
	{
		return 0;
	}

	#pragma endregion

	#pragma region TRIANGLE_3D

	Triangle3D::Triangle3D()
	{

	}

	Triangle3D::Triangle3D(Vec3f p1, Vec3f p2, Vec3f p3)
	{
		v1 = p1;
		v2 = p2;
		v3 = p3;
	}

	Triangle3D::Triangle3D(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3)
	{
		v1 = Vec3f(x1,y1,z1);
		v2 = Vec3f(x2,y2,z2);
		v3 = Vec3f(x3,y3,z3);
	}

	Triangle3D::~Triangle3D()
	{

	}

	//Object and RootClass Stuff

	void Triangle3D::setVertex1(Vec3f p)
	{
		v1 = p;
	}

	void Triangle3D::setVertex1(float x, float y, float z)
	{
		v1 = Vec3f(x,y,z);
	}

	void Triangle3D::setVertex2(Vec3f p)
	{
		v2 = p;
	}

	void Triangle3D::setVertex2(float x, float y, float z)
	{
		v2 = Vec3f(x,y,z);
	}

	void Triangle3D::setVertex3(Vec3f p)
	{
		v3 = p;
	}

	void Triangle3D::setVertex3(float x, float y, float z)
	{
		v3 = Vec3f(x,y,z);
	}

	Vec3f Triangle3D::getVertex1()
	{
		return v1;
	}

	Vec3f Triangle3D::getVertex2()
	{
		return v2;
	}

	Vec3f Triangle3D::getVertex3()
	{
		return v3;
	}

	Vec3f Triangle3D::getCenterPosition()
	{
		return (v1+v2+v3)/3;
	}

	Vec3f Triangle3D::getNormal()
	{
		return v1.crossProduct(v2).normalize();
	}

	float Triangle3D::generateBoundingRadius()
	{
		Vec3f midP = getCenterPosition();
		float maxDis = (v1-midP).getLength();
		maxDis = MathExt::max( maxDis, (v2-midP).getLength() );
		maxDis = MathExt::max( maxDis, (v3-midP).getLength() );
		return maxDis;
	}

	void Triangle3D::onTransformChanged()
	{

	}

	#pragma endregion

	#pragma region TRIANGLE3D_MODEL

	Triangle3DModel::Triangle3DModel()
	{

	}
	Triangle3DModel::~Triangle3DModel()
	{

	}

	void Triangle3DModel::add(Triangle3D tri)
	{
		triangleList.push_back(tri);
	}
	Triangle3D Triangle3DModel::get(size_t index)
	{
		if(index < triangleList.size())
		{
			return triangleList[index];
		}
		return Triangle3D();
	}
	
	std::vector<Triangle3D>& Triangle3DModel::getTriangles()
	{
		return triangleList;
	}

	size_t Triangle3DModel::size()
	{
		return triangleList.size();
	}

	#pragma endregion

	#pragma region COLLISION_MASTER

	//Functions
	bool CollisionMaster::getCollision(Shape* a, Shape* b, bool overrideQuickCheck)
	{
		const TypeInfo aClass = a->getClass();
		const TypeInfo bClass = b->getClass();

		if(!overrideQuickCheck)
		{
			//check if the bounding radius collides before continuing
			float r1 = a->generateBoundingRadius();
			float r2 = b->generateBoundingRadius();
			Vec3f toB = b->getPosition() - a->getPosition();
			if(toB.getLength() > r1+r2)
			{
				//no collision unless radius values are incorrect
				return false;
			}
		}

		if (aClass == TypeInfo::get<Box2D>())
		{
			if (bClass == TypeInfo::get<Box2D>())
			{
				return CollisionMaster::collisionMethod((Box2D*)a, (Box2D*)b);
			}
			else if (bClass == TypeInfo::get<Point2D>())
			{
				return CollisionMaster::collisionMethod((Point2D*)b, (Box2D*)a);
			}
			else if (bClass == TypeInfo::get<Circle>())
			{
				return CollisionMaster::collisionMethod((Box2D*)a, (Circle*)b);
			}
			else if (bClass == TypeInfo::get<Ellipse>())
			{
				return CollisionMaster::collisionMethod((Box2D*)a, (Ellipse*)b);
			}
			else if (bClass == TypeInfo::get<Triangle2D>())
			{
				return CollisionMaster::collisionMethod((Box2D*)a, (Triangle2D*)b);
			}
			else if (bClass == TypeInfo::get<Line2D>())
			{
				return CollisionMaster::collisionMethod((Box2D*)a, (Line2D*)b);
			}

			//No matching function found
		}
		else if (aClass == TypeInfo::get<Point2D>())
		{
			if (bClass == TypeInfo::get<Box2D>())
			{
				return CollisionMaster::collisionMethod((Point2D*)a, (Box2D*)b);
			}
			else if (bClass == TypeInfo::get<Point2D>())
			{
				return CollisionMaster::collisionMethod((Point2D*)a, (Point2D*)b);
			}
			else if (bClass == TypeInfo::get<Circle>())
			{
				return CollisionMaster::collisionMethod((Point2D*)a, (Circle*)b);
			}
			else if (bClass == TypeInfo::get<Ellipse>())
			{
				return CollisionMaster::collisionMethod((Point2D*)a, (Ellipse*)b);
			}
			else if (bClass == TypeInfo::get<Triangle2D>())
			{
				return CollisionMaster::collisionMethod((Point2D*)a, (Triangle2D*)b);
			}
			else if (bClass == TypeInfo::get<Line2D>())
			{
				return CollisionMaster::collisionMethod((Point2D*)a, (Line2D*)b);
			}

			//No matching function found
		}
		else if (aClass == TypeInfo::get<Circle>())
		{
			if (bClass == TypeInfo::get<Box2D>())
			{
				return CollisionMaster::collisionMethod((Box2D*)b, (Circle*)a);
			}
			else if (bClass == TypeInfo::get<Point2D>())
			{
				return CollisionMaster::collisionMethod((Point2D*)b, (Circle*)a);
			}
			else if (bClass == TypeInfo::get<Circle>())
			{
				return CollisionMaster::collisionMethod((Circle*)a, (Circle*)b);
			}
			else if (bClass == TypeInfo::get<Ellipse>())
			{
				return CollisionMaster::collisionMethod((Circle*)a, (Ellipse*)b);
			}
			else if (bClass == TypeInfo::get<Triangle2D>())
			{
				return CollisionMaster::collisionMethod((Circle*)a, (Triangle2D*)b);
			}
			else if (bClass == TypeInfo::get<Line2D>())
			{
				return CollisionMaster::collisionMethod((Circle*)a, (Line2D*)b);
			}

			//No matching function found
		}
		else if (aClass == TypeInfo::get<Line2D>())
		{
			if (bClass == TypeInfo::get<Box2D>())
			{
				return CollisionMaster::collisionMethod((Box2D*)b, (Line2D*)a);
			}
			else if (bClass == TypeInfo::get<Point2D>())
			{
				return CollisionMaster::collisionMethod((Point2D*)b, (Line2D*)a);
			}
			else if (bClass == TypeInfo::get<Circle>())
			{
				return CollisionMaster::collisionMethod((Circle*)b, (Line2D*)a);
			}
			else if (bClass == TypeInfo::get<Ellipse>())
			{
				return CollisionMaster::collisionMethod((Line2D*)a, (Ellipse*)b);
			}
			else if (bClass == TypeInfo::get<Triangle2D>())
			{
				return CollisionMaster::collisionMethod((Line2D*)a, (Triangle2D*)b);
			}
			else if (bClass == TypeInfo::get<Line2D>())
			{
				return CollisionMaster::collisionMethod((Line2D*)a, (Line2D*)b);
			}

			//No matching function found
		}
		else if (aClass == TypeInfo::get<Triangle2D>())
		{
			if (bClass == TypeInfo::get<Box2D>())
			{
				return CollisionMaster::collisionMethod((Box2D*)b, (Triangle2D*)a);
			}
			else if (bClass == TypeInfo::get<Point2D>())
			{
				return CollisionMaster::collisionMethod((Point2D*)b, (Triangle2D*)a);
			}
			else if (bClass == TypeInfo::get<Circle>())
			{
				return CollisionMaster::collisionMethod((Circle*)b, (Triangle2D*)a);
			}
			else if (bClass == TypeInfo::get<Ellipse>())
			{
				return CollisionMaster::collisionMethod((Ellipse*)b, (Triangle2D*)a);
			}
			else if (bClass == TypeInfo::get<Triangle2D>())
			{
				return CollisionMaster::collisionMethod((Triangle2D*)a, (Triangle2D*)b);
			}
			else if (bClass == TypeInfo::get<Line2D>())
			{
				return CollisionMaster::collisionMethod((Line2D*)b, (Triangle2D*)a);
			}

			//No matching function found
		}
		else if (aClass == TypeInfo::get<Ellipse>())
		{
			if (bClass == TypeInfo::get<Box2D>())
			{
				return CollisionMaster::collisionMethod((Box2D*)b, (Ellipse*)a);
			}
			else if (bClass == TypeInfo::get<Point2D>())
			{
				return CollisionMaster::collisionMethod((Point2D*)b, (Ellipse*)a);
			}
			else if (bClass == TypeInfo::get<Circle>())
			{
				return CollisionMaster::collisionMethod((Circle*)b, (Ellipse*)a);
			}
			else if (bClass == TypeInfo::get<Ellipse>())
			{
				return CollisionMaster::collisionMethod((Ellipse*)a, (Ellipse*)b);
			}
			else if (bClass == TypeInfo::get<Triangle2D>())
			{
				return CollisionMaster::collisionMethod((Ellipse*)a, (Triangle2D*)b);
			}
			else if (bClass == TypeInfo::get<Line2D>())
			{
				return CollisionMaster::collisionMethod((Line2D*)b, (Triangle2D*)a);
			}

			//No matching function found
		}
		else
		{
			//No matching function found
		}

		return false;
	}

	#pragma region POINT_COLLISIONS

	bool CollisionMaster::collisionMethod(Point2D* a, Point2D* b)
	{
		if (a->getPosition().x == b->getPosition().x)
		{
			if (a->getPosition().y == b->getPosition().y)
			{
				return true;
			}
		}
		return false;
	}

	bool CollisionMaster::collisionMethod(Point2D* a, Circle* b)
	{
		float dX = MathExt::sqr(a->getPosition().x - b->getPosition().x);
		float dY = MathExt::sqr(a->getPosition().y - b->getPosition().y);

		if (dX + dY <= MathExt::sqr(b->getRadius()))
		{
			return true;
		}
		return false;
	}

	bool CollisionMaster::collisionMethod(Point2D* a, Ellipse* b)
	{
		float dX = MathExt::sqr(a->getPosition().x - b->getPosition().x);
		float dY = MathExt::sqr(a->getPosition().y - b->getPosition().y);

		if(b->getXRadius()!=0)
			dX /= MathExt::sqr(b->getXRadius());
		else
			return false;

		if(b->getYRadius()!=0)
			dY /= MathExt::sqr(b->getYRadius());
		else
			return false;
		
		if (dX + dY <= 1)
		{
			return true;
		}
		return false;
	}

	bool CollisionMaster::collisionMethod(Point2D* a, Box2D* b)
	{
		if (a->getPosition().x >= b->getLeftBound() &&
			a->getPosition().x <= b->getRightBound())
		{
			if (a->getPosition().y >= b->getTopBound() &&
				a->getPosition().y <= b->getBottomBound())
			{
				return true;
			}
		}
		return false;
	}

	bool CollisionMaster::collisionMethod(Point2D* a, Triangle2D* b)
	{
		Vec2f v1 = b->getVertex2() - b->getVertex1();
		Vec2f v2 = b->getVertex3() - b->getVertex1();

		Vec2f toPoint = Vec2f(a->getPosition().x - b->getVertex1().x, a->getPosition().y - b->getVertex1().y);
		//v1*v2 = v1*toPoint + v2*tPoint

		
		if (v1.dot(v2) == v1.dot(toPoint) + v2.dot(toPoint))
		{
			//Possible collision
			v1 = -v1;
			v2 = Vec2f(b->getVertex3() - b->getVertex2());

			toPoint = Vec2f(a->getPosition().x - b->getVertex2().x, a->getPosition().y - b->getVertex2().y);

			if (v1.dot(v2) == v1.dot(toPoint) + v2.dot(toPoint))
			{
				return true;
			}
		}

		return false;
	}

	bool CollisionMaster::collisionMethod(Point2D* a, Line2D* b)
	{
		Line bLine = b->getLine();

		if (bLine.getMinX() == bLine.getMaxX())
		{
			//Vertical line
			if (a->getPosition().x == bLine.getMaxX())
			{
				if (a->getPosition().y >= bLine.getMinY() &&
					a->getPosition().y <= bLine.getMaxY())
				{
					return true;
				}
			}
		}
		else
		{
			float yNeeded = bLine.solveForY( a->getPosition().x);

			if (yNeeded == a->getPosition().y)
			{
				return true;
			}
		}
		
		return false;
	}

	#pragma endregion POINT_COLLISIONS

	#pragma region BOX2D_COLLISIONS

	bool CollisionMaster::collisionMethod(Box2D* a, Box2D* b)
	{
		if (a->getLeftBound() <= b->getRightBound() && a->getRightBound() >= b->getLeftBound())
		{
			if (a->getTopBound() <= b->getBottomBound() && a->getBottomBound() >= b->getTopBound())
			{
				return true;
			}
		}

		if (b->getLeftBound() <= a->getRightBound() && b->getRightBound() >= a->getLeftBound())
		{
			if (b->getTopBound() <= a->getBottomBound() && b->getBottomBound() >= a->getTopBound())
			{
				return true;
			}
		}
		return false;
	}

	bool CollisionMaster::collisionMethod(Box2D* a, Circle* b)
	{
		float curX = MathExt::clamp(b->getPosition().x, a->getLeftBound(), a->getRightBound());
		float curY = MathExt::clamp(b->getPosition().y, a->getTopBound(), a->getBottomBound());

		float length = MathExt::sqr(curX - b->getPosition().x) + MathExt::sqr(curY - b->getPosition().y);
		
		if (length <= MathExt::sqr(b->getRadius()))
		{
			return true;
		}
		return false;
	}

	bool CollisionMaster::collisionMethod(Box2D* a, Ellipse* b)
	{
		float curX = MathExt::clamp(b->getPosition().x, a->getLeftBound(), a->getRightBound());
		float curY = MathExt::clamp(b->getPosition().y, a->getTopBound(), a->getBottomBound());

		Point2D tempPoint(Vec2f(curX, curY));

		return CollisionMaster::collisionMethod( &tempPoint, b);
	}

	bool CollisionMaster::collisionMethod(Box2D* a, Triangle2D* b)
	{
		Line triLine1 = Line( b->getVertex1(), b->getVertex2());
		Line triLine2 = Line( b->getVertex2(), b->getVertex3());
		Line triLine3 = Line( b->getVertex3(), b->getVertex1());

		Line boxLines[4];
		
		boxLines[0] = Line( a->getLeftBound(), a->getTopBound(), a->getLeftBound(), a->getBottomBound());
		boxLines[1] = Line( a->getRightBound(), a->getTopBound(), a->getRightBound(), a->getBottomBound());
		boxLines[2] = Line( a->getLeftBound(), a->getTopBound(), a->getRightBound(), a->getTopBound());
		boxLines[3] = Line( a->getLeftBound(), a->getBottomBound(), a->getRightBound(), a->getBottomBound());
		
		for(int i=0; i<4; i++)
		{
			Vec2f pos1, pos2, pos3;
			bool valid1 = triLine1.getIntersection(boxLines[i], pos1);
			bool valid2 = triLine2.getIntersection(boxLines[i], pos2);
			bool valid3 = triLine3.getIntersection(boxLines[i], pos3);

			if(pos1.x >= a->getLeftBound() && pos1.x <= a->getRightBound())
				if(pos1.y >= a->getTopBound() && pos1.y <= a->getBottomBound())
					return true;

			if(pos2.x >= a->getLeftBound() && pos2.x <= a->getRightBound())
				if(pos2.y >= a->getTopBound() && pos2.y <= a->getBottomBound())
					return true;

			if(pos3.x >= a->getLeftBound() && pos3.x <= a->getRightBound())
				if(pos3.y >= a->getTopBound() && pos3.y <= a->getBottomBound())
					return true;
		}
		
		return false;
	}

	bool CollisionMaster::collisionMethod(Box2D* a, Line2D* b)
	{
		Line bLine = b->getLine();
		if (bLine.getMinX() == bLine.getMaxX())
		{
			//Vertical line
			if (bLine.getMinX() >= a->getLeftBound() &&
				bLine.getMinX() <= a->getRightBound())
			{
				if (bLine.getMinY() <= a->getBottomBound() &&
					bLine.getMaxY() >= a->getTopBound())
				{
					return true;
				}
			}
		}
		else
		{
			float yNeeded1 = bLine.solveForY(a->getLeftBound());
			float yNeeded2 = bLine.solveForY(a->getRightBound());

			float maY = MathExt::max(yNeeded1, yNeeded2);
			float miY = MathExt::min(yNeeded1, yNeeded2);

			maY = MathExt::clamp<float>(maY, bLine.getMinY(), bLine.getMaxY());
			miY = MathExt::clamp<float>(miY, bLine.getMinY(), bLine.getMaxY());
			
			float maX = MathExt::clamp<float>(a->getLeftBound(), bLine.getMinX(), bLine.getMaxX());
			float miX = MathExt::clamp<float>(a->getRightBound(), bLine.getMinX(), bLine.getMaxX());

			if (a->getLeftBound() <= maX && a->getRightBound() >= miX)
			{
				if (a->getTopBound() <= maY && a->getBottomBound() >= miY)
				{
					return true;
				}
			}

			if (miX <= a->getRightBound() && maX >= a->getLeftBound())
			{
				if (miY <= a->getBottomBound() && maY >= a->getTopBound())
				{
					return true;
				}
			}
		}

		return false;
	}

	#pragma endregion BOX2D_COLLISIONS

	#pragma region CIRCLE_COLLISIONS

	bool CollisionMaster::collisionMethod(Circle* a, Circle* b)
	{
		float disX = MathExt::sqr(a->getPosition().x - b->getPosition().x);
		float disY = MathExt::sqr(a->getPosition().y - b->getPosition().y);
		float len1 = disX + disY;

		float len2 = MathExt::sqr(a->getRadius()) + MathExt::sqr(b->getRadius());

		if (len1 <= len2)
		{
			return true;
		}
		return false;
	}

	bool CollisionMaster::collisionMethod(Circle* a, Ellipse* b)
	{
		//Convert ellipse to 2 separate circles using both its radius values.
		//If the circle collides with both, a collision has occured.
		//If the circle collides with one, possible collision
		//If the circle collides with none, no collision

		Vec3f toVec = a->getPosition() - b->getPosition();
		
		toVec.z = 0;

		float dis = toVec.getLength();
		int colCount = 0;

		if(dis < b->getXRadius()+a->getRadius())
			colCount++;

		if(dis < b->getYRadius()+a->getRadius())
			colCount++;

		if(colCount == 0)
			return false;
		if(colCount == 2)
			return true;
		
		if(colCount == 1)
		{
			//possible collision.
			//Find closest point within reason.
			//actual closest point is described by this: -sin(t)*(sqr(ry) - sqr(rx)) = x*rx*tan(t) - y*ry
			GeneralMathFunction f = GeneralMathFunction();
			float rx = b->getXRadius();
			float ry = b->getYRadius();
			float x = toVec.x;
			float y = toVec.y;

			f.setFunction( [rx, ry, x, y](float t) ->float{
				float val = -MathExt::sin(t)*(MathExt::sqr(ry) - MathExt::sqr(rx));
				val +=  y*ry;
				val -= (x*rx*MathExt::tan(t));

				return val;
			});

			//reduce [0, 2*PI] to a smaller range of values
			float minRange, maxRange;
			float offset = 0.000001;
			Vec2f circlePos = Vec2f(a->getPosition().x, a->getPosition().y);
			Vec2f ellipsePos = Vec2f(b->getPosition().x, b->getPosition().y);

			//NOTE: Adjustments need to be made as to not land on an invalid point.
			//tan(PI/2) is undefined. This works due to PI being defined to a limited set of numbers.
			//If PI is defined with more numbers, this can fail.

			if(toVec.x >= 0)
			{
				if(toVec.y >= 0)
				{
					//quad1
					minRange = 0;
					maxRange = PI/2 - offset;
				}
				else
				{
					//quad4
					minRange = 3*PI/2 + offset;
					maxRange = 2*PI;
				}
			}
			else
			{
				if(toVec.y >= 0)
				{
					//quad2
					minRange = PI/2 + offset;
					maxRange = PI;
				}
				else
				{
					//quad3
					minRange = PI;
					maxRange = 3*PI/2 - offset;
				}
			}
			
			minRange = MathExt::roundToDecimal(minRange, 6);
			maxRange = MathExt::roundToDecimal(maxRange, 6);

			//Close approximation with only 10 steps. Equation is complex though and
			//is being called through a pointer. Possibly implement here instead.

			float angleValue = MathExt::bisectionMethod(&f, minRange, maxRange, 10);
			
			Vec2f closestPoint = ellipsePos + Vec2f(rx*MathExt::cos(angleValue),
												ry*MathExt::sin(angleValue));

			if(MathExt::distanceTo(closestPoint, circlePos) <= a->getRadius())
			{
				return true;
			}

			//possibly the top most point or the bottom most point
			closestPoint = ellipsePos + Vec2f(rx*MathExt::cos(PI/2),
									ry*MathExt::sin(PI/2));

			if(MathExt::distanceTo(closestPoint, circlePos) <= a->getRadius())
			{
				return true;
			}

			closestPoint = ellipsePos + Vec2f(rx*MathExt::cos(3*PI/2),
						ry*MathExt::sin(3*PI/2));

			if(MathExt::distanceTo(closestPoint, circlePos) <= a->getRadius())
			{
				return true;
			}
		}

		return false;
	}

	bool CollisionMaster::collisionMethod(Circle* a, Triangle2D* b)
	{
		//Find the closest point to the circle on each line segment in the triangle
		//Will not determine if a is inside of b.
		//Check if circle's center is in b to do that

		Line l1 = Line(b->getVertex1(), b->getVertex2());
		Line l2 = Line(b->getVertex2(), b->getVertex3());
		Line l3 = Line(b->getVertex3(), b->getVertex1());

		Line lineArr[3] = {l1, l2, l3};

		Vec2f circlePos = a->getPosition().toVec2f();

		//check if circle collides with lines in the triangle
		for(int i=0; i<3; i++)
		{
			float A = lineArr[i].getToPoint().x;
			float B = lineArr[i].getToPoint().y;
			float t;
			if(A != 0 && B != 0)
			{
				t = -(A*(lineArr[i].getPoint1().x - circlePos.x) + B*(lineArr[i].getPoint1().y - circlePos.y)) / (MathExt::sqr(A) + MathExt::sqr(B));

				t = MathExt::clamp<float>(t, 0.0, 1.0);

				Vec2f nPoint = lineArr[i].getPoint1() + lineArr[i].getToPoint()*t;

				if( (circlePos - nPoint).getLength() <= a->getRadius() )
				{
					return true;
				}
			}
			else
			{
				if(circlePos == lineArr[i].getPoint1())
				{
					return true;
				}
			}
		}

		//check if circle is inside triangle
		Point2D p = Point2D(circlePos);
		return collisionMethod(&p, b);
	}

	bool CollisionMaster::collisionMethod(Circle* a, Line2D* b)
	{
		//Why this method works and why use this method.
		/*
			This method finds the point on the line with the minimum distance
			to some other point. That point may not be in our line segment though.
			We clamp it back to the range of possible values. When we clamp it, that
			point is the closest point in the line segment. This is due to the line
			being linear. We can safely say that an x or y value closer to the point
			we desire is the best we can get.

			As for why we chose this method, We don't care about the points of
			intersection as much as whether there was a collision at all. This method 
			also handles the offset of the circle very well. For physics applications,
			this version provides us with the point that will have collided first which
			informs us on where to apply the force.
		*/

		Line bLine = b->getLine();
		if (bLine.getMaxX() == bLine.getMinX())
		{
			//vertical line
			//similar to the x version
			// y = (py + slope*px - slope*b) / (1+slope*slope)

			//find the point with the minimum distance
			float y = (a->getPosition().y + bLine.getSlopeRelativeY() * (a->getPosition().x - bLine.getXInt())) / (1 + MathExt::sqr(bLine.getSlopeRelativeY()));

			//clamp it down to the max or min possible x value then solve for y
			y = MathExt::clamp<float>(y, bLine.getMinY(), bLine.getMaxY());
			float x = y * bLine.getSlopeRelativeY() + bLine.getXInt();

			//now solve distance
			float disX = MathExt::sqr(a->getPosition().x - x);
			float disY = MathExt::sqr(a->getPosition().y - y);

			if (disX + disY <= MathExt::sqr(a->getRadius()))
			{
				//There is a collision
				return true;
			}
		}
		else
		{
			// 0 = -2*(px - x) + -2*slope*(py - slope*x - b)
			// 0 = -2px + 2x + 2*slope*slope*x -2*slope*py + 2*slope*b
			// 2px + 2*slope*py - 2*slope*b = 2*x*(1+slope*slope)
			// px + slope*py - slope*b = x*(1+slope*slope)
			// x = (px + slope*py - slope*b) / (1 + slope * slope)

			//find the point with the minimum distance
			float x = (a->getPosition().x + bLine.getSlope() * (a->getPosition().y - bLine.getYInt())) / (1 + MathExt::sqr(bLine.getSlope()));

			//clamp it down to the max or min possible x value then solve for y
			x = MathExt::clamp<float>(x, bLine.getMinX(), bLine.getMaxX());
			float y = x * bLine.getSlope() + bLine.getYInt();

			//now solve distance
			float disX = MathExt::sqr(a->getPosition().x - x);
			float disY = MathExt::sqr(a->getPosition().y - y);

			if (disX + disY <= MathExt::sqr(a->getRadius()))
			{
				//There is a collision
				return true;
			}
		}
		return false;
	}

	#pragma endregion CIRCLE_COLLISIONS

	#pragma region LINE2D_COLLISIONS

	bool CollisionMaster::collisionMethod(Line2D* a, Line2D* b)
	{
		//We want to look for the point of intersection

		Line l1 = Line(a->getPoint1(), a->getPoint2());
		Line l2 = Line(b->getPoint1(), b->getPoint2());

		Vec2f pointOfCol;
		bool couldCollide = l1.getIntersection(l2, pointOfCol);
		if(!couldCollide)
			return false;
		
		if(pointOfCol.x < l1.getMinX() || pointOfCol.x > l1.getMaxX())
		{
			return false;
		}

		if(pointOfCol.x < l2.getMinX() || pointOfCol.x > l2.getMaxX())
		{
			return false;
		}

		if(pointOfCol.y < l1.getMinY() || pointOfCol.y > l1.getMaxY())
		{
			return false;
		}

		if(pointOfCol.y < l2.getMinY() || pointOfCol.y > l2.getMaxY())
		{
			return false;
		}
		
		return true;
	}

	bool CollisionMaster::collisionMethod(Line2D* a, Ellipse* b)
	{
		//Can't determine if line is in ellipse
		
		if(a->getPoint1().x == a->getPoint2().x)
		{
			//vertical line, do quicker method
			float xRadSqr = MathExt::sqr(b->getXRadius());
			float yRadSqr = MathExt::sqr(b->getYRadius());

			float H = b->getPosition().x;
			float K = b->getPosition().y;

			float xCheck = a->getPoint1().x;

			float v1 = (xCheck-H);
			// float v2 = 0;
			
			float A = (1.0 / yRadSqr);
			// float B = 0;
			float C = (MathExt::sqr(v1)/xRadSqr) - 1;

			float polyA = A;
			float polyB = 2*(-A*K);
			float polyC = A*MathExt::sqr(K) + C;

			std::vector<double> solutions = MathExt::solveQuadraticReal(polyA, polyB, polyC);

			if(solutions.size()>0)
			{
				//possible collision
				for(size_t i=0; i<solutions.size(); i++)
				{
					Line l = a->getLine();
					if( solutions[i] >= l.getMinY() && solutions[i] <= l.getMaxY())
					{
						return true;
					}
				}
			}
			else
			{
				//no collision
				return false;
			}
		}
		else if(a->getPoint1().y == a->getPoint2().y)
		{
			//horizontal line, do quicker method
			float xRadSqr = MathExt::sqr(b->getXRadius());
			float yRadSqr = MathExt::sqr(b->getYRadius());

			// float rot = b->getRotation().x;

			// float H = b->getPosition().x;
			float K = b->getPosition().y;

			float yCheck = a->getPoint1().y;

			// float v1 = 0; //Not used
			float v2 = (yCheck-K);
			
			float A = (1.0 / xRadSqr);
			// float B = 0; //Not used
			float C = (MathExt::sqr(v2)/yRadSqr) - 1;

			float polyA = A;
			float polyB = 2*(-A*K);
			float polyC = A*MathExt::sqr(K) + C;

			std::vector<double> solutions = MathExt::solveQuadraticReal(polyA, polyB, polyC);

			if(solutions.size()>0)
			{
				//possible collision
				for(size_t i=0; i<solutions.size(); i++)
				{
					Line l = a->getLine();
					if( solutions[i] >= l.getMinX() && solutions[i] <= l.getMaxX())
					{
						return true;
					}
				}
			}
			else
			{
				//no collision
				return false;
			}
		}
		else
		{
			//Convert line to general equation
			//Assume no rotation currently but factor it in
			
			float A = a->getPoint2().y - a->getPoint1().y;
			float B = a->getPoint1().x - a->getPoint2().x;
			float C = a->getPoint2().x*a->getPoint1().y - a->getPoint2().y*a->getPoint1().x;

			float xRadSqr = MathExt::sqr(b->getXRadius());
			float yRadSqr = MathExt::sqr(b->getYRadius());

			// float rot = b->getRotation().x;

			float H = b->getPosition().x;
			float K = b->getPosition().y;

			float V1 = -( (C/A) + H );
			float V2 = -(B/A);
			float V3 = (-K);
			float V4 = 1.0;

			float polyA = (MathExt::sqr(V2) / xRadSqr) + (MathExt::sqr(V4) / yRadSqr);
			float polyB = 2*((V1*V2 / xRadSqr) + (V3*V4 / yRadSqr));
			float polyC = (MathExt::sqr(V1) / xRadSqr) + (MathExt::sqr(V3) / yRadSqr) - 1;

			std::vector<double> solutions = MathExt::solveQuadraticReal(polyA, polyB, polyC);
			
			if(solutions.size()>0)
			{
				//possible collision
				for(size_t i=0; i<solutions.size(); i++)
				{
					Line l = a->getLine();
					if( solutions[i] >= l.getMinY() && solutions[i] <= l.getMaxY())
					{
						return true;
					}
				}
			}
			else
			{
				//no collision
				return false;
			}

		}
		
		return false;
	}

	bool CollisionMaster::collisionMethod(Line2D* a, Triangle2D* b)
	{
		//check for line intersection with any line in Triangle2D
		Line l = a->getLine();

		Line tl1 = Line(b->getVertex1(), b->getVertex2());
		Line tl2 = Line(b->getVertex2(), b->getVertex3());
		Line tl3 = Line(b->getVertex3(), b->getVertex1());
		
		Vec2f posColPoint1, posColPoint2, posColPoint3;
		bool couldCol1 = l.getIntersection(tl1, posColPoint1);
		bool couldCol2 = l.getIntersection(tl2, posColPoint2);
		bool couldCol3 = l.getIntersection(tl3, posColPoint3);

		if(couldCol1)
			if(posColPoint1.x >= l.getMinX() && posColPoint1.x <= l.getMaxX())
				if(posColPoint1.y >= l.getMinY() && posColPoint1.y <= l.getMaxY())
					return true;

		if(couldCol2)
			if(posColPoint2.x >= l.getMinX() && posColPoint2.x <= l.getMaxX())
				if(posColPoint2.y >= l.getMinY() && posColPoint2.y <= l.getMaxY())
					return true;

		if(couldCol3)
			if(posColPoint3.x >= l.getMinX() && posColPoint3.x <= l.getMaxX())
				if(posColPoint3.y >= l.getMinY() && posColPoint3.y <= l.getMaxY())
					return true;
		
		//potentially inside triangle
		Point2D p = Point2D(a->getPoint1());
		bool col = collisionMethod(&p, b);

		return col;
	}

	#pragma endregion

	#pragma region ELLIPSE_COLLISIONS

	bool CollisionMaster::collisionMethod(Ellipse* a, Ellipse* b)
	{
		//Option 1: Convert to bezier curves and check collision
		//Option 2: Convert to polygon and check collision

		//Using option 1 cause it will be more accurate but probably slower
		std::vector<BezierCurve> aCurve = BezierCurve::approximateEllipse(a->getXRadius(), a->getYRadius(), a->getPosition().x, a->getPosition().y, true);
		std::vector<BezierCurve> bCurve = BezierCurve::approximateEllipse(b->getXRadius(), b->getYRadius(), b->getPosition().x, b->getPosition().y, true);

		for(size_t i=0; i<aCurve.size(); i++)
		{
			for(size_t i2=0; i2<bCurve.size(); i2++)
			{
				if(bezierCurveCollision(aCurve[i], bCurve[i2]))
				{
					return true;
				}
			}
		}
		return false;
	}

	bool CollisionMaster::collisionMethod(Ellipse* a, Triangle2D* b)
	{
		//3 line collisions
		//2 point collision
		Line2D lineArr[3] = { Line2D(b->getVertex1(), b->getVertex2()), Line2D(b->getVertex2(), b->getVertex3()), Line2D(b->getVertex3(), b->getVertex1())};
		for(int i=0; i<3; i++)
		{
			if( collisionMethod(&lineArr[i], a) )
			{
				return true;
			}
		}

		//check if triangle is inside ellipse
		Point2D p = Point2D( b->getVertex1() );
		if(collisionMethod(&p, a))
			return true;

		//check if ellipse is inside triangle
		p = Point2D( a->getPosition().toVec2f() );
		return collisionMethod( &p, b );
	}

	#pragma endregion

	#pragma region TRIANGLE2D_COLLISIONS

	bool CollisionMaster::collisionMethod(Triangle2D* a, Triangle2D* b)
	{
		//Barycentric Coordinates
		//w1*P1x + w2*P2x + w3*P3x = Px
		//w1*P1y + w2*P2y + w3*P3y = Py
		//w1+w2+w3=1
		
		Vec2f p1, p2, p3;
		float det,w1,w2,w3;
		p1 = a->getVertex1();
		p2 = a->getVertex2();
		p3 = a->getVertex3();
		det = (p2.y - p3.y)*(p1.x - p3.x) + (p3.x - p2.x)*(p1.y - p3.y);

		Vec2f point[3] = {b->getVertex1(), b->getVertex2(), b->getVertex3()};
		
		for(int i=0; i<3; i++)
		{
			w1 = ((p2.y - p3.y)*(point[i].x - p3.x) + (p3.x - p2.x)*(point[i].y - p3.y)) / det;
			w2 = ((p3.y - p1.y)*(point[i].x - p3.x) + (p1.x - p3.x)*(point[i].y - p3.y)) / det;
			w3 = 1.0-w1-w2;

			if(w1 >= 0 && w2 >= 0 && w3 >= 0)
			{
				//B collides with A
				return true;
			}
		}

		//Check from other side
		p1 = b->getVertex1();
		p2 = b->getVertex2();
		p3 = b->getVertex3();
		det = (p2.y - p3.y)*(p1.x - p3.x) + (p3.x - p2.x)*(p1.y - p3.y);

		point[0] = a->getVertex1();
		point[1] = a->getVertex2();
		point[2] = a->getVertex3();
		
		for(int i=0; i<3; i++)
		{
			w1 = ((p2.y - p3.y)*(point[i].x - p3.x) + (p3.x - p2.x)*(point[i].y - p3.y)) / det;
			w2 = ((p3.y - p1.y)*(point[i].x - p3.x) + (p1.x - p3.x)*(point[i].y - p3.y)) / det;
			w3 = 1.0-w1-w2;

			if(w1 >= 0 && w2 >= 0 && w3 >= 0)
			{
				//A collides with B
				return true;
			}
		}

		//check for line to line collision.
		//By this point, if there is a collision, 2 lines from A will intersect with 2 lines from B
		//Max possible line checks needed is 4

		Line aArr[2] = { Line(a->getVertex1(), a->getVertex2()), Line(a->getVertex2(), a->getVertex3()) };
		Line bArr[2] = { Line(b->getVertex1(), b->getVertex2()), Line(b->getVertex2(), b->getVertex3()) };

		for(int i=0; i<2; i++)
		{
			for(int i2=0; i2<2; i2++)
			{
				double t = -1;
				aArr[i].getIntersectionParametric( bArr[i2], t );
				if(t >=0 && t <= 1)
				{
					return true;
				}
			}
		}

		//No collision
		return false;
	}

	#pragma endregion

	#pragma region POLYGON2D_COLLISIONS

	bool CollisionMaster::collisionMethod(Polygon2D* a, Point2D* b)
	{
		if(a == nullptr || b == nullptr)
		{
			return false;
		}
		
		Line2D toPoint = Line2D(a->getCenterPoint(), Vec2f(b->getPosition().x, b->getPosition().y) );

		for(size_t i=0; i<a->size(); i++)
		{
			Line2D colLine;
			if(i < a->size()-1 )
				colLine = Line2D(a->getVertex(i), a->getVertex(i+1));
			else
				colLine = Line2D(a->getVertex(i), a->getVertex(0));
			
			if(collisionMethod(&toPoint, &colLine))
			{
				return false;
			}
		}

		return true;
	}

	bool CollisionMaster::collisionMethod(Polygon2D* a, Circle* b)
	{
		//n line collisions with a circle
		//1 point in polygon check

		for(size_t i=0; i<a->size(); i++)
		{
			Line2D l = Line2D(a->getVertex(i), a->getVertex(i+1));
			if(collisionMethod(b, &l))
				return true;
		}

		//check if circle is in the polygon
		Point2D p = Point2D(b->getPosition().toVec2f());
		return collisionMethod(&p, b);
	}

	bool CollisionMaster::collisionMethod(Polygon2D* a, Ellipse* b)
	{
		//n line collisions with a ellipse
		//1 point in polygon check

		for(size_t i=0; i<a->size(); i++)
		{
			Line2D l = Line2D(a->getVertex(i), a->getVertex(i+1));
			if(collisionMethod(&l, b))
				return true;
		}

		//check if ellipse is in the polygon
		Point2D p = Point2D(b->getPosition().toVec2f());
		return collisionMethod(&p, b);
	}

	bool CollisionMaster::SeparatingAxisTheorem(Polygon2D* a, Polygon2D* b)
	{
		if(a == nullptr || b == nullptr)
		{
			return false;
		}

		for(size_t i=0; i<b->size(); i++)
		{
			Vec2f toVec;
			if(i<b->size()-1)
				toVec = b->getVertex(i+1) - b->getVertex(i);
			else
				toVec = b->getVertex(0) - b->getVertex(i);

			Vec2f normal = Vec2f(-toVec.y, toVec.x);

			float minAVal = INFINITY;
			float maxAVal = -INFINITY;
			float minBVal = INFINITY;
			float maxBVal = -INFINITY;
			
			for(size_t i2=0; i2<b->size(); i2++)
			{
				float projection = b->getVertex(i2).project(normal);
				minBVal = MathExt::min(projection, minBVal);
				maxBVal = MathExt::max(projection, maxBVal);
			}

			for(size_t i2=0; i2<a->size(); i2++)
			{
				float projection = a->getVertex(i2).project(normal);
				minAVal = MathExt::min(projection, minAVal);
				maxAVal = MathExt::max(projection, maxAVal);
			}

			//find percentage of overlap
			float overlapVal = maxAVal - minBVal;
			float overlapVal2 = maxBVal - minAVal;
			
			if(overlapVal >= 0 && overlapVal2 >= 0)
			{
				//overlap
			}
			else
			{
				//No collision
				return false;
			}
		}

		for(size_t i=0; i<a->size(); i++)
		{
			Vec2f toVec;
			if(i<a->size()-1)
				toVec = a->getVertex(i+1) - a->getVertex(i);
			else
				toVec = a->getVertex(0) - a->getVertex(i);

			float minAVal = -INFINITY;
			float maxAVal = INFINITY;
			float minBVal = -INFINITY;
			float maxBVal = INFINITY;
			
			for(size_t i2=0; i2<b->size(); i2++)
			{
				float projection = b->getVertex(i2).project(toVec);
				minBVal = MathExt::min(projection, minBVal);
				maxBVal = MathExt::max(projection, maxBVal);
			}

			for(size_t i2=0; i2<a->size(); i2++)
			{
				float projection = a->getVertex(i2).project(toVec);
				minAVal = MathExt::min(projection, minAVal);
				maxAVal = MathExt::max(projection, maxAVal);
			}

			//find percentage of overlap
			float overlapVal = maxAVal - minBVal;
			float overlapVal2 = maxBVal - minAVal;
			
			if(overlapVal >= 0 && overlapVal2 >= 0)
			{
				//overlap
			}
			else
			{
				//No collision
				return false;
			}
		}
		return true;
	}

	bool CollisionMaster::AlternatingDiagonals(Polygon2D* a, Polygon2D* b)
	{
		if(a == nullptr || b == nullptr)
		{
			return false;
		}

		Vec2f aCenter = a->getCenterPoint();
		Vec2f bCenter = b->getCenterPoint();

		for(size_t i=0; i<a->size(); i++)
		{
			Line2D colLine = Line2D(aCenter, a->getVertex(i));

			for(size_t i2=0; i2<b->size(); i2++)
			{
				Line2D bLine;
				if(i2 == b->size()-1)
					bLine = Line2D(b->getVertex(i2), b->getVertex(0));
				else
					bLine = Line2D(b->getVertex(i2), b->getVertex(i2+1));
				
				bool isCollision = CollisionMaster::getCollision(&colLine, &bLine);

				if(isCollision)
					return true;
			}
		}

		for(size_t i=0; i<b->size(); i++)
		{
			Line2D colLine = Line2D(bCenter, b->getVertex(i));

			for(size_t i2=0; i2<a->size(); i2++)
			{
				Line2D aLine;
				if(i2 == a->size()-1)
					aLine = Line2D(a->getVertex(i2), a->getVertex(0));
				else
					aLine = Line2D(a->getVertex(i2), a->getVertex(i2+1));
				
				bool isCollision = CollisionMaster::getCollision(&colLine, &aLine);

				if(isCollision)
					return true;
			}
		}

		//add method for check if fully encapsulated
		//If any point of a is in b, overlap
		//If any point of b is in a, overlap

		Point2D p1 = Point2D(aCenter);
		Point2D p2 = Point2D(bCenter);
		
		if(CollisionMaster::collisionMethod(a, &p2))
		{
			return true;
		}
		if(CollisionMaster::collisionMethod(b, &p1))
		{
			return true;
		}

		return false;
	}

	#pragma endregion

	#pragma region BEZIER_CURVE_STUFF

	bool CollisionMaster::bezierCurveCollision(BezierCurve& a, BezierCurve& b, double tolerance)
	{
		//collision through subdivision
		std::vector<Vec2f> box1 = a.getBoundingBox();
		std::vector<Vec2f> box2 = b.getBoundingBox();
		
		if(box1.size() == 2 && box2.size() == 2)
		{
			//check collision
			Box2D b1 = Box2D(box1[0].x, box1[0].y, box1[1].x, box1[1].y);
			Box2D b2 = Box2D(box2[0].x, box2[0].y, box2[1].x, box2[1].y);
			
			if( collisionMethod(&b1, &b2) )
			{
				//potential collision
				//If size of box is less than 1 in both the x and y, consider it a collision
				float b1Area = MathExt::abs((box1[1].x - box1[0].x)*(box1[1].y - box1[0].y));
				float b2Area = MathExt::abs((box2[1].x - box2[0].x)*(box2[1].y - box2[0].y));

				if(b1Area + b2Area <= tolerance)
				{
					return true;
				}

				std::vector<BezierCurve> aSub = a.subdivide(0.5);
				std::vector<BezierCurve> bSub = b.subdivide(0.5);
				bool col = false;
				
				for(int i=0; i<2; i++)
				{
					for(int i2=0; i2<2; i2++)
					{
						col = bezierCurveCollision(aSub[i], bSub[i2]);
						
						if(col)
							break;
					}

					if(col)
						break;
				}

				return col;
			}
		}
		else
		{
			//error with bezier curve size
			return false;
		}

		return false;
	}

	#pragma endregion


	#pragma endregion

} //NAMESPACE glib END