#pragma once

#include "ext/DXSingleton.h"

#include "MathExt.h"
#include "SimpleFile.h"

namespace glib
{
    class DXShader
    {
    public:
        static const int TYPE_VERTEX = 0;
        static const int TYPE_FRAGMENT = 1;
        
        DXShader(File vertexShaderFile, File fragmentShaderFile);
        ~DXShader();

        void setAsActive();

        // void setBool(std::string varName, bool value);
        // void setInt(std::string varName, int value);
        // void setFloat(std::string varName, float value);

        // void setVec2(std::string varName, Vec2f value);
        // void setVec3(std::string varName, Vec3f value);
        // void setVec4(std::string varName, Vec4f value);

        // void setMat2(std::string varName, Mat2f value, bool transpose = true);
        // void setMat3(std::string varName, Mat3f value, bool transpose = true);
        // void setMat4(std::string varName, Mat4f value, bool transpose = true);

        void setUniformData(void* buffer, size_t sizeOfBuffer, int location, int type);

        bool getValid();
        ID3D11VertexShader* getVertexShaderPointer();
        ID3D11PixelShader* getFragmentShaderPointer();
        
        // unsigned int getShaderID();
    private:

        ID3D11VertexShader* vertexID = nullptr;
        ID3D11PixelShader* fragmentID = nullptr;

        D3D11_INPUT_ELEMENT_DESC inputDesc;
        ID3D11InputLayout* layout = nullptr;
        bool valid = false;
    };

} //NAMESPACE glib END