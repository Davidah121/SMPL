#pragma once
#include <vector>
#include "StandardTypes.h"
#include "Vec2f.h"
#include "Line.h"

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
    };

    class DCELFace
    {
    public:
        static const bool WINDING_CLOCKWISE = false;
        static const bool WINDING_COUNTER_CLOCKWISE = true;
        DCELFace(){}
        DCELFace(DCELEdge* outer, std::vector<DCELEdge*> inner)
        {
            outerComponent = outer;
            innerComponents = inner;
        }
        ~DCELFace(){}
        
        DCELEdge* outerComponent = nullptr;
        std::vector<DCELEdge*> innerComponents;
        bool windingOrder = false;
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
        }
        ~DCELEdge(){}

        Line getLine()
        {
            if(origin != nullptr && next != nullptr && next->origin != nullptr)
                return Line(origin->position, next->origin->position);
            throw std::runtime_error("Invalid DCEL Edge. Missing an origin, next edge, or the next edge is missing an origin.");
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
        DCELVertex* addVertex(Vec2f coord);

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

        DCELFace* getRootFace()
        {
            return allFaces.front();
        }
    private:

        void addVerticesFromPolygon(const std::vector<Vec2f>& points);
        void addEdgesFromPolygon(const std::vector<Vec2f>& points, size_t startIndexOfNewVertices, size_t startIndexOfNewEdges);
        void addFacesFromPolygon(size_t startIndexOfNewEdges);

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

        std::vector<DCELVertex*> allVerticies;
        std::vector<DCELEdge*> allEdges;
        std::vector<DCELFace*> allFaces;
        
    };
}