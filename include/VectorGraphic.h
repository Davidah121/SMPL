#pragma once
#include<vector>
#include<map>
#include "Image.h"
#include "MathExt.h"
#include "SimpleXml.h"

namespace glib
{

	struct criticalPoint
	{
		double xValue;
		bool isYPositive;
	};

	class VectorShape : public Object
	{
	public:
		/**
		 * XML format:
		 * 		< ... fill="_COLOR" fill-opacity="_ALPHA" stroke="_COLOR" 
		 * 				stroke-opacity="_ALPHA" stroke-width="_VALUE"
		 * 				stroke-linecap="TYPE" stroke-linejoin="TYPE"
		 * 				stroke-dasharray="Array" stroke-dashoffset="VALUE"
		 * 				fill-rule="TYPE"
		 * 				transform="_TRANSFORMS">
		 * 		_COLOR can be in multiple formats:
		 * 			RGB TRIPLET = rgb(r,g,b) in int,float,percent
		 * 			HEX = #000000 or #000
		 * 			NAME = black
		 * 		_ALPHA can be in these formats:
		 * 			HEX = #FF
		 * 			int = 255
		 * 			percent = 100%
		 * 			float = 1.0f
		 * 		_VALUE will be followed by px,pt or nothing
		 * 			probably an int or float.
		 * 		_TRANSFORMS can be the following
		 * 			translate(x,y)
		 * 			rotate(r)
		 * 			skewX(x)
		 * 			skewY(y)
		 * 			scale(x,y) or scale(s)
		 * 			matrix(a,b,c,d,e,f)
		 * 		FOR linecap, valid types are:
		 * 			butt, square, round
		 * 		FOR linejoin, valid types are:
		 * 			miter, round, bevel
		 * 		For stroke-dasharray, the format is as follows:
		 * 			dash length, gap length, dash length, gap length, ...
		 * 		FOR fill-rule, valid types are:
		 * 			evenodd, nonzero
		 * 		
		 * 		NOTE: May contain more un-implemented features.
		 * 
		 * 		Note that transforms can be applied to more than one object at 
		 * 		a time by using the tag <g>
		 * 		EXAMPLE
		 * 		<g transform="scale(2)">
		 * 		This will apply the transform to all objects surrounded by it.
		 * 		This may mean that each object needs its own transform.
		 */
		static const bool EVEN_ODD_RULE = false;
		static const bool NON_ZERO = true;

		static const char LINE_CAP_BUTT = 0;
		static const char LINE_CAP_SQUARE = 1;
		static const char LINE_CAP_ROUND = 2;

		static const char LINE_JOIN_MITER = 0;
		static const char LINE_JOIN_ROUND = 1;
		static const char LINE_JOIN_BEVEL = 2;

		VectorShape();
		~VectorShape();
		
		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;

		virtual void draw(Image* buffer, int globalWidth, int globalHeight);
		virtual void drawStroke(Image* buffer, int globalWidth, int globalHeight);

		void setFillMethod(bool value);
		bool getFillMethod();
		
		void setFillColor(Color k);
		Color getFillColor();

		void setStrokeColor(Color k);
		Color getStrokeColor();

		void setStrokeWidth(double k);
		double getStrokeWidth();

		void setLineCap(char c);
		char getLineCap();

		void setLineJoin(char c);
		char getLineJoin();

		void setTransform(Mat3f m);
		Mat3f getTransform();
		
	protected:
		void applyTransform();

	private:
		Color fill = {0,0,0,255};
		Color strokeColor = {0,0,0,0};
		double strokeWidth = 0;
		bool fillMethod = false;
		char lineCap = 0;
		char lineJoin = 0;
		Mat3f transform = Mat3f();
	};

	class VectorRectangle : public VectorShape
	{
	public:
		/**
		 *	XML format:
		*		(Note that rx and ry may not appear)
		* 		<rect x="" y="" rx="" ry="" width="" height="" />
		* 		x = x of top left corner
		* 		y = y of top left corner
		* 		rx = x radius of the corners
		* 		ry = y radius of the corners
		* 		width = width
		* 		height = height
		* 
		* NOTE:
		* 		Fill rule doesn't affect this object
		* 		Line cap doesn't affect this object
		* 		!! Line join does affect this object !!
		*/

