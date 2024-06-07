#pragma once
#include<vector>
#include<map>
#include "Image.h"
#include "MathExt.h"
#include "SimpleXml.h"

namespace smpl
{

	struct criticalPoint
	{
		double xValue;
		Vec2f direction;
		double timeVal;
		Vec2f otherEndPoint;
		bool horizontal;
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
		static const bool NON_ZERO_RULE = true;

		static const char LINE_CAP_BUTT = 0;
		static const char LINE_CAP_SQUARE = 1;
		static const char LINE_CAP_ROUND = 2;

		static const char LINE_JOIN_MITER = 0;
		static const char LINE_JOIN_ROUND = 1;
		static const char LINE_JOIN_BEVEL = 2;

		/**
		 * @brief Construct a new VectorShape object
		 * 		A VectorShape is more of an abstract class. It should be extended using subclasses.
		 * 
		 */
		VectorShape();

		/**
		 * @brief Destroy the VectorShape object
		 * 
		 */
		virtual ~VectorShape();
		
		//Object and RootClass Stuff
		static const RootClass globalClass;
		virtual const RootClass* getClass();

		/**
		 * @brief An abstract draw function.
		 * 		The expected behaviour is the function should only draw the filled in part and not the stroke.
		 * 		Any part that is considered apart of the stroke should not be drawn.
		 * 		This maybe more complex so the stroke may still be done in this function if desired.
		 * 
		 * @param buffer 
		 * @param globalWidth 
		 * 		The maximum width allowed. This may be less than the image's width.
		 * @param globalHeight 
		 * 		The maximum height allowed. This may be less than the image's height.
		 */
		virtual void draw(Image* buffer, int globalWidth, int globalHeight);

		/**
		 * @brief An abstract draw function.
		 * 		The expected behaviour is the function should only draw the stroke of the graphic.
		 * 		This function does not needed to be implemented if the stroke is used in the normal draw function.
		 * 
		 * @param buffer 
		 * @param globalWidth 
		 * 		The maximum width allowed. This may be less than the image's width.
		 * @param globalHeight 
		 * 		The maximum height allowed. This may be less than the image's height.
		 */
		virtual void drawStroke(Image* buffer, int globalWidth, int globalHeight);

		/**
		 * @brief Sets the Fill Method for the graphic
		 * 
		 * @param value 
		 * 		Valid methods are:
		 * 			EVEN_ODD_RULE
		 * 			NON_ZERO_RULE
		 */
		void setFillMethod(bool value);

		/**
		 * @brief Gets the Fill Method for the graphic
		 * 
		 * @return true 
		 * @return false 
		 */
		bool getFillMethod();
		
		/**
		 * @brief Sets the Fill Color for the graphic
		 * 
		 * @param k 
		 */
		void setFillColor(Color k);

		/**
		 * @brief Get the Fill Color for the graphic
		 * 
		 * @return Color 
		 */
		Color getFillColor();

		/**
		 * @brief Set the Stroke Color for the graphic
		 * 
		 * @param k 
		 */
		void setStrokeColor(Color k);

		/**
		 * @brief Get the Stroke Color for the graphic
		 * 
		 * @return Color 
		 */
		Color getStrokeColor();

		/**
		 * @brief Set the Stroke Width for the graphic
		 * 
		 * @param k 
		 */
		void setStrokeWidth(double k);

		/**
		 * @brief Get the Stroke Width for the graphic
		 * 
		 * @return double 
		 */
		double getStrokeWidth();

		/**
		 * @brief Set the Line Cap for the graphic
		 * 		(NOT IMPLEMENTED BY ANY GRAPHIC CURRENTLY)
		 * 
		 * @param c 
		 */
		void setLineCap(char c);

		/**
		 * @brief Get the Line Cap for the graphic
		 * 
		 * @return char 
		 */
		char getLineCap();

		/**
		 * @brief Set the Line Join for the graphic
		 * 		(NOT IMPLEMENTED BY ANY GRAPHIC CURRENTLY)
		 * 
		 * @param c 
		 */
		void setLineJoin(char c);

