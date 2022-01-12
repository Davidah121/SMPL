#include "ext/DXTexture.h"

namespace glib
{
    DXTexture::DXTexture(File f, bool includeAlpha)
    {
        loadImage(f, includeAlpha);
    }

    DXTexture::DXTexture(Image* img, bool includeAlpha)
    {
        setImage(img, includeAlpha);
    }

    DXTexture::~DXTexture()
    {
        if(textureID != nullptr)
            textureID->Release();
        textureID = nullptr;
        
        if(textureView != nullptr)
            textureView->Release();
        textureView = nullptr;
        
        if(sampler != nullptr)
            sampler->Release();
        sampler = nullptr;
    }

    void DXTexture::loadImage(File f, bool includeAlpha)
    {
        int imgSize = 0;
        Image** imgPointer = Image::loadImage(f, &imgSize);

        if(imgSize > 0)
        {
            setImage(imgPointer[0], includeAlpha);
        }

        for(int i=0; i<imgSize; i++)
        {
            delete imgPointer[i];
        }
        delete[] imgPointer;
    }

    void DXTexture::setImage(Image* img, bool includeAlpha)
    {
        ID3D11Device* dev = DXSingleton::getDevice();

        if(img != nullptr)
        {
            width = img->getWidth();
            height = img->getHeight();

            D3D11_TEXTURE2D_DESC desc = {0};
            desc.Width = width;
            desc.Height = height;
            desc.MipLevels = 1;
            desc.ArraySize = 1;
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

            //set the data for the texture
            if(!includeAlpha)
            {
                unsigned int totalSize = width*height;

                Color* oldPixels = img->getPixels();
                Color* nPixels = new Color[totalSize];
                for(int i=0; i<totalSize; i++)
                {
                    *nPixels = *oldPixels;
                    nPixels->alpha = 255;
                    nPixels++;
                    oldPixels++;
                }

                D3D11_SUBRESOURCE_DATA initData;
                initData.pSysMem = nPixels;
                initData.SysMemPitch = width*sizeof(Color);
                initData.SysMemSlicePitch = totalSize*sizeof(Color);

                desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                dev->CreateTexture2D(&desc, &initData, &textureID);
            }
            else
            {
                D3D11_SUBRESOURCE_DATA initData;
                initData.pSysMem = img->getPixels();
                initData.SysMemPitch = width*sizeof(Color);
                initData.SysMemSlicePitch = width*height*sizeof(Color);

                desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                dev->CreateTexture2D(&desc, &initData, &textureID);
            }

            D3D11_SHADER_RESOURCE_VIEW_DESC textureViewDesc;
            ZeroMemory(&textureViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

            textureViewDesc.Format = desc.Format;
            textureViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            textureViewDesc.Texture2D.MipLevels = desc.MipLevels;
            textureViewDesc.Texture2D.MostDetailedMip = 0;

            dev->CreateShaderResourceView(textureID, &textureViewDesc, &textureView);

            D3D11_SAMPLER_DESC sampleDesc;
            ZeroMemory(&sampleDesc, sizeof(D3D11_SAMPLER_DESC));
            sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            sampleDesc.MaxAnisotropy = 0;

            sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
            sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
            sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

            sampleDesc.MipLODBias = 0.0f;
            sampleDesc.MinLOD = 0;
            sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;
            
            sampleDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

            sampleDesc.BorderColor[0] = 0.0f;
            sampleDesc.BorderColor[1] = 0.0f;
            sampleDesc.BorderColor[2] = 0.0f;
            sampleDesc.BorderColor[3] = 0.0f;

            dev->CreateSamplerState(&sampleDesc, &sampler);
        }
    }

    void DXTexture::bind(int location)
    {
        //will bind to the active shader.
        ID3D11DeviceContext* devContext = DXSingleton::getContext();

        devContext->PSSetShaderResources(location, 1, &textureView);
        devContext->PSSetSamplers(location, 1, &sampler);
    }

    int DXTexture::getWidth()
    {
        return width;
    }

    int DXTexture::getHeight()
    {
        return height;
    }
}