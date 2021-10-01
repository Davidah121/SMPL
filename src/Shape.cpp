#include "Shape.h"
#include "StringTools.h"

namespace glib
{
	
	#pragma region SHAPE


		#pragma region ClassStuff
		const Class Shape::myClass = Class("Shape", {&Object::myClass});
		const Class* Shape::getClass()
		{
			return &Shape::myClass;
		}
		#pragma endregion


	Shape::Shape()
	{
	}

	Shape::~Shape()
	{
	}

	void Shape::setPosition(Vec3f posPointer)
	{
		position = posPointer;
	}

	Vec3f Shape::getPosition()
	{
		return position;
	}

	void Shape::setScale(Vec3f posScale)
	{
		scale = posScale;
	}

	Vec3f Shape::getScale()
	{
		return scale;
	}

	void Shape::setRotation(Vec3f posRotation)
	{
		rotation = posRotation;
	}

	Vec3f Shape::getRotation()
	{
		return rotation;
	}

	void Shape::transform()
	{
		
	}

	double Shape::generateBoundingRadius()
	{
		return 0;
	}

	#pragma endregion

	#pragma region POINT2D

		#pragma region ClassStuff
		const Class Point2D::myClass = Class("Point2D", { &Shape::myClass });
		const Class* Point2D::getClass()
		{
			return &Point2D::myClass;
		}
		#pragma endregion

	Point2D::Point2D()
	{

	}

	Point2D::Point2D(Vec2f pos)
	{
		position.x = pos.x;
		position.y = pos.y;
	}

	Point2D::~Point2D()
	{
	}

	double Point2D::generateBoundingRadius()
	{
		return 0;
	}

	#pragma endregion

	#pragma region Box2D

		#pragma region ClassStuff
		const Class Box2D::myClass = Class("Box2D", { &Shape::myClass });
		const Class* Box2D::getClass()
		{
			return &Box2D::myClass;
		}
		#pragma endregion

	Box2D::Box2D()
	{

	}

	Box2D::Box2D(double leftBound, double topBound, double rightBound, double bottomBound)
	{
		this->lBound = leftBound;
		this->tBound = topBound;
		this->rBound = rightBound;
		this->bBound = bottomBound;
	}

	Box2D::~Box2D()
	{
	}

	void Box2D::setLeftBound(double lb)
	{
		lBound = lb;
	}

	double Box2D::getLeftBound()
	{
		return lBound;
	}

	void Box2D::setTopBound(double tb)
	{
		tBound = tb;
	}

	double Box2D::getTopBound()
	{
		return tBound;
	}

	void Box2D::setRightBound(double rb)
	{
		rBound = rb;
	}

	double Box2D::getRightBound()
	{
		return rBound;
	}

	void Box2D::setBottomBound(double bb)
	{
		bBound = bb;
	}

	double Box2D::getBottomBound()
	{
		return bBound;
	}

	double Box2D::generateBoundingRadius()
	{
		return 0;
	}

	#pragma endregion

	#pragma region CIRCLE

		#pragma region ClassStuff
		const Class Circle::myClass = Class("Circle", { &Shape::myClass });
		const Class* Circle::getClass()
		{
			return &Circle::myClass;
		}
		#pragma endregion

	Circle::Circle()
	{

	}

	Circle::Circle(double rad)
	{
		radius = rad;
	}

	Circle::~Circle()
	{
	}

	double Circle::getRadius()
	{
		return radius;
	}

	void Circle::setRadius(double rad)
	{
		radius = rad;
	}

	double Circle::generateBoundingRadius()
	{
		return 0;
	}

	#pragma endregion

	#pragma region ELLIPSE

		#pragma region ClassStuff
		const Class Ellipse::myClass = Class("Ellipse", { &Shape::myClass });
		const Class* Ellipse::getClass()
		{
			return &Ellipse::myClass;
		}
		#pragma endregion

	Ellipse::Ellipse()
	{

	}

	Ellipse::Ellipse(double xRad, double yRad)
	{
		xRadius = xRad;
		yRadius = yRad;
	}
	Ellipse::~Ellipse()
	{

	}

