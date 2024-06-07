#include "ext/GLModel.h"

#ifdef USE_OPENGL
    namespace smpl
    {

        GLModel::GLModel()
        {
            shouldDelete = true;
        }
        
        GLModel::GLModel(GLModel& other)
        {
            vaoID = other.vaoID;
            indexedBuffer = other.indexedBuffer;
            vboIDs = other.vboIDs;
            vboSizes = other.vboSizes;
            attributeEnabled = other.attributeEnabled;
            
            modType = other.modType;
            drawType = other.drawType;
            usage = other.usage;
            size = other.size;
            shouldDelete = true;
        }

        void GLModel::operator=(GLModel& other)
        {
            vaoID = other.vaoID;
            indexedBuffer = other.indexedBuffer;
            vboIDs = other.vboIDs;
            vboSizes = other.vboSizes;
            attributeEnabled = other.attributeEnabled;
            
            modType = other.modType;
            drawType = other.drawType;
            usage = other.usage;
            size = other.size;
            shouldDelete = true;
        }

        GLModel::~GLModel()
        {
            if(shouldDelete)
            {
                if(!GLSingleton::getInit())
                {
                    return;
                }
                glDeleteBuffers(vboIDs.size(), vboIDs.data());
                
                if(vaoID != 0)
                    glDeleteVertexArrays(1, &vaoID);
                
                if(indexedBuffer != 0)
                    glDeleteBuffers(1, &indexedBuffer);

                vboIDs.clear();
                vboSizes.clear();
                attributeEnabled.clear();
                vaoID = 0;
                indexedBuffer = 0;
                size = 0;
            }
        }

        void GLModel::draw()
        {
            if(vaoID != 0)
            {
                glBindVertexArray(vaoID);

                for(int i=0; i<attributeEnabled.size(); i++)
                {
                    if(attributeEnabled[i])
                        glEnableVertexAttribArray(i);
                    else
                        glDisableVertexAttribArray(i);
                }

                if(modType == TYPE_ARRAY)
                {
                    glDrawArrays(drawType, 0, size);
                }
                else
                {
                    if(indexedBuffer != 0)
                    {
                        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexedBuffer);

                        glDrawElements(drawType, size, GL_UNSIGNED_INT, nullptr);

                        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                    }
                }

                glBindVertexArray(0);
            }
        }
        
        GLModel GLModel::convertModel(Model* m, int usage)
        {
            if(!GLSingleton::getInit())
            {
                return GLModel();
            }

            GLModel glModel;
            

            glModel.setDrawType(m->getModelFormat());

            glModel.modType = m->getIndexed();
            glModel.size = m->size();
            glModel.usage = usage;
            glModel.shouldDelete = false;

            glGenVertexArrays(1, &glModel.vaoID);
            glBindVertexArray(glModel.vaoID);

            std::vector<std::vector<int>> rawVertexData = m->getRawVertexData();
            std::vector<std::vector<unsigned int>> rawIndexData = m->getRawIndexData();
            
            //Generate vbos for vertex data.
            std::vector<VertexFormat> vFormat = m->getVertexFormatInfomation();
            glModel.vboIDs = std::vector<unsigned int>(vFormat.size());
            glModel.vboSizes = std::vector<size_t>(vFormat.size());
            glModel.attributeEnabled = std::vector<bool>(vFormat.size());

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
                    glModel.vboSizes[i] = nSize;

                    glBufferData(GL_ARRAY_BUFFER, nSize, rawVertexData[i].data(), usage);

                    if(f.type == Model::TYPE_INT)
                        glVertexAttribPointer(i, (int)f.size, GL_INT, GL_FALSE, f.size*dataSize, nullptr);
                    else
                        glVertexAttribPointer(i, (int)f.size, GL_FLOAT, GL_FALSE, f.size*dataSize, nullptr);
                    
                    glModel.attributeEnabled[i] = true;
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
                    glModel.vboSizes[i] = nSize;

                    glBufferData(GL_ARRAY_BUFFER, nSize, nVertexData[i].data(), usage);

                    if(f.type == Model::TYPE_INT)
                        glVertexAttribPointer(i, f.size, GL_INT, GL_FALSE, f.size*dataSize, nullptr);
                    else
                        glVertexAttribPointer(i, f.size, GL_FLOAT, GL_FALSE, f.size*dataSize, nullptr);
                    

                    glModel.attributeEnabled[i] = true;
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
            if(!GLSingleton::getInit())
            {
                return GLModel();
            }

            GLModel m;

            m.size = 3;
            m.modType = TYPE_ARRAY;
            m.drawType = GL_TRIANGLES;
            m.vboIDs.push_back(0);
            m.vboSizes.push_back(9*sizeof(float));
            m.attributeEnabled.push_back(true);
            m.shouldDelete = false;


            float verticies[] = {
                -0.5f, -0.5f, 0.0f,
                0.0f, 0.5f, 0.0,
                0.5f, -0.5f, 0.0f
            };

            glGenVertexArrays(1, &m.vaoID);
            glBindVertexArray(m.vaoID);

            glGenBuffers(1, m.vboIDs.data());
            glBindBuffer(GL_ARRAY_BUFFER, m.vboIDs[0]);

            glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), nullptr);
            
            glEnableVertexAttribArray(0);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);

            return m;
        }

        void GLModel::createVAO()
        {
            //used to create VAO if it does not exists
            if(vaoID == NULL)
            {
                glGenVertexArrays(1, &vaoID);
            }
        }

        void GLModel::storeDataInt(int attributeID, std::vector<int> data, int valuesPerVertex, bool forceReallocate)
        {
            createVAO(); //Only if it does not exist already. Otherwise, this does nothing
            size_t totalByteSize = data.size()*sizeof(int);
            this->size = data.size() / valuesPerVertex;

            glBindVertexArray(vaoID);
            if(attributeID < 0)
                return; //error

            if(attributeID >= vboIDs.size())
            {
                for(int i=vboIDs.size(); i<=attributeID; i++)
                {
                    vboIDs.push_back(0);
                    vboSizes.push_back(0);
                    attributeEnabled.push_back(false);
                }
            }

            if(vboIDs[attributeID] == 0)
            {
                glGenBuffers(1, &vboIDs[attributeID]);
            }
            glBindBuffer(GL_ARRAY_BUFFER, vboIDs[attributeID]);

            if(vboSizes[attributeID] >= totalByteSize && !forceReallocate)
            {
                //modify in place
                glBufferSubData(GL_ARRAY_BUFFER, 0, totalByteSize, data.data());
            }
            else
            {
                //reallocate buffer
                glBufferData(GL_ARRAY_BUFFER, totalByteSize, data.data(), usage);
            }

            vboSizes[attributeID] = totalByteSize;
            attributeEnabled[attributeID] = true;
            glEnableVertexAttribArray(attributeID);
            glVertexAttribPointer(attributeID, valuesPerVertex, GL_INT, false, valuesPerVertex*sizeof(int), nullptr);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindVertexArray(0);
        }

        void GLModel::storeDataFloat(int attributeID, std::vector<float> data, int valuesPerVertex, bool forceReallocate)
        {
            createVAO(); //Only if it does not exist already. Otherwise, this does nothing
            size_t totalByteSize = data.size()*sizeof(float);
            this->size = data.size() / valuesPerVertex;

            glBindVertexArray(vaoID);
            if(attributeID < 0)
                return; //error

            if(attributeID >= vboIDs.size())
            {
                for(int i=vboIDs.size(); i<=attributeID; i++)
                {
                    vboIDs.push_back(0);
                    vboSizes.push_back(0);
                    attributeEnabled.push_back(false);
                }
            }

            if(vboIDs[attributeID] == 0)
            {
                glGenBuffers(1, &vboIDs[attributeID]);
            }
            glBindBuffer(GL_ARRAY_BUFFER, vboIDs[attributeID]);

            if(vboSizes[attributeID] >= totalByteSize && !forceReallocate)
            {
                //modify in place
                glBufferSubData(GL_ARRAY_BUFFER, 0, totalByteSize, data.data());
            }
            else
            {
                //reallocate buffer
                glBufferData(GL_ARRAY_BUFFER, totalByteSize, data.data(), usage);
            }

            vboSizes[attributeID] = totalByteSize;
            attributeEnabled[attributeID] = true;
            glEnableVertexAttribArray(attributeID);
            glVertexAttribPointer(attributeID, valuesPerVertex, GL_FLOAT, false, valuesPerVertex*sizeof(float), nullptr);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindVertexArray(0);
        }

        void GLModel::storeDataFloat(int attributeID, std::vector<double> data, int valuesPerVertex, bool forceReallocate)
        {
            //convert to float array
            std::vector<float> nData = std::vector<float>(data.size());
            for(int i=0; i<data.size(); i++)
            {
                nData[i] = (float)data[i];
            }
            
            storeDataFloat(attributeID, nData, valuesPerVertex, forceReallocate);
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

        void GLModel::setAttributeEnabled(int index, bool v)
        {
            if(index>=0 && index<=attributeEnabled.size())
                attributeEnabled[index] = v;
        }

        void GLModel::disableAllAttributes()
        {
            for(int i=0; i<attributeEnabled.size(); i++)
            {
                attributeEnabled[i] = false;
            }
        }

        void GLModel::setDrawType(int value)
        {
            switch(value)
            {
                case Model::POINTS:
                    drawType = GL_POINTS;
                    break;
                case Model::LINES:
                    drawType = GL_LINES;
                    break;
                case Model::LINE_STRIP:
                    drawType = GL_LINE_STRIP;
                    break;
                case Model::LINE_LOOP:
                    drawType = GL_LINE_LOOP; //No longer valid. Replace
                    break;
                case Model::TRIANGLES:
                    drawType = GL_TRIANGLES;
                    break;
                case Model::TRIANGLE_STRIP:
                    drawType = GL_TRIANGLE_STRIP;
                    break;
                case Model::TRIANGLE_FAN:
                    drawType = GL_TRIANGLE_FAN; //No longer valid. Replace
                    break;
                case Model::QUADS:
                    drawType = GL_QUADS; //No longer valid. Replace
                    break;
                case Model::QUAD_STRIP:
                    drawType = GL_QUAD_STRIP; //No longer valid. Replace
                    break;
                default:
                    drawType = GL_POINT;
                    break;
            }
        }
    }
#endif