#include "Shape.h"

namespace smpl
{
	
	#pragma region Point3D

	Point3D::Point3D()
	{

	}

	Point3D::Point3D(float x, float y, float z)
	{
		setPosition(Vec3f(x, y, z));
	}

	Point3D::Point3D(const Vec3f& pos)
	{
		setPosition(pos);
	}

	Point3D::~Point3D()
	{

	}

	float Point3D::generateBoundingRadius() const
	{
		return 0;
	}
	
	void Point3D::onTransformChanged()
	{

	}

	#pragma endregion


	#pragma region Line3D
	#pragma endregion

	#pragma region Sphere

	Sphere::Sphere()
	{
		
	}
	Sphere::Sphere(float rad)
	{
		
	}
	Sphere::~Sphere()
	{
		
	}
	float Sphere::getRadius() const
	{
		return radius;
	}
	void Sphere::setRadius(float rad)
	{
		this->baseRadius = rad;
		radius = baseRadius;
	}
	float Sphere::generateBoundingRadius() const
	{
		return radius;
	}
	void Sphere::onTransformChanged()
	{
		float maxScale = __max(__max(getScale().x, getScale().y), getScale().z);
		radius = baseRadius*maxScale;
	}

	#pragma endregion

	#pragma region TRIANGLE_3D

	Triangle3D::Triangle3D()
	{

	}

	Triangle3D::Triangle3D(Vec3f p1, Vec3f p2, Vec3f p3)
	{
		baseV1 = p1 = v1;
		baseV2 = p2 = v2;
		baseV3 = p3 = v3;
	}

	Triangle3D::~Triangle3D()
	{

	}

	//Object and RootClass Stuff

	void Triangle3D::setVertex1(Vec3f p)
	{
		baseV1 = p;
		onTransformChanged();
	}

	void Triangle3D::setVertex2(Vec3f p)
	{
		baseV2 = p;
		onTransformChanged();
	}

	void Triangle3D::setVertex3(Vec3f p)
	{
		baseV3 = p;
		onTransformChanged();
	}

	Vec3f Triangle3D::getVertex1() const
	{
		return v1;
	}

	Vec3f Triangle3D::getVertex2() const
	{
		return v2;
	}

	Vec3f Triangle3D::getVertex3() const
	{
		return v3;
	}

	Vec3f Triangle3D::getCenterPosition() const
	{
		return (v1+v2+v3)/3;
	}

	Vec3f Triangle3D::getNormal() const
	{
		return v1.crossProduct(v2).normalize();
	}

	float Triangle3D::generateBoundingRadius() const
	{
		Vec3f centerPos = (v1+v2+v3)/3;
		float dis = (v1-centerPos).getLength(); //should be equal distance from each point no?
		return dis;
	}

	Vec3f Triangle3D::getMinimumBound() const
	{
		Vec3f tempV1 = getVertex1();
		Vec3f tempV2 = getVertex2();
		Vec3f tempV3 = getVertex3();
		float minX = __min(__min(tempV1.x, tempV2.x), tempV3.x);
		float minY = __min(__min(tempV1.y, tempV2.y), tempV3.y);
		float minZ = __min(__min(tempV1.z, tempV2.z), tempV3.z);
		return Vec3f(minX, minY, minZ);
	}
	Vec3f Triangle3D::getMaximumBound() const
	{
		Vec3f tempV1 = getVertex1();
		Vec3f tempV2 = getVertex2();
		Vec3f tempV3 = getVertex3();
		float maxX = __max(__max(tempV1.x, tempV2.x), tempV3.x);
		float maxY = __max(__max(tempV1.y, tempV2.y), tempV3.y);
		float maxZ = __max(__max(tempV1.z, tempV2.z), tempV3.z);
		return Vec3f(maxX, maxY, maxZ);
	}

	void Triangle3D::onTransformChanged()
	{
		//need to rotate this too. Rotate first? scale first?
		v1 = baseV1.hadamardProduct(getScale()) + getPosition();
		v2 = baseV2.hadamardProduct(getScale()) + getPosition();
		v3 = baseV3.hadamardProduct(getScale()) + getPosition();
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
		
		Vec3f mins = tri.getMinimumBound();
		Vec3f maxs = tri.getMaximumBound();
		minBound.x = __min(mins.x, minBound.x);
		minBound.y = __min(mins.y, minBound.y);
		minBound.z = __min(mins.z, minBound.z);
		maxBound.x = __max(maxs.x, maxBound.x);
		maxBound.y = __max(maxs.y, maxBound.y);
		maxBound.z = __max(maxs.z, maxBound.z);
	}
	Triangle3D Triangle3DModel::get(size_t index) const
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

	const std::vector<Triangle3D>& Triangle3DModel::getTriangles() const
	{
		return triangleList;
	}

	size_t Triangle3DModel::size() const
	{
		return triangleList.size();
	}

