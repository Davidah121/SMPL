#include "ext/GLGraphics.h"

namespace glib
{
    Vec4f GLGraphics::clearColor = Vec4f();
    Vec4f GLGraphics::drawColor = Vec4f(1,1,1,1);
    Mat4f GLGraphics::orthoMat = Mat4f::getIdentity();

    GLModel GLGraphics::drawModel = GLModel();
    
    GLShader* GLGraphics::textureShader = nullptr;
    GLShader* GLGraphics::circleShader = nullptr;
    GLShader* GLGraphics::rectangleShader = nullptr;
    GLShader* GLGraphics::textShader = nullptr;
    GLFont* GLGraphics::activeFont = nullptr;

    GLGraphics GLGraphics::singleton;

    void GLGraphics::init()
    {
        GLGraphics::textureShader = new GLShader("Resources/glsl/vs/rectTextureVertShader.vs", "Resources/glsl/fs/rectTextureFragShader.fs");
        GLGraphics::circleShader = new GLShader("Resources/glsl/vs/circleVertShader.vs", "Resources/glsl/fs/circleFragShader.fs");
        GLGraphics::rectangleShader = new GLShader("Resources/glsl/vs/rectangleVertShader.vs", "Resources/glsl/fs/rectangleFragShader.fs");
        GLGraphics::textShader = new GLShader("Resources/glsl/vs/textVertShader.vs", "Resources/glsl/fs/textFragShader.fs");

        enableBlending();
        setBlendFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    GLGraphics::GLGraphics()
    {

    }

    GLGraphics::~GLGraphics()
    {
        if(GLGraphics::textureShader != nullptr)
            delete textureShader;

        if(GLGraphics::rectangleShader != nullptr)
            delete rectangleShader;

        if(GLGraphics::circleShader != nullptr)
            delete circleShader;

        if(GLGraphics::textShader != nullptr)
            delete textShader;

        GLGraphics::textureShader = nullptr;
        GLGraphics::circleShader = nullptr;
        GLGraphics::rectangleShader = nullptr;
        GLGraphics::textShader = nullptr;
    }

    void GLGraphics::clear(int clearCodes)
    {
        glClear(clearCodes);
    }
    
    void GLGraphics::setClearColor(Vec4f color)
    {
        clearColor = color;
        glClearColor((float)color.x, (float)color.y, (float)color.z, (float)color.w);
    }

    void GLGraphics::setDrawColor(Vec4f color)
    {
        drawColor = color;
    }
    
    Vec4f GLGraphics::getDrawColor()
    {
        return drawColor;
    }

    void GLGraphics::enableDepthTest()
    {
        glEnable(GL_DEPTH_TEST);
    }

    void GLGraphics::disableDepthTest()
    {
        glDisable(GL_DEPTH_TEST);
    }

    void GLGraphics::setDepthTestFunction(int type)
    {
        glDepthFunc(type);
    }

    void GLGraphics::enableFaceCulling()
    {
        glEnable(GL_CULL_FACE);
    }

    void GLGraphics::disableFaceCulling()
    {
        glDisable(GL_CULL_FACE);
    }

    void GLGraphics::setFaceCullingDirection(bool type)
    {
        if(type == CULL_COUNTER_CLOCKWISE)
            glFrontFace(GL_CCW);
        else
            glFrontFace(GL_CW);
    }
    
    void GLGraphics::setFaceCullingType(int type)
    {
        glCullFace(type);
    }

    void GLGraphics::enableBlending()
    {
        glEnable(GL_BLEND);
    }

    void GLGraphics::disableBlending()
    {
        glDisable(GL_BLEND);
    }

    void GLGraphics::setBlendFunction(int src, int dest)
    {
        glBlendFunc(src, dest);
    }
    
    void GLGraphics::setOrthoProjection(int width, int height)
    {
        orthoMat = MathExt::orthographicProjectionMatrix(width, height);
    }

    void GLGraphics::setOrthoProjection(Mat4f mat)
    {
        orthoMat = mat;
    }

    void GLGraphics::setFont(GLFont* font)
    {
        activeFont = font;
    }

    GLFont* GLGraphics::getFont()
    {
        return activeFont;
    }

    void GLGraphics::drawSurface(double x1, double y1, double x2, double y2, GLSurface* s)
    {
        //create rectangle
        std::vector<float> positions = {
            (float)x1, (float)y1,
            (float)x1, (float)y2,
            (float)x2, (float)y2,
            (float)x2, (float)y1
        };

        //create texture coords
        std::vector<float> textureCoords = {
            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f
        };

        drawModel.storeDataFloat(0, positions, 2);
        drawModel.storeDataFloat(1, textureCoords, 2);
        drawModel.setAttributeEnabled(0, true);
        drawModel.setAttributeEnabled(1, true);
        drawModel.setDrawType(Model::QUADS);

        //setup shader
        textureShader->setAsActive();
        textureShader->setVec4("activeColor", drawColor);
        textureShader->setMat4("projectionMatrix", orthoMat);
        textureShader->setInt("textureID", 0);
        
        //draw model
        s->bindTexture();
        drawModel.draw();
        
        GLShader::deactivateCurrentShader();
    }

    void GLGraphics::drawTexture(double x1, double y1, double x2, double y2, GLTexture* tex)
    {
        //create rectangle
        std::vector<float> positions = {
            (float)x1, (float)y1,
            (float)x1, (float)y2,
            (float)x2, (float)y2,
            (float)x2, (float)y1
        };

        //create texture coords
        std::vector<float> textureCoords = {
            0.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 0.0f
        };

        drawModel.storeDataFloat(0, positions, 2);
        drawModel.storeDataFloat(1, textureCoords, 2);
        drawModel.setAttributeEnabled(0, true);
        drawModel.setAttributeEnabled(1, true);
        drawModel.setDrawType(Model::QUADS);

        //setup shader
        textureShader->setAsActive();
        textureShader->setVec4("activeColor", drawColor);
        textureShader->setMat4("projectionMatrix", orthoMat);
        textureShader->setInt("textureID", 0);
        
        //draw model
        tex->bind();
        drawModel.draw();
        
        GLShader::deactivateCurrentShader();
    }

    void GLGraphics::drawTexture(double x, double y, GLTexture* tex)
    {
        drawTexture(x, y, x+tex->getWidth(), y+tex->getHeight(), tex);
    }
    
    void GLGraphics::drawSprite(double x1, double y1, double x2, double y2, GLSprite* sprite, int index)
    {
        if(index >= 0 && index < sprite->getSize())
        {
            drawTexture(x1, y1, x2, y2, sprite->getTexture(index));
        }
    }

    void GLGraphics::drawSprite(double x, double y, GLSprite* sprite, int index)
    {
        if(index >= 0 && index < sprite->getSize())
        {
            drawTexture(x, y, x + sprite->getTexture(index)->getWidth(), y + sprite->getTexture(index)->getHeight(), sprite->getTexture(index));
        }
    }

    void GLGraphics::drawText(std::string text, double x, double y, GLFont* fontPointer)
    {
        GLFont* f;
        if(fontPointer!=nullptr)
            f = fontPointer;
        else
            f = activeFont;
        
        if(f == nullptr)
            return;

        //setup shader
        Mat4f modelMatrix = Mat4f::getIdentity();

        textShader->setAsActive();
        textShader->setVec4("activeColor", drawColor);
        textShader->setMat4("projectionMatrix", orthoMat);
        textShader->setInt("textureID", 0);
        
        //draw models
        double currX = x;
        double currY = y;
        GLTexture* oldTexture = nullptr;
        for(int i=0; i<text.length(); i++)
        {
            int charIndex = f->getCharIndex(text[i]);
            GLTexture* charTexture = f->getTexture(charIndex);
            GLModel* charModel = f->getModel(charIndex);
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
            
            textShader->setMat4("modelMatrix", modelMatrix);

            if(charTexture != oldTexture)
                charTexture->bind();

            charModel->draw();
			
            currX += fci.horizAdv;

            oldTexture = charTexture;
        }
        
        GLShader::deactivateCurrentShader();
    }

    void GLGraphics::drawText(std::wstring text, double x, double y, GLFont* fontPointer)
    {
        GLFont* f;
        if(fontPointer!=nullptr)
            f = fontPointer;
        else
            f = activeFont;
        
        if(f == nullptr)
            return;

        //setup shader
        Mat4f modelMatrix = Mat4f::getIdentity();

        textShader->setAsActive();
        textShader->setVec4("activeColor", drawColor);
        textShader->setMat4("projectionMatrix", orthoMat);
        textShader->setInt("textureID", 0);
        
        //draw models
        double currX = x;
        double currY = y;
        GLTexture* oldTexture = nullptr;

        for(int i=0; i<text.length(); i++)
        {
            int charIndex = f->getCharIndex(text[i]);
            GLTexture* charTexture = f->getTexture(charIndex);
            GLModel* charModel = f->getModel(charIndex);
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
            
            textShader->setMat4("modelMatrix", modelMatrix);

            if(charTexture != oldTexture)
                charTexture->bind();
            charModel->draw();
			
            currX += fci.horizAdv;
            oldTexture = charTexture;
        }
        
        GLShader::deactivateCurrentShader();
    }
    
    void GLGraphics::drawTextLimits(std::string text, double x, double y, double maxWidth, double maxHeight, bool useLineBreak, GLFont* fontPointer)
    {
        GLFont* f;
        if(fontPointer!=nullptr)
            f = fontPointer;
        else
            f = activeFont;
        
        if(f == nullptr)
            return;

        //setup shader
        Mat4f modelMatrix = Mat4f::getIdentity();

        textShader->setAsActive();
        textShader->setVec4("activeColor", drawColor);
        textShader->setMat4("projectionMatrix", orthoMat);
        textShader->setInt("textureID", 0);
        
        //draw models
        double currX = x;
        double currY = y;
        double currW = 0;
        double currH = 0;
        
        GLTexture* oldTexture = nullptr;
        for(int i=0; i<text.length(); i++)
        {
            int charIndex = f->getCharIndex(text[i]);
            GLTexture* charTexture = f->getTexture(charIndex);
            GLModel* charModel = f->getModel(charIndex);
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
                
                textShader->setMat4("modelMatrix", modelMatrix);

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
        
        GLShader::deactivateCurrentShader();
    }

    void GLGraphics::drawTextLimits(std::wstring text, double x, double y, double maxWidth, double maxHeight, bool useLineBreak, GLFont* fontPointer)
    {
        GLFont* f;
        if(fontPointer!=nullptr)
            f = fontPointer;
        else
            f = activeFont;
        
        if(f == nullptr)
            return;

        //setup shader
        Mat4f modelMatrix = Mat4f::getIdentity();

        textShader->setAsActive();
        textShader->setVec4("activeColor", drawColor);
        textShader->setMat4("projectionMatrix", orthoMat);
        textShader->setInt("textureID", 0);
        
        //draw models
        double currX = x;
        double currY = y;
        double currW = 0;
        double currH = 0;
        
        GLTexture* oldTexture = nullptr;
        for(int i=0; i<text.length(); i++)
        {
            int charIndex = f->getCharIndex(text[i]);
            GLTexture* charTexture = f->getTexture(charIndex);
            GLModel* charModel = f->getModel(charIndex);
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
                
                textShader->setMat4("modelMatrix", modelMatrix);

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
        
        GLShader::deactivateCurrentShader();
    }

    void GLGraphics::drawRectangle(double x1, double y1, double x2, double y2, bool outline)
    {
        //create rectangle
        std::vector<float> positions = {
            (float)x1, (float)y1,
            (float)x1, (float)y2,
            (float)x2, (float)y2,
            (float)x2, (float)y1
        };

        drawModel.storeDataFloat(0, positions, 2);
        drawModel.setAttributeEnabled(0, true);
        if(outline)
            drawModel.setDrawType(Model::LINE_LOOP);
        else
            drawModel.setDrawType(Model::QUADS);

        //setup shader
        rectangleShader->setAsActive();
        rectangleShader->setVec4("activeColor", drawColor);
        rectangleShader->setMat4("projectionMatrix", orthoMat);

        //draw model
        drawModel.draw();

        GLShader::deactivateCurrentShader();
    }
    
    void GLGraphics::drawCircle(double x, double y, double radius)
    {
        //create rectangle to surround the circle
        std::vector<float> positions = {
            (float)(x-radius), (float)(y-radius),
            (float)(x-radius), (float)(y+radius),
            (float)(x+radius), (float)(y+radius),
            (float)(x+radius), (float)(y-radius)
        };

        drawModel.storeDataFloat(0, positions, 2);
        drawModel.setAttributeEnabled(0, true);
        drawModel.setDrawType(Model::QUADS);

        //setup shader
        circleShader->setAsActive();
        circleShader->setVec4("activeColor", drawColor);
        circleShader->setMat4("projectionMatrix", orthoMat);
        circleShader->setVec2("circleCenter", Vec2f(x, y));
        circleShader->setFloat("radius", radius);

        //draw model
        drawModel.draw();

        GLShader::deactivateCurrentShader();
    }
    
    void GLGraphics::drawLine(double x1, double y1, double x2, double y2)
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
        rectangleShader->setVec4("activeColor", drawColor);
        rectangleShader->setMat4("projectionMatrix", orthoMat);

        //draw model
        drawModel.draw();

        GLShader::deactivateCurrentShader();
    }

    void GLGraphics::drawBezierCurve(BezierCurve* b, int subdivisions)
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
        rectangleShader->setVec4("activeColor", drawColor);
        rectangleShader->setMat4("projectionMatrix", orthoMat);

        //draw model
        drawModel.draw();

        GLShader::deactivateCurrentShader();
    }

    void GLGraphics::drawTriangle(Vec2f p1, Vec2f p2, Vec2f p3, bool outline)
    {
        //create triangle
        std::vector<float> positions = {
            (float)p1.x, (float)p1.y,
            (float)p2.x, (float)p2.y,
            (float)p3.x, (float)p3.y,
        };

        drawModel.storeDataFloat(0, positions, 2);
        drawModel.setAttributeEnabled(0, true);
        if(outline)
            drawModel.setDrawType(Model::LINE_LOOP);
        else
            drawModel.setDrawType(Model::TRIANGLES);

        //setup shader
        rectangleShader->setAsActive();
        rectangleShader->setVec4("activeColor", drawColor);
        rectangleShader->setMat4("projectionMatrix", orthoMat);

        //draw model
        drawModel.draw();

        GLShader::deactivateCurrentShader();
    }

}