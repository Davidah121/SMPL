#include "ext/DXShader.h"

namespace glib
{

    DXShader::DXShader(File vertexShaderFile, File fragmentShaderFile)
    {
        std::vector<unsigned char> vData, fData;

        SimpleFile f = SimpleFile(vertexShaderFile, SimpleFile::READ | SimpleFile::ASCII);
        if(f.isOpen())
        {
            vData = f.readFullFileAsBytes();
            f.close();
        }
        else
        {
            //ERROR LOADING VERTEX SHADER
            StringTools::println("ERROR LOADING VERTEX SHADER - %ls", vertexShaderFile.getFullFileName().c_str());
            valid = false;
        }

        SimpleFile f2 = SimpleFile(fragmentShaderFile, SimpleFile::READ | SimpleFile::ASCII);
        if(f2.isOpen())
        {
            fData = f2.readFullFileAsBytes();
            f2.close();
        }
        else
        {
            //ERROR LOADING FRAGMENT SHADER
            StringTools::println("ERROR LOADING FRAGMENT SHADER - %ls", fragmentShaderFile.getFullFileName().c_str());
            valid = false;
        }

        ID3D11Device* dev = DXSingleton::getDevice();
        ID3D11DeviceContext* devContext = DXSingleton::getContext();

        HRESULT err;

        err = dev->CreateVertexShader(vData.data(), vData.size(), nullptr, &vertexID);

        if(err != NO_ERROR)
        {
            StringTools::println("ERROR CREATING VERTEX SHADER - %ls", vertexShaderFile.getFullFileName().c_str());
            StringTools::println("ERROR VALUE %lx", err);
            valid = false;
        }

        err = dev->CreatePixelShader(fData.data(), fData.size(), nullptr, &fragmentID);

        if(err != NO_ERROR)
        {
            StringTools::println("ERROR CREATING FRAGMENT SHADER - %ls", fragmentShaderFile.getFullFileName().c_str());
            StringTools::println("ERROR VALUE %lx", err);
            valid = false;
        }
        
        inputDesc.SemanticName = "POSITION";
        inputDesc.SemanticIndex = 0;
        inputDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
        inputDesc.InputSlot = 0;
        inputDesc.AlignedByteOffset = 0;
        inputDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        inputDesc.InstanceDataStepRate = 0;
        
        err = dev->CreateInputLayout(&inputDesc, 1, vData.data(), vData.size(), &layout);

        if(err != NO_ERROR)
        {
            StringTools::println("ERROR CREATING LAYOUT");
            StringTools::println("ERROR VALUE %lx", err);
        }
        devContext->IASetInputLayout(layout);

        if(!valid)
            valid = true;
    }

    DXShader::~DXShader()
    {
        if(vertexID != nullptr)
            vertexID->Release();
        if(fragmentID != nullptr)
            fragmentID->Release();
        if(layout != nullptr)
            layout->Release();

        vertexID = nullptr;
        fragmentID = nullptr;
        valid = false;
    }

    void DXShader::setAsActive()
    {
        ID3D11DeviceContext* context = DXSingleton::getContext();

        context->VSSetShader(vertexID, nullptr, 0);
        context->PSSetShader(fragmentID, nullptr, 0);
    }

    void DXShader::setUniformData(void* buffer, size_t sizeOfBuffer, int location, int type)
    {
        ID3D11DeviceContext* context = DXSingleton::getContext();
        ID3D11Device* device = DXSingleton::getDevice();

        ID3D11Buffer* d3dBuffer = nullptr;

        D3D11_BUFFER_DESC desc;
        desc.ByteWidth = sizeOfBuffer;
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA subResource;
        subResource.pSysMem = buffer;
        subResource.SysMemPitch = 0;
        subResource.SysMemSlicePitch = 0;

        HRESULT hr = device->CreateBuffer( &desc, &subResource, &d3dBuffer);

        if(FAILED(hr))
        {
            //ERROR
            return;
        }

        if(type==TYPE_VERTEX)
            context->VSSetConstantBuffers(location, 1, &d3dBuffer);
        else if(type==TYPE_FRAGMENT)
            context->PSSetConstantBuffers(location, 1, &d3dBuffer);

        d3dBuffer->Release();
    }

    ID3D11VertexShader* DXShader::getVertexShaderPointer()
    {
        return vertexID;
    }

    ID3D11PixelShader* DXShader::getFragmentShaderPointer()
    {
        return fragmentID;
    }

    bool DXShader::getValid()
    {
        return valid;
    }
};