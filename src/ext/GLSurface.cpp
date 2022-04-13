#ifdef USE_OPENGL

    #include "ext/GLSurface.h"

    namespace glib
    {
        GLSurface* GLSurface::previousSurfaceBound = nullptr;
        
        GLSurface::GLSurface(int width, int height, GLSurfaceParameters params)
        {
            if(!GLSingleton::getInit())
            {
                return;
            }
            this->width = width;
            this->height = height;
            this->params = params;

            init();
        }

        GLSurface::~GLSurface()
        {
            if(!GLSingleton::getInit())
            {
                return;
            }

            if(fboID!=0)
                glDeleteFramebuffers(1, &fboID);

            if(secondFboID!=0)
                glDeleteFramebuffers(1, &secondFboID);

            if(textureID!=0)
                glDeleteTextures(1, &textureID);

            if(textureMsaaID!=0)
                glDeleteRenderbuffers(1, &textureMsaaID);

            if(renderBuffer1!=0)
                glDeleteRenderbuffers(1, &renderBuffer1);

            if(renderBuffer2!=0)
                glDeleteRenderbuffers(1, &renderBuffer2);

            fboID = 0;
            secondFboID = 0;
            textureID = 0;
            textureMsaaID = 0;
            renderBuffer1 = 0;
            renderBuffer2 = 0;

            valid = false;
        }

        void GLSurface::bind()
        {
            if(previousSurfaceBound!=nullptr)
            {
                if(previousSurfaceBound == this)
                {
                    unbind();
                }
                else
                {
                    previousSurfaceBound->unbind();
                }
            }

            if(valid)
            {
                previousSurfaceBound = this;
                glViewport(0, 0, width, height);
                glBindFramebuffer(GL_FRAMEBUFFER, fboID);
                if(params.msaaLevel > 0)
                    needsToBlit = true;
            }
            else
            {
                unbind();
            }
            
        }

        void GLSurface::bindTexture(int location)
        {
            if(valid)
            {
                int textureLocation = MathExt::clamp(GL_TEXTURE0 + location, GL_TEXTURE0, GL_MAX_TEXTURE_UNITS);
                
                glActiveTexture(textureLocation);
                glBindTexture(GL_TEXTURE_2D, textureID);
            }
        }

        void GLSurface::unbind()
        {
            if(needsToBlit == true)
            {
                glBindFramebuffer(GL_READ_FRAMEBUFFER, fboID);
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, secondFboID);

                glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
                needsToBlit = false;
                
                glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        bool GLSurface::getValid()
        {
            return valid;
        }

        void GLSurface::init()
        {
            glGenFramebuffers(1, &fboID);
            glBindFramebuffer(GL_FRAMEBUFFER, fboID);

            int textureType = params.attachment;
            int extra = params.attachmentLocation;
            extra = MathExt::clamp<unsigned char>(extra, 0, 15);
            int samples = params.msaaLevel;

            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            
            if(params.linearFiltering)
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            }
            else
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            }
            

            if(samples <= 1)
            {

                switch (textureType)
                {
                case GLSurface::COLOR:
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+extra, GL_TEXTURE_2D, textureID, 0);
                    break;
                case GLSurface::DEPTH:
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureID, 0);
                    break;
                case GLSurface::STENCIL:
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_STENCIL_INDEX8, width, height, 0, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, NULL);
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, textureID, 0);
                    break;
                case GLSurface::COLOR_AND_DEPTH:
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+extra, GL_TEXTURE_2D, textureID, 0);

                    //attach render buffer
                    glGenRenderbuffers(1, &renderBuffer1);
                    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer1);

                    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

                    glBindRenderbuffer(GL_RENDERBUFFER, 0);
                    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer1);
                    break;
                default:
                    break;
                }
                
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            else
            {
                glGenRenderbuffers(1, &textureMsaaID);
                glBindRenderbuffer(GL_RENDERBUFFER, textureMsaaID);

                
                switch (textureType)
                {
                case GLSurface::COLOR:
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
                    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_RGB, width, height);
                    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+extra, GL_RENDERBUFFER, textureMsaaID);
                    break;
                case GLSurface::DEPTH:
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
                    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH_COMPONENT, width, height);
                    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, textureMsaaID);
                    break;
                case GLSurface::STENCIL:
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_STENCIL_INDEX8, width, height, 0, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, NULL);
                    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_STENCIL_INDEX8, width, height);
                    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, textureMsaaID);
                    break;
                case GLSurface::COLOR_AND_DEPTH:
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
                    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_RGB, width, height);
                    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+extra, GL_RENDERBUFFER, textureMsaaID);
                    
                    //attach render buffer
                    glGenRenderbuffers(1, &renderBuffer1);
                    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer1);

                    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH_COMPONENT, width, height);

                    glBindRenderbuffer(GL_RENDERBUFFER, 0);
                    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer1);
                    break;
                default:
                    break;
                }
                
                glBindTexture(GL_TEXTURE_2D, 0);

                glBindRenderbuffer(GL_RENDERBUFFER, 0);

                //create normal fbo without msaa to have a usable texture
                glGenFramebuffers(1, &secondFboID);
                glBindFramebuffer(GL_FRAMEBUFFER, secondFboID);

                glGenRenderbuffers(1, &renderBuffer2);
                glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer2);

                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

                glBindRenderbuffer(GL_RENDERBUFFER, 0);

                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+extra, GL_TEXTURE_2D, textureID, 0);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer2);

            }

            if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                StringTools::println("INVALID SURFACE");
                valid = false;
                return;
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            valid = true;
        }

        int GLSurface::getWidth()
        {
            return width;
        }

        int GLSurface::getHeight()
        {
            return height;
        }
    }
    
#endif