	double Ellipse::getXRadius()
	{
		return xRadius;
	}
	void Ellipse::setXRadius(double rad)
	{
		xRadius = rad;
	}
	double Ellipse::getYRadius()
	{
		return yRadius;
	}
	void Ellipse::setYRadius(double rad)
	{
		yRadius = rad;
	}

	double Ellipse::generateBoundingRadius()
	{
		return 0;
	}

	#pragma endregion

	#pragma region LINE2D

		#pragma region ClassStuff
		const Class Line2D::myClass = Class("Line2D", { &Shape::myClass });
		const Class* Line2D::getClass()
		{
			return &Line2D::myClass;
		}
		#pragma endregion

	Line2D::Line2D()
	{

	}

	Line2D::Line2D(double x1, double y1, double x2, double y2)
	{
		l = Line(x1, y1, x2, y2);
	}

	Line2D::Line2D(Vec2f p1, Vec2f p2)
	{
		l = Line(p1,p2);
	}

	Line2D::~Line2D()
	{
	}

	void Line2D::setPoint1(Vec2f p)
	{
		l = Line(p, l.getPoint2());
	}

	void Line2D::setPoint1(double x, double y)
	{
		l = Line(Vec2f(x,y), l.getPoint2());
	}

	void Line2D::setPoint2(Vec2f p)
	{
		l = Line(l.getPoint1(), p);
	}