		/**
		 * @brief Get the Line Join for the graphic
		 * 
		 * @return char 
		 */
		char getLineJoin();

		/**
		 * @brief Set the Transform for the graphic
		 * 
		 * @param m 
		 */
		void setTransform(Mat3f m);

		/**
		 * @brief Get the Transform for the graphic
		 * 
		 * @return Mat3f 
		 */
		Mat3f getTransform();
		
	protected:
		/**
		 * @brief A function useful for subclasses. May be removed.
		 * 
		 */
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

		/**
		 * @brief Construct a new VectorRectangle object
		 * 		Follows the SVG format for rectangles.
		 * 
		 */
		VectorRectangle();

		/**
		 * @brief Destroy the VectorRectangle object
		 * 
		 */
		~VectorRectangle();

		//Object and RootClass Stuff
		static const RootClass globalClass;
		virtual const RootClass* getClass();

		/**
		 * @brief Draws the VectorRectangle
		 * 		Also draws the stroke.
		 * 
		 * @param buffer 
		 * @param globalWidth 
		 * 		The maximum width allowed. This may be less than the image's width.
		 * @param globalHeight 
		 * 		The maximum height allowed. This may be less than the image's height.
		 */
		void draw(Image* buffer, int globalWidth, int globalHeight);

		/**
		 * @brief Draws the stroke for the VectorRectangle
		 * 		If the shape has any transform other than the Identity matrix, this should be called.
		 * 
		 * @param buffer 
		 * @param globalWidth 
		 * 		The maximum width allowed. This may be less than the image's width.
		 * @param globalHeight 
		 * 		The maximum height allowed. This may be less than the image's height.
		 */
		void drawStroke(Image* buffer, int globalWidth, int globalHeight);

		/**
		 * @brief Sets the x value of the VectorRectangle
		 * 
		 * @param x 
		 */
		void setX(double x);

		/**
		 * @brief Gets the x value of the VectorRectangle
		 * 
		 * @return double 
		 */
		double getX();

		/**
		 * @brief Sets the y value of the VectorRectangle
		 * 
		 * @param y 
		 */
		void setY(double y);

		/**
		 * @brief Gets the y value of the VectorRectangle
		 * 
		 * @return double 
		 */
		double getY();

		/**
		 * @brief Sets the rx value of the VectorRectangle
		 * 		rx is the x radius for rounded corners
		 * 
		 * @param rx 
		 */
		void setRX(double rx);

		/**
		 * @brief Gets the rx value of the VectorRectangle
		 * 
		 * @return double 
		 */
		double getRX();

		/**
		 * @brief Sets the ry value of the VectorRectangle
		 * 		ry is the y radius for rounded corners
		 * 
		 * @param ry 
		 */
		void setRY(double ry);

		/**
		 * @brief Gets the ry value of the VectorRectangle
		 * 
		 * @return double 
		 */
		double getRY();

		/**
		 * @brief Sets the width of the VectorRectangle
		 * 
		 * @param width 
		 */
		void setWidth(double width);

		/**
		 * @brief Gets the width of the VectorRectangle
		 * 
		 * @return double 
		 */
		double getWidth();

		/**
		 * @brief Sets the height of the VectorRectangle
		 * 
		 * @param width 
		 */
		void setHeight(double height);

		/**
		 * @brief Gets the height of the VectorRectangle
		 * 
		 * @return double 
		 */
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

		/**
		 * @brief Construct a new VectorCircle object
		 * 		Follows the SVG format for circles.
		 * 
		 */
		VectorCircle();

		/**
		 * @brief Destroy the VectorCircle object
		 * 
		 */
		~VectorCircle();

		//Object and RootClass Stuff
		static const RootClass globalClass;
		virtual const RootClass* getClass();

