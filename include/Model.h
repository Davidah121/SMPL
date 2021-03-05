#pragma once
#include "MathExt.h"

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
	static const Class* myClass;

    static const int INT = 0;
    static const int VEC2 = 1;
    static const int VEC3 = 2;
    static const int VEC4 = 3;

    static const int POSITION = 0;
    static const int TEXTURE = 1;
    static const int NORMAL = 2;
    static const int OTHER = 3;

    static const int TRIANGLE_STRIP = 0;
    static const int TRIANGLE_FAN = 1;

    void addVertexFormatInfo(int type, int usage);

    void addInt(int value);
    void addVec2(Vec2f value);
    void addVec3(Vec3f value);
    void addVec4(Vec4f value);
    
    std::vector<float> getVertex(int i);
    std::vector<VertexFormat> getFormatInfomation();
    int getVerticies();

    void clear();
private:
    std::vector<float> vertexInfo = std::vector<float>();
    std::vector<VertexFormat> orderInfo = std::vector<VertexFormat>();
    int sizeOfVertex = 0;
};