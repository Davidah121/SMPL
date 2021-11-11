#pragma once
#include "MathExt.h"
#include "ext/GLWindow.h"
#include "ext/GLTexture.h"
#include "ext/GLSurface.h"
#include "ext/GLShader.h"
#include "ext/GLModel.h"

namespace glib
{

    class GLGraphics
    {
    public:
        static const int COLOR_BUFFER = GL_COLOR_BUFFER_BIT;
        static const int DEPTH_BUFFER = GL_DEPTH_BUFFER_BIT;
        static const int STENCIL_BUFFER = GL_STENCIL_BUFFER_BIT;

        static const int DEPTH_LESS = GL_LESS;
        static const int DEPTH_LEQUAL = GL_LEQUAL;

        static const int DEPTH_GREATER = GL_GREATER;
        static const int DEPTH_GEQUAL = GL_GEQUAL;

        static const int DEPTH_EQUAL= GL_EQUAL;
        static const int DEPTH_NEQUAL= GL_NOTEQUAL;

        static const int DEPTH_NEVER= GL_NEVER;
        static const int DEPTH_ALWAYS = GL_ALWAYS;

        static const int CULL_FRONT = GL_FRONT;
        static const int CULL_BACK = GL_BACK;
        static const int CULL_ALL = GL_FRONT_AND_BACK;
        
        
        static void clear(int clearCodes);
        static void setClearColor(Vec4f color);

        static void enableDepthTest();
        static void disableDepthTest();

        static void setDepthTestFunction(int type);

        static void enableFaceCulling();
        static void disableFaceCulling();
        static void setFaceCullingType(int type);

        // static void drawSurface(GLSurface s);
        
    private:
        static Vec4f clearColor;
    };

}