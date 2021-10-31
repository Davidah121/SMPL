#include "ext/DXModel.h"

namespace glib
{

    DXModel::DXModel()
    {
        float verticies[] = {
            -0.5f, -0.5f, 0.0f,
            0.0f, 0.5f, 0.0f,
            0.5f, -0.5f, 0.0f
        };

        D3D11_BUFFER_DESC bd = {0};
        bd.ByteWidth = sizeof(float)*9;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA srd;
        srd.pSysMem = verticies;
        srd.SysMemPitch = 0;
        srd.SysMemSlicePitch = 0;

        ID3D11Device* devPointer = DXSingleton::getDevice();

        devPointer->CreateBuffer(&bd, &srd, &modelData);
    }

    DXModel::~DXModel()
    {
        if(modelData!=nullptr)
            modelData->Release();
        
        modelData = nullptr;
    }

    void DXModel::draw()
    {
        unsigned int stride = sizeof(float)*3;
        unsigned int offset = 0;

        ID3D11DeviceContext* devContext = DXSingleton::getContext();
        devContext->IASetVertexBuffers(0, 1, &modelData, &stride, &offset);
        devContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        devContext->Draw(3, 0);
    }
};