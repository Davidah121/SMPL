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

        formatInfo.push_back({(unsigned char)type, (unsigned char)usage});
        sizeOfVertex += type+1;

        vertexData.push_back( std::vector<double>() );
        vertexIndexInfo.push_back( std::vector<int>() );
    }

    void Model::addIndicies(std::vector<int> indexInfo)
    {
        if(indexInfo.size() == formatInfo.size())
            vertexIndexInfo.push_back( indexInfo );
    }

    void Model::setIndicies(int vertexLocation, std::vector<int> indexInfo)
    {
        if(vertexLocation >= 0 && vertexLocation <= vertexIndexInfo.size())
            if(indexInfo.size() == formatInfo.size())
                vertexIndexInfo[vertexLocation] = indexInfo;
    }

    void Model::addInt(int value, int list)
    {
        if(list>=0 && list<formatInfo.size())
            vertexData[list].push_back( (double)value );
    }
    
    void Model::addVec2f(Vec2f value, int list)
    {
        if(list>=0 && list<formatInfo.size())
        {
            vertexData[list].push_back( value.x );
            vertexData[list].push_back( value.y );
        }
    }

    void Model::addVec3f(Vec3f value, int list)
    {
        if(list>=0 && list<formatInfo.size())
        {
            vertexData[list].push_back( value.x );
            vertexData[list].push_back( value.y );
            vertexData[list].push_back( value.z );
        }
    }

    void Model::addVec4f(Vec4f value, int list)
    {
        if(list>=0 && list<formatInfo.size())
        {
            vertexData[list].push_back( value.x );
            vertexData[list].push_back( value.y );
            vertexData[list].push_back( value.z );
            vertexData[list].push_back( value.w );
        }
    }

    std::vector<double> Model::getVertex(int i)
    {
        std::vector<double> rValue = std::vector<double>(sizeOfVertex);

        if(i>=size())
        {
            return rValue;
        }

        if(indexed)
        {
            for(int j=0; j<vertexIndexInfo.size(); j++)
            {
                int valuesToGrab = this->formatInfo[j].type + 1;
                int index = vertexIndexInfo[i][j];

                for(int k=0; k<valuesToGrab; k++)
                {
                    rValue.push_back( vertexData[j][index*valuesToGrab + k] );
                }
            }
        }
        else
        {
            for(int j=0; j<formatInfo.size(); j++)
            {
                int valuesToGrab = this->formatInfo[j].type + 1;
                int index = i * valuesToGrab;

                for(int k=0; k<valuesToGrab; k++)
                {
                    rValue.push_back( vertexData[j][index*valuesToGrab + k] );
                }
            }
        }

        return rValue;
    }

    std::vector<VertexFormat> Model::getVertexFormatInfomation()
    {
        return formatInfo;
    }

    int Model::size()
    {
        if(indexed)
            return vertexIndexInfo.size();
        else
        {
            if(vertexData.size() > 0 && formatInfo.size() > 0)
                return vertexData[0].size() / (formatInfo[0].type+1);
        }
        return 0;
    }

    void Model::clear()
    {
        sizeOfVertex = 0;
        formatInfo.clear();
        vertexData.clear();
        vertexIndexInfo.clear();
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

    void Model::loadModel(File file)
    {
        if(file.getExtension() == L".obj")
        {
            loadOBJ(file);
        }
        else
        {
            //can't load
        }
    }

    void Model::loadOBJ(File file)
    {
        SimpleFile s = SimpleFile(file, SimpleFile::READ);
        if(s.isOpen())
        {
            std::vector<std::string> fileInfo = s.readFullFileString();
            s.close();

            //do stuff

            //assume simple file format
            //vertex definition. One per line break. (w is optional. default to 1)
            //v x y z (w)
            
            //texture coordinates. One per line break. (v and w are optional. default to 0)
            //vt u (v) (w)

            //list of vertex normals. One per line break.
            //vn x y z

            //skipping vp

            //Face. (could be a triangle or quad) 3 or 4 indicies per line
            //f v1/vt1/vn1 ...
            //f v1//vn1 ...
            //f v1/vt1 ...

            //line elements
            //l v1 v2 v3
            std::vector<Vec3f> points;
            std::vector<Vec2f> texCoords;
            std::vector<Vec3f> normals;

            clear();
            bool hasPos = false;
            bool hasTex = false;
            bool hasNorm = false;
            indexed = true;
            

            for(std::string& s : fileInfo)
            {
                int type = -1;
                if(s[0] == 'v')
                {
                    switch (s[1])
                    {
                    case ' ':
                        type = 0; //Vertex Position
                        if(!hasPos)
                        {
                            addVertexFormatInfo(TYPE_VEC3, USAGE_POSITION);
                            hasPos = true;
                        }
                        break;
                    case 't':
                        type = 1; //Texture Coordinate
                        if(!hasTex)
                        {
                            addVertexFormatInfo(TYPE_VEC2, USAGE_TEXTURE);
                            hasTex = true;
                        }
                        break;
                    case 'n':
                        type = 2; //Vertex Normal
                        if(!hasNorm)
                        {
                            addVertexFormatInfo(TYPE_VEC3, USAGE_NORMAL);
                            hasNorm = true;
                        }
                    default:
                        break;
                    }
                }
                else if(s[0] == 'f')
                {
                    type = 3; //triangle or quad stuff
                }
                else if(s[0] == 'l')
                {
                    type = 4; //polyline stuff
                }

                if(type == 0)
                {
                    std::vector<std::string> split = StringTools::splitString(s, ' ');
                    if(split.size() >= 4)
                    {
                        Vec3f v;
                        v.x = std::stod(split[1]);
                        v.y = std::stod(split[2]);
                        v.z = std::stod(split[3]);
                        points.push_back(v);
                    }
                }
                else if(type == 1)
                {
                    std::vector<std::string> split = StringTools::splitString(s, ' ');
                    if(split.size() >= 3)
                    {
                        Vec2f v;
                        v.x = std::stod(split[1]);
                        v.y = std::stod(split[2]);
                        texCoords.push_back(v);
                    }
                }
                else if(type == 2)
                {
                    std::vector<std::string> split = StringTools::splitString(s, ' ');
                    if(split.size() >= 4)
                    {
                        Vec3f v;
                        v.x = std::stod(split[1]);
                        v.y = std::stod(split[2]);
                        v.z = std::stod(split[3]);
                        normals.push_back(v);
                    }
                }
                else if(type == 3)
                {
                    std::vector<std::string> split = StringTools::splitString(s, ' ');
                    if(split.size() >= 4)
                    {
                        if(split.size() == 4)
                        {
                            //triangles
                            modelFormat = TRIANGLES;
                        }
                        else if(split.size() == 5)
                        {
                            //Quads
                            modelFormat = QUADS;
                        }

                        for(int i=1; i<split.size(); i++)
                        {
                            std::vector<std::string> subSplit = StringTools::splitString(split[i], '/', false);
                            std::vector<int> indexValues;
                            for(int j=0; j<subSplit.size(); j++)
                            {
                                if(!subSplit[j].empty())
                                {
                                    indexValues.push_back( stoi(subSplit[j]) );
                                }
                            }

                            addIndicies(indexValues);
                        }
                    }
                }
                else if(type == 4)
                {
                    //polyline
                    std::vector<std::string> split = StringTools::splitString(s, ' ');
                    if(split.size() > 1)
                    {
                        modelFormat = LINES;
                        for(int i=1; i<split.size(); i++)
                        {
                            addIndicies( { stoi(split[i]) } );
                        }
                    }
                }
            }
        }
        else
        {
            //can't load
        }
    }

} //NAMESPACE glib END