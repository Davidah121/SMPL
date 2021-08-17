#include "Model.h"
#include "StringTools.h"
#include "File.h"
#include "SimpleFile.h"

namespace glib
{

    const Class Model::myClass = Class("Model", {&Object::myClass});
    const Class* Model::getClass()
    {
        return &Model::myClass;
    }

    Model::Model()
    {

    }

    Model::~Model()
    {
        clear();
    }

    void Model::addVertexFormatInfo(unsigned char type, unsigned char usage)
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
        vertexInfo.push_back( (double)value );
    }
    void Model::addVec2f(Vec2f value)
    {
        vertexInfo.push_back( value.x );
        vertexInfo.push_back( value.y );
    }

    void Model::addVec3f(Vec3f value)
    {
        vertexInfo.push_back( value.x );
        vertexInfo.push_back( value.y );
        vertexInfo.push_back( value.z );
    }

    void Model::addVec4f(Vec4f value)
    {
        vertexInfo.push_back( value.x );
        vertexInfo.push_back( value.y );
        vertexInfo.push_back( value.z );
        vertexInfo.push_back( value.w );
    }

    std::vector<double> Model::getVertex(int i)
    {
        std::vector<double> rValue = std::vector<double>(sizeOfVertex);

        if(i>=getVerticies())
        {
            return rValue;
        }

        int location = i*sizeOfVertex;
        int endLoc = location+sizeOfVertex;

        while(location<endLoc)
        {
            rValue.push_back( vertexInfo[location] );
            location++;
        }

        return rValue;
    }

    std::vector<VertexFormat> Model::getVertexFormatInfomation()
    {
        return orderInfo;
    }

    std::vector<double>& Model::getAllVertices()
    {
        return vertexInfo;
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

    void Model::setModelFormat(unsigned char format)
    {
        if(format>9)
            modelFormat = 9;
        else
            modelFormat = format;
    }

    unsigned char Model::getModelFormat()
    {
        return modelFormat;
    }

    int Model::getSizeOfVertex()
    {
        return sizeOfVertex;
    }

    void Model::loadModel(std::string filename)
    {
        Model::loadModel( StringTools::toWideString(filename) );
    }

    void Model::loadModel(std::wstring filename)
    {
        File h = File(filename);
        
        if(h.getExtension() == L".obj")
        {
            loadOBJ(filename);
        }
        else
        {
            //can't load
        }
    }

    void Model::loadOBJ(std::wstring filename)
    {
        SimpleFile s = SimpleFile(filename, SimpleFile::READ);
        if(s.isOpen())
        {
            std::vector<std::string> fileData = s.readFullFileString();
            s.close();

            //do stuff
        }
        else
        {
            //can't load
        }
    }

} //NAMESPACE glib END