	void Line2D::setPoint2(double x, double y)
	{
		
		l = Line(l.getPoint1(), Vec2f(x,y));
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

	double Line2D::generateBoundingRadius()
	{
		return 0;
	}

	#pragma endregion

	#pragma region TRIANGLE2D

		#pragma region ClassStuff
		const Class Triangle2D::myClass = Class("Triangle2D", { &Shape::myClass });
		const Class* Triangle2D::getClass()
		{
			return &Triangle2D::myClass;
		}
		#pragma endregion

	Triangle2D::Triangle2D()
	{

	}

	Triangle2D::Triangle2D(Vec2f p1, Vec2f p2, Vec2f p3)
	{
		v1 = p1;
		v2 = p2;
		v3 = p3;
	}

	Triangle2D::Triangle2D(double x1, double y1, double x2, double y2, double x3, double y3)
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

	void Triangle2D::setVertex1(double x, double y)
	{
		v1.x = x;
		v1.y = y;
	}

	void Triangle2D::setVertex2(Vec2f p)
	{
		v2 = p;
	}

	void Triangle2D::setVertex2(double x, double y)
	{
		v2.x = x;
		v2.y = y;
	}

	void Triangle2D::setVertex3(Vec2f p)
	{
		v3 = p;
	}

	void Triangle2D::setVertex3(double x, double y)
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

	double Triangle2D::generateBoundingRadius()
	{
		return 0;
	}

	#pragma endregion

	#pragma region POLYGON2D

		#pragma region ClassStuff
		const Class Polygon2D::myClass = Class("Polygon2D", {&Shape::myClass});
		const Class* Polygon2D::getClass()
		{
			return &Polygon2D::myClass;
		}
		#pragma endregion

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

	Vec2f Polygon2D::getVertex(int index)
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
			double lastDirVal = MathExt::dirToPoint( points[1].x, points[1].y, points[2].x, points[2].y);
			int lastQuadrant = lastDirVal / (PI/2);
			bool dir = lastQuadrant <= 2; //going counter-clockwise

			for(int i=3; i<points.size()+1; i++)
			{
				Vec2f thisToVec;
				if(i != points.size())
					thisToVec = points[i] - points[i-1];
				else
					thisToVec = points[0] - points[i-1];
				
				double thisDirVal = MathExt::dirToPoint(0, 0, thisToVec.x, thisToVec.y);
				bool thisDir = false;

				if(dir)
				{
					double dirDis = 0;
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
					double dirDis = 0;
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

	Polygon2D Polygon2D::approximateCircle(double radius, int n)
	{
		if(n <= 3)
			return Polygon2D();
		
		Polygon2D out;
		double du = 2*PI / n;
		for(int i=0; i<=n; i++)
		{
			double x = radius * MathExt::cos(du*i);
			double y = radius * MathExt::sin(du*i);

			out.addVertex( Vec2f(x,y) );
		}

		return out;
	}

	Polygon2D Polygon2D::approximateEllipse(double xRadius, double yRadius, int n)
	{
		if(n <= 3)
			return Polygon2D();
		
		Polygon2D out;
		double du = 2*PI / n;
		for(int i=0; i<=n; i++)
		{
			double x = xRadius * MathExt::cos(du*i);
			double y = yRadius * MathExt::sin(du*i);

			out.addVertex( Vec2f(x,y) );
		}

		return out;
	}

	Polygon2D Polygon2D::approximateArc(double xRadius, double yRadius, double startAngle, double endAngle, int n)
	{
		if(n <= 3)
			return Polygon2D();
		
		Polygon2D out;
		out.addVertex( Vec2f(0,0) );
		double du = (endAngle - startAngle) / n;
		for(int i=0; i<=n; i++)
		{
			double x = xRadius * MathExt::cos(startAngle + du*i);
			double y = yRadius * MathExt::sin(startAngle + du*i);

			out.addVertex( Vec2f(x,y) );
		}

		return out;
	}

	double Polygon2D::generateBoundingRadius()
	{
		return 0;
	}

	#pragma endregion

	#pragma region COLLISION_MASTER

	//Functions
	bool CollisionMaster::getCollision(Shape* a, Shape* b)
	{
		std::string aClassName = a->getClass()->getClassName();
		std::string bClassName = b->getClass()->getClassName();

		if (aClassName == "Box2D")
		{
			if (bClassName == "Box2D")
			{
				return CollisionMaster::collisionMethod((Box2D*)a, (Box2D*)b);
			}
			else if (bClassName == "Point2D")
			{
				return CollisionMaster::collisionMethod((Point2D*)b, (Box2D*)a);
			}
			else if (bClassName == "Circle")
			{
				return CollisionMaster::collisionMethod((Box2D*)a, (Circle*)b);
			}
			else if (bClassName == "Ellipse")
			{
				return CollisionMaster::collisionMethod((Box2D*)a, (Ellipse*)b);
			}
			else if (bClassName == "Triangle2D")
			{
				return CollisionMaster::collisionMethod((Box2D*)a, (Triangle2D*)b);
			}
			else if (bClassName == "Line2D")
			{
				return CollisionMaster::collisionMethod((Box2D*)a, (Line2D*)b);
			}

			//No matching function found
		}
		else if (aClassName == "Point2D")
		{
			if (bClassName == "Box2D")
			{
				return CollisionMaster::collisionMethod((Point2D*)a, (Box2D*)b);
			}
			else if (bClassName == "Point2D")
			{
				return CollisionMaster::collisionMethod((Point2D*)a, (Point2D*)b);
			}
			else if (bClassName == "Circle")
			{
				return CollisionMaster::collisionMethod((Point2D*)a, (Circle*)b);
			}
			else if (bClassName == "Ellipse")
			{
				return CollisionMaster::collisionMethod((Point2D*)a, (Ellipse*)b);
			}
			else if (bClassName == "Triangle2D")
			{
				return CollisionMaster::collisionMethod((Point2D*)a, (Triangle2D*)b);
			}
			else if (bClassName == "Line2D")
			{
				return CollisionMaster::collisionMethod((Point2D*)a, (Line2D*)b);
			}

			//No matching function found
		}
		else if (aClassName == "Circle")
		{
			if (bClassName == "Box2D")
			{
				return CollisionMaster::collisionMethod((Box2D*)b, (Circle*)a);
			}
			else if (bClassName == "Point2D")
			{
				return CollisionMaster::collisionMethod((Point2D*)b, (Circle*)a);
			}
			else if (bClassName == "Circle")
			{
				return CollisionMaster::collisionMethod((Circle*)a, (Circle*)b);
			}
			else if (bClassName == "Ellipse")
			{
				return CollisionMaster::collisionMethod((Circle*)a, (Ellipse*)b);
			}
			else if (bClassName == "Triangle2D")
			{
				return CollisionMaster::collisionMethod((Circle*)a, (Triangle2D*)b);
			}
			else if (bClassName == "Line2D")
			{
				return CollisionMaster::collisionMethod((Circle*)a, (Line2D*)b);
			}

			//No matching function found
		}
		else if (aClassName == "Line2D")
		{
			if (bClassName == "Box2D")
			{
				return CollisionMaster::collisionMethod((Box2D*)b, (Line2D*)a);
			}
			else if (bClassName == "Point2D")
			{
				return CollisionMaster::collisionMethod((Point2D*)b, (Line2D*)a);
			}
			else if (bClassName == "Circle")
			{
				return CollisionMaster::collisionMethod((Circle*)b, (Line2D*)a);
			}
			else if (bClassName == "Ellipse")
			{
				return CollisionMaster::collisionMethod((Line2D*)a, (Ellipse*)b);
			}
			else if (bClassName == "Triangle2D")
			{
				return CollisionMaster::collisionMethod((Line2D*)a, (Triangle2D*)b);
			}
			else if (bClassName == "Line2D")
			{
				return CollisionMaster::collisionMethod((Line2D*)a, (Line2D*)b);
			}

			//No matching function found
		}
		else if (aClassName == "Triangle2D")
		{
			if (bClassName == "Box2D")
			{
				return CollisionMaster::collisionMethod((Box2D*)b, (Triangle2D*)a);
			}
			else if (bClassName == "Point2D")
			{
				return CollisionMaster::collisionMethod((Point2D*)b, (Triangle2D*)a);
			}
			else if (bClassName == "Circle")
			{
				return CollisionMaster::collisionMethod((Circle*)b, (Triangle2D*)a);
			}
			else if (bClassName == "Ellipse")
			{
				return CollisionMaster::collisionMethod((Ellipse*)b, (Triangle2D*)a);
			}
			else if (bClassName == "Triangle2D")
			{
				return CollisionMaster::collisionMethod((Triangle2D*)a, (Triangle2D*)b);
			}
			else if (bClassName == "Line2D")
			{
				return CollisionMaster::collisionMethod((Line2D*)b, (Triangle2D*)a);
			}

			//No matching function found
		}
		else if (aClassName == "Ellipse")
		{
			if (bClassName == "Box2D")
			{
				return CollisionMaster::collisionMethod((Box2D*)b, (Ellipse*)a);
			}
			else if (bClassName == "Point2D")
			{
				return CollisionMaster::collisionMethod((Point2D*)b, (Ellipse*)a);
			}
			else if (bClassName == "Circle")
			{
				return CollisionMaster::collisionMethod((Circle*)b, (Ellipse*)a);
			}
			else if (bClassName == "Ellipse")
			{
				return CollisionMaster::collisionMethod((Ellipse*)a, (Ellipse*)b);
			}
			else if (bClassName == "Triangle2D")
			{
				return CollisionMaster::collisionMethod((Ellipse*)a, (Triangle2D*)b);
			}
			else if (bClassName == "Line2D")
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
		double dX = MathExt::sqr(a->getPosition().x - b->getPosition().x);
		double dY = MathExt::sqr(a->getPosition().y - b->getPosition().y);

		if (dX + dY <= MathExt::sqr(b->getRadius()))
		{
			return true;
		}
		return false;
	}

	bool CollisionMaster::collisionMethod(Point2D* a, Ellipse* b)
	{
		double dX = MathExt::sqr(a->getPosition().x - b->getPosition().x);
		double dY = MathExt::sqr(a->getPosition().y - b->getPosition().y);

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

		
		if (MathExt::dot(v1, v2) == MathExt::dot(v1, toPoint) + MathExt::dot(v2, toPoint))
		{
			//Possible collision
			v1 = -v1;
			v2 = Vec2f(b->getVertex3() - b->getVertex2());

			toPoint = Vec2f(a->getPosition().x - b->getVertex2().x, a->getPosition().y - b->getVertex2().y);

			if (MathExt::dot(v1, v2) == MathExt::dot(v1, toPoint) + MathExt::dot(v2, toPoint))
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
			double yNeeded = bLine.solveForY( a->getPosition().x);

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
		double curX = MathExt::clamp(b->getPosition().x, a->getLeftBound(), a->getRightBound());
		double curY = MathExt::clamp(b->getPosition().y, a->getTopBound(), a->getBottomBound());

		double length = MathExt::sqr(curX - b->getPosition().x) + MathExt::sqr(curY - b->getPosition().y);
		
		if (length <= MathExt::sqr(b->getRadius()))
		{
			return true;
		}
		return false;
	}

	bool CollisionMaster::collisionMethod(Box2D* a, Ellipse* b)
	{
		double curX = MathExt::clamp(b->getPosition().x, a->getLeftBound(), a->getRightBound());
		double curY = MathExt::clamp(b->getPosition().y, a->getTopBound(), a->getBottomBound());

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
			Vec2f pos1 = triLine1.getIntersection(boxLines[i]);
			Vec2f pos2 = triLine2.getIntersection(boxLines[i]);
			Vec2f pos3 = triLine3.getIntersection(boxLines[i]);

			if(pos1.y >= a->getBottomBound() && pos1.y <= a->getTopBound())
				return true;

			if(pos2.y >= a->getBottomBound() && pos2.y <= a->getTopBound())
				return true;

			if(pos3.y >= a->getBottomBound() && pos3.y <= a->getTopBound())
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
			double yNeeded1 = bLine.solveForY(a->getLeftBound());
			double yNeeded2 = bLine.solveForY(a->getRightBound());

			double maY = MathExt::max(yNeeded1, yNeeded2);
			double miY = MathExt::min(yNeeded1, yNeeded2);

			maY = MathExt::clamp(maY, bLine.getMinY(), bLine.getMaxY());
			miY = MathExt::clamp(miY, bLine.getMinY(), bLine.getMaxY());
			
			double maX = MathExt::clamp(a->getLeftBound(), bLine.getMinX(), bLine.getMaxX());
			double miX = MathExt::clamp(a->getRightBound(), bLine.getMinX(), bLine.getMaxX());

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
		double disX = MathExt::sqr(a->getPosition().x - b->getPosition().x);
		double disY = MathExt::sqr(a->getPosition().y - b->getPosition().y);
		double len1 = disX + disY;

		double len2 = MathExt::sqr(a->getRadius()) + MathExt::sqr(b->getRadius());

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

		double dis = MathExt::vecLength(toVec);
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
			double rx = b->getXRadius();
			double ry = b->getYRadius();
			double x = toVec.x;
			double y = toVec.y;

			f.setFunction( [rx, ry, x, y](double t) ->double{
				double val = -MathExt::sin(t)*(MathExt::sqr(ry) - MathExt::sqr(rx));
				val +=  y*ry;
				val -= (x*rx*MathExt::tan(t));

				return val;
			});

			//reduce [0, 2*PI] to a smaller range of values
			double minRange, maxRange;
			double offset = 0.000001;
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

			double angleValue = MathExt::bisectionMethod(&f, minRange, maxRange, 10);
			
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


		return false;
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
			double y = (a->getPosition().y + bLine.getSlopeRelativeY() * (a->getPosition().x - bLine.getXInt())) / (1 + MathExt::sqr(bLine.getSlopeRelativeY()));

			//clamp it down to the max or min possible x value then solve for y
			y = MathExt::clamp(y, bLine.getMinY(), bLine.getMaxY());
			double x = y * bLine.getSlopeRelativeY() + bLine.getXInt();

			//now solve distance
			double disX = MathExt::sqr(a->getPosition().x - x);
			double disY = MathExt::sqr(a->getPosition().y - y);

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
			double x = (a->getPosition().x + bLine.getSlope() * (a->getPosition().y - bLine.getYInt())) / (1 + MathExt::sqr(bLine.getSlope()));

			//clamp it down to the max or min possible x value then solve for y
			x = MathExt::clamp(x, bLine.getMinX(), bLine.getMaxX());
			double y = x * bLine.getSlope() + bLine.getYInt();

			//now solve distance
			double disX = MathExt::sqr(a->getPosition().x - x);
			double disY = MathExt::sqr(a->getPosition().y - y);

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

		Vec2f pointOfCol = l1.getIntersection(l2);
		
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
			double xRadSqr = MathExt::sqr(b->getXRadius());
			double yRadSqr = MathExt::sqr(b->getYRadius());

			double rot = b->getRotation().x;

			double H = b->getPosition().x;
			double K = b->getPosition().y;

			double xCheck = a->getPoint1().x;

			double v1 = MathExt::dcos(rot)*(xCheck-H);
			double v2 = -MathExt::dsin(rot)*(xCheck-H);
			
			double A = (MathExt::sqr(MathExt::dsin(rot)) / xRadSqr) + (MathExt::sqr(MathExt::dcos(rot)) / yRadSqr);
			double B = (MathExt::dsin(rot)*v1 / xRadSqr) + (MathExt::dcos(rot)*v2 / yRadSqr);
			double C = (MathExt::sqr(v1)/xRadSqr) + (MathExt::sqr(v2)/yRadSqr) - 1;

			double polyA = A;
			double polyB = 2*(B-A*K);
			double polyC = A*MathExt::sqr(K) - 2*B*K + C;

			std::vector<double> solutions = MathExt::solveQuadraticReal(polyA, polyB, polyC);

			if(solutions.size()>0)
			{
				//possible collision
				for(int i=0; i<solutions.size(); i++)
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
			double xRadSqr = MathExt::sqr(b->getXRadius());
			double yRadSqr = MathExt::sqr(b->getYRadius());

			double rot = b->getRotation().x;

			double H = b->getPosition().x;
			double K = b->getPosition().y;

			double yCheck = a->getPoint1().y;

			double v1 = MathExt::dsin(rot)*(yCheck-K);
			double v2 = MathExt::dcos(rot)*(yCheck-K);
			
			double A = (MathExt::sqr(MathExt::dcos(rot)) / xRadSqr) + (MathExt::sqr(MathExt::dsin(rot)) / yRadSqr);
			double B = (MathExt::dcos(rot)*v1 / xRadSqr) - (MathExt::dsin(rot)*v2 / yRadSqr);
			double C = (MathExt::sqr(v1)/xRadSqr) + (MathExt::sqr(v2)/yRadSqr) - 1;

			double polyA = A;
			double polyB = 2*(B-A*K);
			double polyC = A*MathExt::sqr(K) - 2*B*K + C;

			std::vector<double> solutions = MathExt::solveQuadraticReal(polyA, polyB, polyC);

			if(solutions.size()>0)
			{
				//possible collision
				for(int i=0; i<solutions.size(); i++)
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
			
			double A = a->getPoint2().y - a->getPoint1().y;
			double B = a->getPoint1().x - a->getPoint2().x;
			double C = a->getPoint2().x*a->getPoint1().y - a->getPoint2().y*a->getPoint1().x;

			double xRadSqr = MathExt::sqr(b->getXRadius());
			double yRadSqr = MathExt::sqr(b->getYRadius());

			double rot = b->getRotation().x;

			double H = b->getPosition().x;
			double K = b->getPosition().y;

			double V1 = -(MathExt::dsin(rot)*K + MathExt::dcos(rot)*((C/A) + H));
			double V2 = (MathExt::dsin(rot) - MathExt::dcos(rot)*(B/A));
			double V3 = (MathExt::dsin(rot)*((C/A)+H) - MathExt::dcos(rot)*K);
			double V4 = (MathExt::dcos(rot) + MathExt::dsin(rot)*(B/A));

			double polyA = (MathExt::sqr(V2) / xRadSqr) + (MathExt::sqr(V4) / yRadSqr);
			double polyB = 2*((V1*V2 / xRadSqr) + (V3*V4 / yRadSqr));
			double polyC = (MathExt::sqr(V1) / xRadSqr) + (MathExt::sqr(V3) / yRadSqr) - 1;

			std::vector<double> solutions = MathExt::solveQuadraticReal(polyA, polyB, polyC);
			
			if(solutions.size()>0)
			{
				//possible collision
				for(int i=0; i<solutions.size(); i++)
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
		
		Vec2f posColPoint1 = l.getIntersection(tl1);
		Vec2f posColPoint2 = l.getIntersection(tl2);
		Vec2f posColPoint3 = l.getIntersection(tl3);

		if(posColPoint1.x >= l.getMinX() && posColPoint1.x <= l.getMaxX())
			if(posColPoint1.y >= l.getMinY() && posColPoint1.y <= l.getMaxY())
				return true;

		if(posColPoint2.x >= l.getMinX() && posColPoint2.x <= l.getMaxX())
			if(posColPoint2.y >= l.getMinY() && posColPoint2.y <= l.getMaxY())
				return true;

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

		return false;
	}

	bool CollisionMaster::collisionMethod(Ellipse* a, Triangle2D* b)
	{
		return false;
	}

	#pragma endregion

	#pragma region TRIANGLE2D_COLLISIONS

	bool CollisionMaster::collisionMethod(Triangle2D* a, Triangle2D* b)
	{
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

		for(int i=0; i<a->size(); i++)
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

	bool CollisionMaster::SeparatingAxisTheorem(Polygon2D* a, Polygon2D* b)
	{
		if(a == nullptr || b == nullptr)
		{
			return false;
		}

		for(int i=0; i<b->size(); i++)
		{
			Vec2f toVec;
			if(i<b->size()-1)
				toVec = b->getVertex(i+1) - b->getVertex(i);
			else
				toVec = b->getVertex(0) - b->getVertex(i);

			Vec2f normal = Vec2f(-toVec.y, toVec.x);

			double minAVal = INFINITY;
			double maxAVal = -INFINITY;
			double minBVal = INFINITY;
			double maxBVal = -INFINITY;
			
			for(int i2=0; i2<b->size(); i2++)
			{
				double projection = MathExt::scalarVectorProjection(b->getVertex(i2), normal);
				minBVal = MathExt::min(projection, minBVal);
				maxBVal = MathExt::max(projection, maxBVal);
			}

			for(int i2=0; i2<a->size(); i2++)
			{
				double projection = MathExt::scalarVectorProjection(a->getVertex(i2), normal);
				minAVal = MathExt::min(projection, minAVal);
				maxAVal = MathExt::max(projection, maxAVal);
			}

			//find percentage of overlap
			double overlapVal = maxAVal - minBVal;
			double overlapVal2 = maxBVal - minAVal;
			
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

		for(int i=0; i<a->size(); i++)
		{
			Vec2f toVec;
			if(i<a->size()-1)
				toVec = a->getVertex(i+1) - a->getVertex(i);
			else
				toVec = a->getVertex(0) - a->getVertex(i);

			double minAVal = -INFINITY;
			double maxAVal = INFINITY;
			double minBVal = -INFINITY;
			double maxBVal = INFINITY;
			
			for(int i2=0; i2<b->size(); i2++)
			{
				double projection = MathExt::scalarVectorProjection(b->getVertex(i2), toVec);
				minBVal = MathExt::min(projection, minBVal);
				maxBVal = MathExt::max(projection, maxBVal);
			}

			for(int i2=0; i2<a->size(); i2++)
			{
				double projection = MathExt::scalarVectorProjection(a->getVertex(i2), toVec);
				minAVal = MathExt::min(projection, minAVal);
				maxAVal = MathExt::max(projection, maxAVal);
			}

			//find percentage of overlap
			double overlapVal = maxAVal - minBVal;
			double overlapVal2 = maxBVal - minAVal;
			
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

		for(int i=0; i<a->size(); i++)
		{
			Line2D colLine = Line2D(aCenter, a->getVertex(i));

			for(int i2=0; i2<b->size(); i2++)
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

		for(int i=0; i<b->size(); i++)
		{
			Line2D colLine = Line2D(bCenter, b->getVertex(i));

			for(int i2=0; i2<a->size(); i2++)
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



	#pragma endregion

} //NAMESPACE glib END