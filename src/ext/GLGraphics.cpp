#include "ext/GLGraphics.h"

#ifdef USE_OPENGL

    namespace smpl
    {
        Vec4f GLGraphics::clearColor = Vec4f();
        Vec4f GLGraphics::drawColor = Vec4f(1,1,1,1);
        Mat4f GLGraphics::orthoMat = Mat4f::getIdentity();
        Mat4f GLGraphics::surfaceMat = Mat4f::getIdentity();
        
        int GLGraphics::viewportX = 0;
        int GLGraphics::viewportY = 0;
        int GLGraphics::viewportWidth = 0;
        int GLGraphics::viewportHeight = 0;

        bool GLGraphics::antiAlias = false;

        GLModel GLGraphics::drawModel = GLModel();
        
        GLShader* GLGraphics::textureShader = nullptr;
        GLShader* GLGraphics::circleShader = nullptr;
        GLShader* GLGraphics::ellipseShader = nullptr;
        GLShader* GLGraphics::rectangleShader = nullptr;
        GLShader* GLGraphics::textShader = nullptr;
        GLFont* GLGraphics::defaultFont = nullptr;
        GLFont* GLGraphics::activeFont = nullptr;

        bool GLGraphics::hasInit = false;
        GLGraphics GLGraphics::singleton;

        void GLGraphics::init()
        {
            if(!GLGraphics::hasInit)
            {
                GLGraphics::textureShader = new GLShader("Resources/glsl/vs/rectTextureVertShader.vs", "Resources/glsl/fs/rectTextureFragShader.fs");
                GLGraphics::circleShader = new GLShader("Resources/glsl/vs/circleVertShader.vs", "Resources/glsl/fs/circleFragShader.fs");
                GLGraphics::ellipseShader = new GLShader("Resources/glsl/vs/ellipseVertShader.vs", "Resources/glsl/fs/ellipseFragShader.fs");
                GLGraphics::rectangleShader = new GLShader("Resources/glsl/vs/rectangleVertShader.vs", "Resources/glsl/fs/rectangleFragShader.fs");
                GLGraphics::textShader = new GLShader("Resources/glsl/vs/textVertShader.vs", "Resources/glsl/fs/textFragShader.fs");

                GLGraphics::defaultFont = new GLFont("./Resources/DefaultFont.fnt");
                GLGraphics::activeFont = GLGraphics::defaultFont;
                
                enableBlending();
                setBlendFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                hasInit = true;
            }
        }

        void GLGraphics::dispose()
        {
            if(GLGraphics::textureShader != nullptr)
                delete textureShader;

            if(GLGraphics::rectangleShader != nullptr)
                delete rectangleShader;

            if(GLGraphics::circleShader != nullptr)
                delete circleShader;
                
            if(GLGraphics::ellipseShader != nullptr)
                delete ellipseShader;

            if(GLGraphics::textShader != nullptr)
                delete textShader;
                
            if(GLGraphics::defaultFont != nullptr)
                delete defaultFont;

            GLGraphics::textureShader = nullptr;
            GLGraphics::circleShader = nullptr;
            GLGraphics::ellipseShader = nullptr;
            GLGraphics::rectangleShader = nullptr;
            GLGraphics::textShader = nullptr;
            GLGraphics::defaultFont = nullptr;
            GLGraphics::activeFont = nullptr;

            GLGraphics::hasInit = false;
        }

        GLGraphics::GLGraphics()
        {

        }

        GLGraphics::~GLGraphics()
        {
            dispose();
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
            orthoMat = MathExt::orthographicProjectionMatrix2(0, width, 0, height, 0, 0);
            surfaceMat = MathExt::orthographicProjectionMatrix2(0, width, 0, height, 0, 0); //Separate matrix for surface drawing.
        }

        void GLGraphics::setProjection(Mat4f mat)
        {
            orthoMat = mat;
        }

        void GLGraphics::setViewport(int x, int y, int width, int height)
        {
            glViewport(x, y, width, height+1);
            viewportX = x;
            viewportY = y;
            viewportWidth = width;
            viewportHeight = height;
        }

        void GLGraphics::setClippingRectangle(int x, int y, int width, int height)
        {
            glEnable(GL_SCISSOR_TEST);
            int actualY = viewportHeight - y - height-1;
            int actualW = width+1;
            int actualH = height+1;
            
            glScissor(x, actualY, actualW, actualH);
        }

        void GLGraphics::resetClippingRectangle()
        {
            glDisable(GL_SCISSOR_TEST);
        }
        
        void GLGraphics::setFont(GLFont* font)
        {
            activeFont = font;
        }

        GLFont* GLGraphics::getFont()
        {
            return activeFont;
        }

        void GLGraphics::drawSurface(float x1, float y1, float width, float height, GLSurface* s)
        {
            if(!hasInit)
                return; //throw error
            
            //create rectangle
            std::vector<float> positions = {
                x1, y1,
                x1, y1+height,
                x1+width, y1+height,
                x1+width, y1
            };

            //create texture coords
            std::vector<float> textureCoords = {
                0.0f, 1.0f,
                0.0f, 0.0f,
                1.0f, 0.0f,
                1.0f, 1.0f
            };

            drawModel.disableAllAttributes();
            drawModel.storeDataFloat(0, positions, 2);
            drawModel.storeDataFloat(1, textureCoords, 2);
            drawModel.setAttributeEnabled(0, true);
            drawModel.setAttributeEnabled(1, true);
            drawModel.setDrawType(Model::QUADS);

            //setup shader
            textureShader->setAsActive();
            textureShader->setVec4("activeColor", drawColor);
            textureShader->setMat4("projectionMatrix", surfaceMat);
            textureShader->setInt("textureID", 0);
            
            //draw model
            s->bindTexture();
            drawModel.draw();
            
            GLShader::deactivateCurrentShader();
        }

        void GLGraphics::drawTexture(float x1, float y1, float width, float height, GLTexture* tex)
        {
            if(!hasInit)
                return; //throw error
                
            //create rectangle
            std::vector<float> positions = {
                x1, y1,
                x1, y1+height,
                x1+width, y1+height,
                x1+width, y1
            };

            //create texture coords
            std::vector<float> textureCoords = {
                0.0f, 0.0f,
                0.0f, 1.0f,
                1.0f, 1.0f,
                1.0f, 0.0f
            };

            drawModel.disableAllAttributes();
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

        void GLGraphics::drawTexture(float x, float y, GLTexture* tex)
        {
            drawTexture(x, y, tex->getWidth(), tex->getHeight(), tex);
        }

        
        void GLGraphics::drawTexturePart(Vec2f p1, Vec2f size, Vec2f uv1, Vec2f uv2, GLTexture* tex)
        {
            if(!hasInit)
                return; //throw error

            //create rectangle
            Vec2f p2 = p1+size;
            std::vector<float> positions = {
                p1.x, p1.y,
                p1.x, p2.y,
                p2.x, p2.y,
                p2.x, p1.y
            };

            //create texture coords
            std::vector<float> textureCoords = {
                uv1.x, uv1.y,
                uv1.x, uv2.y,
                uv2.x, uv2.y,
                uv2.x, uv1.y
            };

            drawModel.disableAllAttributes();
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
        
        void GLGraphics::drawSprite(float x1, float y1, float width, float height, GLSprite* sprite, int index)
        {
            if(index >= 0 && index < sprite->getSize())
            {
                drawTexture(x1, y1, width, height, sprite->getTexture(index));
            }
        }

        void GLGraphics::drawSprite(float x, float y, GLSprite* sprite, int index)
        {
            if(index >= 0 && index < sprite->getSize())
            {
                drawTexture(x, y, sprite->getTexture(index)->getWidth(), sprite->getTexture(index)->getHeight(), sprite->getTexture(index));
            }
        }

        void GLGraphics::drawText(StringBridge text, float x, float y, GLFont* fontPointer)
        {
            GLGraphics::drawTextLimitsHighlighted(text, x, y, -1, -1, Font::NO_WRAP, -1, -1, Vec4f(), fontPointer);
        }
        void GLGraphics::drawTextHighlighted(StringBridge text, float x, float y, int highlightStart, int highlightEnd, Vec4f highlightColor, GLFont* fontPointer)
        {
            GLGraphics::drawTextLimitsHighlighted(text, x, y, -1, -1, Font::NO_WRAP, highlightStart, highlightEnd, highlightColor, fontPointer);
        }
        
        void GLGraphics::drawTextLimits(StringBridge text, float x, float y, float maxWidth, float maxHeight, char wrapMode, GLFont* fontPointer)
        {
            GLGraphics::drawTextLimitsHighlighted(text, x, y, maxWidth, maxHeight, wrapMode, -1, -1, Vec4f(), fontPointer);
        }

        void GLGraphics::drawTextLimitsHighlighted(StringBridge text, float x, float y, float maxWidth, float maxHeight, char wrapMode, int highlightStart, int highlightEnd, Vec4f highlightColor, GLFont* fontPointer)
        {
            if(!hasInit)
                return; //throw error

            GLFont* f;
            if(fontPointer!=nullptr)
                f = fontPointer;
            else
                f = activeFont;
            
            if(f == nullptr)
                return;

            //setup shader
            Mat4f modelMatrix = Mat4f::getIdentity();

            Vec4f oldDrawColor = drawColor;
            textShader->setAsActive();
            textShader->setVec4("activeColor", drawColor);
            textShader->setMat4("projectionMatrix", orthoMat);
            textShader->setInt("textureID", 0);

            std::u32string str = text.getData();
            auto boxes = f->getAllCharBoxes(str, maxWidth, wrapMode);
            GLTexture* oldTexture = nullptr;

            for(auto boxPair : boxes)
            {
                int charIndex = f->getCharIndex(str[boxPair.charIndex]);
                GLTexture* charTexture = f->getTexture(charIndex);
                GLModel* charModel = f->getModel(charIndex);
                FontCharInfo fci = f->getFontCharInfo(charIndex);
                if(charModel == nullptr || boxPair.boundingBox.getWidth() == 0 || boxPair.boundingBox.getHeight() == 0)
                {
					//special case for linebreaks to show highlighted sections
					if(boxPair.charIndex >= highlightStart && boxPair.charIndex < highlightEnd)
					{
						int x1 = x+boxPair.boundingBox.getLeftBound();
						int y1 = y+boxPair.rowStartPosition;
						int highlightWidth = 4;

                        GLGraphics::setDrawColor(highlightColor);
                        GLGraphics::drawRectangle(x1, y1, highlightWidth, f->getVerticalAdvance(), false);
                        GLGraphics::setDrawColor(oldDrawColor);
					}
                    continue;
                }

                if(boxPair.boundingBox.getRightBound() > maxWidth && maxWidth > 0)
				{
					fci.width = maxWidth - boxPair.boundingBox.getRightBound();
				}
				if(boxPair.boundingBox.getBottomBound() > maxHeight && maxHeight > 0)
				{
					fci.height = maxHeight - boxPair.boundingBox.getBottomBound();
				}

				if(str[boxPair.charIndex] != ' ')
				{
                    modelMatrix[0][3] = x + boxPair.boundingBox.getLeftBound();
                    modelMatrix[1][3] = y + boxPair.boundingBox.getTopBound();
                    textShader->setAsActive();
                    textShader->setMat4("modelMatrix", modelMatrix);

                    if(charTexture != oldTexture)
                        charTexture->bind();
                    charModel->draw();
				}

				if(boxPair.charIndex >= highlightStart && boxPair.charIndex < highlightEnd)
				{
					int x1 = x+boxPair.boundingBox.getLeftBound();
					int y1 = y+boxPair.rowStartPosition;
					int highlightWidth = boxPair.boundingBox.getWidth() + fci.horizAdv;
                    
                    GLGraphics::setDrawColor(highlightColor);
                    GLGraphics::drawRectangle(x1, y1, highlightWidth, f->getVerticalAdvance(), false);
                    GLGraphics::setDrawColor(oldDrawColor);
				}
                oldTexture = charTexture;
            }
            
            GLShader::deactivateCurrentShader();
        }

        void GLGraphics::drawRectangle(float x1, float y1, float width, float height, bool outline)
        {
            if(!hasInit)
            {
                return; //throw error
            }

            //create rectangle
            std::vector<float> positions = {
                x1, y1,
                x1, y1+height,
                x1+width, y1+height,
                x1+width, y1
            };

            drawModel.disableAllAttributes();
            drawModel.storeDataFloat(0, positions, 2);
            drawModel.setAttributeEnabled(0, true);
            drawModel.setDrawType(Model::QUADS);
            if(outline == true)
                drawModel.setDrawType(Model::LINE_LOOP);

            //setup shader
            rectangleShader->setAsActive();
            rectangleShader->setVec4("activeColor", drawColor);
            rectangleShader->setMat4("projectionMatrix", orthoMat);

            //draw model
            drawModel.draw();

            GLShader::deactivateCurrentShader();
        }
        
        void GLGraphics::drawCircle(float x, float y, float radius, bool outline)
        {
            if(outline)
                drawCircle(x, y, radius, 1.0f);
            else
                drawCircle(x, y, radius, radius);
        }

        void GLGraphics::drawCircle(float x, float y, float rad, float maxDisFromRad)
        {
            if(!hasInit)
                return; //throw error

            //create rectangle to surround the circle
            std::vector<float> positions = {
                x-rad, y-rad,
                x-rad, y+rad,
                x+rad, y+rad,
                x+rad, y-rad
            };

            drawModel.disableAllAttributes();
            drawModel.storeDataFloat(0, positions, 2);
            drawModel.setAttributeEnabled(0, true);
            drawModel.setDrawType(Model::QUADS);

            //setup shader
            circleShader->setAsActive();
            circleShader->setVec4("activeColor", drawColor);
            circleShader->setMat4("projectionMatrix", orthoMat);
            circleShader->setVec2("circleCenter", Vec2f(x, y));
            circleShader->setBool("antiAlias", GLGraphics::antiAlias);
            circleShader->setFloat("radius", rad);
            circleShader->setFloat("maxDisFromEdge", maxDisFromRad);

            //draw model
            drawModel.draw();

            GLShader::deactivateCurrentShader();
        }
        
        void GLGraphics::drawEllipse(float x, float y, float xRadius, float yRadius, bool outline)
        {
            if(xRadius == yRadius)
                drawCircle(x, y, xRadius, outline);
            else
            {
                if(!outline)
                    drawEllipse(x, y, xRadius, yRadius, __max(xRadius, yRadius));
                else
                    drawEllipse(x, y, xRadius, yRadius, 1.0f);
            }
        }
        void GLGraphics::drawEllipse(float x, float y, float xRadius, float yRadius, float maxDisFromEdge)
        {
            drawEllipse(x, y, Vec2f(xRadius, 0), Vec2f(0, yRadius), maxDisFromEdge);
        }

        void GLGraphics::drawEllipse(float x, float y, Vec2f dir1, Vec2f dir2, float maxDisFromEdge)
        {
            if(!hasInit)
                return; //throw error

            float x1 = x-__max(abs(dir1.x), abs(dir2.x));
            float x2 = x+__max(abs(dir1.x), abs(dir2.x));
            float y1 = y-__max(abs(dir1.y), abs(dir2.y));
            float y2 = y+__max(abs(dir1.y), abs(dir2.y));

            float minX = __min(x1, x2)-16;
            float maxX = __max(x1, x2)+16;
            float minY = __min(y1, y2)-16;
            float maxY = __max(y1, y2)+16;
            
            //create rectangle to surround the ellipse
            std::vector<float> positions = {
                minX, minY,
                minX, maxY,
                maxX, maxY,
                maxX, minY
            };

            drawModel.disableAllAttributes();
            drawModel.storeDataFloat(0, positions, 2);
            drawModel.setAttributeEnabled(0, true);
            drawModel.setDrawType(Model::QUADS);

            //setup shader
            ellipseShader->setAsActive();
            ellipseShader->setVec4("activeColor", drawColor);
            ellipseShader->setMat4("projectionMatrix", orthoMat);
            ellipseShader->setVec2("ellipseCenter", Vec2f(x, y));
            ellipseShader->setVec2("dir1", dir1.normalize());
            ellipseShader->setVec2("dir2", dir2.normalize());
            ellipseShader->setVec2("radi", Vec2f(dir1.getLength(), dir2.getLength()));
            ellipseShader->setFloat("maxDisFromEdge", maxDisFromEdge);

            //draw model
            drawModel.draw();

            GLShader::deactivateCurrentShader();
        }

        
        void GLGraphics::drawLine(float x1, float y1, float x2, float y2)
        {
            if(!hasInit)
                return; //throw error
            
            //use rectangle shader
            std::vector<float> positions = {
                x1, y1,
                x2, y2
            };

            drawModel.disableAllAttributes();
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
            if(!hasInit)
                return; //throw error

            //use rectangle shader
            std::vector<float> positions;
            for(int i=0; i<subdivisions; i++)
            {
                Vec2f bPos = b->getFunctionAt((float)i/subdivisions);
                positions.push_back(bPos.x);
                positions.push_back(bPos.y);
            }

            drawModel.disableAllAttributes();
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
            if(!hasInit)
                return; //throw error

            //create triangle
            std::vector<float> positions = {
                p1.x, p1.y,
                p2.x, p2.y,
                p3.x, p3.y,
            };

            drawModel.disableAllAttributes();
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
#endif