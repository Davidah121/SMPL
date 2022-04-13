#pragma once

#ifdef USE_DIRECTX
    #include "ext/DXSingleton.h"

    #include "MathExt.h"
    #include "SimpleFile.h"
    
    #ifdef DXDEBUG
        #include <d3dcompiler.h>
    #endif

    namespace glib
    {
        class DXShader
        {
        public:
            static const int TYPE_VERTEX = 0;
            static const int TYPE_FRAGMENT = 1;
            
            DXShader(File vertexShaderFile, File fragmentShaderFile, std::vector<D3D11_INPUT_ELEMENT_DESC> inputDescArr);
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
            
            static void deactivateCurrentShader();
            static DXShader* getActiveShader();

            /**
             * @brief A debugging tool for developers to compile shaders at run time for easier
             *      modification. Requires the D3dcompiler_44.dll or later. Should not be used in
             *      released products. Not allowed in UWP applications or anything submitted to the
             *      Windows Store.
             * 
             * @param vertexShaderSource 
             * @param fragmentShaderSource 
             * @return DXShader 
             */
            static DXShader compileShader(File vertexShaderSource, File fragmentShaderSource, std::vector<D3D11_INPUT_ELEMENT_DESC> inputDesc = {});

            static std::vector<D3D11_INPUT_ELEMENT_DESC> generateInputDescription(File vertexShaderSource, std::string functionName = "main");
            
        private:

            DXShader(); //For the CompileShader function
            static bool compileVertexShader(File vertexShaderSource, ID3DBlob** output);
            static bool compileFragmentShader(File fragmentShaderSource, ID3DBlob** output);

            ID3D11VertexShader* vertexID = nullptr;
            ID3D11PixelShader* fragmentID = nullptr;

            ID3D11InputLayout* layout = nullptr;
            bool valid = false;

            static DXShader* activeShader;
        };

    } //NAMESPACE glib END

#endif