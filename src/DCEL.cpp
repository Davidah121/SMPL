#include "DCEL.h"
#include "StringTools.h"

namespace smpl
{
    DCEL::DCEL()
    {
        //add the most outer face that contains all the inner faces.
        //this one is special and is never connected to any outerComponent.
        //The root face as you might call it
        allFaces.push_back(new DCELFace(nullptr, {}));
    }

    DCEL::~DCEL()
    {
        for(DCELVertex* vP : allVerticies)
        {
            if(vP != nullptr)
                delete vP;
        }
        for(DCELEdge* eP : allEdges)
        {
            if(eP != nullptr)
                delete eP;
        }
        for(DCELFace* fP : allFaces)
        {
            if(fP != nullptr)
                delete fP;
        }
        allVerticies.clear();
        allEdges.clear();
        allFaces.clear();
    }

    void DCEL::addPolygon(const std::vector<Vec2f>& points)
    {
        size_t startIndexOfNewVertices = allVerticies.size();
        size_t startIndexOfNewEdges = allEdges.size();

        //determine if there is self intersection or intersection with the existing edges
        //for those cases, ..... add new points/split/whatever else you need to do. Do this later

        addVerticesFromPolygon(points);
        addEdgesFromPolygon(points, startIndexOfNewVertices, startIndexOfNewEdges);
        addFacesFromPolygon(startIndexOfNewEdges);

        //determine if new face is inside of other faces and update accordingly
    }

    void DCEL::addVerticesFromPolygon(const std::vector<Vec2f>& points)
    {
        //add all vertices
        for(Vec2f c : points)
        {
            allVerticies.push_back(new DCELVertex(c, nullptr)); //no edge yet
        }
    }

    void DCEL::addEdgesFromPolygon(const std::vector<Vec2f>& points, size_t startIndexOfNewVertices, size_t startIndexOfNewEdges)
    {
        //add edges connecting them to vertices
        for(size_t i=0; i<points.size(); i++)
        {
            int currentVert = startIndexOfNewVertices + i;
            int nextVert = startIndexOfNewVertices + (i+1) % points.size();
            DCELEdge* newEdge = new DCELEdge(allVerticies[currentVert], nullptr, nullptr, nullptr, nullptr);
            DCELEdge* twinEdge = new DCELEdge(allVerticies[nextVert], nullptr, nullptr, nullptr, nullptr);
            newEdge->twin = twinEdge;
            twinEdge->twin = newEdge;

            if(i > 0)
            {
                newEdge->previous = allEdges[allEdges.size()-2];
                twinEdge->previous = allEdges[allEdges.size()-1];

                allEdges[allEdges.size()-2]->next = newEdge;
                allEdges[allEdges.size()-1]->next = twinEdge;
            }
            
            allEdges.push_back(newEdge);
            allEdges.push_back(twinEdge);
        }

        //special case for first 2 edges

        allEdges[startIndexOfNewEdges + 0]->previous = allEdges[allEdges.size()-2];
        allEdges[startIndexOfNewEdges + 1]->previous = allEdges[allEdges.size()-1];

        allEdges[allEdges.size()-2]->next = allEdges[startIndexOfNewEdges + 0];
        allEdges[allEdges.size()-1]->next = allEdges[startIndexOfNewEdges + 1];
    }

    void DCEL::addFacesFromPolygon(size_t startIndexOfNewEdges)
    {
        //add faces using those edges (Skip outer faces right now)
        //trace the edges to find a face. A face is inside another face if at least one vertex is inside
        //assuming we don't allow self intersection
        //Its a polygon so the face can be described by the first edge
        
        DCELFace* newFace = new DCELFace(allEdges[startIndexOfNewEdges], {});
        allFaces.push_back(newFace); //the current polygon
        
        newFace->outerComponent->incidentFace = newFace;
        DCELEdge* startEdge = newFace->outerComponent->next;

        while(startEdge != newFace->outerComponent)
        {
            startEdge->incidentFace = newFace;
            startEdge = startEdge->next;
        }
    }

    DCELVertex* DCEL::addVertex(Vec2f coord)
    {
        //check if it self intersects with any vertex in our list
        //Can be done in O(LogN) with sorting/binary search tree
        for(size_t i=0; i<allVerticies.size(); i++)
        {
            if(allVerticies[i]->position == coord)
            {
                return allVerticies[i]; //don't add anything. Just return this vertex
            }
        }

        DCELVertex* newestVertex = new DCELVertex(coord, nullptr);
        allVerticies.push_back(newestVertex);

        //check if it intersects with existing edges.
        //Can be done in O(NLogN) or something with plane sweep
        //note that twin edges are skipped as they are handled together
        for(size_t i=0; i<allEdges.size(); i+=2)
        {
            try
            {
                Line l = allEdges[i]->getLine();
                double tValue = l.getPointAsParametricValue(coord);
                if(tValue != NAN && tValue > 0 && tValue < 1)
                {
                    //intersection found. split edge into 2. Do the same for the twin edge
                    splitEdgeByVertex(allEdges[i], newestVertex);
                }
            }
            catch(const std::exception& e)
            {
                StringTools::println("%llu - %s", i, e.what());
            }
            
        }
        return newestVertex;
    }

