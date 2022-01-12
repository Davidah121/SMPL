#pragma once

#include "ext/GLSingleton.h"

#include "MathExt.h"
#include "SimpleFile.h"

namespace glib
{
    class GLShader
    {
    public:
        GLShader();
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
        
        
        void setUniformData(void* buffer, size_t sizeOfBuffer, int location, int type);

        bool getValid();
        unsigned int getShaderID();
        unsigned int getVertexID();
        unsigned int getFragmentID();

        static GLShader createShaderFromStringLiteral(std::string vertexString, std::string fragmentString);

        static void deactivateCurrentShader();
        static GLShader* getActiveShader();
        
    private:

        void init(std::string vertexString, std::string fragmentString);

        unsigned int shaderID = 0;
        unsigned int vertexID = 0;
        unsigned int fragmentID = 0;
        bool valid = false;

        static GLShader* activeShader;
    };

} //NAMESPACE glib END