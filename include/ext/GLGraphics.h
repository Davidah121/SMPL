#pragma once
#ifdef USE_OPENGL

    #include "MathExt.h"
    #include "ext/GLSingleton.h"
    #include "ext/GLWindow.h"
    #include "ext/GLTexture.h"
    #include "ext/GLSurface.h"
    #include "ext/GLShader.h"
    #include "ext/GLModel.h"
    #include "ext/GLSprite.h"
    #include "ext/GLFont.h"
    #include "StringBridge.h"


    namespace smpl
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

            static const bool CULL_COUNTER_CLOCKWISE = true;
            static const bool CULL_CLOCKWISE = false;
            
            static void init();
            static void dispose();

            static void clear(int clearCodes);
            static void setClearColor(Vec4f color);
            static Vec4f getClearColor();

            static void setDrawColor(Vec4f color);
            static Vec4f getDrawColor();

            static void enableDepthTest();
            static void disableDepthTest();

            static void setDepthTestFunction(int type);

            static void enableFaceCulling();
            static void disableFaceCulling();
            static void setFaceCullingDirection(bool type);
            static void setFaceCullingType(int type);

            static void enableBlending();
            static void disableBlending();
            static void setBlendFunction(int src, int dest);

            static void setOrthoProjection(int width, int height);
            static void setProjection(Mat4f mat);
            static void setViewport(int x, int y, int width, int height);

            static void setClippingRectangle(int x, int y, int width, int height);
            static void resetClippingRectangle();

            static void setFont(GLFont* font);
            static GLFont* getFont();

            static void drawSurface(float x1, float y1, float width, float height, GLSurface* s);
            static void drawTexture(float x1, float y1, float width, float height, GLTexture* tex);
            static void drawTexture(float x, float y, GLTexture* tex);

            static void drawTexturePart(Vec2f p1, Vec2f size, Vec2f uv1, Vec2f uv2, GLTexture* tex);
            
            static void drawSprite(float x1, float y1, float width, float height, GLSprite* sprite, int index);
            static void drawSprite(float x, float y, GLSprite* sprite, int index);

            static void drawText(StringBridge text, float x, float y, GLFont* fontPointer = nullptr);
            static void drawTextHighlighted(StringBridge text, float x, float y, int highlightStart, int highlightEnd, Vec4f highlightColor, GLFont* fontPointer = nullptr);
            static void drawTextLimits(StringBridge text, float x, float y, float maxWidth, float maxHeight, char wrapMode, GLFont* fontPointer = nullptr);
            static void drawTextLimitsHighlighted(StringBridge text, float x, float y, float maxWidth, float maxHeight, char wrapMode, int highlightStart, int highlightEnd, Vec4f highlightColor, GLFont* fontPointer = nullptr);


            static void drawRectangle(float x1, float y1, float width, float height, bool outline);
            
            static void drawCircle(float x, float y, float radius, bool outline);
            static void drawCircle(float x, float y, float radius, float maxDisFromEdge);

            static void drawEllipse(float x, float y, float xRadius, float yRadius, bool outline);
            static void drawEllipse(float x, float y, float xRadius, float yRadius, float innerRadiusDistance);
            static void drawEllipse(float x, float y, Vec2f dir1, Vec2f dir2, float innerRadiusDistance);

            static void drawTriangle(Vec2f p1, Vec2f p2, Vec2f p3, bool outline);

            static void drawLine(float x1, float y1, float x2, float y2);
            static void drawBezierCurve(BezierCurve* b, int subdivisions);
            
            /**
             * @brief Sets whether certain methods should using AntiAliasing.
             *      Note that this is not the same as any of the AA methods used before the fragment/pixel shader level
             *          such as MSAA nor is it similar to FXAA or any of those methods.
             *      Instead, it is applied in specific shader code for specific methods that support it such as circles or ellipses.
             * 
             * @return true 
             * @return false 
             */
            static bool setAntiAliasing();
        private:
            GLGraphics();
            ~GLGraphics();

            static Vec4f clearColor;
            static Vec4f drawColor;
            static Mat4f orthoMat;
            static Mat4f surfaceMat;

            static int viewportX;
            static int viewportY;
            static int viewportWidth;
            static int viewportHeight;

            //Note that the following shaders will be implemented in the resource folder
            //This may change to being literal strings
            static GLShader* textureShader;
            static GLShader* circleShader;
            static GLShader* ellipseShader;
            static GLShader* rectangleShader;
            static GLShader* textShader;
            static GLFont* activeFont;

            static GLFont* defaultFont;

            static GLModel drawModel;
            static bool hasInit;
            static GLGraphics singleton;

            static bool antiAlias;
        };

    }
#endif