		VectorRectangle();
		~VectorRectangle();

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;

		void draw(Image* buffer, int globalWidth, int globalHeight);
		void drawStroke(Image* buffer, int globalWidth, int globalHeight);

		void setX(double x);
		double getX();

		void setY(double y);
		double getY();

		void setRX(double rx);
		double getRX();

		void setRY(double ry);
		double getRY();

		void setWidth(double width);
		double getWidth();

		void setHeight(double height);
		double getHeight();

	protected:
		void applyTransform();

	private:

		double x = 0;
		double y = 0;
		double rx = 0;
		double ry = 0;
		double width = 0;
		double height = 0;
		void drawTransformed(Image* buffer, int globalWidth, int globalHeight);
		void drawStrokeTransformed(Image* buffer, int globalWidth, int globalHeight);

	};

	class VectorCircle : public VectorShape
	{
	public:

		/**
		 * 	XML Format:
		 * 		<circle cx="" cy="" r=""/>
		 * 		cx = center x of the circle
		 * 		cy = center y of the circle
		 * 		r = radius
		 * 	NOTE:
		 * 		Fill rule doesn't affect this object
		 * 		Line cap doesn't affect this object
		 * 		Line join doesn't affect this object
		 */ 
		VectorCircle();
		~VectorCircle();

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;

		void draw(Image* img, int globalWidth, int globalHeight);
		void drawStroke(Image* buffer, int globalWidth, int globalHeight);
		
		void setX(double x);
		double getX();

		void setY(double y);
		double getY();

		void setRadius(double r);
		double getRadius();

	protected:
		void applyTransform();

	private:

		double cx = 0;
		double cy = 0;
		double radius = 0;
		void drawTransformed(Image* buffer, int globalWidth, int globalHeight);
		void drawStrokeTransformed(Image* buffer, int globalWidth, int globalHeight);
		
	};

	class VectorEllipse : public VectorShape
	{
	public:

		/**
		 * 	XML Format:
		 * 		<ellipse cx="" cy="" rx="" ry=""/>
		 * 		cx = center x of the ellipse
		 * 		cy = center y of the ellipse
		 * 		rx = x radius
		 * 		ry = y radius
		 * 
		 * NOTE:
		 * 		Fill rule doesn't affect this object
		 * 		Line cap doesn't affect this object
		 * 		Line join doesn't affect this object
		 */ 
		VectorEllipse();
		~VectorEllipse();

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;

		void draw(Image* img, int globalWidth, int globalHeight);
		void drawStroke(Image* buffer, int globalWidth, int globalHeight);
		
		void setX(double x);
		double getX();

		void setY(double y);
		double getY();

		void setXRadius(double rx);
		double getXRadius();

		void setYRadius(double ry);
		double getYRadius();

	protected:
		void applyTransform();
		
	private:
		double cx = 0;
		double cy = 0;
		double rx = 0;
		double ry = 0;
		void drawTransformed(Image* buffer, int globalWidth, int globalHeight);
		void drawStrokeTransformed(Image* buffer, int globalWidth, int globalHeight);
		
	};

	class VectorLine : public VectorShape
	{
	public:

		/**
		 * 	XML Format:
		 * 		<line x1="" x2="" y1="" y2=""/>
		 * 		x1 = start x
		 * 		x2 = end x
		 * 		y1 = start y
		 * 		y2 = end y
		 * 
		 * NOTE:
		 * 		Fill rule doesn't affect this object
		 * 		!! Line cap does affect this object !!
		 * 		Line join doesn't affect this object
		 */ 
		VectorLine();
		~VectorLine();

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;

		void draw(Image* img, int globalWidth, int globalHeight);
		void drawStroke(Image* buffer, int globalWidth, int globalHeight);
		
		void setX1(double x1);
		double getX1();

		void setY1(double y1);
		double getY1();

		void setX2(double x2);
		double getX2();

		void setY2(double y2);
		double getY2();

	protected:
		void applyTransform();

	private:
		double x1 = 0;
		double x2 = 0;
		double y1 = 0;
		double y2 = 0;
	};

	class VectorPolyline : public VectorShape
	{
	public:

