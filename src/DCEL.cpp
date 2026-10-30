#include "DCEL.h"
#include "StringTools.h"

namespace smpl
{
	DCEL::DCEL()
	{
		//add the most outer face that contains all the inner faces.
		//this one is special and is never connected to any outerComponent.
		//The root face as you might call it
		// allFaces.push_back(new DCELFace(nullptr, {}));
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
		for(DCELEdge* eP : allTwinEdges)
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
		allTwinEdges.clear();
		allFaces.clear();
	}

	void DCEL::addPolygon(const std::vector<Vec2f>& points)
	{
		// size_t startIndexOfNewVertices = allVerticies.size();
		// size_t startIndexOfNewEdges = allEdges.size();

		//determine if there is self intersection or intersection with the existing edges
		//for those cases, ..... add new points/split/whatever else you need to do. Do this later

		// addVerticesFromPolygon(points);
		// addEdgesFromPolygon(points, startIndexOfNewVertices, startIndexOfNewEdges);
		// addFacesFromPolygon(startIndexOfNewEdges);

		//determine if new face is inside of other faces and update accordingly


		//not ideal but its fine I guess. Would like to not have to go through the process of searching for verticies and edge intersections
		//but that wouldn't be general
		auto startVPointer = addVertex(points[0]);
		auto lastVPointer = startVPointer;
		for(size_t i=1; i<points.size(); i++)
		{
			auto nextVPointer = addVertex(points[i]);
			addEdge(lastVPointer, nextVPointer);
			lastVPointer = nextVPointer;
		}
		addEdge(lastVPointer, startVPointer);

		//Ignore face stuff
	}

	DCELVertex* DCEL::addVertex(const Vec2f& coord)
	{
		//check if it self intersects with any vertex in our list
		//Can be done in O(LogN) with sorting/binary search tree
		//O(N) right now
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
		//Can be done in O(LogN) or something with plane sweep
		//note that twin edges are skipped as they are handled together
		//O(N) right now
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
		DCELEdge* nextEdge = nullptr;
		DCELEdge* nextTwinEdge = nullptr;

		for(size_t i=0; i<allTwinEdges.size(); i++)
		{
			if(allTwinEdges[i]->origin == v1)
			{
				nextTwinEdge = allTwinEdges[i];
				previousEdge = allTwinEdges[i]->twin;
				break;
			}
		}

		//find what would be the next edge if it exists
		for(size_t i=0; i<allEdges.size(); i++)
		{
			if(allEdges[i]->origin == v2)
			{
				nextEdge = allEdges[i];
				previousTwinEdge = allEdges[i]->twin;
				break;
			}
		}

		DCELEdge* newEdge = new DCELEdge(v1, nullptr, nullptr, nextEdge, previousEdge);
		DCELEdge* twinEdge = new DCELEdge(v2, newEdge, nullptr, nextTwinEdge, previousTwinEdge);
		

		if(previousEdge != nullptr)
			previousEdge->next = newEdge;
		if(nextTwinEdge != nullptr)
			nextTwinEdge->previous = twinEdge;

		if(nextEdge != nullptr)
			nextEdge->previous = newEdge;
		if(previousTwinEdge != nullptr)
			previousTwinEdge->next = twinEdge;

		newEdge->twin = twinEdge;
		v1->incidentEdge = newEdge;
		
		allEdges.push_back(newEdge);
		allTwinEdges.push_back(twinEdge);

		//check for intersection between edges
		if(allEdges.size() > 0)
		{
			for(size_t i=0; i<allEdges.size(); i++)
			{
				if(newEdge != allEdges[i])
					intersectEdges(allEdges[i], newEdge);
			}
		}

		//attempt to add a face for both the twin edge and outer edge
		//should be the outer face. (will fail if its not inside another object)
		//actually don't need this but keep it temporarily
		// addFace(newEdge);

		//should be the inner face
		// addFace(twinEdge);

		return newEdge;
	}
	
