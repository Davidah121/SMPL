#pragma once

#include "ext/GLShader.h"
#include "ext/DXShader.h"


namespace glib
{
    class Shader
    {
    public:
        Shader(bool type);
        ~Shader();

    private:
        GLShader* openGLShader = nullptr;
        DXShader* directXShader = nullptr;

    };
}