		/**
		 * 	XML Format:
		 * 		<polyline points=""/>
		 * 		points = "x1,y1 x2,y2 x3,y3 .... xn,yn"
		 * 		there can be a space after the comma
		 * 		must be divisable by 2 however
		 * 		the space after y must be there. No comma after y
		 * 
		 * 	Notes:
		 * 		Just like polygon but the last point is not connected to the first.
		 * 		When filling, treat it as if they are connected.
		 * 		when filling these shapes, start from min point and when you pass a line, fill
		 * 
		 * NOTE:
		 * 		!! Fill rule does affect this object !!
		 * 		!! Line cap does affect this object !!
		 * 		!! Line join does affect this object !!
		 */ 
		VectorPolyline();
		~VectorPolyline();

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;

		void draw(Image* img, int globalWidth, int globalHeight);
		void drawStroke(Image* buffer, int globalWidth, int globalHeight);
		
		void addPoint(double x, double y);
		void addPoint(Vec2f p);
		double getPointX(int index);
		double getPointY(int index);
		Vec2f getPoint(int index);
		
		void clear();
		int size();

	protected:
		void applyTransform();
		
	private:
		std::vector<Vec2f> points = std::vector<Vec2f>();
	};

	class VectorPolygon : public VectorShape
	{
	public:

		/**
		 * 	XML Format:
		 * 		<polygon points=""/>
		 * 		points = "x1,y1 x2,y2 x3,y3 .... xn,yn"
		 * 		there can be a space after the comma
		 * 		must be divisable by 2 however
		 * 		the space after y must be there. No comma after y
		 * 
		 * 	Notes:
		 * 		Just like polyline but the last point is always connected to the first.
		 * 		when filling these shapes, start from min point and when you pass a line, fill
		 * 
		 * NOTE:
		 * 		!! Fill rule does affect this object !!
		 * 		!! Line cap does affect this object !!
		 * 		!! Line join does affect this object !!
		 */ 
		VectorPolygon();
		~VectorPolygon();

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;

		void draw(Image* img, int globalWidth, int globalHeight);
		void drawStroke(Image* buffer, int globalWidth, int globalHeight);
		
		void addPoint(double x, double y);
		void addPoint(Vec2f p);
		double getPointX(int index);
		double getPointY(int index);
		Vec2f getPoint(int index);
		
		void clear();
		int size();

	protected:
		void applyTransform();
		
	private:
		std::vector<Vec2f> points = std::vector<Vec2f>();
	};

	struct PathCommand
	{
		unsigned char c;
		std::vector<Vec2f> points;
	};

	class VectorPath : public VectorShape
	{
	public:

		/**
		 * 	XML Format:
		 * 		<path d=""/>
		 * 		d contains the data and commands.
		 * 		(may contain spaces between the commands and data)
		 * 		(also note that lowercase means relative to the previous location)
		 * 		M X,Y = move to x, y. Ex: M20,30
		 * 		L X,Y = line to x, y. Ex: L50,50
		 * 		H x = horizontal line to x. Ex: H0
		 * 		V y = vertical line to y. Ex: V0
		 * 		Z = close path. (Z and z are the same in this case)
		 * 			Goes back to the first point
		 * 
		 * 		C x1 y1, x2 y2, x y = Cubic bezier curve.
		 * 			start of the curve is the previous point, x1 y1 is the control for first point
		 * 			x y is the end point. x2 y2 is the control for the end point
		 * 			
		 * 		S x2 y2, x y = Cubic bezier curve (Shorthand)
		 * 			Assumes that x1 y1 are the reflection of x2 y2 across the start point
		 * 			x1 = startX + (startX - previousX2)
		 * 			y1 = startY + (startY - previousY2)
		 * 		
		 * 		Q x1 y1, x y = Quadratic bezier curve
		 * 			start of the curve is the previous point, x1 y1 is the control point
		 * 			x y is the end point
		 * 		
		 * 		T x y = Quadratic bezier curve (Shorthand)
		 * 			similar to S, assumes x1 y1 are the reflection of the previous control point
		 * 				across the start point.
		 * 			x1 = startX + (startX - previousX1)
		 * 			y1 = startY + (startY - previousY1)
		 * 		
		 * 		A rx ry xrot largeArcFlag sweepFlag x y = Arc
		 * 			rx = x radius
		 * 			ry = y radius
		 * 			xRot = the rotation of the ellipse
		 * 			largeArcFlag = should angle be greater than 180 flag
		 * 			sweepFlag = which direction. (positive or negative/clockwise or counter clockwise)
		 * 			x = x endPoint
		 * 			y = y endPoint
		 * 			
		 * 			The combination of both flag determines which circle to use
		 * 			There are 2 circles which are reflected over the start point and end point
		 * 			The start point is the previous end point
		 * 			The end point is specified by x,y
		 * 			May need to expand the ellipse so that they both make contact with the start and end points
		 * 			Solve some system of equations to do that.
		 * 
		 * NOTE:
		 * 		!! Fill rule does affect this object !!
		 * 		!! Line cap does affect this object !!
		 * 		!! Line join does affect this object !!
		 */ 
		VectorPath();
		VectorPath(const VectorPath& other);
		void operator=(VectorPath& other);
		~VectorPath();

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;

