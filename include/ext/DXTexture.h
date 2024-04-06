#pragma once

#ifdef USE_DIRECTX

    #include "ext/DXSingleton.h"
    #include "Image.h"

    /**
     *  Needs to have customization for edge behavior, filter type,
     *  and everything not implemented in GLTexture yet.
     * 
     */

    namespace smpl
    {
        class DXTexture
        {
        public:
            static const bool TEXTURE2D = false;
            static const bool TEXTURE3D = true;

            DXTexture(File f, bool includeAlpha=true);
            DXTexture(Image* img, bool includeAlpha=true);
            ~DXTexture();

            void loadImage(File f, bool includeAlpha=true);
            void setImage(Image* img, bool includeAlpha=true);

            void bind(int location = 0);

            int getWidth();
            int getHeight();
        private:

            ID3D11Texture2D* textureID = nullptr;
            ID3D11ShaderResourceView* textureView = nullptr;
            ID3D11SamplerState* sampler = nullptr;
            
            bool textureType = TEXTURE2D;
            int width = 0;
            int height = 0;
        };
    }

#endif