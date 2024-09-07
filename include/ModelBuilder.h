#pragma once
#include "BuildOptions.h"
#include "Model.h"

namespace smpl
{
    class DLL_OPTION ModelBuilder
    {
    public:
        static Model* createSphere(double radius, int steps, bool faceNormals = true);
        static Model* createBox(double xScale, double yScale, double zScale, bool faceNormals = true);
        static Model* createEllipsoid(double xRadius, double yRadius, double zRadius, int steps, bool faceNormals = true);
        static Model* createCylinder(double radius, double zScale, int steps, bool faceNormals = true);
        static Model* createCapsule(double radius, double zScale, int steps, bool faceNormals = true);
    private:
    };
}