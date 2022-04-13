#ifdef USE_DIRECTX

    #include "ext/DXSurface.h"
    #include "StringTools.h"

    namespace glib
    {
        
        DXSurface* DXSurface::currentSurface = nullptr;

        DXSurface::DXSurface(int width, int height, DXSurfaceParameters params)
        {
            this->width = width;
            this->height = height;
            this->params = params;

            valid = init();
        }

        DXSurface::~DXSurface()
        {
            ID3D11DeviceContext* devContext = DXSingleton::getContext();
            
            if(currentSurface == this)
                unbindSurface();
            
            if(renderTargetTexture != nullptr)
            {
                renderTargetTexture->Release();
                renderTargetTexture = nullptr;
            }

            if(renderTarget != nullptr)
            {
                renderTarget->Release();
                renderTarget = nullptr;
            }

            if(resourceView != nullptr)
            {
                resourceView->Release();
                resourceView = nullptr;
            }
            
            if(depthStencilTexture != nullptr)
            {
                depthStencilTexture->Release();
                depthStencilTexture = nullptr;
            }
            if(depthResourceView != nullptr)
            {
                depthResourceView->Release();
                depthResourceView = nullptr;
            }
            if(depthStencilView != nullptr)
            {
                depthStencilView->Release();
                depthStencilView = nullptr;
            }


            if(sampler != nullptr)
            {
                sampler->Release();
                sampler = nullptr;
            }
            

            width = 0;
            height = 0;
            valid = false;
            memset(&params, 0, sizeof(DXSurfaceParameters));
        }

        void DXSurface::bind()
        {
            if(valid)
            {
                currentSurface = this;
                ID3D11DeviceContext* context = DXSingleton::getContext();
                if(context == nullptr)
                    return;
                
                context->OMSetRenderTargets(1, &renderTarget, depthStencilView);
            }
        }

        void DXSurface::unbind()
        {
            DXSurface::unbindSurface();
        }

        void DXSurface::unbindSurface()
        {
            ID3D11DeviceContext* context = DXSingleton::getContext();
            if(context == nullptr)
                return;
            
            ID3D11RenderTargetView* renderTarget = DXSingleton::getBackBuffer();
            context->OMSetRenderTargets(1, &renderTarget, nullptr);
            currentSurface = nullptr;
        }

        void DXSurface::bindTexture(int location)
        {
            if(valid)
            {
                if(params.type == COLOR || params.type == COLOR_AND_DEPTH)
                {
                    //will bind to the active shader.
                    ID3D11DeviceContext* devContext = DXSingleton::getContext();

                    devContext->PSSetShaderResources(location, 1, &resourceView);
                    devContext->PSSetSamplers(location, 1, &sampler);
                }
            }
        }

        void DXSurface::bindDepthTexture(int location)
        {
            if(valid)
            {
                if(params.type == DEPTH_AND_STENCIL || params.type == COLOR_AND_DEPTH)
                {
                    //will bind to the active shader.
                    ID3D11DeviceContext* devContext = DXSingleton::getContext();

                    devContext->PSSetShaderResources(location, 1, &depthResourceView);
                    devContext->PSSetSamplers(location, 1, &sampler);
                }
            }
        }

        
            void bindDepthTexture(int location = 0);

        int DXSurface::getWidth()
        {
            return width;
        }

        int DXSurface::getHeight()
        {
            return height;
        }
        
        ID3D11RenderTargetView* DXSurface::getCurrentView()
        {
            if(currentSurface != nullptr)
                return currentSurface->renderTarget;
            return nullptr;
        }
        
        ID3D11DepthStencilView* DXSurface::getCurrentDepthView()
        {
            if(currentSurface != nullptr)
                return currentSurface->depthStencilView;
            return nullptr;
        }


        bool DXSurface::init()
        {
            HRESULT res;
            ID3D11Device* dev = DXSingleton::getDevice();
            if(dev == nullptr)
                return false;
            
            D3D11_RENDER_TARGET_VIEW_DESC renderDesc;
            memset(&renderDesc, 0, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
            
            D3D11_SHADER_RESOURCE_VIEW_DESC resourceDesc;
            memset(&resourceDesc, 0, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
            
            D3D11_DEPTH_STENCIL_VIEW_DESC  depthResourceDesc;
            memset(&depthResourceDesc, 0, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC ));

            D3D11_TEXTURE2D_DESC textureDesc;
            memset(&textureDesc, 0, sizeof(D3D11_TEXTURE2D_DESC));

            D3D11_TEXTURE2D_DESC depthTextureDesc;
            memset(&depthTextureDesc, 0, sizeof(D3D11_TEXTURE2D_DESC));

            D3D11_SAMPLER_DESC sampleDesc;
            memset(&sampleDesc, 0, sizeof(D3D11_SAMPLER_DESC));

            if(params.type == DXSurface::COLOR || params.type == DXSurface::COLOR_AND_DEPTH)
            {
                textureDesc.Width = width;
                textureDesc.Height = height;
                textureDesc.MipLevels = 1;
                textureDesc.ArraySize = 1;
                textureDesc.SampleDesc.Count = 1;
                textureDesc.SampleDesc.Quality = 0;
                textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
                textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

                renderDesc.Format = textureDesc.Format;
                renderDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
                renderDesc.Texture2D.MipSlice = 0;

                resourceDesc.Format = textureDesc.Format;
                resourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                resourceDesc.Texture2D.MostDetailedMip = 0;
                resourceDesc.Texture2D.MipLevels = 1;

                res = dev->CreateTexture2D(&textureDesc, nullptr, &renderTargetTexture);
                if(res != S_OK)
                {
                    //ERROR CREATING TEXTURE
                    return false;
                }

                res = dev->CreateRenderTargetView(renderTargetTexture, &renderDesc, &renderTarget);
                if(res != S_OK)
                {
                    //ERROR CREATING RENDER TARGET
                    return false;
                }

                res = dev->CreateShaderResourceView(renderTargetTexture, &resourceDesc, &resourceView);
                if(res != S_OK)
                {
                    //ERROR CREATING SHADER RESOURCE VIEW
                    return false;
                }
                
            }

            //Create Depth Stencil buffer for depth testing if required

            if(params.type == COLOR_AND_DEPTH || params.type == DEPTH_AND_STENCIL)
            {
                depthTextureDesc.Width = width;
                depthTextureDesc.Height = height;
                depthTextureDesc.MipLevels = 1;
                depthTextureDesc.ArraySize = 1;
                depthTextureDesc.SampleDesc.Count = 1;
                depthTextureDesc.SampleDesc.Quality = 0;
                depthTextureDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
                depthTextureDesc.Usage = D3D11_USAGE_DEFAULT;
                depthTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
                
                depthResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
                depthResourceDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

                resourceDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
                resourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                resourceDesc.Texture2D.MostDetailedMip = 0;
                resourceDesc.Texture2D.MipLevels = -1;

                res = dev->CreateTexture2D(&depthTextureDesc, nullptr, &depthStencilTexture);
                if(res != S_OK)
                {
                    StringTools::println("1: %llx", res);
                    //ERROR CREATING DEPTH TEXTURE
                    return false;
                }

                res = dev->CreateDepthStencilView(depthStencilTexture, &depthResourceDesc, &depthStencilView);
                if(res != S_OK)
                {
                    StringTools::println("2: %llx", res);
                    //ERROR CREATING DEPTH STENCIL VIEW
                    return false;
                }

                res = dev->CreateShaderResourceView(depthStencilTexture, &resourceDesc, &depthResourceView);
                if(res != S_OK)
                {
                    StringTools::println("3: %llx", res);
                    //ERROR CREATING SHADER RESOURCE VIEW
                    return false;
                }
            }
            // else if(params.type == DEPTH_AND_STENCIL)
            // {
            //     depthTextureDesc.Width = width;
            //     depthTextureDesc.Height = height;
            //     depthTextureDesc.MipLevels = 1;
            //     depthTextureDesc.ArraySize = 1;
            //     depthTextureDesc.SampleDesc.Count = 1;
            //     depthTextureDesc.SampleDesc.Quality = 0;
            //     depthTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
            //     depthTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

            //     depthResourceDesc.Format = depthResourceDesc.Format;
            //     depthResourceDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

            //     resourceDesc.Format = depthResourceDesc.Format;
            //     resourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            //     resourceDesc.Texture2D.MostDetailedMip = 0;
            //     resourceDesc.Texture2D.MipLevels = 1;

            //     res = dev->CreateTexture2D(&depthTextureDesc, nullptr, &depthStencilTexture);
            //     if(res != S_OK)
            //     {
            //         //ERROR CREATING DEPTH TEXTURE
            //         return false;
            //     }

            //     res = dev->CreateDepthStencilView(depthStencilTexture, &depthResourceDesc, &depthStencilView);
            //     if(res != S_OK)
            //     {
            //         //ERROR CREATING DEPTH STENCIL VIEW
            //         return false;
            //     }

            //     res = dev->CreateShaderResourceView(depthStencilTexture, &resourceDesc, &depthResourceView);
            //     if(res != S_OK)
            //     {
            //         //ERROR CREATING SHADER RESOURCE VIEW
            //         return false;
            //     }
            // }

            sampleDesc.Filter = (D3D11_FILTER)params.filterType;
            sampleDesc.MaxAnisotropy = 0;

            sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
            sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
            sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

            sampleDesc.MipLODBias = 0.0f;
            sampleDesc.MinLOD = 0;
            sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;
            
            sampleDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

            sampleDesc.BorderColor[0] = 0.0f;
            sampleDesc.BorderColor[1] = 0.0f;
            sampleDesc.BorderColor[2] = 0.0f;
            sampleDesc.BorderColor[3] = 0.0f;

            dev->CreateSamplerState(&sampleDesc, &sampler);

            return true;
        }
    }

#endif