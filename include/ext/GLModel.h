#pragma once
#include "MathExt.h"

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "Model.h"

namespace glib
{

    class GLModel
    {
    public:
        static const bool TYPE_ARRAY = false;
        static const bool TYPE_INDEXED = true;

        GLModel();
        ~GLModel();

        void draw();

        static GLModel convertModel(Model* m, int tUsage = GL_STATIC_DRAW);
        static GLModel generateTestModel();

        bool getModelType();
        int getDrawType();
        int getModelUsage();

        size_t getSize();

    private:
        unsigned int vaoID = 0;
        unsigned int indexedBuffer = 0;
        std::vector<unsigned int> vboIDs;

        bool modType = TYPE_ARRAY;
        int drawType = GL_TRIANGLES;

        int usage = GL_DYNAMIC_DRAW;

        size_t size = 0;
        size_t first = 0;
    };
    
}