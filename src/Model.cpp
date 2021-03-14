#include "Model.h"

const Class* Model::myClass = new Class("Model", {Object::myClass});
const Class* Model::getClass()
{
	return Model::myClass;
}

Model::Model()
{

}

Model::~Model()
{
    clear();
}

void Model::addVertexFormatInfo(int type, int usage)
{
    if(type>3 || usage>3)
    {
        //invalid type or usage. Exit
        return;
    }

    orderInfo.push_back({(unsigned char)type, (unsigned char)usage});
    sizeOfVertex += type+1;
}

void Model::addInt(int value)
{
    vertexInfo.push_back( (float)value );
}
void Model::addVec2(Vec2f value)
{
    vertexInfo.push_back( value.x );
    vertexInfo.push_back( value.y );
}

void Model::addVec3(Vec3f value)
{
    vertexInfo.push_back( value.x );
    vertexInfo.push_back( value.y );
    vertexInfo.push_back( value.z );
}

void Model::addVec4(Vec4f value)
{
    vertexInfo.push_back( value.x );
    vertexInfo.push_back( value.y );
    vertexInfo.push_back( value.z );
    vertexInfo.push_back( value.w );
}

std::vector<float> Model::getVertex(int i)
{
    std::vector<float> rValue = std::vector<float>(sizeOfVertex);

    if(i>=getVerticies())
    {
        return rValue;
    }

    for(int k=0; k<sizeOfVertex; k++)
    {
        int loc = i*sizeOfVertex + k;
        rValue.push_back( vertexInfo[loc] );
    }

    return rValue;
}

std::vector<VertexFormat> Model::getFormatInfomation()
{
    return orderInfo;
}

int Model::getVerticies()
{
    return vertexInfo.size()/sizeOfVertex;
}

void Model::clear()
{
    sizeOfVertex = 0;
    orderInfo.clear();
    vertexInfo.clear();
}