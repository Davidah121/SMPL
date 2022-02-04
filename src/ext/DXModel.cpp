#include "ext/DXModel.h"

namespace glib
{

    DXModel::DXModel()
    {
        shouldDelete = true;
    }

    DXModel::DXModel(DXModel& other)
    {
        modType = other.modType;
        drawType = other.drawType;
        vertexData = other.vertexData;
        offsets = other.offsets;
        strides = other.strides;
        indexData = other.indexData;
        size = other.size;
        shouldDelete = true;
    }

    void DXModel::operator=(DXModel& other)
    {
        modType = other.modType;
        drawType = other.drawType;
        vertexData = other.vertexData;
        offsets = other.offsets;
        strides = other.strides;
        indexData = other.indexData;
        size = other.size;
        shouldDelete = true;
    }
    
    DXModel::~DXModel()
    {
        if(shouldDelete)
        {
            for(int i=0; i<vertexData.size(); i++)
            {
                if(vertexData[i]!=nullptr)
                    vertexData[i]->Release();
                
                vertexData[i] = nullptr;
            }
            
            if(indexData!=nullptr)
                indexData->Release();
            
            indexData = nullptr;

            vertexData.clear();
            offsets.clear();
            strides.clear();
        }
    }

    void DXModel::draw()
    {
        ID3D11DeviceContext* devContext = DXSingleton::getContext();

        //setup a new list of all active attributes here
        //

        if(modType == TYPE_ARRAY)
        {
            devContext->IASetVertexBuffers(0, vertexData.size(), vertexData.data(), strides.data(), offsets.data());
            devContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)drawType);

