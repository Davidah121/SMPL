#ifdef USE_DIRECTX

    #include "ext/DXGraphics.h"

    namespace glib
    {
        Vec4f DXGraphics::clearColor = Vec4f(0,0,0,0);
        Vec4f DXGraphics::drawColor = Vec4f(1,1,1,1);
        Mat4f DXGraphics::orthoMat = Mat4f::getIdentity();

        DXModel DXGraphics::drawModel = DXModel();
        
        DXShader* DXGraphics::textureShader = nullptr;
        DXShader* DXGraphics::circleShader = nullptr;
        DXShader* DXGraphics::rectangleShader = nullptr;
        DXShader* DXGraphics::textShader = nullptr;
        DXFont* DXGraphics::activeFont = nullptr;

        D3D11_DEPTH_STENCIL_DESC DXGraphics::depthStencilDesc;
        D3D11_RASTERIZER_DESC DXGraphics::rasterDesc;
        D3D11_BLEND_DESC DXGraphics::blendDesc;

        DXGraphics DXGraphics::singleton;

        void DXGraphics::init()
        {
            createDepthStencilState();
            createRasterState();
            createBlendState();

            DXGraphics::rectangleShader = new DXShader("Resources/hlsl/vs/rectangleVertShader.cso", "Resources/hlsl/fs/rectangleFragShader.cso",
            { {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0} });

            DXGraphics::circleShader = new DXShader("Resources/hlsl/vs/circleVertShader.cso", "Resources/hlsl/fs/circleFragShader.cso",
            { {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0} });

            DXGraphics::textureShader = new DXShader("Resources/hlsl/vs/rectTextureVertShader.cso", "Resources/hlsl/fs/rectTextureFragShader.cso", 
            { {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0} });

            DXGraphics::textShader = new DXShader("Resources/hlsl/vs/textVertShader.cso", "Resources/hlsl/fs/textFragShader.cso", 
            { {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0} });

            enableBlending();
            // setBlendFunction(DX_SRC_ALPHA, DX_ONE_MINUS_SRC_ALPHA);
        }

        void DXGraphics::createDepthStencilState()
        {
            //Create DepthStencilState
            depthStencilDesc.DepthEnable = TRUE;
            depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
            depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
            depthStencilDesc.StencilEnable = FALSE;
            depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
            depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

            // Stencil operations if pixel is front-facing
            depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
            depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
            depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
            depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

            // Stencil operations if pixel is back-facing
            depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
            depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
            depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
            depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        }

        void DXGraphics::createRasterState()
        {
            rasterDesc.FillMode = D3D11_FILL_SOLID;
            rasterDesc.CullMode = (D3D11_CULL_MODE)CULL_BACK;
            rasterDesc.FrontCounterClockwise = CULL_CLOCKWISE;
            rasterDesc.DepthBias = 0;
            rasterDesc.SlopeScaledDepthBias = 0;
            rasterDesc.DepthBiasClamp = 0;
            rasterDesc.DepthClipEnable = true;
            rasterDesc.ScissorEnable = false;
            rasterDesc.MultisampleEnable = true;
            rasterDesc.AntialiasedLineEnable = false;
        }

        
        void DXGraphics::createBlendState()
        {
            blendDesc.RenderTarget->BlendEnable = TRUE;
            blendDesc.RenderTarget->BlendOp = D3D11_BLEND_OP_ADD;
            blendDesc.RenderTarget->BlendOpAlpha = D3D11_BLEND_OP_ADD;
            blendDesc.RenderTarget->SrcBlend = D3D11_BLEND_SRC_ALPHA;
            blendDesc.RenderTarget->SrcBlendAlpha = D3D11_BLEND_ONE;
            blendDesc.RenderTarget->DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
            blendDesc.RenderTarget->DestBlendAlpha = D3D11_BLEND_ZERO;
            blendDesc.RenderTarget->RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        }
        
        DXGraphics::DXGraphics()
        {

        }

        DXGraphics::~DXGraphics()
        {
            if(DXGraphics::textureShader != nullptr)
                delete textureShader;

            if(DXGraphics::rectangleShader != nullptr)
                delete rectangleShader;

            if(DXGraphics::circleShader != nullptr)
                delete circleShader;

            if(DXGraphics::textShader != nullptr)
                delete textShader;

            DXGraphics::textureShader = nullptr;
            DXGraphics::circleShader = nullptr;
            DXGraphics::rectangleShader = nullptr;
            DXGraphics::textShader = nullptr;
        }

        void DXGraphics::clear(int clearCodes)
        {
            //implement clear color buffer, depth buffer, stencil buffer
            float colorArr[4] = {(float)clearColor.x, (float)clearColor.y, (float)clearColor.z, (float)clearColor.w};
            ID3D11DeviceContext* d3dContext = DXSingleton::getContext();

            if(DXSurface::getCurrentView() != nullptr)
            {
                if(clearCodes & DXGraphics::COLOR_BUFFER)
                    d3dContext->ClearRenderTargetView(DXSurface::getCurrentView(), colorArr);

                if(clearCodes & DXGraphics::DEPTH_BUFFER)
                    if(DXSurface::getCurrentDepthView() != nullptr)
                        d3dContext->ClearDepthStencilView(DXSurface::getCurrentDepthView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0, 0);
            }
            else
            {
                ID3D11RenderTargetView* backBuffer = DXSingleton::getBackBuffer();
                d3dContext->ClearRenderTargetView(backBuffer, colorArr);
            }
        }
        
        void DXGraphics::setClearColor(Vec4f color)
        {
            clearColor = color;
        }

        Vec4f DXGraphics::getClearColor()
        {
            return clearColor;
        }

        void DXGraphics::setDrawColor(Vec4f color)
        {
            drawColor = color;
        }
        
        Vec4f DXGraphics::getDrawColor()
        {
            return drawColor;
        }

        void DXGraphics::enableDepthTest()
        {
            ID3D11Device* device = DXSingleton::getDevice();
            ID3D11DeviceContext* context = DXSingleton::getContext();
            
            depthStencilDesc.DepthEnable = TRUE;

            ID3D11DepthStencilState* depthStencilState;
            HRESULT res = device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
            
            if(res == S_OK)
                context->OMSetDepthStencilState(depthStencilState, 0);

            depthStencilState->Release();
        }

        void DXGraphics::disableDepthTest()
        {
            ID3D11Device* device = DXSingleton::getDevice();
            ID3D11DeviceContext* context = DXSingleton::getContext();
            
            depthStencilDesc.DepthEnable = FALSE;

            ID3D11DepthStencilState* depthStencilState;
            HRESULT res = device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
            
            if(res == S_OK)
                context->OMSetDepthStencilState(depthStencilState, 0);
                
            depthStencilState->Release();
        }

        void DXGraphics::setDepthTestFunction(int type)
        {
            ID3D11Device* device = DXSingleton::getDevice();
            ID3D11DeviceContext* context = DXSingleton::getContext();
            
            depthStencilDesc.DepthFunc = (D3D11_COMPARISON_FUNC)type;

            ID3D11DepthStencilState* depthStencilState;
            HRESULT res = device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
            
            if(res == S_OK)
                context->OMSetDepthStencilState(depthStencilState, 0);

            depthStencilState->Release();
        }

        void DXGraphics::enableFaceCulling(int type, bool direction)
        {
            ID3D11Device* device = DXSingleton::getDevice();
            ID3D11DeviceContext* context = DXSingleton::getContext();

            rasterDesc.CullMode = (D3D11_CULL_MODE)type;
            rasterDesc.FrontCounterClockwise = direction;

            ID3D11RasterizerState* rasterState;
            HRESULT res = device->CreateRasterizerState(&rasterDesc, &rasterState);
            
            if(res == S_OK)
                context->RSSetState(rasterState);

            rasterState->Release();
        }

        void DXGraphics::disableFaceCulling()
        {
            ID3D11Device* device = DXSingleton::getDevice();
            ID3D11DeviceContext* context = DXSingleton::getContext();

            rasterDesc.CullMode = D3D11_CULL_NONE;

            ID3D11RasterizerState* rasterState;
            HRESULT res = device->CreateRasterizerState(&rasterDesc, &rasterState);
            
            if(res == S_OK)
                context->RSSetState(rasterState);
            
            rasterState->Release();
        }

        void DXGraphics::enableBlending()
        {
            blendDesc.RenderTarget->BlendEnable = TRUE;

            ID3D11BlendState* blendState;
            ID3D11Device* device = DXSingleton::getDevice();
            ID3D11DeviceContext* context = DXSingleton::getContext();

            HRESULT res = device->CreateBlendState(&blendDesc, &blendState);
            
            if(res == S_OK)
                context->OMSetBlendState(blendState, nullptr, 0xFFFFFFFF );
            
            blendState->Release();
        }

        void DXGraphics::disableBlending()
        {
            blendDesc.RenderTarget->BlendEnable = FALSE;
            ID3D11BlendState* blendState;
            ID3D11Device* device = DXSingleton::getDevice();
            ID3D11DeviceContext* context = DXSingleton::getContext();

            HRESULT res = device->CreateBlendState(&blendDesc, &blendState);
            
            if(res == S_OK)
                context->OMSetBlendState(blendState, nullptr, 0xFFFFFFFF );

            blendState->Release();
        }

        void DXGraphics::setBlendFunction(int src, int dest)
        {
            // glBlendFunc(src, dest);
        }
        
        void DXGraphics::setOrthoProjection(int width, int height)
        {
            orthoMat = MathExt::orthographicProjectionMatrix(width, height);
        }

        void DXGraphics::setOrthoProjection(Mat4f mat)
        {
            orthoMat = mat;
        }

        void DXGraphics::setFont(DXFont* font)
        {
            activeFont = font;
        }

        DXFont* DXGraphics::getFont()
        {
            return activeFont;
        }

        void DXGraphics::drawSurface(double x1, double y1, double x2, double y2, DXSurface* s)
        {
            //create rectangle
            std::vector<float> positions = {
                (float)x1, (float)y1,
                (float)x2, (float)y2,
                (float)x1, (float)y2,

                (float)x1, (float)y1,
                (float)x2, (float)y1,
                (float)x2, (float)y2
            };

            //create texture coords
            std::vector<float> textureCoords = {
                0.0f, 0.0f,
                1.0f, 1.0f,
                0.0f, 1.0f,

                0.0f, 0.0f,
                1.0f, 0.0f,
                1.0f, 1.0f
            };

            drawModel.storeDataFloat(0, positions, 2);
            drawModel.storeDataFloat(1, textureCoords, 2);
            drawModel.setAttributeEnabled(0, true);
            drawModel.setAttributeEnabled(1, true);
            drawModel.setDrawType(Model::TRIANGLES);

            //setup shader
            textureShader->setAsActive();
            struct UniformBuffer
            {
                float projectionMatrix[16];
            };
            struct ColorBuffer
            {
                float activeColor[4];
            };

            UniformBuffer buffer;
            ColorBuffer buffer2;

            drawColor.fillArray(buffer2.activeColor);
            orthoMat.fillArray(buffer.projectionMatrix);
            textureShader->setUniformData(&buffer, sizeof(UniformBuffer), 0, DXShader::TYPE_VERTEX);
            textureShader->setUniformData(&buffer2, sizeof(ColorBuffer), 0, DXShader::TYPE_FRAGMENT);
            
            //draw model
            s->bindTexture();
            drawModel.draw();
            
            DXShader::deactivateCurrentShader();
        }

        void DXGraphics::drawTexture(double x1, double y1, double x2, double y2, DXTexture* tex)
        {
            //create rectangle
            std::vector<float> positions = {
                (float)x1, (float)y1,
                (float)x2, (float)y2,
                (float)x1, (float)y2,

                (float)x1, (float)y1,
                (float)x2, (float)y1,
                (float)x2, (float)y2
            };

            //create texture coords
            std::vector<float> textureCoords = {
                0.0f, 0.0f,
                1.0f, 1.0f,
                0.0f, 1.0f,

                0.0f, 0.0f,
                1.0f, 0.0f,
                1.0f, 1.0f
            };

            drawModel.storeDataFloat(0, positions, 2);
            drawModel.storeDataFloat(1, textureCoords, 2);
            drawModel.setAttributeEnabled(0, true);
            drawModel.setAttributeEnabled(1, true);
            drawModel.setDrawType(Model::TRIANGLES);

            //setup shader
            textureShader->setAsActive();
            struct UniformBuffer
            {
                float projectionMatrix[16];
            };
            struct ColorBuffer
            {
                float activeColor[4];
            };

            UniformBuffer buffer;
            ColorBuffer buffer2;

            drawColor.fillArray(buffer2.activeColor);
            orthoMat.fillArray(buffer.projectionMatrix);
            textureShader->setUniformData(&buffer, sizeof(UniformBuffer), 0, DXShader::TYPE_VERTEX);
            textureShader->setUniformData(&buffer2, sizeof(ColorBuffer), 0, DXShader::TYPE_FRAGMENT);
            
            //draw model
            tex->bind();
            drawModel.draw();
            
            DXShader::deactivateCurrentShader();
        }

        void DXGraphics::drawTexture(double x, double y, DXTexture* tex)
        {
            drawTexture(x, y, x+tex->getWidth(), y+tex->getHeight(), tex);
        }
        
        void DXGraphics::drawSprite(double x1, double y1, double x2, double y2, DXSprite* sprite, int index)
        {
            if(index >= 0 && index < sprite->getSize())
            {
                drawTexture(x1, y1, x2, y2, sprite->getTexture(index));
            }
        }

        void DXGraphics::drawSprite(double x, double y, DXSprite* sprite, int index)
        {
            if(index >= 0 && index < sprite->getSize())
            {
                drawTexture(x, y, x + sprite->getTexture(index)->getWidth(), y + sprite->getTexture(index)->getHeight(), sprite->getTexture(index));
            }
        }

        void DXGraphics::drawText(std::string text, double x, double y, DXFont* fontPointer)
        {
            DXFont* f;
            if(fontPointer!=nullptr)
                f = fontPointer;
            else
                f = activeFont;
            
            if(f == nullptr)
                return;

            //setup shader
            Mat4f modelMatrix = Mat4f::getIdentity();

            textShader->setAsActive();
            struct StaticUniformBuffer
            {
                float projectionMatrix[16];
            };
            struct ColorBuffer
            {
                float activeColor[4];
            };

            ColorBuffer colBuffer;

            drawColor.fillArray(colBuffer.activeColor);
            textureShader->setUniformData(&colBuffer, sizeof(ColorBuffer), 0, DXShader::TYPE_FRAGMENT);

            struct MatrixUniformBuffer
            {
                float modelMatrix[16];
            };

            StaticUniformBuffer staticBuffer;
            orthoMat.fillArray(staticBuffer.projectionMatrix);
            textShader->setUniformData(&staticBuffer, sizeof(StaticUniformBuffer), 0, DXShader::TYPE_VERTEX);

            
            MatrixUniformBuffer modelBuffer;
            
            //draw models
            double currX = x;
            double currY = y;
            DXTexture* oldTexture = nullptr;
            for(int i=0; i<text.length(); i++)
            {
                int charIndex = f->getCharIndex(text[i]);
                DXTexture* charTexture = f->getTexture(charIndex);
                DXModel* charModel = f->getModel(charIndex);
                FontCharInfo fci = f->getFontCharInfo(text[i]);

                if(charModel == nullptr)
                {
                    continue;
                }

                if(text[i] == '\n')
                {
                    currX = x;
                    currY += f->getVerticalAdvance();
                    continue;
                }

                if(text[i] == ' ')
                {
                    currX += fci.horizAdv;
                    continue;
                }

                modelMatrix[0][3] = currX+fci.xOffset;
                modelMatrix[1][3] = currY+fci.yOffset;
                
                modelMatrix.fillArray(modelBuffer.modelMatrix);
                textShader->setUniformData(&modelBuffer, sizeof(MatrixUniformBuffer), 1, DXShader::TYPE_VERTEX);

                if(charTexture != oldTexture)
                    charTexture->bind();

                charModel->draw();
                
                currX += fci.horizAdv;

                oldTexture = charTexture;
            }
            
            DXShader::deactivateCurrentShader();
        }

        void DXGraphics::drawText(std::wstring text, double x, double y, DXFont* fontPointer)
        {
            DXFont* f;
            if(fontPointer!=nullptr)
                f = fontPointer;
            else
                f = activeFont;
            
            if(f == nullptr)
                return;

            //setup shader
            Mat4f modelMatrix = Mat4f::getIdentity();

            textShader->setAsActive();
            struct StaticUniformBuffer
            {
                float projectionMatrix[16];
            };
            struct ColorBuffer
            {
                float activeColor[4];
            };

            ColorBuffer colBuffer;

            drawColor.fillArray(colBuffer.activeColor);
            textureShader->setUniformData(&colBuffer, sizeof(ColorBuffer), 0, DXShader::TYPE_FRAGMENT);

            struct MatrixUniformBuffer
            {
                float modelMatrix[16];
            };

            StaticUniformBuffer staticBuffer;
            orthoMat.fillArray(staticBuffer.projectionMatrix);
            textShader->setUniformData(&staticBuffer, sizeof(StaticUniformBuffer), 0, DXShader::TYPE_VERTEX);

            
            MatrixUniformBuffer modelBuffer;
            
            //draw models
            double currX = x;
            double currY = y;
            DXTexture* oldTexture = nullptr;

            for(int i=0; i<text.length(); i++)
            {
                int charIndex = f->getCharIndex(text[i]);
                DXTexture* charTexture = f->getTexture(charIndex);
                DXModel* charModel = f->getModel(charIndex);
                FontCharInfo fci = f->getFontCharInfo(text[i]);

                if(charModel == nullptr)
                {
                    continue;
                }

                if(text[i] == '\n')
                {
                    currX = x;
                    currY += f->getVerticalAdvance();
                    continue;
                }

                if(text[i] == ' ')
                {
                    currX += fci.horizAdv;
                    continue;
                }

                modelMatrix[0][3] = currX+fci.xOffset;
                modelMatrix[1][3] = currY+fci.yOffset;
                
                modelMatrix.fillArray(modelBuffer.modelMatrix);
                textShader->setUniformData(&modelBuffer, sizeof(MatrixUniformBuffer), 1, DXShader::TYPE_VERTEX);

                if(charTexture != oldTexture)
                    charTexture->bind();
                charModel->draw();
                
                currX += fci.horizAdv;
                oldTexture = charTexture;
            }
            
            DXShader::deactivateCurrentShader();
        }
        
        void DXGraphics::drawTextLimits(std::string text, double x, double y, double maxWidth, double maxHeight, bool useLineBreak, DXFont* fontPointer)
        {
            DXFont* f;
            if(fontPointer!=nullptr)
                f = fontPointer;
            else
                f = activeFont;
            
            if(f == nullptr)
                return;

            //setup shader
            Mat4f modelMatrix = Mat4f::getIdentity();

            textShader->setAsActive();
            struct StaticUniformBuffer
            {
                float projectionMatrix[16];
            };
            struct ColorBuffer
            {
                float activeColor[4];
            };

            ColorBuffer colBuffer;

            drawColor.fillArray(colBuffer.activeColor);
            textureShader->setUniformData(&colBuffer, sizeof(ColorBuffer), 0, DXShader::TYPE_FRAGMENT);

            struct MatrixUniformBuffer
            {
                float modelMatrix[16];
            };

            StaticUniformBuffer staticBuffer;
            orthoMat.fillArray(staticBuffer.projectionMatrix);
            textShader->setUniformData(&staticBuffer, sizeof(StaticUniformBuffer), 0, DXShader::TYPE_VERTEX);

            
            MatrixUniformBuffer modelBuffer;
            
            //draw models
            double currX = x;
            double currY = y;
            double currW = 0;
            double currH = 0;
            
            DXTexture* oldTexture = nullptr;
            for(int i=0; i<text.length(); i++)
            {
                int charIndex = f->getCharIndex(text[i]);
                DXTexture* charTexture = f->getTexture(charIndex);
                DXModel* charModel = f->getModel(charIndex);
                FontCharInfo fci = f->getFontCharInfo(text[i]);

                if(charModel == nullptr)
                {
                    continue;
                }

                if(text[i] == '\n')
                {
                    currX = x;
                    currW = 0;

                    if(useLineBreak)
                    {
                        currH += f->getVerticalAdvance();
                        currY += f->getVerticalAdvance();

                        if(currH >= maxHeight)
                        {
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                    continue;
                }

                if(currW + fci.width > maxWidth)
                {
                    fci.width = maxWidth - currW;
                }
                if(currH + fci.height > maxHeight)
                {
                    fci.height = maxHeight - currH;
                }

                if(text[i] != ' ')
                {
                    modelMatrix[0][3] = currX+fci.xOffset;
                    modelMatrix[1][3] = currY+fci.yOffset;
                    
                    modelMatrix.fillArray(modelBuffer.modelMatrix);
                    textShader->setUniformData(&modelBuffer, sizeof(MatrixUniformBuffer), 1, DXShader::TYPE_VERTEX);

                    if(charTexture != oldTexture)
                        charTexture->bind();
                    charModel->draw();
                }
                
                currX += fci.horizAdv;
                currW += fci.horizAdv;

                if(currW >= maxWidth)
                {
                    currW = 0;
                    currX = x;

                    if(useLineBreak)
                    {
                        currH += f->getVerticalAdvance();
                        currY += f->getVerticalAdvance();

                        if(currH >= maxHeight)
                        {
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                oldTexture = charTexture;
            }
            
            DXShader::deactivateCurrentShader();
        }

        void DXGraphics::drawTextLimits(std::wstring text, double x, double y, double maxWidth, double maxHeight, bool useLineBreak, DXFont* fontPointer)
        {
            DXFont* f;
            if(fontPointer!=nullptr)
                f = fontPointer;
            else
                f = activeFont;
            
            if(f == nullptr)
                return;

            //setup shader
            Mat4f modelMatrix = Mat4f::getIdentity();

            textShader->setAsActive();
            struct StaticUniformBuffer
            {
                float projectionMatrix[16];
            };
            struct ColorBuffer
            {
                float activeColor[4];
            };

            ColorBuffer colBuffer;

            drawColor.fillArray(colBuffer.activeColor);
            textureShader->setUniformData(&colBuffer, sizeof(ColorBuffer), 0, DXShader::TYPE_FRAGMENT);

            struct MatrixUniformBuffer
            {
                float modelMatrix[16];
            };

            StaticUniformBuffer staticBuffer;
            orthoMat.fillArray(staticBuffer.projectionMatrix);
            textShader->setUniformData(&staticBuffer, sizeof(StaticUniformBuffer), 0, DXShader::TYPE_VERTEX);

            
            MatrixUniformBuffer modelBuffer;
            
            //draw models
            double currX = x;
            double currY = y;
            double currW = 0;
            double currH = 0;
            
            DXTexture* oldTexture = nullptr;
            for(int i=0; i<text.length(); i++)
            {
                int charIndex = f->getCharIndex(text[i]);
                DXTexture* charTexture = f->getTexture(charIndex);
                DXModel* charModel = f->getModel(charIndex);
                FontCharInfo fci = f->getFontCharInfo(text[i]);

                if(charModel == nullptr)
                {
                    continue;
                }

                if(text[i] == '\n')
                {
                    currX = x;
                    currW = 0;

                    if(useLineBreak)
                    {
                        currH += f->getVerticalAdvance();
                        currY += f->getVerticalAdvance();

                        if(currH >= maxHeight)
                        {
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                    continue;
                }

                if(currW + fci.width > maxWidth)
                {
                    fci.width = maxWidth - currW;
                }
                if(currH + fci.height > maxHeight)
                {
                    fci.height = maxHeight - currH;
                }

                if(text[i] != ' ')
                {
                    modelMatrix[0][3] = currX+fci.xOffset;
                    modelMatrix[1][3] = currY+fci.yOffset;
                    
                    modelMatrix.fillArray(modelBuffer.modelMatrix);
                    textShader->setUniformData(&modelBuffer, sizeof(MatrixUniformBuffer), 1, DXShader::TYPE_VERTEX);

                    if(charTexture != oldTexture)
                        charTexture->bind();
                    charModel->draw();
                }
                
                currX += fci.horizAdv;
                currW += fci.horizAdv;

                if(currW >= maxWidth)
                {
                    currW = 0;
                    currX = x;

                    if(useLineBreak)
                    {
                        currH += f->getVerticalAdvance();
                        currY += f->getVerticalAdvance();

                        if(currH >= maxHeight)
                        {
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                oldTexture = charTexture;
            }
            
            DXShader::deactivateCurrentShader();
        }

        void DXGraphics::drawRectangle(double x1, double y1, double x2, double y2, bool outline)
        {
            //create rectangle
            std::vector<float> positions = {
                (float)x1, (float)y1,
                (float)x2, (float)y2,
                (float)x1, (float)y2,

                (float)x1, (float)y1,
                (float)x2, (float)y1,
                (float)x2, (float)y2
            };

            drawModel.storeDataFloat(0, positions, 2);
            drawModel.setAttributeEnabled(0, true);
            if(outline)
                drawModel.setDrawType(Model::LINE_LOOP);
            else
                drawModel.setDrawType(Model::TRIANGLES);

            //setup shader
            rectangleShader->setAsActive();
            struct UniformBuffer
            {
                float activeColor[4];
                float projectionMatrix[16];
            };

            UniformBuffer buffer;
            drawColor.fillArray(buffer.activeColor);
            orthoMat.fillArray(buffer.projectionMatrix);
            rectangleShader->setUniformData(&buffer, sizeof(UniformBuffer), 0, DXShader::TYPE_VERTEX);

            //draw model
            drawModel.draw();

            DXShader::deactivateCurrentShader();
        }
        
        void DXGraphics::drawCircle(double x, double y, double radius)
        {
            //create rectangle to surround the circle
            std::vector<float> positions = {
                (float)(x-radius), (float)(y-radius),
                (float)(x+radius), (float)(y+radius),
                (float)(x-radius), (float)(y+radius),

                (float)(x-radius), (float)(y-radius),
                (float)(x+radius), (float)(y-radius),
                (float)(x+radius), (float)(y+radius)
            };

            drawModel.storeDataFloat(0, positions, 2);
            drawModel.setAttributeEnabled(0, true);
            drawModel.setDrawType(Model::TRIANGLES);

            //setup shader
            circleShader->setAsActive();
            struct UniformBuffer
            {
                float activeColor[4];
                float projectionMatrix[16];
            };

            struct UniformBuffer2
            {
                float circleCenter[2];
                float radius;
                float extra;
            };

            UniformBuffer buffer;
            UniformBuffer2 buffer2;
            drawColor.fillArray(buffer.activeColor);
            orthoMat.fillArray(buffer.projectionMatrix);

            buffer2.circleCenter[0] = (float)x;
            buffer2.circleCenter[1] = (float)y;
            buffer2.radius = radius;
            circleShader->setUniformData(&buffer, sizeof(UniformBuffer), 0, DXShader::TYPE_VERTEX);
            circleShader->setUniformData(&buffer2, sizeof(UniformBuffer2), 0, DXShader::TYPE_FRAGMENT);

            //draw model
            drawModel.draw();

            DXShader::deactivateCurrentShader();
        }
        
        void DXGraphics::drawLine(double x1, double y1, double x2, double y2)
        {
            //use rectangle shader
            std::vector<float> positions = {
                (float)(x1), (float)(y1),
                (float)(x2), (float)(y2)
            };

            drawModel.storeDataFloat(0, positions, 2);
            drawModel.setAttributeEnabled(0, true);
            drawModel.setDrawType(Model::LINES);

            //setup shader
            rectangleShader->setAsActive();
            struct UniformBuffer
            {
                float activeColor[4];
                float projectionMatrix[16];
            };

            UniformBuffer buffer;
            drawColor.fillArray(buffer.activeColor);
            orthoMat.fillArray(buffer.projectionMatrix);
            rectangleShader->setUniformData(&buffer, sizeof(UniformBuffer), 0, DXShader::TYPE_VERTEX);

            //draw model
            drawModel.draw();

            DXShader::deactivateCurrentShader();
        }

        void DXGraphics::drawBezierCurve(BezierCurve* b, int subdivisions)
        {
            //use rectangle shader
            std::vector<float> positions;
            for(int i=0; i<subdivisions; i++)
            {
                Vec2f bPos = b->getFuctionAt((double)i/subdivisions);
                positions.push_back((float)bPos.x);
                positions.push_back((float)bPos.y);
            }

            drawModel.storeDataFloat(0, positions, 2);
            drawModel.setAttributeEnabled(0, true);
            drawModel.setDrawType(Model::LINE_STRIP);

            //setup shader
            rectangleShader->setAsActive();
            struct UniformBuffer
            {
                float activeColor[4];
                float projectionMatrix[16];
            };

            UniformBuffer buffer;
            drawColor.fillArray(buffer.activeColor);
            orthoMat.fillArray(buffer.projectionMatrix);
            rectangleShader->setUniformData(&buffer, sizeof(UniformBuffer), 0, DXShader::TYPE_VERTEX);

            //draw model
            drawModel.draw();

            DXShader::deactivateCurrentShader();
        }

        void DXGraphics::drawTriangle(Vec2f p1, Vec2f p2, Vec2f p3, bool outline)
        {
            //create triangle
            std::vector<float> positions = {
                (float)p1.x, (float)p1.y,
                (float)p2.x, (float)p2.y,
                (float)p3.x, (float)p3.y
            };

            drawModel.storeDataFloat(0, positions, 2);
            drawModel.setAttributeEnabled(0, true);
            if(outline)
                drawModel.setDrawType(Model::LINE_LOOP);
            else
                drawModel.setDrawType(Model::TRIANGLES);

            //setup shader
            rectangleShader->setAsActive();
            struct UniformBuffer
            {
                float activeColor[4];
                float projectionMatrix[16];
            };

            UniformBuffer buffer;
            drawColor.fillArray(buffer.activeColor);
            orthoMat.fillArray(buffer.projectionMatrix);
            rectangleShader->setUniformData(&buffer, sizeof(UniformBuffer), 0, DXShader::TYPE_VERTEX);

            //draw model
            drawModel.draw();

            DXShader::deactivateCurrentShader();
        }

    }

#endif