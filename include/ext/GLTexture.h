#pragma once

#ifdef USE_OPENGL

    #include "StandardTypes.h"
    #include "ext/GLSingleton.h"
    #include "Image.h"

    namespace smpl
    {

        struct GLTextureParams
        {
            int edgeBehavior = GL_CLAMP_TO_EDGE;
            int filterType = GL_LINEAR;
        };

        class GLTexture
        {
        public:
            static const bool TEXTURE2D = false;
            static const bool TEXTURE3D = true;

            static const int BEHAVIOR_CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE;
            static const int BEHAVIOR_CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER;
            
            static const int FILTER_NEAREST = GL_NEAREST;
            static const int FILTER_LINEAR = GL_LINEAR;
            
			GLTexture();

            GLTexture(const File& f, bool includeAlpha=true, GLTextureParams params = {BEHAVIOR_CLAMP_TO_EDGE, FILTER_LINEAR});
            GLTexture(Image* img, bool includeAlpha=true, GLTextureParams params = {BEHAVIOR_CLAMP_TO_EDGE, FILTER_LINEAR});

            GLTexture(const GLTexture& o) = default;
			GLTexture& operator=(const GLTexture& o) = default;

            GLTexture(GLTexture&& o) = default;
			GLTexture& operator=(GLTexture&& o) = default;
			
            ~GLTexture();

			/**
			 * @brief Creates a brand new texture utilizing an existing texture.
			 *		This will create a new textureID but contain the same information as the original texture.
			 * 
			 * @param o 
			 */
			void copyTexture(const GLTexture& o);

            void loadImage(const File& f, bool includeAlpha=true, GLTextureParams params = {BEHAVIOR_CLAMP_TO_EDGE, FILTER_LINEAR});
            void setImage(Image* img, bool includeAlpha=true, GLTextureParams params = {BEHAVIOR_CLAMP_TO_EDGE, FILTER_LINEAR});

            /**
             * @brief Attempts to convert the OpenGL texture to a software Image for processing.
             *      That processing can be either saving it or certain operations that are either limited to or best done
             *      at a CPU level.
             * 
             * @return Image* 
             */
            Image* toImage();

            void bind(int location = 0);

            int getWidth();
            int getHeight();

        private:
            int edgeBehavior = GL_CLAMP_TO_EDGE;
            int filterType = GL_LINEAR;
			bool includesAlpha = true;

            unsigned int textureID = 0;
            bool textureType = TEXTURE2D;

            int width = 0;
            int height = 0;
        };

    } // namespace smpl

#endif