		/**
		 * @brief Draws the VectorCircle
		 * 
		 * @param img 
		 * @param globalWidth 
		 * 		The maximum width allowed. This may be less than the image's width.
		 * @param globalHeight 
		 * 		The maximum height allowed. This may be less than the image's height.
		 */
		void draw(Image* img, int globalWidth, int globalHeight);
		void testDraw(Image* img, int globalWidth, int globalHeight);

		/**
		 * @brief Draws the stroke of the VectorCircle
		 * 
		 * @param img 
		 * @param globalWidth 
		 * 		The maximum width allowed. This may be less than the image's width.
		 * @param globalHeight 
		 * 		The maximum height allowed. This may be less than the image's height.
		 */
		void drawStroke(Image* buffer, int globalWidth, int globalHeight);
		
		/**
		 * @brief Sets the x value of the center of the VectorCircle
		 * 
		 * @param x 
		 */
		void setX(double x);

		/**
		 * @brief Gets the x value for the center of the VectorCircle
		 * 
		 * @return double 
		 */
		double getX();

		/**
		 * @brief Sets the y value of the center of the VectorCircle
		 * 
		 * @param y 
		 */
		void setY(double y);

		/**
		 * @brief Gets the y value for the center of the VectorCircle
		 * 
		 * @return double 
		 */
		double getY();

		/**
		 * @brief Sets the radius of the VectorCircle
		 * 
		 * @param x 
		 */
		void setRadius(double r);

		/**
		 * @brief Gets the radius of the VectorCircle
		 * 
		 * @return double 
		 */
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

		/**
		 * @brief Construct a new VectorEllipse object
		 * 		Follows the SVG format for an ellipse
		 * 
		 */
		VectorEllipse();

		/**
		 * @brief Destroy the VectorEllipse object
		 * 
		 */
		~VectorEllipse();

		//Object and RootClass Stuff
		static const RootClass globalClass;
		virtual const RootClass* getClass();

		/**
		 * @brief Draws a VectorEllipse
		 * 
		 * @param img 
		 * @param globalWidth 
		 * 		The maximum width allowed. This may be less than the image's width.
		 * @param globalHeight 
		 * 		The maximum height allowed. This may be less than the image's height.
		 */
		void draw(Image* img, int globalWidth, int globalHeight);

		/**
		 * @brief Draws the stroke for a VectorEllipse
		 * 
		 * @param buffer 
		 * @param globalWidth 
		 * 		The maximum width allowed. This may be less than the image's width.
		 * @param globalHeight 
		 * 		The maximum height allowed. This may be less than the image's height.
		 */
		void drawStroke(Image* buffer, int globalWidth, int globalHeight);
		
		/**
		 * @brief Sets the x value of the center of the Ellipse
		 * 
		 * @param x 
		 */
		void setX(double x);

		/**
		 * @brief Gets the x value of the center of the Ellipse
		 * 
		 * @return double 
		 */
		double getX();

		/**
		 * @brief Sets the y value of the center of the Ellipse
		 * 
		 * @param y 
		 */
		void setY(double y);

		/**
		 * @brief Gets the y value of the center of the Ellipse
		 * 
		 * @return double 
		 */
		double getY();

		/**
		 * @brief Sets the x radius for the Ellipse
		 * 
		 * @param ry 
		 */
		void setXRadius(double rx);

		/**
		 * @brief Gets the x radius for the Ellipse
		 * 
		 * @return double 
		 */
		double getXRadius();

		/**
		 * @brief Sets the y radius for the Ellipse
		 * 
		 * @param ry 
		 */
		void setYRadius(double ry);

		/**
		 * @brief Gets the y radius for the Ellipse
		 * 
		 * @return double 
		 */
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

		/**
		 * @brief Construct a new VectorLine object
		 * 		Follows the SVG format for a line
		 * 
		 */
		VectorLine();

		/**
		 * @brief Destroy the VectorLine object
		 * 
		 */
		~VectorLine();

		//Object and RootClass Stuff
		static const RootClass globalClass;
		virtual const RootClass* getClass();

