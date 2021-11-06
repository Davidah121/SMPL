#include "ext/GLModel.h"

namespace glib
{

    GLModel::GLModel()
    {

    }

    GLModel::~GLModel()
    {
        glDeleteBuffers(vboIDs.size(), vboIDs.data());
        
        if(vaoID != 0)
            glDeleteVertexArrays(1, &vaoID);
        
        if(indexedBuffer != 0)
            glDeleteBuffers(1, &indexedBuffer);

        vboIDs.clear();
        vaoID = 0;
        indexedBuffer = 0;
    }

    void GLModel::draw()
    {
        if(vaoID != 0)
        {
            if(modType == TYPE_ARRAY)
            {
                glBindVertexArray(vaoID);
                glDrawArrays(drawType, first, size);
                glBindVertexArray(0);
            }
            else
            {
                if(indexedBuffer != 0)
                {
                    glBindVertexArray(vaoID);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexedBuffer);

                    glDrawElements(drawType, size, GL_UNSIGNED_INT, nullptr);

                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                    glBindVertexArray(0);
                }
            }
        }
    }
    
    GLModel GLModel::convertModel(Model* m, int usage)
    {
        GLModel glModel;
        switch(m->getModelFormat())
        {
            case Model::POINTS:
                glModel.drawType = GL_POINT;
                break;
            case Model::LINES:
                glModel.drawType = GL_LINE;
                break;
            case Model::LINE_STRIP:
                glModel.drawType = GL_LINE_STRIP;
                break;
            case Model::LINE_LOOP:
                glModel.drawType = GL_LINE_LOOP;
                break;
            case Model::TRIANGLES:
                glModel.drawType = GL_TRIANGLES;
                break;
            case Model::TRIANGLE_STRIP:
                glModel.drawType = GL_TRIANGLE_STRIP;
                break;
            case Model::TRIANGLE_FAN:
                glModel.drawType = GL_TRIANGLE_FAN;
                break;
            case Model::QUADS:
                glModel.drawType = GL_QUADS;
                break;
            case Model::QUAD_STRIP:
                glModel.drawType = GL_QUAD_STRIP;
                break;
            default:
                break;
        }

        glModel.modType = m->getIndexed();
        glModel.size = m->size();
        glModel.usage = usage;

        glGenVertexArrays(1, &glModel.vaoID);
        glBindVertexArray(glModel.vaoID);

        std::vector<std::vector<int>> rawVertexData = m->getRawVertexData();
        std::vector<std::vector<unsigned int>> rawIndexData = m->getRawIndexData();
        
        //Generate vbos for vertex data.
        std::vector<VertexFormat> vFormat = m->getVertexFormatInfomation();
        glModel.vboIDs = std::vector<unsigned int>(vFormat.size());
        
        glGenBuffers(glModel.vboIDs.size(), glModel.vboIDs.data());

        if(glModel.modType != TYPE_INDEXED)
        {
            int i=0;
            for(VertexFormat& f : vFormat)
            {
                //always 4; sizeof(int) == sizeof(float) == 4
                int dataSize = sizeof(int);
                size_t nSize = dataSize * rawVertexData[i].size();
                glBindBuffer(GL_ARRAY_BUFFER, glModel.vboIDs[i]);
                glBufferData(GL_ARRAY_BUFFER, nSize, rawVertexData[i].data(), usage);

                if(f.type == Model::TYPE_INT)
                    glVertexAttribPointer(i, f.size, GL_INT, GL_FALSE, f.size*dataSize, nullptr);
                else
                    glVertexAttribPointer(i, f.size, GL_FLOAT, GL_FALSE, f.size*dataSize, nullptr);
                
                glEnableVertexAttribArray(i);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                i++;
            }
        }
        else if(glModel.modType == TYPE_INDEXED)
        {
            //generate a list of unique verticies
            //Cannot use multiple index values on a vertex
            std::vector<std::vector<unsigned int>> uniqueIndexGroups;
            std::vector<unsigned int> nIndexData;

            for(int i=0; i<rawIndexData.size(); i++)
            {
                bool exists = false;
                long index = -1;

                //search in the uniqueIndexGroup for existance
                for(int j=0; j<uniqueIndexGroups.size(); j++)
                {
                    int cmpVal = memcmp(uniqueIndexGroups[j].data(), rawIndexData[i].data(), vFormat.size()*sizeof(unsigned int));
                    if(cmpVal == 0)
                    {
                        index = j;
                        exists = true;
                        break;
                    }
                }

                if(!exists)
                {
                    //add to uniqueIndexGroup
                    uniqueIndexGroups.push_back( rawIndexData[i] );
                }

                if(index != -1)
                {
                    nIndexData.push_back( (unsigned int)index );
                }
                else
                {
                    nIndexData.push_back( uniqueIndexGroups.size()-1 );
                }
            }

            //Next, generate unique vertex data for the buffers
            std::vector<std::vector<int>> nVertexData = std::vector<std::vector<int>>(vFormat.size());

            for(int i=0; i<uniqueIndexGroups.size(); i++)
            {
                for(int j=0; j<nVertexData.size(); j++)
                {
                    int vertLocation = uniqueIndexGroups[i][j] * vFormat[j].size;
                    for(int k=0; k<vFormat[j].size; k++)
                    {
                        nVertexData[j].push_back( rawVertexData[j][vertLocation + k]);
                    }
                }
            }

            //Lastly, put everything into buffers
            int i=0;
            for(VertexFormat& f : vFormat)
            {
                //always 4; sizeof(int) == sizeof(float) == 4
                int dataSize = sizeof(int);
                size_t nSize = dataSize * nVertexData[i].size();
                glBindBuffer(GL_ARRAY_BUFFER, glModel.vboIDs[i]);
                glBufferData(GL_ARRAY_BUFFER, nSize, nVertexData[i].data(), usage);

                if(f.type == Model::TYPE_INT)
                    glVertexAttribPointer(i, f.size, GL_INT, GL_FALSE, f.size*dataSize, nullptr);
                else
                    glVertexAttribPointer(i, f.size, GL_FLOAT, GL_FALSE, f.size*dataSize, nullptr);
                
                glEnableVertexAttribArray(i);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                i++;
            }

            glModel.size = nIndexData.size();

            glGenBuffers(1, &glModel.indexedBuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glModel.indexedBuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, glModel.size*4, nIndexData.data(), usage);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }

        glBindVertexArray(0);

        return glModel;
    }

    GLModel GLModel::generateTestModel()
    {
        GLModel m;

        m.size = 3;
        m.modType = TYPE_ARRAY;
        m.first = 0;
        m.drawType = GL_TRIANGLES;
        m.vboIDs.push_back(0);


        float verticies[] = {
            -0.5f, -0.5f, 0.0f,
            0.0f, 0.5f, 0.0,
            0.5f, -0.5f, 0.0f
        };

        glGenVertexArrays(1, &m.vaoID);
        glBindVertexArray(m.vaoID);

        glGenBuffers(1, &m.vboIDs[0]);

        glBindBuffer(GL_ARRAY_BUFFER, m.vboIDs[0]);

        glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        return m;
    }

    bool GLModel::getModelType()
    {
        return modType;
    }

    int GLModel::getDrawType()
    {
        return drawType;
    }

    int GLModel::getModelUsage()
    {
        return usage;
    }

    size_t GLModel::getSize()
    {
        return size;
    }

}