	void DCEL::splitEdgeByVertex(DCELEdge* currEdge, DCELVertex* newVertex)
	{
		//create 2 new edges. One for e and one for its twin
		DCELEdge* twinEdge = currEdge->twin;

		DCELEdge* newEdge = new DCELEdge(newVertex, nullptr, currEdge->incidentFace, currEdge->next, currEdge);
		DCELEdge* newTwin = new DCELEdge(newVertex, newEdge, twinEdge->incidentFace, twinEdge->next, twinEdge);

		newEdge->twin = newTwin;
		currEdge->next = newEdge;
		twinEdge->next = newTwin;

		if(newEdge->next != nullptr)
			newEdge->next->previous = newEdge;
		if(newTwin->next != nullptr)
			newTwin->next->previous = newTwin;
		
		allEdges.push_back(newEdge);
		allTwinEdges.push_back(newTwin);

		//NOTE: No need to use addFace() because these edges move in the same exact direction.
		// Just spliting the edge but its still tracing the same boundary.
	}

	
	///TODO: FINISH
	void DCEL::intersectEdges(DCELEdge* a1, DCELEdge* b1)
	{
		//upon an intersection, add a new vertex at that intersection
		//you should now have 4 edges (and 4 twin edges) described by e1, e2, e1->next, e2->next
		//now fix the references such that the clockwise (counter-clockwise) order is maintained
		Line l1 = a1->getLine();
		Line l2 = b1->getLine();

		double timePoint = -1;
		if(!l1.getIntersectionParametric(l2, timePoint))
			return; //no intersection. Ignore
		
		if(timePoint <= 0 || timePoint >= 1 || timePoint == NAN)
			return; //no intersection. Ignore
		
		//solve for the point of intersection which will be a new vertex likely
		Vec2f intersectionPoint = l1.getPoint1() + timePoint*l1.getToPoint();
		
		//note that if the otherside has a timepoint of > 0 && < 1, its probably still an intersection. Should instead split that line by vertex or something actually.
		//	odd case
		double timePointOther = -1;
		if(!l2.getIntersectionParametric(l1, timePointOther))
			return; //no intersection. Ignore
		if(timePointOther <= 0 || timePointOther >= 1 || timePointOther == NAN)
			return; //no intersection. Ignore
		
		//now add that intersection point
		DCELVertex* newestVertex = addVertex(intersectionPoint);

		//now you have a total of 8 edges (4 normal, 4 twin) to deal with. Constant amount of work though
		DCELEdge* a2 = a1->next;
		DCELEdge* b2 = b1->next;
		DCELEdge* a1Twin = a1->twin;
		DCELEdge* b1Twin = b1->twin;

		DCELEdge* a2Twin = nullptr;
		DCELEdge* b2Twin = nullptr;
		if(a2 != nullptr)
			a2Twin = a2->twin;
		if(b2 != nullptr)
			b2Twin = b2->twin;
		
		a1->next = b2;

		if(b2 != nullptr)
			b2->previous = a1;
		
		if(b2Twin != nullptr)
			b2Twin->next = a2;
		
		if(a2 != nullptr)
			a2->previous = b2Twin;

		if(b2Twin != nullptr)
			a2Twin->next = b1Twin;

		b1Twin->previous = a2Twin;

		b1->next = a1Twin;
		a1Twin->previous = b1;

		//in this case, the directions of the edges are changing so addFace() must be called
		a1->incidentFace = nullptr;
		a1Twin->incidentFace = nullptr;
		b1->incidentFace = nullptr;
		b1Twin->incidentFace = nullptr;
		
		//////////////////////
		///NOTE: need a special case for the end of a line colliding with the middle of another line but handle this later.
		////////////////////
	}


	void DCEL::generateFaces()
	{
		for(DCELEdge* edge : allEdges)
		{
			addFace(edge);
		}
		
		for(DCELEdge* edge : allTwinEdges)
		{
			addFace(edge);
		}
	}
	///TODO: Test? Looks complete but unsure
	void DCEL::addFace(DCELEdge* e1)
	{
		if(e1->incidentFace != nullptr)
			return; //already bound to a face.

		//going around all of the boundary specified by following e1->next
		//if it loops around back to the original edge, its a face.
		//note that this does not handle the infinite loop case
		DCELEdge* previousEdge = e1;
		DCELEdge* nextEdge = e1->next;
		double sumOverEdges = 0;

		do
		{
			if(nextEdge == nullptr)
				return;
			Vec2f p1 = previousEdge->origin->position;
			Vec2f p2 = nextEdge->origin->position;

			sumOverEdges += (p2.x-p1.x)*(p2.y+p1.y);
			
			previousEdge = nextEdge;
			nextEdge = nextEdge->next;
		} while (previousEdge != e1);

		//if sumOverEdges is negative, counter clockwise. Useful for later
		//add new face
		DCELFace* incidentFace = new DCELFace(e1, ((sumOverEdges > 0) ? DCELFace::CLOCKWISE : DCELFace::COUNTER_CLOCKWISE));
		allFaces.push_back(incidentFace);

		//add face to each edge
		e1->incidentFace = incidentFace;
		nextEdge = e1->next;
		while(nextEdge != e1)
		{
			nextEdge->incidentFace = incidentFace;
			nextEdge = nextEdge->next;
		}

		//will need to handle parenting stuff eventually. Do it here
	}

}