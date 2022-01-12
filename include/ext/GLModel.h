#pragma once

#include "MathExt.h"
#include "ext/GLSingleton.h"
#include "Model.h"

namespace glib
{
    
    class GLModel
    {
    public:
        static const bool TYPE_ARRAY = false;
        static const bool TYPE_INDEXED = true;

        GLModel();
        GLModel(GLModel& other);
        void operator=(GLModel& other);
        ~GLModel();

        void draw();

        static GLModel convertModel(Model* m, int tUsage = GL_STATIC_DRAW);
        static GLModel generateTestModel();

        bool getModelType();
        int getDrawType();
        int getModelUsage();

        size_t getSize();

        void storeDataInt(int attributeID, std::vector<int> data, int valuesPerVertex, bool forceReallocate = false);
        void storeDataFloat(int attributeID, std::vector<float> data, int valuesPerVertex, bool forceReallocate = false);
        void storeDataFloat(int attributeID, std::vector<double> data, int valuesPerVertex, bool forceReallocate = false);
        
        void setAttributeEnabled(int attributeID, bool v);

        void disableAllAttributes();

        void setDrawType(int value);
    private:
        void createVAO();

        unsigned int vaoID = 0;
        unsigned int indexedBuffer = 0;
        std::vector<unsigned int> vboIDs;
        std::vector<size_t> vboSizes; //size in bytes
        std::vector<bool> attributeEnabled;

        bool modType = TYPE_ARRAY;
        int drawType = GL_TRIANGLES;

        int usage = GL_DYNAMIC_DRAW;

        size_t size = 0;

        bool shouldDelete = true;
    };
    
}