		/**
		 * @brief Draws a VectorLine
		 * 		In reality, this does not draw anything because only the stroke for the line is drawn.
		 * 
		 * @param img 
		 * @param globalWidth 
		 * 		The maximum width allowed. This may be less than the image's width.
		 * @param globalHeight 
		 * 		The maximum height allowed. This may be less than the image's height.
		 */
		void draw(Image* img, int globalWidth, int globalHeight);

		/**
		 * @brief Draws the stroke for the VectorLine
		 * 		This is the only function that draws anything for the line.
		 * 
		 * @param img 
		 * @param globalWidth 
		 * 		The maximum width allowed. This may be less than the image's width.
		 * @param globalHeight 
		 * 		The maximum height allowed. This may be less than the image's height.
		 */
		void drawStroke(Image* buffer, int globalWidth, int globalHeight);
		
		/**
		 * @brief Sets the start x value for the line.
		 * 
		 * @param x1 
		 */
		void setX1(double x1);

		/**
		 * @brief Gets the start x value for the line.
		 * 
		 * @return double 
		 */
		double getX1();

		/**
		 * @brief Sets the start y value for the line.
		 * 
		 * @param y1 
		 */
		void setY1(double y1);

		/**
		 * @brief Gets the start y value for the line.
		 * 
		 * @return double 
		 */
		double getY1();

		/**
		 * @brief Sets the end x value for the line.
		 * 
		 * @param x2 
		 */
		void setX2(double x2);

		/**
		 * @brief Gets the end x value for the line.
		 * 
		 * @return double 
		 */
		double getX2();

		/**
		 * @brief Sets the end y value for the line.
		 * 
		 * @param y2 
		 */
		void setY2(double y2);

		/**
		 * @brief Gets the end y value for the line.
		 * 
		 * @return double 
		 */
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

		/**
		 * @brief Construct a new VectorPolyline object
		 * 		Follows the SVG format for a polyline
		 * 		Polylines behaves like a polygon but the last point is not connected to the first.
		 * 
		 */
		VectorPolyline();

		/**
		 * @brief Destroy the VectorPolyline object
		 * 
		 */
		~VectorPolyline();

		//Object and RootClass Stuff
		static const RootClass globalClass;
		virtual const RootClass* getClass();

		/**
		 * @brief Draws the VectorPolyline.
		 * 		Specifically draws just the fill for the polyline.
		 * 
		 * @param img 
		 * @param globalWidth 
		 * 		The maximum width allowed. This may be less than the image's width.
		 * @param globalHeight 
		 * 		The maximum height allowed. This may be less than the image's height.
		 */
		void draw(Image* img, int globalWidth, int globalHeight);

		/**
		 * @brief Draws the stroke for the VectorPolyline.
		 * 
		 * @param img 
		 * @param globalWidth 
		 * 		The maximum width allowed. This may be less than the image's width.
		 * @param globalHeight 
		 * 		The maximum height allowed. This may be less than the image's height.
		 */
		void drawStroke(Image* buffer, int globalWidth, int globalHeight);
		
		/**
		 * @brief Adds a new point to the VectorPolyline
		 * 
		 * @param x 
		 * @param y 
		 */
		void addPoint(double x, double y);

		/**
		 * @brief Adds a new point to the VectorPolyline
		 * 
		 * @param p 
		 */
		void addPoint(Vec2f p);

		/**
		 * @brief Gets the x value of a point based on the index.
		 * 
		 * @param index 
		 * @return double 
		 */
		double getPointX(int index);

		/**
		 * @brief Gets the y value of a point based on the index.
		 * 
		 * @param index 
		 * @return double 
		 */
		double getPointY(int index);

		/**
		 * @brief Gets a point based on the index.
		 * 
		 * @param index 
		 * @return Vec2f 
		 */
		Vec2f getPoint(int index);
		
		/**
		 * @brief Clears all points
		 * 
		 */
		void clear();

		/**
		 * @brief Returns the amount of points in the VectorPolyline
		 * 
		 * @return int 
		 */
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

		/**
		 * @brief Construct a new VectorPolygon object
		 * 		Follows the SVG format for a polygon
		 */
		VectorPolygon();