            devContext->Draw(size, 0);
        }
        else
        {
            devContext->IASetVertexBuffers(0, vertexData.size(), vertexData.data(), strides.data(), offsets.data());
            devContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)drawType);
            devContext->IASetIndexBuffer(indexData, DXGI_FORMAT_R32_UINT, 0);

            devContext->DrawIndexed(size, 0, 0);
        }
    }

    DXModel DXModel::convertModel(Model* m)
    {
        ID3D11Device* devPointer = DXSingleton::getDevice();

        DXModel model;
        model.shouldDelete = false;

        model.setDrawType(m->getModelFormat());

        model.modType = m->getIndexed();
        model.size = m->size();

        std::vector<std::vector<int>> rawVertexData = m->getRawVertexData();
        std::vector<std::vector<unsigned int>> rawIndexData = m->getRawIndexData();
        
        std::vector<VertexFormat> vFormat = m->getVertexFormatInfomation();
        model.vertexData = std::vector<ID3D11Buffer*>(vFormat.size());
        model.offsets = std::vector<unsigned int>(vFormat.size());
        model.strides = std::vector<unsigned int>(vFormat.size());
        model.attributeEnabled = std::vector<bool>(vFormat.size());

        if(model.modType != TYPE_INDEXED)
        {
            int i=0;
            for(VertexFormat& f : vFormat)
            {
                //always 4; sizeof(int) == sizeof(float) == 4
                int dataSize = sizeof(int);
                size_t nSize = dataSize * rawVertexData[i].size();

                D3D11_BUFFER_DESC bd = {0};
                bd.ByteWidth = nSize;
                bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

                D3D11_SUBRESOURCE_DATA srd;
                srd.pSysMem = rawVertexData[i].data();
                srd.SysMemPitch = 0;
                srd.SysMemSlicePitch = 0;

                devPointer->CreateBuffer(&bd, &srd, &model.vertexData[i]);

                model.strides[i] = dataSize * f.size;
                model.offsets[i] = 0;
                model.attributeEnabled[i] = true;
                i++;
            }
        }
        else if(model.modType == TYPE_INDEXED)
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

                D3D11_BUFFER_DESC bd = {0};
                bd.ByteWidth = nSize;
                bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

                D3D11_SUBRESOURCE_DATA srd;
                srd.pSysMem = nVertexData[i].data();
                srd.SysMemPitch = 0;
                srd.SysMemSlicePitch = 0;

                devPointer->CreateBuffer(&bd, &srd, &model.vertexData[i]);

                model.strides[i] = dataSize * f.size;
                model.offsets[i] = 0;
                model.attributeEnabled[i] = true;
                i++;
            }

            model.size = nIndexData.size();

            D3D11_BUFFER_DESC bd = {0};
            bd.ByteWidth = nIndexData.size() * sizeof(int);
            bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

            D3D11_SUBRESOURCE_DATA srd;
            srd.pSysMem = nIndexData.data();
            srd.SysMemPitch = 0;
            srd.SysMemSlicePitch = 0;

            devPointer->CreateBuffer(&bd, &srd, &model.indexData);
        }

        return model;
    }

    DXModel DXModel::generateTestModel()
    {
        DXModel model;
        model.vertexData.push_back(nullptr);
        model.offsets.push_back(0);
        model.strides.push_back(sizeof(float)*3);
        
        model.size = 3;

        model.shouldDelete = false;
        model.modType = TYPE_ARRAY;

        float verticies[] = {
            -0.5f, -0.5f, 0.0f,
            0.0f, 0.5f, 0.0f,
            0.5f, -0.5f, 0.0f
        };

        D3D11_BUFFER_DESC bd = {0};
        bd.ByteWidth = sizeof(float)*9; //sizeof(float) * numberOfValuesInTotal
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA srd;
        srd.pSysMem = verticies;
        srd.SysMemPitch = 0;
        srd.SysMemSlicePitch = 0;

        ID3D11Device* devPointer = DXSingleton::getDevice();
        devPointer->CreateBuffer(&bd, &srd, &model.vertexData[0]);

        return model;
    }

    bool DXModel::getModelType()
    {
        return modType;
    }

    int DXModel::getDrawType()
    {
        return drawType;
    }

    size_t DXModel::getSize()
    {
        return size;
    }

    void DXModel::storeDataInt(int attributeID, std::vector<int> data, int valuesPerVertex, bool forceReallocate)
    {
        //ignore in-place modifications for now
        size_t totalByteSize = data.size()*sizeof(int);
        this->size = data.size() / valuesPerVertex;

        if(attributeID < 0)
            return; //error
        
        if(attributeID >= vertexData.size())
        {
            for(int i=vertexData.size(); i<=attributeID; i++)
            {
                vertexData.push_back(nullptr);
                offsets.push_back(0);
                strides.push_back(0);
                attributeEnabled.push_back(false);
            }
        }

        if(vertexData[attributeID] != nullptr)
        {
            //potentially remove or resize or reuse
            //for now, just free the memory then re-create
            if(vertexData[attributeID]!=nullptr)
                vertexData[attributeID]->Release();
            
            vertexData[attributeID] = nullptr;
        }

        D3D11_BUFFER_DESC bd = {0};
        bd.ByteWidth = sizeof(int)*valuesPerVertex*size;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA srd;
        srd.pSysMem = data.data();
        srd.SysMemPitch = 0;
        srd.SysMemSlicePitch = 0;

        ID3D11Device* devPointer = DXSingleton::getDevice();
        devPointer->CreateBuffer(&bd, &srd, &vertexData[attributeID]);

        offsets[attributeID] = 0;
        strides[attributeID] = sizeof(int)*valuesPerVertex;
        attributeEnabled[attributeID] = false;
    }
    void DXModel::storeDataFloat(int attributeID, std::vector<float> data, int valuesPerVertex, bool forceReallocate)
    {
        //ignore in-place modifications for now
        size_t totalByteSize = data.size()*sizeof(int);
        this->size = data.size() / valuesPerVertex;

        if(attributeID < 0)
            return; //error
        
        if(attributeID >= vertexData.size())
        {
            for(int i=vertexData.size(); i<=attributeID; i++)
            {
                vertexData.push_back(nullptr);
                offsets.push_back(0);
                strides.push_back(0);
                attributeEnabled.push_back(false);
            }
        }

        if(vertexData[attributeID] != nullptr)
        {
            //potentially remove or resize or reuse
            //for now, just free the memory then re-create
            if(vertexData[attributeID]!=nullptr)
                vertexData[attributeID]->Release();
            
            vertexData[attributeID] = nullptr;
        }

        D3D11_BUFFER_DESC bd = {0};
        bd.ByteWidth = sizeof(float)*valuesPerVertex*size;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA srd;
        srd.pSysMem = data.data();
        srd.SysMemPitch = 0;
        srd.SysMemSlicePitch = 0;

        ID3D11Device* devPointer = DXSingleton::getDevice();
        devPointer->CreateBuffer(&bd, &srd, &vertexData[attributeID]);

        offsets[attributeID] = 0;
        strides[attributeID] = sizeof(float)*valuesPerVertex;
        attributeEnabled[attributeID] = false;
    }

    void DXModel::storeDataFloat(int attributeID, std::vector<double> data, int valuesPerVertex, bool forceReallocate)
    {
        //convert to float array
        std::vector<float> nData = std::vector<float>(data.size());
        for(int i=0; i<data.size(); i++)
        {
            nData[i] = (float)data[i];
        }
        
        storeDataFloat(attributeID, nData, valuesPerVertex, forceReallocate);
    }
    
    void DXModel::setAttributeEnabled(int attributeID, bool v)
    {
        if(attributeID>=0 && attributeID<=attributeEnabled.size())
            attributeEnabled[attributeID] = v;
    }

    void DXModel::disableAllAttributes()
    {
        for(int i=0; i<attributeEnabled.size(); i++)
        {
            attributeEnabled[i] = false;
        }
    }

    void DXModel::setDrawType(int value)
    {
        switch(value)
        {
            case Model::POINTS:
                drawType = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
                break;
            case Model::LINES:
                drawType = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
                break;
            case Model::LINE_STRIP:
                drawType = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
                break;
            case Model::TRIANGLES:
                drawType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
                break;
            case Model::TRIANGLE_STRIP:
                drawType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
                break;
            default:
                drawType = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
                break;
        }
    }
};