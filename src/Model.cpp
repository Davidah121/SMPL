#include "Model.h"
#include "StringTools.h"
#include "File.h"
#include "SimpleFile.h"
#include "SimpleXml.h"

namespace smpl
{
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
        VertexFormat f;
        f.type = type;
        f.usage = usage;
        f.size = type;
        if(type == TYPE_INT)
        {
            f.size = 1;
        }

        formatInfo.push_back(f);
        vertexData.push_back( std::vector<int>() );
    }

    void Model::addIndicies(std::vector<unsigned int> indexInfo)
    {
        if(indexInfo.size() == formatInfo.size())
            vertexIndexInfo.push_back( indexInfo );
    }

    void Model::setIndicies(size_t vertexLocation, std::vector<unsigned int> indexInfo)
    {
        if(vertexLocation <= vertexIndexInfo.size())
            if(indexInfo.size() == formatInfo.size())
                vertexIndexInfo[vertexLocation] = indexInfo;
    }

    void Model::addInt(int value, size_t list)
    {
        if(list<formatInfo.size())
            vertexData[list].push_back( value );
    }

    void Model::addFloat(float value, size_t list)
    {
        if(list<formatInfo.size())
            vertexData[list].push_back( *((int*)&value) );
    }
    
    void Model::addVec2f(Vec2f value, size_t list)
    {
        if(list<formatInfo.size())
        {
            float v;
            v = (float)value.x;
            vertexData[list].push_back( *((int*)&v) );

            v = (float)value.y;
            vertexData[list].push_back( *((int*)&v) );
        }
    }

    void Model::addVec3f(Vec3f value, size_t list)
    {
        if(list<formatInfo.size())
        {
            float v;
            v = (float)value.x;
            vertexData[list].push_back( *((int*)&v) );

            v = (float)value.y;
            vertexData[list].push_back( *((int*)&v) );

            v = (float)value.z;
            vertexData[list].push_back( *((int*)&v) );
        }
    }

    void Model::addVec4f(Vec4f value, size_t list)
    {
        if(list<formatInfo.size())
        {
            float v;
            v = (float)value.x;
            vertexData[list].push_back( *((int*)&v) );

            v = (float)value.y;
            vertexData[list].push_back( *((int*)&v) );

            v = (float)value.z;
            vertexData[list].push_back( *((int*)&v) );

            v = (float)value.w;
            vertexData[list].push_back( *((int*)&v) );
        }
    }

    std::vector<std::vector<int>> Model::getVertex(size_t i)
    {
        std::vector<std::vector<int>> rValues;

        if(i>=size())
        {
            return rValues;
        }

        if(indexed)
        {
            for(size_t j=0; j<formatInfo.size(); j++)
            {
                std::vector<int> values;
                size_t valuesToGrab = this->formatInfo[j].size;
                size_t index = vertexIndexInfo[i][j]*valuesToGrab;

                for(size_t k=0; k<valuesToGrab; k++)
                {
                    values.push_back( vertexData[j][index + k]);
                }

                rValues.push_back(values);
            }
        }
        else
        {
            for(size_t j=0; j<formatInfo.size(); j++)
            {
                std::vector<int> values;
                size_t valuesToGrab = this->formatInfo[j].size;
                size_t index = i*valuesToGrab;

                for(size_t k=0; k<valuesToGrab; k++)
                {
                    values.push_back( vertexData[j][index + k]);
                }

                rValues.push_back(values);
            }
        }

        return rValues;
    }

    int Model::getInt(size_t index, size_t list)
    {
        if(list < vertexData.size())
        {
            size_t valuesToGrab = 1;
            size_t offset = valuesToGrab*index;
            
            if(offset < vertexData[list].size())
            {
                return vertexData[list][offset];
            }
            else
            {
                //throw exception
                return 0;
            }
        }
        return 0;
    
    }
    float Model::getFloat(size_t index, size_t list)
    {
        if(list < vertexData.size())
        {
            size_t valuesToGrab = 1;
            size_t offset = valuesToGrab*index;
            if(offset < vertexData[list].size())
            {
                return *((float*)&vertexData[list][offset]);
            }
            else
            {
                //throw exception
                return NAN;
            }
        }
        return NAN;
    }

    Vec2f Model::getVec2f(size_t index, size_t list)
    {
        if(list < vertexData.size())
        {
            size_t valuesToGrab = 2;
            size_t offset = valuesToGrab*index;
            if(offset+1 < vertexData[list].size())
            {
                Vec2f v;
                v.x = *((float*)&vertexData[list][offset]);
                v.y = *((float*)&vertexData[list][offset+1]);
                return v;
            }
            else
            {
                //throw exception
                return Vec2f();
            }
        }
        return Vec2f();
    }

    Vec3f Model::getVec3f(size_t index, size_t list)
    {
        if(list < vertexData.size())
        {
            size_t valuesToGrab = 3;
            size_t offset = valuesToGrab*index;
            if(offset+2 < vertexData[list].size())
            {
                Vec3f v;
                v.x = *((float*)&vertexData[list][offset]);
                v.y = *((float*)&vertexData[list][offset+1]);
                v.z = *((float*)&vertexData[list][offset+2]);
                return v;
            }
            else
            {
                //throw exception
                return Vec3f();
            }
        }
        return Vec3f();
    }

    Vec4f Model::getVec4f(size_t index, size_t list)
    {
        if(list < vertexData.size())
        {
            size_t valuesToGrab = 4;
            size_t offset = valuesToGrab*index;
            if(offset+3 < vertexData[list].size())
            {
                Vec4f v;
                v.x = *((float*)&vertexData[list][offset]);
                v.y = *((float*)&vertexData[list][offset+1]);
                v.z = *((float*)&vertexData[list][offset+2]);
                v.w = *((float*)&vertexData[list][offset+3]);
                return v;
            }
            else
            {
                //throw exception
                return Vec4f();
            }
        }
        return Vec4f();
    }

    std::vector<VertexFormat> Model::getVertexFormatInfomation()
    {
        return formatInfo;
    }

    void Model::clear()
    {
        formatInfo.clear();
        vertexData.clear();
        vertexIndexInfo.clear();
        indexed = false;
        modelFormat = Model::TRIANGLES;
    }

    void Model::setModelFormat(unsigned char format)
    {
        modelFormat = MathExt::clamp<int>(format, POINTS, QUAD_STRIP);
    }

    unsigned char Model::getModelFormat()
    {
        return modelFormat;
    }

    size_t Model::size()
    {
        if(indexed)
            return vertexIndexInfo.size();
        else
        {
            if(vertexData.size() > 0 && formatInfo.size() > 0)
                if(formatInfo[0].size > 0)
                    return vertexData[0].size() / (formatInfo[0].size);
        }
        return 0;
    }

    size_t Model::sizeOfList(size_t index)
    {
        if(index < vertexData.size())
        {
            return vertexData[index].size();
        }
        return 0;
    }

    size_t Model::getSizeOfVertex()
    {
        size_t sum=0;
        for(size_t i=0; i<formatInfo.size(); i++)
        {
            sum += formatInfo[i].size;
        }

        return sum;
    }

    std::vector<std::vector<int>> Model::getRawVertexData()
    {
        return vertexData;
    }

    std::vector<std::vector<unsigned int>> Model::getRawIndexData()
    {
        return vertexIndexInfo;
    }

    void Model::setIndexed(bool v)
    {
        indexed = v;
    }

    bool Model::getIndexed()
    {
        return indexed;
    }

    Triangle2DModel Model::convertTo2DTriModel()
    {
        Triangle2DModel mod;

        if(modelFormat < TRIANGLES || modelFormat > QUAD_STRIP)
            return Triangle2DModel();   //NOT Correct format. Will be lines or points which are not triangles.

        int formatNum = -1;
        int offset = 0;
        for(int i=0; i<formatInfo.size(); i++)
        {
            if(formatInfo[i].usage == USAGE_POSITION)
            {
                formatNum = i;
                break;
            }
            offset += formatInfo[i].size;
        }
        
        if(formatNum < 0)
            return Triangle2DModel(); //NO USAGE_POSITION. Can't tell what information to use.

        if(formatInfo[formatNum].size < 2)
            return Triangle2DModel(); //NOT Enough dimensions in each vertex. Need at least 2

        
        size_t amtVert = size();
        bool order = false;

        switch (modelFormat)
        {
        case Model::TRIANGLES:
            //draw triangles at point x,y to point x2,y2 to point x3,y3
            for(int i=2; i<amtVert; i+=3)
            {
                std::vector<std::vector<int>> vertInfo = getVertex(i-2);
                std::vector<std::vector<int>> vertInfo2 = getVertex(i-1);
                std::vector<std::vector<int>> vertInfo3 = getVertex(i);

                float* positionData1 = (float*)vertInfo[offset].data();
                float* positionData2 = (float*)vertInfo2[offset].data();
                float* positionData3 = (float*)vertInfo3[offset].data();

                Vec2f pos = Vec2f( positionData1[0], positionData1[1] );
                Vec2f pos2 = Vec2f( positionData2[0], positionData2[1] );
                Vec2f pos3 = Vec2f( positionData3[0], positionData3[1] );

                mod.add( Triangle2D(pos, pos2, pos3) );
            }
            break;
        case Model::TRIANGLE_FAN:
            //draw triangles using first point to point x2,y2 to point x3,y3
            
            for(int i=2; i<amtVert; i+=1)
            {
                std::vector<std::vector<int>> vertInfo = getVertex(0);
                std::vector<std::vector<int>> vertInfo2 = getVertex(i-1);
                std::vector<std::vector<int>> vertInfo3 = getVertex(i);

                float* positionData1 = (float*)vertInfo[offset].data();
                float* positionData2 = (float*)vertInfo2[offset].data();
                float* positionData3 = (float*)vertInfo3[offset].data();
                
                Vec2f pos = Vec2f( positionData1[0], positionData1[1] );
                Vec2f pos2 = Vec2f( positionData2[0], positionData2[1] );
                Vec2f pos3 = Vec2f( positionData3[0], positionData3[1] );
                
                mod.add( Triangle2D(pos, pos2, pos3) );
            }
            break;
        case Model::TRIANGLE_STRIP:
            for(int i=2; i<amtVert; i++)
            {
                std::vector<std::vector<int>> vertInfo, vertInfo2, vertInfo3;

                if(!order)
                {
                    vertInfo = getVertex(i-2);
                    vertInfo2 = getVertex(i-1);
                }
                else
                {
                    vertInfo = getVertex(i-1);
                    vertInfo2 = getVertex(i-2);
                }

                vertInfo3 = getVertex(i);
                
                float* positionData1 = (float*)vertInfo[offset].data();
                float* positionData2 = (float*)vertInfo2[offset].data();
                float* positionData3 = (float*)vertInfo3[offset].data();

                Vec2f pos = Vec2f( positionData1[0], positionData1[1] );
                Vec2f pos2 = Vec2f( positionData2[0], positionData2[1] );
                Vec2f pos3 = Vec2f( positionData3[0], positionData3[1] );
                    
                mod.add( Triangle2D(pos, pos2, pos3) );
                
                order = !order;
            }

            break;
        case Model::QUADS:
            //draw 2 triangles using 4 points
            for(int i=3; i<amtVert; i+=4)
            {
                std::vector<std::vector<int>> vertInfo, vertInfo2, vertInfo3, vertInfo4;

                vertInfo = getVertex(i-3);
                vertInfo2 = getVertex(i-2);
                vertInfo3 = getVertex(i-1);
                vertInfo4 = getVertex(i);
                
                
                float* positionData1 = (float*)vertInfo[offset].data();
                float* positionData2 = (float*)vertInfo2[offset].data();
                float* positionData3 = (float*)vertInfo3[offset].data();
                float* positionData4 = (float*)vertInfo4[offset].data();
                
                Vec2f pos = Vec2f( positionData1[0], positionData1[1] );
                Vec2f pos2 = Vec2f( positionData2[0], positionData2[1] );
                Vec2f pos3 = Vec2f( positionData3[0], positionData3[1] );
                
                mod.add( Triangle2D(pos, pos2, pos3) );

                pos = Vec2f( positionData1[0], positionData1[1] );
                pos2 = Vec2f( positionData3[0], positionData3[1] );
                pos3 = Vec2f( positionData4[0], positionData4[1] );
                
                mod.add( Triangle2D(pos, pos2, pos3) );
                
            }
            break;
        case Model::QUAD_STRIP:
            //draw 2 triangles using 4 points

            for(int i=3; i<amtVert; i+=2)
            {
                std::vector<std::vector<int>> vertInfo, vertInfo2, vertInfo3, vertInfo4;

                if(!order)
                {
                    vertInfo = getVertex(i-3);
                    vertInfo2 = getVertex(i-2);
                    vertInfo3 = getVertex(i-1);
                    vertInfo4 = getVertex(i);
                }
                else
                {
                    vertInfo = getVertex(i-1);
                    vertInfo2 = getVertex(i-2);
                    vertInfo3 = getVertex(i-3);
                    vertInfo4 = getVertex(i);
                }

                float* positionData1 = (float*)vertInfo[offset].data();
                float* positionData2 = (float*)vertInfo2[offset].data();
                float* positionData3 = (float*)vertInfo3[offset].data();
                float* positionData4 = (float*)vertInfo4[offset].data();
            
                Vec2f pos = Vec2f( positionData1[0], positionData1[1] );
                Vec2f pos2 = Vec2f( positionData2[0], positionData2[1] );
                Vec2f pos3 = Vec2f( positionData3[0], positionData3[1] );
                    
                mod.add( Triangle2D(pos, pos2, pos3) );

                pos = Vec2f( positionData1[0], positionData1[1] );
                pos2 = Vec2f( positionData3[0], positionData3[1] );
                pos3 = Vec2f( positionData4[0], positionData4[1] );
                
                mod.add( Triangle2D(pos, pos2, pos3) );

                order = !order;
            }
            break;
        default:
            break;
        }
        
        return mod;
    }

    Triangle3DModel Model::convertTo3DTriModel()
    {
        Triangle3DModel mod;

        if(modelFormat < TRIANGLES || modelFormat > QUAD_STRIP)
            return Triangle3DModel();   //NOT Correct format. Will be lines or points which are not triangles.

        int formatNum = -1;
        int offset = 0;
        for(int i=0; i<formatInfo.size(); i++)
        {
            if(formatInfo[i].usage == USAGE_POSITION)
            {
                formatNum = i;
                break;
            }
            offset += formatInfo[i].size;
        }
        
        if(formatNum < 0)
            return Triangle3DModel(); //NO USAGE_POSITION. Can't tell what information to use.

        if(formatInfo[formatNum].size < 3)
            return Triangle3DModel(); //NOT Enough dimensions in each vertex. Need at least 3

        
        size_t amtVert = size();
        bool order = false;

        switch (modelFormat)
        {
        case Model::TRIANGLES:
            //draw triangles at point x,y to point x2,y2 to point x3,y3
            for(int i=2; i<amtVert; i+=3)
            {
                std::vector<std::vector<int>> vertInfo = getVertex(i-2);
                std::vector<std::vector<int>> vertInfo2 = getVertex(i-1);
                std::vector<std::vector<int>> vertInfo3 = getVertex(i);

                float* positionData1 = (float*)vertInfo[offset].data();
                float* positionData2 = (float*)vertInfo2[offset].data();
                float* positionData3 = (float*)vertInfo3[offset].data();

                Vec3f pos = Vec3f( positionData1[0], positionData1[1], positionData1[2] );
                Vec3f pos2 = Vec3f( positionData2[0], positionData2[1], positionData2[2] );
                Vec3f pos3 = Vec3f( positionData3[0], positionData3[1], positionData3[2] );

                mod.add( Triangle3D(pos, pos2, pos3) );
            }
            break;
        case Model::TRIANGLE_FAN:
            //draw triangles using first point to point x2,y2 to point x3,y3
            
            for(int i=2; i<amtVert; i+=1)
            {
                std::vector<std::vector<int>> vertInfo = getVertex(0);
                std::vector<std::vector<int>> vertInfo2 = getVertex(i-1);
                std::vector<std::vector<int>> vertInfo3 = getVertex(i);

                float* positionData1 = (float*)vertInfo[offset].data();
                float* positionData2 = (float*)vertInfo2[offset].data();
                float* positionData3 = (float*)vertInfo3[offset].data();
                
                Vec3f pos = Vec3f( positionData1[0], positionData1[1], positionData1[2] );
                Vec3f pos2 = Vec3f( positionData2[0], positionData2[1], positionData2[2] );
                Vec3f pos3 = Vec3f( positionData3[0], positionData3[1], positionData3[2] );
                
                mod.add( Triangle3D(pos, pos2, pos3) );
            }
            break;
        case Model::TRIANGLE_STRIP:
            for(int i=2; i<amtVert; i++)
            {
                std::vector<std::vector<int>> vertInfo, vertInfo2, vertInfo3;

                if(!order)
                {
                    vertInfo = getVertex(i-2);
                    vertInfo2 = getVertex(i-1);
                }
                else
                {
                    vertInfo = getVertex(i-1);
                    vertInfo2 = getVertex(i-2);
                }

                vertInfo3 = getVertex(i);
                
                float* positionData1 = (float*)vertInfo[offset].data();
                float* positionData2 = (float*)vertInfo2[offset].data();
                float* positionData3 = (float*)vertInfo3[offset].data();

                Vec3f pos = Vec3f( positionData1[0], positionData1[1], positionData1[2] );
                Vec3f pos2 = Vec3f( positionData2[0], positionData2[1], positionData2[2] );
                Vec3f pos3 = Vec3f( positionData3[0], positionData3[1], positionData3[2] );
                    
                mod.add( Triangle3D(pos, pos2, pos3) );
                
                order = !order;
            }

            break;
        case Model::QUADS:
            //draw 2 triangles using 4 points
            for(int i=3; i<amtVert; i+=4)
            {
                std::vector<std::vector<int>> vertInfo, vertInfo2, vertInfo3, vertInfo4;

                vertInfo = getVertex(i-3);
                vertInfo2 = getVertex(i-2);
                vertInfo3 = getVertex(i-1);
                vertInfo4 = getVertex(i);
                
                
                float* positionData1 = (float*)vertInfo[offset].data();
                float* positionData2 = (float*)vertInfo2[offset].data();
                float* positionData3 = (float*)vertInfo3[offset].data();
                float* positionData4 = (float*)vertInfo4[offset].data();
                
                Vec3f pos = Vec3f( positionData1[0], positionData1[1], positionData1[2] );
                Vec3f pos2 = Vec3f( positionData2[0], positionData2[1], positionData2[2] );
                Vec3f pos3 = Vec3f( positionData3[0], positionData3[1], positionData3[2] );
                
                mod.add( Triangle3D(pos, pos2, pos3) );

                pos = Vec3f( positionData1[0], positionData1[1], positionData1[2] );
                pos2 = Vec3f( positionData3[0], positionData3[1], positionData3[2] );
                pos3 = Vec3f( positionData4[0], positionData4[1], positionData4[2] );
                
                mod.add( Triangle3D(pos, pos2, pos3) );
                
            }
            break;
        case Model::QUAD_STRIP:
            //draw 2 triangles using 4 points

            for(int i=3; i<amtVert; i+=2)
            {
                std::vector<std::vector<int>> vertInfo, vertInfo2, vertInfo3, vertInfo4;

                if(!order)
                {
                    vertInfo = getVertex(i-3);
                    vertInfo2 = getVertex(i-2);
                    vertInfo3 = getVertex(i-1);
                    vertInfo4 = getVertex(i);
                }
                else
                {
                    vertInfo = getVertex(i-1);
                    vertInfo2 = getVertex(i-2);
                    vertInfo3 = getVertex(i-3);
                    vertInfo4 = getVertex(i);
                }

                float* positionData1 = (float*)vertInfo[offset].data();
                float* positionData2 = (float*)vertInfo2[offset].data();
                float* positionData3 = (float*)vertInfo3[offset].data();
                float* positionData4 = (float*)vertInfo4[offset].data();
                
                Vec3f pos = Vec3f( positionData1[0], positionData1[1], positionData1[2] );
                Vec3f pos2 = Vec3f( positionData2[0], positionData2[1], positionData2[2] );
                Vec3f pos3 = Vec3f( positionData3[0], positionData3[1], positionData3[2] );
                
                mod.add( Triangle3D(pos, pos2, pos3) );

                pos = Vec3f( positionData1[0], positionData1[1], positionData1[2] );
                pos2 = Vec3f( positionData3[0], positionData3[1], positionData3[2] );
                pos3 = Vec3f( positionData4[0], positionData4[1], positionData4[2] );
                
                mod.add( Triangle3D(pos, pos2, pos3) );

                order = !order;
            }
            break;
        default:
            break;
        }

        return mod;
    }

    void Model::saveModel(File file, unsigned char type)
    {
        if(type == MODEL_TYPE_STL)
            saveSTL(file);
        // else if(type == MODEL_TYPE_OBJ)
        //     saveOBJ(file);
    }

    void Model::loadModel(File file)
    {
        std::string ext = file.getExtension();

        if(ext == ".obj")
        {
            loadOBJ(file);
        }
        else if(ext == ".stl")
        {
            loadSTL(file);
        }
        else if(ext == ".dae")
        {
            loadCollada(file);
        }
        else
        {
            //can't load
        }
    }

    void Model::loadOBJ(File file)
    {
        clear();
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

            indexed = true;

            int posLocation = -1;
            int texLocation = -1;
            int normLocation = -1;

            int currLoc = 0;
            

            for(std::string& s : fileInfo)
            {
                int type = -1;
                if(s[0] == 'v')
                {
                    switch (s[1])
                    {
                    case ' ':
                        type = 0; //Vertex Position
                        if(posLocation == -1)
                        {
                            addVertexFormatInfo(TYPE_VEC3, USAGE_POSITION);
                            posLocation = currLoc;
                            currLoc++;
                        }
                        break;
                    case 't':
                        type = 1; //Texture Coordinate
                        if(texLocation == -1)
                        {
                            addVertexFormatInfo(TYPE_VEC2, USAGE_TEXTURE);
                            texLocation = currLoc;
                            currLoc++;
                        }
                        break;
                    case 'n':
                        type = 2; //Vertex Normal
                        if(normLocation == -1)
                        {
                            addVertexFormatInfo(TYPE_VEC3, USAGE_NORMAL);
                            normLocation = currLoc;
                            currLoc++;
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
                        v.x = std::stof(split[1]);
                        v.y = std::stof(split[2]);
                        v.z = std::stof(split[3]);

                        addVec3f(v, (size_t)posLocation);
                    }
                }
                else if(type == 1)
                {
                    std::vector<std::string> split = StringTools::splitString(s, ' ');
                    if(split.size() >= 3)
                    {
                        Vec2f v;
                        v.x = std::stof(split[1]);
                        v.y = std::stof(split[2]);

                        addVec2f(v, (size_t)texLocation);
                    }
                }
                else if(type == 2)
                {
                    std::vector<std::string> split = StringTools::splitString(s, ' ');
                    if(split.size() >= 4)
                    {
                        Vec3f v;
                        v.x = std::stof(split[1]);
                        v.y = std::stof(split[2]);
                        v.z = std::stof(split[3]);

                        addVec3f(v, (size_t)normLocation);
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
                            std::vector<unsigned int> indexValues;
                            for(int j=0; j<subSplit.size(); j++)
                            {
                                if(!subSplit[j].empty())
                                {
                                    indexValues.push_back( (unsigned int)(stoi(subSplit[j])-1) );
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
                            addIndicies( { (unsigned int)(stoi(split[i])-1) } );
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

    
    void Model::loadSTL(File file)
    {
        clear();
        SimpleFile s = SimpleFile(file, SimpleFile::READ | SimpleFile::ASCII);
        if(s.isOpen())
        {
            //Find if ascii or binary
            std::string type;
            for(int i=0; i<5; i++)
            {
                type += (char)s.readByte();
            }

            Vec3f normal, pos;

            addVertexFormatInfo(TYPE_VEC3, USAGE_POSITION);
            addVertexFormatInfo(TYPE_VEC3, USAGE_NORMAL);

            if(type == "solid")
            {
                //ascii file
                std::vector<std::string> fileInfo = s.readFullFileString();
                s.close();

                for(int i=0; i<fileInfo.size(); i++)
                {
                    std::vector<std::string> splitInfo = StringTools::splitString(fileInfo[i], ' ', true);

                    if(splitInfo.size() > 0)
                    {
                        if(splitInfo[0] == "facet")
                        {
                            if(splitInfo[1] == "normal")
                            {
                                normal.x = stod(splitInfo[2]);
                                normal.y = stod(splitInfo[3]);
                                normal.z = stod(splitInfo[4]);
                            }
                        }
                        else if(splitInfo[0] == "vertex")
                        {
                            pos.x = stod(splitInfo[1]);
                            pos.y = stod(splitInfo[2]);
                            pos.z = stod(splitInfo[3]);

                            addVec3f(pos, 0);
                            addVec3f(normal, 1);
                        }
                        else if(splitInfo[0] == "endsolid")
                        {
                            break;
                        }
                    }
                }
            }
            else
            {
                //binary type
                std::vector<unsigned char> fileBytes = s.readFullFileAsBytes();
                s.close();

                size_t index = 80-5;
                size_t totalSize = fileBytes.size();
                
                unsigned int numTriangles = fileBytes[index] + ((unsigned int)fileBytes[index+1]<<8) + ((unsigned int)fileBytes[index+2]<<16) + ((unsigned int)fileBytes[index+3]<<24);
                index+=4;

                while(index < totalSize)
                {
                    unsigned int normalX, normalY, normalZ, posX, posY, posZ;

                    normalX = fileBytes[index] + ((unsigned int)fileBytes[index+1]<<8) + ((unsigned int)fileBytes[index+2]<<16) + ((unsigned int)fileBytes[index+3]<<24);
                    index+=4;
                    normalY = fileBytes[index] + ((unsigned int)fileBytes[index+1]<<8) + ((unsigned int)fileBytes[index+2]<<16) + ((unsigned int)fileBytes[index+3]<<24);
                    index+=4;
                    normalZ = fileBytes[index] + ((unsigned int)fileBytes[index+1]<<8) + ((unsigned int)fileBytes[index+2]<<16) + ((unsigned int)fileBytes[index+3]<<24);
                    index+=4;

                    normal.x = *((float*)&normalX);
                    normal.y = *((float*)&normalY);
                    normal.z = *((float*)&normalZ);


                    posX = fileBytes[index] + ((unsigned int)fileBytes[index+1]<<8) + ((unsigned int)fileBytes[index+2]<<16) + ((unsigned int)fileBytes[index+3]<<24);
                    index+=4;
                    posY = fileBytes[index] + ((unsigned int)fileBytes[index+1]<<8) + ((unsigned int)fileBytes[index+2]<<16) + ((unsigned int)fileBytes[index+3]<<24);
                    index+=4;
                    posZ = fileBytes[index] + ((unsigned int)fileBytes[index+1]<<8) + ((unsigned int)fileBytes[index+2]<<16) + ((unsigned int)fileBytes[index+3]<<24);
                    index+=4;

                    pos.x = *((float*)&posX);
                    pos.y = *((float*)&posY);
                    pos.z = *((float*)&posZ);

                    addVec3f(pos, 0);

                    posX = fileBytes[index] + ((unsigned int)fileBytes[index+1]<<8) + ((unsigned int)fileBytes[index+2]<<16) + ((unsigned int)fileBytes[index+3]<<24);
                    index+=4;
                    posY = fileBytes[index] + ((unsigned int)fileBytes[index+1]<<8) + ((unsigned int)fileBytes[index+2]<<16) + ((unsigned int)fileBytes[index+3]<<24);
                    index+=4;
                    posZ = fileBytes[index] + ((unsigned int)fileBytes[index+1]<<8) + ((unsigned int)fileBytes[index+2]<<16) + ((unsigned int)fileBytes[index+3]<<24);
                    index+=4;
                    
                    pos.x = *((float*)&posX);
                    pos.y = *((float*)&posY);
                    pos.z = *((float*)&posZ);

                    addVec3f(pos, 0);

                    posX = fileBytes[index] + ((unsigned int)fileBytes[index+1]<<8) + ((unsigned int)fileBytes[index+2]<<16) + ((unsigned int)fileBytes[index+3]<<24);
                    index+=4;
                    posY = fileBytes[index] + ((unsigned int)fileBytes[index+1]<<8) + ((unsigned int)fileBytes[index+2]<<16) + ((unsigned int)fileBytes[index+3]<<24);
                    index+=4;
                    posZ = fileBytes[index] + ((unsigned int)fileBytes[index+1]<<8) + ((unsigned int)fileBytes[index+2]<<16) + ((unsigned int)fileBytes[index+3]<<24);
                    index+=4;
                    
                    pos.x = *((float*)&posX);
                    pos.y = *((float*)&posY);
                    pos.z = *((float*)&posZ);

                    addVec3f(pos, 0);

                    addVec3f(normal, 1);
                    addVec3f(normal, 1);
                    addVec3f(normal, 1);

                    //custom attribute byte count. Will skip
                    unsigned int extraAttribByteCount = fileBytes[index] + ((unsigned int)fileBytes[index+1] << 8);
                    index += 2 + extraAttribByteCount;

                }
            }
        }
        else
        {
            //can't open file
        }
    }

    //TODO - Rewrite
    void Model::loadCollada(File file)
    {
        return;

        // clear();
        // indexed = true;

        // SimpleXml xmlData = SimpleXml(file);

        // if(xmlData.getNodes().size() <= 0)
        // {
        //     //unsuccessful
        //     return;
        // }


        // XmlNode* rootColladaNode = nullptr;
        // //Find COLLADA
        // for(XmlNode* currNode : xmlData.getNodes())
        // {
        //     if(StringTools::equalsIgnoreCase<char>(currNode->getTitle(), "COLLADA"))
        //     {
        //         rootColladaNode = currNode;
        //         break;
        //     }
        // }

        // if(rootColladaNode == nullptr)
        // {
        //     //unsuccessful
        //     return;
        // }

        // //search for library_geometries
        // XmlNode* libGeometryNodes = nullptr;
        // for(ChildNode& c : rootColladaNode->getChildNodes())
        // {
        //     if(c.type != ChildNode::TYPE_NODE)
        //         continue;
        //     XmlNode* currNode = c.node;
        //     if(StringTools::equalsIgnoreCase<char>(currNode->getTitle(), "library_geometries"))
        //     {
        //         libGeometryNodes = currNode;
        //         break;
        //     }
        // }

        // if(libGeometryNodes == nullptr)
        // {
        //     //unsuccessful
        //     return;
        // }

        // //build geometry arrays
        // for(ChildNode& c : libGeometryNodes->getChildNodes())
        // {
        //     if(c.type != ChildNode::TYPE_NODE)
        //         continue;
        //     XmlNode* currNode = c.node;

        //     std::string name = "";
        //     std::vector<std::string> ids;
        //     std::string vertexSource = "";
        //     size_t currList = 0;

        //     if(StringTools::equalsIgnoreCase<char>(currNode->getTitle(), "geometry"))
        //     {
        //         auto nameAttrib = currNode->getAttribute("name");
        //         if(nameAttrib != nullptr)
        //             name = nameAttrib->data;
                
        //         XmlNode* meshNode = nullptr;

        //         for(ChildNode& innerNode : currNode->getChildNodes())
        //         {
        //             if(innerNode.type != ChildNode::TYPE_NODE)
        //                 continue;

        //             if(StringTools::equalsIgnoreCase<char>(innerNode.node->getTitle(), "mesh"))
        //             {
        //                 meshNode = innerNode.node;
        //                 break;
        //             }
        //         }

        //         if(meshNode == nullptr)
        //             continue;
                
        //         for(ChildNode& c2 : meshNode->getChildNodes())
        //         {
        //             if(c2.type != ChildNode::TYPE_NODE)
        //                 continue;
        //             XmlNode* innerNode = c2.node;

        //             if(StringTools::equalsIgnoreCase<char>(innerNode->getTitle(), "source"))
        //             {
        //                 //make safe
        //                 unsigned char vertAttribType = 0;

        //                 auto temp = innerNode->getAttribute("id");
        //                 if(temp != nullptr)
        //                     ids.push_back( temp->data );
                        
        //                 XmlNode* accessorNode = innerNode->getChildNodes()[1]->getChildNodes()[0];
                        
        //                 temp = accessorNode->getAttribute("stride");
        //                 if(temp != nullptr)
        //                     vertAttribType = stoi(temp->data);
                        
                        
        //                 //add format information
        //                 if(StringTools::equalsIgnoreCase<char>(innerNode->getChildNodes()[0]->getTitle(), "float_array"))
        //                 {
        //                     addVertexFormatInfo(vertAttribType, USAGE_OTHER);
        //                 }
        //                 else if(StringTools::equalsIgnoreCase<char>(innerNode->getChildNodes()[0]->getTitle(), "int_array"))
        //                 {
        //                     addVertexFormatInfo(TYPE_INT, USAGE_OTHER);
        //                     vertAttribType = 0;
        //                 }
                        
        //                 //add vertex attribute information
        //                 std::vector<std::string> split = StringTools::splitString(innerNode->getChildNodes()[0]->getValue(), ' ');
                        
        //                 for(std::string& v : split)
        //                 {
        //                     if(vertAttribType == 0)
        //                     {
        //                         addInt( stoi(v), currList);
        //                     }
        //                     else
        //                     {
        //                         addFloat( stof(v), currList);
        //                     }
        //                 }

        //                 currList++;
        //             }
        //             else if(StringTools::equalsIgnoreCase<char>(innerNode->getTitle(), "vertices"))
        //             {
        //                 auto temp = innerNode->getAttribute("source");
        //                 if(temp != nullptr)
        //                     vertexSource = temp->data;
                        
        //             }
        //             else if(StringTools::equalsIgnoreCase<char>(innerNode->getTitle(), "triangles"))
        //             {
        //                 modelFormat = TRIANGLES;
        //                 XmlNode* lastParentNode = innerNode;
        //                 for(XmlNode* childNodes : lastParentNode->getChildNodes())
        //                 {
        //                     if(StringTools::equalsIgnoreCase<char>(childNodes->getTitle(), "input"))
        //                     {
        //                         //setting usage for the formats
        //                         unsigned char usageNum = USAGE_OTHER;
                                
        //                         std::vector<HashPair<std::string, std::string>*> allAttribs = childNodes->getRawAttributes().getAll();

        //                         for(HashPair<std::string, std::string>* attrib : allAttribs)
        //                         {
        //                             if(StringTools::equalsIgnoreCase<char>(attrib->key, "semantic"))
        //                             {
        //                                 if(StringTools::equalsIgnoreCase<char>(attrib->data, "VERTEX"))
        //                                 {
        //                                     usageNum = USAGE_POSITION;
        //                                 }
        //                                 else if(StringTools::equalsIgnoreCase<char>(attrib->data, "POSITION"))
        //                                 {
        //                                     usageNum = USAGE_POSITION;
        //                                 }
        //                                 else if(StringTools::equalsIgnoreCase<char>(attrib->data, "NORMAL"))
        //                                 {
        //                                     usageNum = USAGE_NORMAL;
        //                                 }
        //                                 else if(StringTools::equalsIgnoreCase<char>(attrib->data, "TEXCOORD"))
        //                                 {
        //                                     usageNum = USAGE_TEXTURE;
        //                                 }
        //                             }
        //                             else if(StringTools::equalsIgnoreCase<char>(attrib->key, "source"))
        //                             {
        //                                 size_t index = 0;
        //                                 while(index < ids.size())
        //                                 {
        //                                     std::string testV = "#" + ids[index];

        //                                     if(StringTools::equalsIgnoreCase<char>(attrib->data, testV))
        //                                     {
        //                                         break;
        //                                     }
        //                                     index++;
        //                                 }

        //                                 if(index < ids.size())
        //                                     formatInfo[index].usage = usageNum;

        //                                 break;
        //                             }
        //                         }
        //                     }
        //                     else if(StringTools::equalsIgnoreCase<char>(childNodes->getTitle(), "p"))
        //                     {
        //                         //index information
        //                         std::vector<std::string> split = StringTools::splitString(childNodes->getValue(), ' ');

        //                         std::vector<unsigned int> tempIndexInfo;
        //                         for(std::string& v : split)
        //                         {
        //                             tempIndexInfo.push_back( stoi(v) );
        //                             if(tempIndexInfo.size() >= formatInfo.size())
        //                             {
        //                                 addIndicies(tempIndexInfo);
        //                                 tempIndexInfo.clear();
        //                             }
        //                         }

        //                     }
        //                 }
        //             }
        //         }
        //     }
        // }
    }

    void Model::saveSTL(File file)
    {
        //must be a valid format type. So triangles or quads
        if(modelFormat <= LINE_LOOP)
            return;
        
        //must have at least position defined. Normal will be calculated from triangle def
        int indexOfPositions = -1;
        int sizeOfPosition = 0;
        for(int i=0; i<formatInfo.size(); i++)
        {
            if(formatInfo[i].usage == USAGE_POSITION)
            {
                indexOfPositions = i;
                sizeOfPosition = formatInfo[i].size;
                break;
            }
        }

        if(indexOfPositions <0 || sizeOfPosition < 2)
        {
            //can't reliably save stl
            return;
        }

        SimpleFile outputFile = SimpleFile(file, SimpleFile::WRITE | SimpleFile::ASCII);
        if(outputFile.isOpen())
        {
            //write 80 bytes of junk. Specify Standard Binary STL created with GLib
            std::string headerWithoutJunk = "Standard Binary STL created with GLib.";
            outputFile.writeString(headerWithoutJunk);
            //write a bunch of zeros as junk
            for(int i=headerWithoutJunk.size(); i<80; i++)
                outputFile.writeByte(0);

            //now write the number of triangles
            uint32_t numTriangles = 0;
            
            //simplify things by using Triangle2DModel and Triangle3DModel classes
            if(sizeOfPosition == 2)
            {
                Triangle2DModel justTriangles = convertTo2DTriModel();
                numTriangles = justTriangles.size();
                outputFile.writeBytes((unsigned char*)&numTriangles, 4);
                
                for(int i=0; i<numTriangles; i++)
                {
                    Triangle2D tri = justTriangles.get(i);
                    Vec3f v1 = tri.getVertex1();
                    Vec3f v2 = tri.getVertex2();
                    Vec3f v3 = tri.getVertex3();
                    
                    Vec3f normal = Vec3f(0, 0, 1);

                    saveSTLVec3f(outputFile, normal);
                    saveSTLVec3f(outputFile, v1);
                    saveSTLVec3f(outputFile, v2);
                    saveSTLVec3f(outputFile, v3);
                    //no additional attributes
                    outputFile.writeByte(0);
                    outputFile.writeByte(0);
                }
            }
            else if(sizeOfPosition > 2)
            {
                Triangle3DModel justTriangles = convertTo3DTriModel();
                numTriangles = justTriangles.size();
                outputFile.writeBytes((unsigned char*)&numTriangles, 4);
                
                for(int i=0; i<numTriangles; i++)
                {
                    Triangle3D tri = justTriangles.get(i);
                    Vec3f v1 = tri.getVertex1();
                    Vec3f v2 = tri.getVertex2();
                    Vec3f v3 = tri.getVertex3();
                    
                    Vec3f normal = v1.crossProduct(v2);

                    saveSTLVec3f(outputFile, normal);
                    saveSTLVec3f(outputFile, v1);
                    saveSTLVec3f(outputFile, v2);
                    saveSTLVec3f(outputFile, v3);
                    //no additional attributes
                    outputFile.writeByte(0);
                    outputFile.writeByte(0);
                }
            }
        }

        outputFile.close();
    }

    void Model::saveSTLVec3f(SimpleFile& file, Vec3f v)
    {
        saveSTLFloat(file, v.x);
        saveSTLFloat(file, v.y);
        saveSTLFloat(file, v.z);
    }
    void Model::saveSTLFloat(SimpleFile& file, float v)
    {
        file.writeBytes((unsigned char*)&v, 4);
    }

} //NAMESPACE glib END