		/**
		 * @brief Destroy the VectorPolygon object
		 * 
		 */
		~VectorPolygon();

		//Object and RootClass Stuff
		static const RootClass globalClass;
		virtual const RootClass* getClass();

		/**
		 * @brief Draws the VectorPolygon.
		 * 
		 * @param img 
		 * @param globalWidth 
		 * 		The maximum width allowed. This may be less than the image's width.
		 * @param globalHeight 
		 * 		The maximum height allowed. This may be less than the image's height.
		 */
		void draw(Image* img, int globalWidth, int globalHeight);

		/**
		 * @brief Draws the stroke for the VectorPolygon.
		 * 
		 * @param img 
		 * @param globalWidth 
		 * 		The maximum width allowed. This may be less than the image's width.
		 * @param globalHeight 
		 * 		The maximum height allowed. This may be less than the image's height.
		 */
		void drawStroke(Image* buffer, int globalWidth, int globalHeight);
		
		/**
		 * @brief Adds a new point to the VectorPolygon
		 * 
		 * @param x 
		 * @param y 
		 */
		void addPoint(double x, double y);

		/**
		 * @brief Adds a new point to the VectorPolygon
		 * 
		 * @param p
		 */
		void addPoint(Vec2f p);

		/**
		 * @brief Gets the x value of a point based on the index.
		 * 
		 * @param index 
		 * @return double 
		 */
		double getPointX(int index);

		/**
		 * @brief Gets the y value of a point based on the index.
		 * 
		 * @param index 
		 * @return double 
		 */
		double getPointY(int index);

		/**
		 * @brief Gets a point based on the index.
		 * 
		 * @param index 
		 * @return Vec2f 
		 */
		Vec2f getPoint(int index);
		
		/**
		 * @brief Clears all points
		 * 
		 */
		void clear();

		/**
		 * @brief Returns the amount of points in the VectorPolygon
		 * 
		 * @return int 
		 */
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

		/**
		 * @brief Construct a new VectorPath object
		 * 		Follows the SVG format for a path
		 * 
		 * 		Most complex VectorGraphic that can represent any other VectorGraphic.
		 * 
		 */
		VectorPath();

		/**
		 * @brief Construct a new VectorPath object from another VectorPath
		 * 
		 * @param other 
		 */
		VectorPath(const VectorPath& other);

		/**
		 * @brief Copies a VectorPath object
		 * 
		 * @param other 
		 */
		void operator=(VectorPath& other);

		/**
		 * @brief Destroy the VectorPath object
		 * 
		 */
		~VectorPath();

		//Object and RootClass Stuff
		static const RootClass globalClass;
		virtual const RootClass* getClass();

		/**
		 * @brief Draws the VectorPath
		 * 
		 * @param img 
		 * @param globalWidth 
		 * 		The maximum width allowed. This may be less than the image's width.
		 * @param globalHeight 
		 * 		The maximum height allowed. This may be less than the image's height.
		 */
		void draw(Image* img, int globalWidth, int globalHeight);

		/**
		 * @brief Draws the stroke for the VectorPath
		 * 
		 * @param img 
		 * @param globalWidth 
		 * 		The maximum width allowed. This may be less than the image's width.
		 * @param globalHeight 
		 * 		The maximum height allowed. This may be less than the image's height.
		 */
		void drawStroke(Image* buffer, int globalWidth, int globalHeight);

		/**
		 * @brief Adds a move to command.
		 * 
		 * @param x 
		 * @param y 
		 */
		void addMoveTo(double x, double y);

		/**
		 * @brief Adds a move to command.
		 * 
		 * @param p
		 */
		void addMoveTo(Vec2f p);

		/**
		 * @brief Adds a move to relative command.
		 * 
		 * @param x 
		 * @param y 
		 */
		void addMoveToRel(double x, double y);

		/**
		 * @brief Adds a move to relative command.
		 * 
		 * @param p
		 */
		void addMoveToRel(Vec2f p);

