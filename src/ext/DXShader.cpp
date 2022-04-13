#ifdef USE_DIRECTX

    #include "ext/DXShader.h"

    namespace glib
    {
        DXShader* DXShader::activeShader = nullptr;

        DXShader::DXShader()
        {
            valid = false;
        }

        DXShader::DXShader(File vertexShaderFile, File fragmentShaderFile, std::vector<D3D11_INPUT_ELEMENT_DESC> inputDescArr)
        {
            std::vector<unsigned char> vData, fData;

            SimpleFile f = SimpleFile(vertexShaderFile, SimpleFile::READ | SimpleFile::ASCII);
            if(f.isOpen())
            {
                vData = f.readFullFileAsBytes();
                f.close();
            }
            else
            {
                //ERROR LOADING VERTEX SHADER
                StringTools::println("ERROR LOADING VERTEX SHADER - %ls", vertexShaderFile.getFullFileName().c_str());
                valid = false;
            }

            SimpleFile f2 = SimpleFile(fragmentShaderFile, SimpleFile::READ | SimpleFile::ASCII);
            if(f2.isOpen())
            {
                fData = f2.readFullFileAsBytes();
                f2.close();
            }
            else
            {
                //ERROR LOADING FRAGMENT SHADER
                StringTools::println("ERROR LOADING FRAGMENT SHADER - %ls", fragmentShaderFile.getFullFileName().c_str());
                valid = false;
            }

            ID3D11Device* dev = DXSingleton::getDevice();
            ID3D11DeviceContext* devContext = DXSingleton::getContext();

            HRESULT err;

            err = dev->CreateVertexShader(vData.data(), vData.size(), nullptr, &vertexID);

            if(err != NO_ERROR)
            {
                StringTools::println("ERROR CREATING VERTEX SHADER - %ls", vertexShaderFile.getFullFileName().c_str());
                StringTools::println("ERROR VALUE %lx", err);
                valid = false;
            }

            err = dev->CreatePixelShader(fData.data(), fData.size(), nullptr, &fragmentID);

            if(err != NO_ERROR)
            {
                StringTools::println("ERROR CREATING FRAGMENT SHADER - %ls", fragmentShaderFile.getFullFileName().c_str());
                StringTools::println("ERROR VALUE %lx", err);
                valid = false;
            }
            
            // inputDesc.SemanticName = "POSITION";
            // inputDesc.SemanticIndex = 0;
            // inputDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
            // inputDesc.InputSlot = 0;
            // inputDesc.AlignedByteOffset = 0;
            // inputDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
            // inputDesc.InstanceDataStepRate = 0;
            
            err = dev->CreateInputLayout(inputDescArr.data(), inputDescArr.size(), vData.data(), vData.size(), &layout);

            if(err != NO_ERROR)
            {
                StringTools::println("ERROR CREATING LAYOUT");
                StringTools::println("ERROR VALUE %lx", err);
            }
            devContext->IASetInputLayout(layout);

            if(!valid)
                valid = true;
        }

        DXShader::~DXShader()
        {
            if(vertexID != nullptr)
                vertexID->Release();
            if(fragmentID != nullptr)
                fragmentID->Release();
            if(layout != nullptr)
                layout->Release();

            vertexID = nullptr;
            fragmentID = nullptr;
            valid = false;
        }

        void DXShader::setAsActive()
        {
            if(valid)
            {
                deactivateCurrentShader();
                
                ID3D11DeviceContext* context = DXSingleton::getContext();

                context->VSSetShader(vertexID, nullptr, 0);
                context->PSSetShader(fragmentID, nullptr, 0);
                context->IASetInputLayout(layout);
                activeShader = this;
            }
        }

        void DXShader::setUniformData(void* buffer, size_t sizeOfBuffer, int location, int type)
        {
            if(valid)
            {
                ID3D11DeviceContext* context = DXSingleton::getContext();
                ID3D11Device* device = DXSingleton::getDevice();

                ID3D11Buffer* d3dBuffer = nullptr;

                D3D11_BUFFER_DESC desc;
                desc.ByteWidth = sizeOfBuffer;
                desc.Usage = D3D11_USAGE_DYNAMIC;
                desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
                desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
                desc.MiscFlags = 0;
                desc.StructureByteStride = 0;

                D3D11_SUBRESOURCE_DATA subResource;
                subResource.pSysMem = buffer;
                subResource.SysMemPitch = 0;
                subResource.SysMemSlicePitch = 0;

                HRESULT hr = device->CreateBuffer( &desc, &subResource, &d3dBuffer);

                if(FAILED(hr))
                {
                    //ERROR
                    return;
                }

                if(type==TYPE_VERTEX)
                    context->VSSetConstantBuffers(location, 1, &d3dBuffer);
                else if(type==TYPE_FRAGMENT)
                    context->PSSetConstantBuffers(location, 1, &d3dBuffer);

                d3dBuffer->Release();
            }
        }

        ID3D11VertexShader* DXShader::getVertexShaderPointer()
        {
            return vertexID;
        }

        ID3D11PixelShader* DXShader::getFragmentShaderPointer()
        {
            return fragmentID;
        }

        bool DXShader::getValid()
        {
            return valid;
        }

        void DXShader::deactivateCurrentShader()
        {
            ID3D11DeviceContext* context = DXSingleton::getContext();

            context->VSSetShader(0, nullptr, 0);
            context->PSSetShader(0, nullptr, 0);
            context->GSSetShader(0, nullptr, 0);
            context->HSSetShader(0, nullptr, 0);
            context->DSSetShader(0, nullptr, 0);
            context->CSSetShader(0, nullptr, 0);

            context->IASetInputLayout(0);
            activeShader = nullptr;
        }

        DXShader* DXShader::getActiveShader()
        {
            return activeShader;
        }

        DXShader DXShader::compileShader(File vertexShaderSource, File fragmentShaderSource, std::vector<D3D11_INPUT_ELEMENT_DESC> inputDesc)
        {
            DXShader s = DXShader();
            s.valid = true;

            ID3DBlob* vertexShaderBlob = nullptr;
            ID3DBlob* fragmentShaderBlob = nullptr;

            bool ok = compileVertexShader(vertexShaderSource, &vertexShaderBlob);
            if(!ok)
            {
                s.valid = false;
                return s;
            }
            
            ok = compileFragmentShader(fragmentShaderSource, &fragmentShaderBlob);
            if(!ok)
            {
                s.valid = false;
                return s;
            }
            

            ID3D11Device* dev = DXSingleton::getDevice();
            ID3D11DeviceContext* devContext = DXSingleton::getContext();

            HRESULT err;

            err = dev->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &s.vertexID);

            if(err != NO_ERROR)
            {
                StringTools::println("ERROR CREATING VERTEX SHADER - %ls", vertexShaderSource.getFullFileName().c_str());
                StringTools::println("ERROR VALUE %lx", err);
                s.valid = false;

                delete vertexShaderBlob;
                return s;
            }

            err = dev->CreatePixelShader(fragmentShaderBlob->GetBufferPointer(), fragmentShaderBlob->GetBufferSize(), nullptr, &s.fragmentID);

            if(err != NO_ERROR)
            {
                StringTools::println("ERROR CREATING FRAGMENT SHADER - %ls", fragmentShaderSource.getFullFileName().c_str());
                StringTools::println("ERROR VALUE %lx", err);
                s.valid = false;

                delete fragmentShaderBlob;
                return s;
            }

            if(inputDesc.size() == 0)
            {
                //attempt to extract input description
                std::vector<D3D11_INPUT_ELEMENT_DESC> inputDescArr = generateInputDescription(vertexShaderSource);

                for(int i=0; i<inputDescArr.size(); i++)
                {
                    StringTools::println("%s, %d, %d", inputDescArr[i].SemanticName, inputDescArr[i].InputSlot, inputDescArr[i].SemanticIndex);
                }
                
                err = dev->CreateInputLayout(inputDescArr.data(), inputDescArr.size(), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &s.layout);
                if(err != NO_ERROR)
                {
                    StringTools::println("ERROR CREATING LAYOUT");
                    StringTools::println("ERROR VALUE %lx", err);
                    s.valid = false;
                }

                //delete pointerData in inputDescArr
                for(int i=0; i<inputDescArr.size(); i++)
                {
                    if(inputDescArr[i].SemanticName != nullptr)
                        delete[] inputDescArr[i].SemanticName;
                }
            }
            else
            {
                //use user defined input description
                err = dev->CreateInputLayout(inputDesc.data(), inputDesc.size(), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &s.layout);
                if(err != NO_ERROR)
                {
                    StringTools::println("ERROR CREATING LAYOUT");
                    StringTools::println("ERROR VALUE %lx", err);
                    s.valid = false;
                }
            }

            return s;
        }
        
        
        bool DXShader::compileVertexShader(File vertexShaderSource, ID3DBlob** output)
        {
            ID3D11Device* device = DXSingleton::getDevice();
            UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
            LPCSTR profile = "vs_5_0";

            const D3D_SHADER_MACRO defines[] = 
            {
                "EXAMPLE_DEFINE", "1",
                NULL, NULL
            };

            LPCSTR entryPoint = "main";

            ID3DBlob* errorBlob = nullptr;
            ID3DBlob* shaderBlob = nullptr;

            HRESULT res = D3DCompileFromFile(vertexShaderSource.getFullFileName().c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, 
                    entryPoint, profile, flags, 0, output, &errorBlob);
            
            if(res != S_OK)
            {
                StringTools::println("ERROR COMPILING VERTEX SHADER - %ls", vertexShaderSource.getFullFileName().c_str());
                StringTools::println("ERROR VALUE %lx", res);
                if(errorBlob->GetBufferSize() > 0)
                    StringTools::println("ERROR MESSAGE: %s", (char*)errorBlob->GetBufferPointer());
                return false;
            }
            return true;
        }

        bool DXShader::compileFragmentShader(File fragmentShaderSource, ID3DBlob** output)
        {
            ID3D11Device* device = DXSingleton::getDevice();
            UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
            LPCSTR profile = "ps_5_0";

            const D3D_SHADER_MACRO defines[] = 
            {
                "EXAMPLE_DEFINE", "1",
                NULL, NULL
            };

            LPCSTR entryPoint = "main";

            ID3DBlob* errorBlob = nullptr;

            HRESULT res = D3DCompileFromFile(fragmentShaderSource.getFullFileName().c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, 
                    entryPoint, profile, flags, 0, output, &errorBlob);
            
            if(res != S_OK)
            {
                StringTools::println("ERROR COMPILING FRAGMENT/PIXEL SHADER - %ls", fragmentShaderSource.getFullFileName().c_str());
                StringTools::println("ERROR VALUE %lx", res);
                if(errorBlob->GetBufferSize() > 0)
                    StringTools::println("ERROR MESSAGE: %s", (char*)errorBlob->GetBufferPointer());
                return false;
            }
            return true;
        }

        std::vector<D3D11_INPUT_ELEMENT_DESC> DXShader::generateInputDescription(File vertexShaderSource, std::string functionName)
        {
            //extract all functions

            //syntax: "ReturnValue" "FunctionName"( ... ) { ... }

            std::vector<std::string> wordsAndSeparators;
            std::vector<D3D11_INPUT_ELEMENT_DESC> allDesc;
            std::vector<unsigned char> bytes;
            SimpleFile f = SimpleFile(vertexShaderSource, SimpleFile::READ);
            if(f.isOpen())
                bytes = f.readFullFileAsBytes();
            f.close();

            //read bytes till potential function
            //function must have the syntax above.
            //Nothing can begin with a number.
            //space separates data

            std::string temp;
            for(int i=0; i<bytes.size(); i++)
            {
                char c = bytes[i];

                if(c > ' ')
                {
                    if(c == ',' || c == ':' || c == '(' || c == ')' || c == '{' || c == '}')
                    {
                        if(temp != "")
                            wordsAndSeparators.push_back(temp);

                        temp = c;
                        wordsAndSeparators.push_back(temp);
                        temp = "";
                    }
                    else
                    {
                        temp+=c;
                    }
                }
                else if(c == ' ')
                {
                    if(temp != "")
                        wordsAndSeparators.push_back(temp);
                    temp = "";
                }
                
            }

            if(temp != "")
                wordsAndSeparators.push_back(temp);
            temp = "";

            int inputSlot = 0;
            D3D11_INPUT_ELEMENT_DESC tempDesc;

            for(int i=0; i<wordsAndSeparators.size(); i++)
            {
                if(wordsAndSeparators[i] == functionName)
                {
                    //look for parenthesis on the right
                    if( i+1 >= wordsAndSeparators.size())
                        break;

                    if(wordsAndSeparators[i+1] == "(")
                    {
                        //look for type then name then colon then external name then comma
                        int startIndex = i+2;
                        int stage = 0;
                        while(startIndex < wordsAndSeparators.size())
                        {
                            temp = wordsAndSeparators[startIndex];

                            if(temp == ")")
                            {
                                std::string sematicNameStr = tempDesc.SemanticName;
                                if(sematicNameStr != "")
                                {
                                    tempDesc.InputSlot = inputSlot;
                                    tempDesc.SemanticIndex = 0;
                                    tempDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
                                    tempDesc.InstanceDataStepRate = 0;
                                    tempDesc.AlignedByteOffset = 0;
                                    inputSlot++;
                                    allDesc.push_back(tempDesc);
                                }
                                break;
                            }

                            if(stage==0)
                            {
                                if(temp == "in")
                                {
                                    //skip
                                }
                                else
                                {
                                    //setting type
                                    if(temp == "float" || temp == "float1")
                                    {
                                        tempDesc.Format = DXGI_FORMAT_R32_FLOAT;
                                    }
                                    else if(temp == "float2")
                                    {
                                        tempDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
                                    }
                                    else if(temp == "float3")
                                    {
                                        tempDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
                                    }
                                    else if(temp == "float4")
                                    {
                                        tempDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
                                    }
                                    else if(temp == "int" || temp == "int1")
                                    {
                                        tempDesc.Format = DXGI_FORMAT_R32_SINT;
                                    }
                                    else if(temp == "int2")
                                    {
                                        tempDesc.Format = DXGI_FORMAT_R32G32_SINT;
                                    }
                                    else if(temp == "int3")
                                    {
                                        tempDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
                                    }
                                    else if(temp == "int4")
                                    {
                                        tempDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
                                    }
                                    else
                                    {
                                        tempDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
                                    }
                                    stage++;
                                }
                            }
                            else if(stage == 1)
                            {
                                //variable name
                                stage++;
                            }
                            else if(stage == 2)
                            {
                                //must be colon
                                if(temp != ":")
                                {
                                    //error
                                    break;
                                }
                                stage++;
                            }
                            else if(stage == 3)
                            {
                                //should be semantic name
                                char* cstr = new char[temp.size()+1];
                                memcpy(cstr, temp.c_str(), temp.size()+1);

                                tempDesc.SemanticName = cstr;
                                stage++;
                            }
                            else
                            {
                                if(temp == ",")
                                {
                                    //valid
                                    tempDesc.InputSlot = inputSlot;
                                    tempDesc.SemanticIndex = 0;
                                    tempDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
                                    tempDesc.InstanceDataStepRate = 0;
                                    tempDesc.AlignedByteOffset = 0;
                                    inputSlot++;
                                    allDesc.push_back(tempDesc);
                                }
                                else
                                {
                                    //probably an error
                                    if(tempDesc.SemanticName != nullptr)
                                        delete[] tempDesc.SemanticName;

                                    tempDesc.SemanticName = nullptr;
                                    break;
                                }
                                ZeroMemory(&tempDesc, sizeof(D3D11_INPUT_ELEMENT_DESC));
                                stage = 0;
                            }
                            
                            startIndex++;
                        }
                    }
                }

                if(allDesc.size() > 0)
                    break;
            }

            return allDesc;
        }
        
    };

#endif