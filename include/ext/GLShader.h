#pragma once

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "MathExt.h"
#include "SimpleFile.h"

namespace glib
{
    class GLShader
    {
    public:
        GLShader(File vertexShaderFile, File fragmentShaderFile);
        ~GLShader();

        void setAsActive();

        void setBool(std::string varName, bool value);
        void setInt(std::string varName, int value);
        void setFloat(std::string varName, float value);

        void setVec2(std::string varName, Vec2f value);
        void setVec3(std::string varName, Vec3f value);
        void setVec4(std::string varName, Vec4f value);

        void setMat2(std::string varName, Mat2f value, bool transpose = true);
        void setMat3(std::string varName, Mat3f value, bool transpose = true);
        void setMat4(std::string varName, Mat4f value, bool transpose = true);
        

        bool getValid();
        unsigned int getShaderID();
        unsigned int getVertexID();
        unsigned int getFragmentID();
        
    private:
        unsigned int shaderID;
        unsigned int vertexID;
        unsigned int fragmentID;
        bool valid = false;
    };

} //NAMESPACE glib END