		/**
		 * @brief Adds a line to command.
		 * 
		 * @param x 
		 * @param y 
		 */
		void addLineTo(double x, double y);

		/**
		 * @brief Adds a line to command.
		 * 
		 * @param p
		 */
		void addLineTo(Vec2f p);

		/**
		 * @brief Adds a line to relative command.
		 * 
		 * @param x 
		 * @param y 
		 */
		void addLineToRel(double x, double y);

		/**
		 * @brief Adds a line to relative command.
		 * 
		 * @param p
		 */
		void addLineToRel(Vec2f p);

		/**
		 * @brief Adds a horizontal to command.
		 * 
		 * @param x 
		 */
		void addHorizontalTo(double x);

		/**
		 * @brief Adds a horizontal to relative command.
		 * 
		 * @param x 
		 */
		void addHorizontalToRel(double x);

		/**
		 * @brief Adds a vertical to command.
		 * 
		 * @param y 
		 */
		void addVerticalTo(double y);

		/**
		 * @brief Adds a vertical to relative command.
		 * 
		 * @param y 
		 */
		void addVerticalToRel(double y);

		/**
		 * @brief Adds a close path command
		 * 
		 */
		void addClosePath();

		/**
		 * @brief Adds a quadratic to command.
		 * 		Uses a Quadratic Bezier curve
		 * 
		 * @param x1 
		 * @param y1 
		 * @param x2 
		 * @param y2 
		 */
		void addQuadTo(double x1, double y1, double x2, double y2);

		/**
		 * @brief Adds a quadratic to command.
		 * 		Uses a Quadratic Bezier curve
		 * 
		 * @param p1 
		 * @param p2
		 */
		void addQuadTo(Vec2f p1, Vec2f p2);

		/**
		 * @brief Adds a quadratic to relative command.
		 * 		Uses a Quadratic Bezier curve
		 * 
		 * @param x1 
		 * @param y1 
		 * @param x2 
		 * @param y2 
		 */
		void addQuadToRel(double x1, double y1, double x2, double y2);

		/**
		 * @brief Adds a quadratic to relative command.
		 * 		Uses a Quadratic Bezier curve
		 * 
		 * @param p1 
		 * @param p2
		 */
		void addQuadToRel(Vec2f p1, Vec2f p2);

		/**
		 * @brief Adds a short quadratic to command.
		 * 		Builds a new quadratic bezier curve using the previous one.
		 * 
		 * @param x 
		 * @param y
		 */
		void addQuadToShort(double x, double y);

		/**
		 * @brief Adds a short quadratic to command.
		 * 		Builds a new quadratic bezier curve using the previous one.
		 * 
		 * @param p
		 */
		void addQuadToShort(Vec2f p);

		/**
		 * @brief Adds a short quadratic to relative command.
		 * 		Builds a new quadratic bezier curve using the previous one.
		 * 
		 * @param x 
		 * @param y
		 */
		void addQuadToShortRel(double x, double y);

		/**
		 * @brief Adds a short quadratic to relative command.
		 * 		Builds a new quadratic bezier curve using the previous one.
		 * 
		 * @param p
		 */
		void addQuadToShortRel(Vec2f p);

		/**
		 * @brief Adds a cubic to command.
		 * 		Uses a Cubic Bezier Curve.
		 * 
		 * @param x1 
		 * @param y1 
		 * @param x2 
		 * @param y2 
		 * @param x3 
		 * @param y3 
		 */
		void addCubicTo(double x1, double y1, double x2, double y2, double x3, double y3);

		/**
		 * @brief Adds a cubic to command.
		 * 		Uses a Cubic Bezier Curve.
		 * 
		 * @param p1 
		 * @param p2 
		 * @param p3 
		 */
		void addCubicTo(Vec2f p1, Vec2f p2, Vec2f p3);

		/**
		 * @brief Adds a cubic to relative command.
		 * 		Uses a Cubic Bezier Curve.
		 * 
		 * @param x1 
		 * @param y1 
		 * @param x2 
		 * @param y2 
		 * @param x3 
		 * @param y3 
		 */
		void addCubicToRel(double x1, double y1, double x2, double y2, double x3, double y3);