		void draw(Image* img, int globalWidth, int globalHeight);
		void drawStroke(Image* buffer, int globalWidth, int globalHeight);

		void addMoveTo(double x, double y);
		void addMoveTo(Vec2f p);
		void addMoveToRel(double x, double y);
		void addMoveToRel(Vec2f p);

		void addLineTo(double x, double y);
		void addLineTo(Vec2f p);
		void addLineToRel(double x, double y);
		void addLineToRel(Vec2f p);

		void addHorizontalTo(double x);
		void addHorizontalToRel(double x);

		void addVerticalTo(double y);
		void addVerticalToRel(double y);

		void addClosePath();

		void addQuadTo(double x1, double y1, double x2, double y2);
		void addQuadTo(Vec2f p1, Vec2f p2);

		void addQuadToRel(double x1, double y1, double x2, double y2);
		void addQuadToRel(Vec2f p1, Vec2f p2);

		void addQuadToShort(double x, double y);
		void addQuadToShort(Vec2f p);

		void addQuadToShortRel(double x, double y);
		void addQuadToShortRel(Vec2f p);

		void addCubicTo(double x1, double y1, double x2, double y2, double x3, double y3);
		void addCubicTo(Vec2f p1, Vec2f p2, Vec2f p3);

		void addCubicToRel(double x1, double y1, double x2, double y2, double x3, double y3);
		void addCubicToRel(Vec2f p1, Vec2f p2, Vec2f p3);

		void addCubicToShort(double x1, double y1, double x2, double y2);
		void addCubicToShort(Vec2f p1, Vec2f p2);

		void addCubicToShortRel(double x1, double y1, double x2, double y2);
		void addCubicToShortRel(Vec2f p1, Vec2f p2);

		void addArcTo(double rx, double ry, double angle, bool largeArcFlag, bool sweepFlag, double x, double y);
		void addArcTo(Vec2f radi, double angle, bool largeArcFlag, bool sweepFlag, Vec2f p);
		void addArcToRel(double rx, double ry, double angle, bool largeArcFlag, bool sweepFlag, double x, double y);
		void addArcToRel(Vec2f radi, double angle, bool largeArcFlag, bool sweepFlag, Vec2f p);
		
		void clear();
		int size();

	protected:
		void applyTransform();
		
	private:
		std::vector<PathCommand> commands = std::vector<PathCommand>();
		std::vector<Vec2f> getArcStuff(Vec2f radi, double angle, Vec2f p1, Vec2f p2);
		
		void drawLineTo(Vec2f currentPos, PathCommand command, int minY, int maxY, std::vector<criticalPoint>* scanLines, std::vector<int>* strokeScanLines, bool relative);
		void drawVerticalTo(Vec2f currentPos, PathCommand command, int minY, int maxY, std::vector<criticalPoint>* scanLines, std::vector<int>* strokeScanLines, bool relative);
		void drawHorizontalTo(Vec2f currentPos, PathCommand command, int minY, int maxY, std::vector<criticalPoint>* scanLines, std::vector<int>* strokeScanLines, bool relative);
		void drawQuadTo(Vec2f currentPos, PathCommand command, int minY, int maxY, std::vector<criticalPoint>* scanLines, std::vector<int>* strokeScanLines, bool relative);
		void drawQuadShortTo(Vec2f currentPos, Vec2f extraPos, PathCommand command, int minY, int maxY, std::vector<criticalPoint>* scanLines, std::vector<int>* strokeScanLines, bool relative);
		void drawCubicTo(Vec2f currentPos, PathCommand command, int minY, int maxY, std::vector<criticalPoint>* scanLines, std::vector<int>* strokeScanLines, bool relative);
		void drawCubicShortTo(Vec2f currentPos, Vec2f extraPos, PathCommand command, int minY, int maxY, std::vector<criticalPoint>* scanLines, std::vector<int>* strokeScanLines, bool relative);
		