	Vec3f Triangle3DModel::getMinimumBound() const
	{
		return minBound;
	}

	Vec3f Triangle3DModel::getMaximumBound() const
	{
		return maxBound;
	}

	float Triangle3DModel::generateBoundingRadius() const
	{
		Vec3f midPoint = (minBound + maxBound)/2.0;
		Vec3f toTopLeft = minBound - midPoint;
		
		return toTopLeft.getLength();
	}

	#pragma endregion

	#pragma region Box3D

	Box3D::Box3D()
	{

	}
	Box3D::Box3D(const Vec3f& minValues, const Vec3f& maxValues)
	{
		baseTopLeft = minValues;
		baseBottomRight = maxValues;
		topLeft = minValues;
		bottomRight = maxValues;
	}
	Box3D::~Box3D()
	{

	}
	
	void Box3D::setMinBound(const Vec3f& v)
	{
		baseTopLeft = v;
	}
	void Box3D::setMaxBound(const Vec3f& v)
	{
		baseBottomRight = v;
	}

	
	Vec3f Box3D::getMinBound() const
	{
		return topLeft;
	}
	Vec3f Box3D::getMaxBound() const
	{
		return bottomRight;
	}
	
	float Box3D::getWidth() const
	{
		return bottomRight.x-topLeft.x;
	}

	float Box3D::getHeight() const
	{
		return bottomRight.y-topLeft.y;
	}
	
	float Box3D::getDepth() const
	{
		return bottomRight.z-topLeft.z;
	}

	float Box3D::generateBoundingRadius() const
	{
		Vec3f center = (topLeft+bottomRight)/2;
		return (topLeft - center).getLength();
	}

	bool Box3D::operator==(Box3D other) const
	{
		return topLeft==other.topLeft && bottomRight==other.bottomRight;
	}
	bool Box3D::operator!=(Box3D other) const
	{
		return topLeft!=other.topLeft || bottomRight!=other.bottomRight;
	}

	void Box3D::onTransformChanged()
	{
		topLeft = baseTopLeft.hadamardProduct(getScale()) + getPosition();
		bottomRight = baseBottomRight.hadamardProduct(getScale()) + getPosition();
	}
	#pragma endregion

	bool CollisionMaster::collisionMethod(const Point3D& a, const Point3D& b)
	{
		return a.getPosition() == b.getPosition();
	}
	bool CollisionMaster::collisionMethod(const Point3D& a, const Box3D& b)
	{
		Vec3f minV = b.getMinBound();
		Vec3f maxV = b.getMaxBound();
		Vec3f pointPos = a.getPosition();
		if(minV.x >= pointPos.x && maxV.x <= pointPos.x)
			if(minV.y >= pointPos.y && maxV.y <= pointPos.y)
				if(minV.z >= pointPos.z && maxV.z <= pointPos.z)
					return true;
		return false;
	}
	bool CollisionMaster::collisionMethod(const Point3D& a, const Sphere& b)
	{
		Vec3f bPos = b.getPosition();
		float rad = b.getRadius();
		Vec3f pointPos = a.getPosition();

		Vec3f diff = pointPos-bPos;
		float sqrDis = MathExt::sqr(diff.x) + MathExt::sqr(diff.y) + MathExt::sqr(diff.z);
		return sqrDis <= MathExt::sqr(rad);
	}
	bool CollisionMaster::collisionMethod(const Point3D& a, const Triangle3D& b)
	{
		//https://gdbooks.gitbooks.io/3dcollisions/content/Chapter4/point_in_triangle.html
		Vec3f newV1 = b.getVertex1() - a.getPosition();
		Vec3f newV2 = b.getVertex2() - a.getPosition();
		Vec3f newV3 = b.getVertex3() - a.getPosition();
		Vec3f u = newV2.crossProduct(newV3).normalize();
		Vec3f v = newV3.crossProduct(newV1).normalize();
		Vec3f w = newV1.crossProduct(newV2).normalize();
		
		if(u.dot(v) < 0)
			return false;
		if(u.dot(w) < 0)
			return false;
		
		return true;
	}