		/**
		 * @brief Adds a cubic to relative command.
		 * 		Uses a Cubic Bezier Curve.
		 * 
		 * @param p1 
		 * @param p2 
		 * @param p3 
		 */
		void addCubicToRel(Vec2f p1, Vec2f p2, Vec2f p3);

		/**
		 * @brief Adds a cubic to short command.
		 * 		Builds a new cubic bezier curve using the previous one.
		 * 
		 * @param x1 
		 * @param y1 
		 * @param x2 
		 * @param y2 
		 */
		void addCubicToShort(double x1, double y1, double x2, double y2);

		/**
		 * @brief Adds a cubic to short command.
		 * 		Builds a new cubic bezier curve using the previous one.
		 * 
		 * @param p1 
		 * @param p2 
		 */
		void addCubicToShort(Vec2f p1, Vec2f p2);

		/**
		 * @brief Adds a cubic to short relative command.
		 * 		Builds a new cubic bezier curve using the previous one.
		 * 
		 * @param x1 
		 * @param y1 
		 * @param x2 
		 * @param y2 
		 */
		void addCubicToShortRel(double x1, double y1, double x2, double y2);

		/**
		 * @brief Adds a cubic to short relative command.
		 * 		Builds a new cubic bezier curve using the previous one.
		 * 
		 * @param p1 
		 * @param p2 
		 */
		void addCubicToShortRel(Vec2f p1, Vec2f p2);

		/**
		 * @brief Adds an arc to command.
		 * 
		 * @param rx 
		 * @param ry 
		 * @param angle 
		 * @param largeArcFlag 
		 * 		Whether to use the larger version of the arc when drawing.
		 * @param sweepFlag 
		 * 		Whether to go in postive or negative direction regarding the angle.
		 * 		Whether to go in the counter-clockwise or clockwise direction.
		 * @param x 
		 * @param y 
		 */
		void addArcTo(double rx, double ry, double angle, bool largeArcFlag, bool sweepFlag, double x, double y);

		/**
		 * @brief Adds an arc to command.
		 * 
		 * @param radi
		 * @param angle 
		 * @param largeArcFlag 
		 * 		Whether to use the larger version of the arc when drawing.
		 * @param sweepFlag 
		 * 		Whether to go in postive or negative direction regarding the angle.
		 * 		Whether to go in the counter-clockwise or clockwise direction.
		 * @param p
		 */
		void addArcTo(Vec2f radi, double angle, bool largeArcFlag, bool sweepFlag, Vec2f p);

		/**
		 * @brief Adds an arc to relative command.
		 * 
		 * @param rx 
		 * @param ry 
		 * @param angle 
		 * @param largeArcFlag 
		 * 		Whether to use the larger version of the arc when drawing.
		 * @param sweepFlag 
		 * 		Whether to go in postive or negative direction regarding the angle.
		 * 		Whether to go in the counter-clockwise or clockwise direction.
		 * @param x 
		 * @param y 
		 */
		void addArcToRel(double rx, double ry, double angle, bool largeArcFlag, bool sweepFlag, double x, double y);

		/**
		 * @brief Adds an arc to relative command.
		 * 
		 * @param radi
		 * @param angle 
		 * @param largeArcFlag 
		 * 		Whether to use the larger version of the arc when drawing.
		 * @param sweepFlag 
		 * 		Whether to go in postive or negative direction regarding the angle.
		 * 		Whether to go in the counter-clockwise or clockwise direction.
		 * @param p
		 */
		void addArcToRel(Vec2f radi, double angle, bool largeArcFlag, bool sweepFlag, Vec2f p);
		
		/**
		 * @brief Clears all commands from the VectorPath
		 * 
		 */
		void clear();

		/**
		 * @brief Returns the amount of commands in the VectorPath
		 * 
		 * @return int 
		 */
		int size();

		static bool oldMethod;
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

