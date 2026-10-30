#pragma once
#include <vector>
#include "StandardTypes.h"
#include "Vec2.h"
#include "Line.h"
#include "BinarySearchTree.h"

namespace smpl
{
	class DCELVertex;
	class DCELEdge;
	class DCELFace;

	class DCELVertex
	{
	public:
		DCELVertex(){}
		DCELVertex(Vec2f position, DCELEdge* incidentEdge)
		{
			this->position = position;
			this->incidentEdge = incidentEdge;
		}
		~DCELVertex(){}

		DCELEdge* incidentEdge = nullptr;
		Vec2f position;

		bool operator<(const DCELVertex& other)
		{
			if(position.x == other.position.x)
				return position.y < other.position.y;
			return position.x < other.position.x;
		}
		bool operator<=(const DCELVertex& other)
		{
			if(position.x == other.position.x)
				return position.y <= other.position.y;
			return position.x <= other.position.x;
		}
		bool operator>(const DCELVertex& other)
		{
			if(position.x == other.position.x)
				return position.y > other.position.y;
			return position.x > other.position.x;
		}
		bool operator>=(const DCELVertex& other)
		{
			if(position.x == other.position.x)
				return position.y >= other.position.y;
			return position.x >= other.position.x;
		}
		bool operator==(const DCELVertex& other)
		{
			return position.x == other.position.x && position.y == other.position.y;
		}
		bool operator!=(const DCELVertex& other)
		{
			return position.x != other.position.x || position.y != other.position.y;
		}
	};

	class DCELFace
	{
	public:
		static const bool COUNTER_CLOCKWISE = true;
		static const bool CLOCKWISE = false;
		DCELFace(){}
		DCELFace(DCELEdge* outer, bool edgeOrder)
		{
			this->edgeOrder = edgeOrder;
			outerComponent = outer;
		}
		~DCELFace(){}
		
		bool edgeOrder = COUNTER_CLOCKWISE;
		DCELEdge* outerComponent = nullptr;
	};

	class DCELEdge
	{
	public:
		DCELEdge(){}
		DCELEdge(DCELVertex* position, DCELEdge* twin, DCELFace* incidentFace, DCELEdge* next, DCELEdge* previous)
		{
			origin = position;
			this->twin = twin;
			this->incidentFace = incidentFace;
			this->next = next;
			this->previous = previous;
			position->incidentEdge = this;
		}
		~DCELEdge(){}

		Line getLine()
		{
			//rule. Assuming nothing went wrong and you aren't deleting things randomly for some ungodly reason,
			//	origin and twin will ALWAYS be valid.
			//	These will be changed to something like unique pointers / weak pointers or whatever
			return Line(origin->position, twin->origin->position);
		}

		DCELVertex* origin = nullptr;
		DCELEdge* twin = nullptr;
		DCELFace* incidentFace = nullptr;
		DCELEdge* next = nullptr;
		DCELEdge* previous = nullptr;
	};

	class DCEL
	{
	public:
		DCEL();
		~DCEL();

		void addPolygon(const std::vector<Vec2f>& points);

		/**
		* @brief Adds a vertex to the DCEL and returns the pointer to that vertex.
		*       A simple approach to adding a vertex that does not require as much information nor is it as complicated.
		*       If it self intersects with an existing vertex, a new vertex is not added and that existing vertex is returned
		*       If the vertex intersects with an existing edge, that edge is split into 2 automatically.
		*   
		* @param coord
		*       The coordinates of the vertex.
		* @return DCELVertex*
		*/
		DCELVertex* addVertex(const Vec2f& coord);

		/**
		* @brief Adds an edge using an origin vertex and the next vertex.
		*      A simple approach to adding an edge that does not require directly understanding the underlying DCEL.
		*      Adds a twin edge automatically.
		*      If possible, fills out the previous edge and next edge
		*      If this edge completes a face (that is it fully connects a face), automatically create a new face.
		*      If this edge intersects another, automatically add a new vertex at that intersection.
		*      Note that it is highly desirable that the edges all move in the same (counter)clockwise order.
		*          The twin edge will always be in the opposite direction
		* 
		* @param origin
		*      The start point of this edge
		* @param nextVertex
		*      The end point of this edge
		*/
		DCELEdge* addEdge(DCELVertex* origin, DCELVertex* nextVertex);

		DCELVertex* getVertex(size_t index)
		{
			return allVerticies[index];
		}
		DCELEdge* getEdge(size_t index)
		{
			return allEdges[index];
		}
		DCELFace* getFace(size_t index)
		{
			return allFaces[index];
		}

		size_t getTotalVertices()
		{
			return allVerticies.size();
		}
		size_t getTotalEdges()
		{
			return allEdges.size();
		}
		size_t getTotalFaces()
		{
			return allFaces.size();
		}

		void generateFaces();
	private:

		/**
		* @brief Assuming the edge is intersecting with the vertex, split into 2 such that
		*      there are 2 edges:
		*          origin -> newVertex
		*          newVertex -> next.origin
		*      Do this for the twin edge too while making sure the previous, next, and twin edges are correct
		*/
		void splitEdgeByVertex(DCELEdge* e, DCELVertex* newVertex);

		/**
		* @brief Splits the intersecting edges to remove the existing intersection if it exists.
		*      Automatically adds a new vertex.
		*      If no intersection exists, returns early.
		*/
		void intersectEdges(DCELEdge* e1, DCELEdge* e2);
		
		/**
		* @brief Adds a face IFF the bounding edges are fully connected.
		*      If ANY vertex on this face is inside of another face, the entire face should also be inside of that face.
		*          Note that we are tracing the outer boundary so if any intersections exist, that should create a new face
		*          where that face should be processed separately.
		*/
		void addFace(DCELEdge* e1);

		BinarySearchTree<DCELVertex> tree = BinarySearchTree<DCELVertex>(BinarySearchTree<DCELVertex>::RED_BLACK);

		std::vector<DCELVertex*> allVerticies;
		std::vector<DCELEdge*> allEdges;
		std::vector<DCELEdge*> allTwinEdges;
		std::vector<DCELFace*> allFaces;
		
	};
}