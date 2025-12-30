#include "ext/ScreenCapturer.h"
#include "System.h"


namespace smpl
{
    
    #ifdef USE_DIRECTX
    DXCapturer::DXCapturer()
    {
        initD3D11();
        initDXGI();
    }

    DXCapturer::~DXCapturer()
    {
        d3dContext->Release();
        d3dDevice->Release();
    }

    void DXCapturer::initD3D11()
    {
        //init with pre defined variables
        D3D_FEATURE_LEVEL FeatureLevels[] =
        {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
            D3D_FEATURE_LEVEL_9_1
        };

        D3D_FEATURE_LEVEL featureLevelAcquired; //not used most likely

        HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, FeatureLevels, 4, D3D11_SDK_VERSION, &(this->d3dDevice), &featureLevelAcquired, &d3dContext);
        if(FAILED(hr))
        {
            std::cout << "ERROR CREATING D3D11Device : " << hr << " : " << GetLastError() << std::endl;
            throw std::runtime_error("FAILED TO CREATE D3D11 DEVICE");
        }
    }
    void DXCapturer::initDXGI()
    {
        HRESULT hr = S_OK;
        IDXGIDevice* dxgiDevice = nullptr;
        IDXGIAdapter* dxgiAdapter = nullptr;
        IDXGIOutput* dxgiOutput = nullptr;
        IDXGIOutput1* dxgiOutput1 = nullptr;

        hr = d3dDevice->QueryInterface<IDXGIDevice>(&dxgiDevice);
        if(FAILED(hr))
        {
            std::cout << "ERROR CREATING IDXGIDevice : " << hr << " : " << GetLastError() << std::endl;
            throw std::runtime_error("FAILED TO CREATE IDXGIDevice");
        }

        hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);
        dxgiDevice->Release();
        dxgiDevice = nullptr;
        if(FAILED(hr))
        {
            std::cout << "ERROR GETTING IDXGIAdapter : " << hr << " : " << GetLastError() << std::endl;
            throw std::runtime_error("FAILED TO GETTING IDXGIAdapter");
        }

        hr = dxgiAdapter->EnumOutputs(0, &dxgiOutput);
        dxgiAdapter->Release();
        dxgiAdapter = nullptr;
        if(FAILED(hr))
        {
            std::cout << "ERROR CREATING IDXGIOutput : " << hr << " : " << GetLastError() << std::endl;
            throw std::runtime_error("FAILED TO CREATE IDXGIOutput");
        }

        hr = dxgiOutput->QueryInterface<IDXGIOutput1>(&dxgiOutput1);
        dxgiOutput->Release();
        dxgiOutput = nullptr;
        if(FAILED(hr))
        {
            std::cout << "ERROR CREATING IDXGIOutput1 : " << hr << " : " << GetLastError() << std::endl;
            throw std::runtime_error("FAILED TO CREATE IDXGIOutput1");
        }
        
        hr = dxgiOutput1->DuplicateOutput(d3dDevice, &(this->desktopDuplicator));
        dxgiOutput1->Release();
        dxgiOutput1 = nullptr;
        if(FAILED(hr))
        {
            std::cout << "ERROR CREATING IDXGIOutputDuplication : " << hr << " : " << GetLastError() << std::endl;
            throw std::runtime_error("FAILED TO CREATE IDXGIOutputDuplication");
        }
    }

    smpl::Image* DXCapturer::capture()
    {
        HRESULT hr = S_OK;
        size_t t1,t2;
        t1 = smpl::System::getCurrentTimeMicro();

        IDXGIResource* dxgiResource = nullptr;
        DXGI_OUTDUPL_FRAME_INFO frameInfo;

        desktopDuplicator->ReleaseFrame();
        hr = this->desktopDuplicator->AcquireNextFrame(1, &frameInfo, &dxgiResource);
        if(hr == DXGI_ERROR_WAIT_TIMEOUT)
        {
            return nullptr;
        }
        else if(FAILED(hr))
        {
            std::cout << "ERROR ACQUIRING NEW FRAME : " << hr << " : " << GetLastError() << std::endl;
            throw std::runtime_error("FAILED TO ACQUIRE NEW FRAME");
        }

        hr = dxgiResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&acquiredImage);

        dxgiResource->Release();
        dxgiResource = nullptr;
        if(FAILED(hr))
        {
            std::cout << "ERROR GRABBING ID3D11Texture2D : " << hr << " : " << GetLastError() << std::endl;
            throw std::runtime_error("FAILED TO GRAB ID3D11Texture2D");
        }
        
        t2 = smpl::System::getCurrentTimeMicro();
        // printf("TIME TO GET TEXTURE: %llu\n", t2-t1);
        
        t1 = smpl::System::getCurrentTimeMicro();
        //convert into software image
        D3D11_TEXTURE2D_DESC desc;
        ID3D11Texture2D* destImage = nullptr;
        acquiredImage->GetDesc(&desc);

        desc.BindFlags = 0;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        desc.Usage = D3D11_USAGE_STAGING;
        desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        desc.MiscFlags = 0;

        hr = d3dDevice->CreateTexture2D(&desc, NULL, &destImage);
        if(FAILED(hr))
        {
            std::cout << "ERROR CREATING SOFTWARE TEXTURE : " << hr << " : " << GetLastError() << std::endl;
            throw std::runtime_error("FAILED TO CREATE SOFTWARE TEXTURE");
        }

        t2 = smpl::System::getCurrentTimeMicro();
        // printf("TIME TO CREATE NEW TEXTURE: %llu\n", t2-t1);


        t1 = smpl::System::getCurrentTimeMicro();
        d3dContext->CopyResource(destImage, acquiredImage);


        D3D11_MAPPED_SUBRESOURCE resource;
        unsigned int subresource = D3D11CalcSubresource(0, 0, 0);
        hr = d3dContext->Map(destImage, subresource, D3D11_MAP_READ, 0, &resource);
        if(FAILED(hr))
        {
            std::cout << "ERROR MAPPING TEXTURE : " << hr << " : " << GetLastError() << std::endl;
            throw std::runtime_error("FAILED TO MAP SOFTWARE TEXTURE");
        }
        t2 = smpl::System::getCurrentTimeMicro();
        // printf("TIME TO COPY AND MAP TEXTURE: %llu\n", t2-t1);
        
        t1 = smpl::System::getCurrentTimeMicro();
        smpl::Image* finalSoftwareImage = new smpl::Image(desc.Width, desc.Height);
        unsigned char* rawBytes = (unsigned char*)resource.pData;
        size_t index = 0;
        smpl::Color* finalSoftwareImagePixels = finalSoftwareImage->getPixels();

        for(int y=0; y<finalSoftwareImage->getHeight(); y++)
        {
            for(int x=0; x<finalSoftwareImage->getWidth(); x++)
            {
                smpl::Color c = {rawBytes[index+2], rawBytes[index+1], rawBytes[index], rawBytes[index+3]};
                *finalSoftwareImagePixels = c;
                index+=4;
                finalSoftwareImagePixels++;
            }
        }
        d3dContext->Unmap(destImage, subresource);

        destImage->Release();
        acquiredImage->Release();
        
        t2 = smpl::System::getCurrentTimeMicro();
        // printf("TIME TO COPY TEXTURE INTO IMAGE AND FREE: %llu\n", t2-t1);

        return finalSoftwareImage;
    }
    #endif
}