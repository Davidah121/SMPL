#pragma once
#include <vector>
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
        DCELFace(){}
        DCELFace(DCELEdge* outer, std::vector<DCELEdge*> inner)
        {
            outerComponent = outer;
            innerComponents = inner;
        }
        ~DCELFace(){}
        
        DCELEdge* outerComponent = nullptr;
        std::vector<DCELEdge*> innerComponents;
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

        DCELVertex* origin = nullptr;
        DCELEdge* twin = nullptr;
        DCELFace* incidentFace = nullptr;
        DCELEdge* next = nullptr;
        DCELEdge* previous = nullptr;
    };

    class DCEL
    {
    public:
        DCEL(){}
        ~DCEL()
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

        void addPolygon(std::vector<Vec2f> points)
        {
            //
        }

    private:
        std::vector<DCELVertex*> allVerticies;
        std::vector<DCELEdge*> allEdges;
        std::vector<DCELFace*> allFaces;
        
    };
}