#pragma once

#ifdef USE_OPENGL

    #include "ext/GLTexture.h"

    namespace smpl
    {
        struct GLSurfaceParameters
        {
            int attachment = 0;
            int attachmentLocation = 0;
            bool linearFiltering = false;
            int msaaLevel = 0;
        };

        class GLSurface
        {
        public:
            static const int COLOR = 0x0000;
            static const int DEPTH = 0x0100;
            static const int STENCIL = 0x0200;
            static const int COLOR_AND_DEPTH = 0x0300;

            static const int NO_MSAA = 0;
            static const int MSAA_2 = 1;
            static const int MSAA_4 = 2;
            static const int MSAA_8 = 3;
            static const int MSAA_16 = 4;
            

            GLSurface(int width, int height, GLSurfaceParameters params = {});
            ~GLSurface();

            void bind();
            void bindTexture(int location = 0);
            
            void unbind();
            
            bool getValid();
            
            int getWidth();
            int getHeight();

            /**
             * @brief Attempts to convert the OpenGL surface to a software Image for processing.
             *      That processing can be either saving it or certain operations that are either limited to or best done
             *      at a CPU level.
             * 
             * @return Image* 
             */
            Image* toImage();
        private:
            void init();
            
            unsigned int fboID = 0;
            unsigned int secondFboID = 0; //used if msaa is enabled

            unsigned int textureID = 0;

            unsigned int textureMsaaID = 0; //used if msaa is enabled. Is a render buffer
            unsigned int renderBuffer1 = 0;
            unsigned int renderBuffer2 = 0; //used if msaa is enabled

            int width = 0;
            int height = 0;

            GLSurfaceParameters params;
            bool valid = false;
            bool needsToBlit = false;

            static GLSurface* previousSurfaceBound;
        };
    }
    
#endif