		void drawArcTo(Vec2f currentPos, PathCommand command, int minY, int maxY, std::vector<criticalPoint>* scanLines, std::vector<int>* strokeScanLines, bool relative);
		void drawCloseTo(Vec2f currentPos, Vec2f closePoint, int minY, int maxY, std::vector<criticalPoint>* scanLines, std::vector<int>* strokeScanLines);
		
		void copy(VectorPath& other);
	};

	class VectorGraphic : public Object
	{
	public:

		/**
		 * Some Notes:
		 * 		These are the basic shapes defined in an SVG. There are other shapes and stuff however, the format used should be similar.
		 * 		Path is a bezier curve or line or arc.
		 * 		Polygon always connects the first point to the last. Can be filled
		 * 		Polyline does not connect the first point to the last. Can be filled
		 * 		Different Transforms can be applied to different objects.
		 * SVG Notes:
		 * 		Uses XML to specify information about the graphic.
		 * 		Width and Height are specified. May have additional metrics on them though.
		 * 		Formating of the xml will be specified in its corresponding class.
		 * 
		 * XML format:
		 * 		<svg x="??" y="??" width="??" height="??" background-color="??" viewBox="x y width height">
		 * 		Note that other tags are ignored
		 * 
		 * Transform information:
		 * 		This transform is separated from the individual objects so that this is free to be manipulated
		 * 		for drawing in an application. The transforms tied to the objects are unique to them. While they
		 * 		can be changed, they will likely come from an svg file and therefore should not be changed freely.
		 */


		static const unsigned char RECTANGLE = 1;
		static const unsigned char CIRCLE = 2;
		static const unsigned char ELLIPSE = 3;
		static const unsigned char LINE = 4;
		static const unsigned char POLYLINE = 5;
		static const unsigned char POLYGON = 6;
		static const unsigned char PATH = 7;

		//future implementation
		static const unsigned char TEXT = 8;
		static const unsigned char IMAGE = 9;

		VectorGraphic(int width=0, int height=0);
		VectorGraphic(const VectorGraphic& other);
		void operator=(const VectorGraphic& other);
		~VectorGraphic();

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;

		void addShape(VectorShape* k);
		void clearShapes();

		void draw(Image* buffer);

		void setTransform(Mat3f m);
		Mat3f getTransform();

		void setViewBox(Mat3f m);
		Mat3f getViewBox();

		int getWidth();
		int getHeight();

		//save and load with Xml File
		bool save(std::string filename);
		bool load(std::string filename);

		//save and load with XmlNode
		bool load(XmlNode* svgParentNode);
		XmlNode* writeAsXmlNode();

		static void setAALevel(unsigned char level);
		static unsigned char getAALevel();

	private:
		std::vector<VectorShape*> shapes = std::vector<VectorShape*>();

		void copy(const VectorGraphic& other);

		void loadNode(XmlNode* parentNode, VectorShape* groupPointer = nullptr);

		void loadTransforms(XmlNode* node, VectorShape* shape, VectorShape* groupPointer = nullptr);
		void loadRectangle(XmlNode* node, VectorRectangle* shape);
		void loadCircle(XmlNode* node, VectorCircle* shape);
		void loadEllipse(XmlNode* node, VectorEllipse* shape);
		void loadLine(XmlNode* node, VectorLine* shape);
		void loadPolygon(XmlNode* node, VectorPolygon* shape);
		void loadPolyline(XmlNode* node, VectorPolyline* shape);
		void loadPath(XmlNode* node, VectorPath* shape);
		
		int width = 0;
		int height = 0;

		//transforms
		Mat3f transform = Mat3f::getIdentity();
		Mat3f viewBox = Mat3f::getIdentity();

		static double toNumber(std::string value, bool* percentage);
		static Color toColor(std::string value);
		static unsigned char AA_LEVEL;
	};

} //NAMESPACE glib END