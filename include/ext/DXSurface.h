#pragma once

#ifdef USE_DIRECTX

    #include "ext/DXSingleton.h"

    namespace smpl
    {
        struct DXSurfaceParameters
        {
            int type = 0;
            int filterType = 0;
            int msaaLevel = 1;
        };

        class DXSurface
        {
        public:
            static const int COLOR = 0x0000;
            static const int DEPTH_AND_STENCIL = 0x0100;
            static const int COLOR_AND_DEPTH = 0x0200;

            static const int NO_MSAA = 1;
            static const int MSAA_2 = 2;
            static const int MSAA_4 = 4;
            static const int MSAA_8 = 8;
            static const int MSAA_16 = 16;

            static const int NEAREST_FILTER = D3D11_FILTER_MIN_MAG_MIP_POINT;
            static const int LINEAR_FILTER = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            // static const int ANISOTROPIC = D3D11_FILTER_ANISOTROPIC;


            DXSurface(int width, int height, DXSurfaceParameters params = {});
            ~DXSurface();

            void bind();
            void unbind();
            void bindTexture(int location = 0);
            void bindDepthTexture(int location = 0);

            int getWidth();
            int getHeight();

            static void unbindSurface();

            
            static ID3D11RenderTargetView* getCurrentView();
            static ID3D11DepthStencilView* getCurrentDepthView();
        private:

            bool init();

            ID3D11RenderTargetView* renderTarget = nullptr;
            ID3D11Texture2D* renderTargetTexture = nullptr;
            ID3D11ShaderResourceView* resourceView = nullptr;
            ID3D11SamplerState* sampler = nullptr;

            ID3D11DepthStencilView* depthStencilView= nullptr;
            ID3D11Texture2D* depthStencilTexture = nullptr;
            ID3D11ShaderResourceView* depthResourceView = nullptr;

            DXSurfaceParameters params;
            static DXSurface* currentSurface;

            int width = 0;
            int height = 0;
            bool valid = false;
        };
    }

#endif