		/**
		 * @brief Construct a new VectorGraphic object
		 * 		Contains a series of VectorShapes.
		 * 		If the width <= 0 or the height <= 0, the bounds are set to the image the it is being drawn upon.
		 * 
		 * @param width 
		 * 		The maximum width of the VectorGraphic
		 * 		Default is 0
		 * @param height 
		 * 		The maximum height of the VectorGraphic
		 * 		Default is 0
		 */
		VectorGraphic(int width=0, int height=0);

		/**
		 * @brief Construct a new VectorGraphic object from another VectorGraphic object.
		 * 
		 * @param other 
		 */
		VectorGraphic(const VectorGraphic& other);

		/**
		 * @brief Copies a VectorGraphic
		 * 
		 * @param other 
		 */
		void operator=(const VectorGraphic& other);

		/**
		 * @brief Destroy the VectorGraphic object
		 * 
		 */
		~VectorGraphic();

		//Object and RootClass Stuff
		static const RootClass globalClass;
		virtual const RootClass* getClass();

		/**
		 * @brief Adds a VectorShape to the VectorGraphic
		 * 
		 * @param k 
		 */
		void addShape(VectorShape* k);

		/**
		 * @brief Clears all VectorShapes added to the VectorGraphic
		 * 		Any shape added will be deleted and should be considered invalid.
		 * 
		 */
		void clearShapes();

		/**
		 * @brief Gets a vector shape in the vector graphic.
		 * 
		 * @param index 
		 * @return VectorShape* 
		 */
		VectorShape* getShape(size_t index);

		/**
		 * @brief Returns the number of vector shapes in the vector graphic
		 * 
		 * @return size_t 
		 */
		size_t size();

		/**
		 * @brief Draws the VectorGraphic on an image.
		 * 
		 * @param buffer 
		 */
		void draw(Image* buffer);

		/**
		 * @brief Sets the Transform of the VectorGraphic
		 * 
		 * @param m 
		 */
		void setTransform(Mat3f m);

		/**
		 * @brief Gets the Transform of the VectorGraphic
		 * 
		 * @return Mat3f 
		 */
		Mat3f getTransform();

		/**
		 * @brief Sets the View Box of the VectorGraphic
		 * 
		 * @param m 
		 */
		void setViewBox(Mat3f m);

		/**
		 * @brief Gets the View Box of the VectorGraphic
		 * 
		 * @return Mat3f 
		 */
		Mat3f getViewBox();

		/**
		 * @brief Gets the Width of the VectorGraphic
		 * 
		 * @return int 
		 */
		int getWidth();

		/**
		 * @brief Gets the Height of the VectorGraphic
		 * 
		 * @return int 
		 */
		int getHeight();

		//save and load with Xml File

		/**
		 * @brief Saves the VectorGraphic to a .svg file
		 * 		(NOT IMPLEMENTED)
		 * 
		 * @param file 
		 * @return true 
		 * @return false 
		 */
		bool save(File file);

		/**
		 * @brief Loads a VectorGraphic from a .svg file
		 * 
		 * @param file 
		 * @return true 
		 * @return false 
		 */
		bool load(File file);

		//save and load with XmlNode

		/**
		 * @brief Loads a VectorGraphic using the parent svg xml node
		 * 
		 * @param svgParentNode 
		 * @return true 
		 * @return false 
		 */
		bool load(XmlNode* svgParentNode);

		/**
		 * @brief Creates an XmlNode using data from the VectorGraphic for saving
		 * 		(NOT YET IMPLEMENTED)
		 * 
		 * @return XmlNode* 
		 */
		XmlNode* writeAsXmlNode();

		/**
		 * @brief Sets the Anti-Aliasing level for VectorGraphic drawing.
		 * 		(Barely Implemented)
		 * 
		 * @param level 
		 * 		Valid values from [0, 3]
		 */
		static void setAALevel(unsigned char level);

		/**
		 * @brief Gets the Anti-Aliasing level for VectorGraphic drawing.
		 * 
		 * @return unsigned char 
		 */
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