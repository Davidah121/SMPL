#pragma once
#include "MathExt.h"

namespace glib
{

    struct VertexFormat
    {
        unsigned char type = 0;
        unsigned char usage = 0;
    };

    class Model : public Object
    {
    public:
        Model();
        ~Model();

        //Object and Class Stuff
        const Class* getClass();
        static const Class myClass;

        static const unsigned char TYPE_INT = 0;
        static const unsigned char TYPE_VEC2 = 1;
        static const unsigned char TYPE_VEC3 = 2;
        static const unsigned char TYPE_VEC4 = 3;

        static const unsigned char USAGE_POSITION = 0;
        static const unsigned char USAGE_TEXTURE = 1;
        static const unsigned char USAGE_NORMAL = 2;
        static const unsigned char USAGE_OTHER = 3;

        static const unsigned char POINTS = 0;
        
        static const unsigned char LINES = 1;
        static const unsigned char LINE_STRIP = 2;
        static const unsigned char LINE_LOOP = 3;

        static const unsigned char TRIANGLES = 4;
        static const unsigned char TRIANGLE_STRIP = 5;
        static const unsigned char TRIANGLE_FAN = 6;

        static const unsigned char QUADS = 7;
        static const unsigned char QUAD_STRIP = 8;
        
        void addVertexFormatInfo(unsigned char type, unsigned char usage);

        void addInt(int value);
        void addVec2f(Vec2f value);
        void addVec3f(Vec3f value);
        void addVec4f(Vec4f value);
        
        std::vector<double> getVertex(int i);
        std::vector<VertexFormat> getVertexFormatInfomation();
        std::vector<double>& getAllVertices();

        int getVerticies();

        void setModelFormat(unsigned char format);
        unsigned char getModelFormat();
        int getSizeOfVertex();

        void clear();

        void loadModel(std::string filename);
        void loadModel(std::wstring filename);
        
    private:
        void loadOBJ(std::wstring filename);

        std::vector<double> vertexInfo = std::vector<double>();
        std::vector<VertexFormat> orderInfo = std::vector<VertexFormat>();
        int sizeOfVertex = 0;
        unsigned char modelFormat = TRIANGLES;
    };

} //NAMESPACE glib END