    DCELEdge* DCEL::addEdge(DCELVertex* v1, DCELVertex* v2)
    {
        //find the last twin edge connected to v1. Its twin is the previous
        DCELEdge* previousEdge = nullptr;
        DCELEdge* previousTwinEdge = nullptr;
        for(size_t i=1; i<allEdges.size(); i+=2)
        {
            if(allEdges[i]->origin == v1)
            {
                previousTwinEdge = allEdges[i];
                previousEdge = allEdges[i]->twin;
                break;
            }
        }

        //find what would be the next edge if it exists
        DCELEdge* nextEdge = nullptr;
        DCELEdge* nextTwinEdge = nullptr;
        for(size_t i=0; i<allEdges.size(); i+=2)
        {
            if(allEdges[i]->origin == v2)
            {
                nextEdge = allEdges[i];
                nextTwinEdge = allEdges[i]->twin;
                break;
            }
        }

        DCELEdge* newEdge = new DCELEdge(v1, nullptr, getRootFace(), nextEdge, previousEdge);
        DCELEdge* twinEdge = new DCELEdge(v2, newEdge, getRootFace(), nextTwinEdge, previousTwinEdge);

        if(previousEdge != nullptr)
        {
            previousEdge->next = newEdge;
            previousTwinEdge->next = twinEdge;
        }

        if(nextEdge != nullptr)
        {
            nextEdge->previous = newEdge;
            nextTwinEdge->previous = twinEdge;
        }

        newEdge->twin = twinEdge;
        v1->incidentEdge = newEdge;
        
        allEdges.push_back(newEdge);
        allEdges.push_back(twinEdge);

        //check for intersection between edges
        for(size_t i=0; i<allEdges.size()-2; i+=2)
        {
            intersectEdges(allEdges[i], newEdge);
        }

        //attempt to add a face for both the twin edge and outer edge
        //should be the outer face. (will fail if its not inside another object)
        //actually don't need this but keep it temporarily
        addFace(newEdge);

        //should be the inner face
        addFace(twinEdge);

        return newEdge;
    }
    
    void DCEL::splitEdgeByVertex(DCELEdge* currEdge, DCELVertex* newVertex)
    {
        //create 2 new edges. One for e and one for its twin
        DCELEdge* oldNextEdge = currEdge->next;
        DCELEdge* oldTwinEdge = currEdge->twin;
        DCELEdge* newEdge = new DCELEdge(newVertex, nullptr, currEdge->incidentFace, currEdge->next, currEdge);

        oldNextEdge->previous = newEdge;
        currEdge->next = newEdge;

        DCELEdge* newTwin = new DCELEdge(newVertex, currEdge, oldTwinEdge->incidentFace, oldTwinEdge->next, oldTwinEdge);
        oldTwinEdge->next->previous = newTwin;
        oldTwinEdge->next = newTwin;

        //update twin references
        currEdge->twin = newTwin;
        oldTwinEdge->twin = newEdge;
        newEdge->twin = oldTwinEdge;

        allEdges.push_back(newEdge);
        allEdges.push_back(newTwin);
    }

    void DCEL::intersectEdges(DCELEdge* e1, DCELEdge* e2)
    {
        //upon an intersection, add a new vertex at that intersection
        //you should now have 4 edges (and 4 twin edges) described by e1, e2, e1->next, e2->next
        //now fix the references such that the clockwise (counter-clockwise) order is maintained

        Line l1 = e1->getLine();
        Line l2 = e2->getLine();

        Vec2f intersectionPoint;
        if(!l1.getIntersection(l2, intersectionPoint))
            return; //no intersection. Ignore
        
        //now add that intersection point
        DCELVertex* newestVertex = addVertex(intersectionPoint);

        //now you have a total of 8 edges (4 normal, 4 twin) to deal with. Constant amount of work though
        DCELEdge* e1Next = e1->next;
        DCELEdge* e2Next = e2->next;

        //

    }

    void DCEL::addFace(DCELEdge* e1)
    {
        //IFF going around all of the boundary specified by following e1->next
        //note that this does not handle the infinite loop case
        DCELEdge* previousEdge = e1;
        DCELEdge* nextEdge = e1->next;
        double sumOverEdges = 0;
        while(nextEdge != e1)
        {
            if(nextEdge == nullptr)
                break;
            Vec2f p1 = previousEdge->origin->position;
            Vec2f p2 = nextEdge->origin->position;

            sumOverEdges += (p2.x-p1.x)*(p2.y+p1.y);
            
            previousEdge = nextEdge;
            nextEdge = nextEdge->next;
        }

        if(e1 != nextEdge)
        {
            //invalid.
            return;
        }

        //if sumOverEdges is negative, don't add a new face. The twin edges will add it
        if(sumOverEdges < 0)
        {
            return;
        }

        //add new face
        DCELFace* newestFace = new DCELFace(e1, {});
        newestFace->windingOrder = true;

        allFaces.push_back(newestFace);

        //add face to each edge
        e1->incidentFace = newestFace;
        nextEdge = e1->next;
        while(nextEdge != e1)
        {
            nextEdge->incidentFace = newestFace;
            nextEdge = nextEdge->next;
        }

        //lazy approach. Add to the root face instantly.
        //Should check if its in its inner components and add to those in a recursive manner
        getRootFace()->innerComponents.push_back(e1);
    }

}