	bool CollisionMaster::collisionMethod(const Box3D& a, const Box3D& b)
	{
		Vec3f minA = a.getMinBound();
		Vec3f maxA = a.getMaxBound();
		Vec3f minB = b.getMinBound();
		Vec3f maxB = b.getMaxBound();
		
		if(minA.x <= maxB.x && maxA.x >= minB.x)
			if(minA.y <= maxB.y && maxA.y >= minB.y)
				if(minA.z <= maxB.z && maxA.z >= minB.z)
					return true;
		return false;
	}
	bool CollisionMaster::collisionMethod(const Box3D& a, const Sphere& b)
	{
		Vec3f minBound = a.getMinBound();
		Vec3f maxBound = a.getMaxBound();
		Vec3f bPos = b.getPosition();

		float curX = MathExt::clamp(bPos.x, minBound.x, maxBound.x);
		float curY = MathExt::clamp(bPos.y, minBound.y, maxBound.y);
		float curZ = MathExt::clamp(bPos.z, minBound.z, maxBound.z);

		float sqrLength = MathExt::sqr(curX - b.getPosition().x) + MathExt::sqr(curY - b.getPosition().y) + MathExt::sqr(curZ - b.getPosition().z);
		if (sqrLength <= MathExt::sqr(b.getRadius()))
			return true;

		return false;
	}
	bool CollisionMaster::collisionMethod(const Box3D& a, const Triangle3D& b)
	{
		Vec3f minA = a.getMinBound();
		Vec3f maxA = a.getMaxBound();
		std::vector<Vec3f> boxPoints = {
			Vec3f(minA.x, minA.y, minA.z), Vec3f(maxA.x, minA.y, minA.z),
			Vec3f(minA.x, maxA.y, minA.z), Vec3f(maxA.x, maxA.y, minA.z),
			Vec3f(minA.x, minA.y, maxA.z), Vec3f(maxA.x, minA.y, maxA.z),
			Vec3f(minA.x, maxA.y, maxA.z), Vec3f(maxA.x, maxA.y, maxA.z)
		};
		std::vector<Vec3f> triPoints = {
			b.getVertex1(), b.getVertex2(), b.getVertex3()
		};

		//axis aligned so these are the axises
		Vec3f boxAxis1 = Vec3f(1, 0, 0);
		Vec3f boxAxis2 = Vec3f(0, 1, 0);
		Vec3f boxAxis3 = Vec3f(0, 0, 1);

		if(!SATTest(boxAxis1, boxPoints, triPoints))
			return false;
		if(!SATTest(boxAxis2, boxPoints, triPoints))
			return false;
		if(!SATTest(boxAxis3, boxPoints, triPoints))
			return false;

		//triangle normal as an axis
		Vec3f triangleNormal = b.getNormal();

		if(!SATTest(triangleNormal, boxPoints, triPoints))
			return false;

		//cross products
		Vec3f cross11 = boxAxis1.crossProduct(triPoints[1]-triPoints[0]);
		Vec3f cross12 = boxAxis1.crossProduct(triPoints[2]-triPoints[1]);
		Vec3f cross13 = boxAxis1.crossProduct(triPoints[0]-triPoints[2]);
		
		if(!SATTest(cross11, boxPoints, triPoints))
			return false;
		if(!SATTest(cross12, boxPoints, triPoints))
			return false;
		if(!SATTest(cross13, boxPoints, triPoints))
			return false;

		Vec3f cross21 = boxAxis2.crossProduct(triPoints[1]-triPoints[0]);
		Vec3f cross22 = boxAxis2.crossProduct(triPoints[2]-triPoints[1]);
		Vec3f cross23 = boxAxis2.crossProduct(triPoints[0]-triPoints[2]);
		
		if(!SATTest(cross21, boxPoints, triPoints))
			return false;
		if(!SATTest(cross22, boxPoints, triPoints))
			return false;
		if(!SATTest(cross23, boxPoints, triPoints))
			return false;

		Vec3f cross31 = boxAxis3.crossProduct(triPoints[1]-triPoints[0]);
		Vec3f cross32 = boxAxis3.crossProduct(triPoints[2]-triPoints[1]);
		Vec3f cross33 = boxAxis3.crossProduct(triPoints[0]-triPoints[2]);

		if(!SATTest(cross31, boxPoints, triPoints))
			return false;
		if(!SATTest(cross32, boxPoints, triPoints))
			return false;
		if(!SATTest(cross33, boxPoints, triPoints))
			return false;
		
		return true;
	}

	bool CollisionMaster::collisionMethod(const Sphere& a, const Sphere& b)
	{
		Vec3f aPos = a.getPosition();
		Vec3f bPos = b.getPosition();
		float aRad = a.getRadius();
		float bRad = b.getRadius();

		Vec3f diff = aPos-bPos;
		float sqrDis = MathExt::sqr(diff.x) + MathExt::sqr(diff.y) + MathExt::sqr(diff.z);
		return sqrDis <= MathExt::sqr(aRad+bRad);
	}
	bool CollisionMaster::collisionMethod(const Sphere& a, const Triangle3D& b)
	{
		//lazy. Make a plane class
		Vec3f planeNormal = b.getNormal();
		float planeDistance = planeNormal.dot(b.getVertex1());

		Vec3f sphereCenter = a.getPosition();
		float tempDisFromPlane = planeNormal.dot(sphereCenter) - planeDistance;
		Point3D closestPointToPlane = sphereCenter - tempDisFromPlane*planeNormal;
		
		//check if that point is in the triangle first
		if(collisionMethod(closestPointToPlane, b))
			return true;
		

		return false;
	}
	
	bool CollisionMaster::collisionMethod(const Triangle3D& a, const Triangle3D& b)
	{
		//do SAT checks
		return false;
	}
}