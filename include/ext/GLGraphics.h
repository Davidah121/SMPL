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


    namespace glib
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

            static void drawSurface(double x1, double y1, double x2, double y2, GLSurface* s);
            static void drawTexture(double x1, double y1, double x2, double y2, GLTexture* tex);
            static void drawTexture(double x, double y, GLTexture* tex);

            static void drawTexturePart(Vec4f positionData, Vec4f textureData, GLTexture* tex);
            
            static void drawSprite(double x1, double y1, double x2, double y2, GLSprite* sprite, int index);
            static void drawSprite(double x, double y, GLSprite* sprite, int index);

            static void drawText(std::string text, double x, double y, GLFont* fontPointer = nullptr);
            static void drawText(std::wstring text, double x, double y, GLFont* fontPointer = nullptr);

            static void drawTextLimits(std::string text, double x, double y, double maxWidth, double maxHeight, bool useLineBreaks, GLFont* fontPointer = nullptr);
            static void drawTextLimits(std::wstring text, double x, double y, double maxWidth, double maxHeight, bool useLineBreaks, GLFont* fontPointer = nullptr);

            static void drawTextLimitsHighlighted(std::string text, double x, double y, double maxWidth, double maxHeight, bool useLineBreaks, int highlightStart, int highlightEnd, Vec4f highlightColor, GLFont* fontPointer = nullptr);
            static void drawTextLimitsHighlighted(std::wstring text, double x, double y, double maxWidth, double maxHeight, bool useLineBreaks, int highlightStart, int highlightEnd, Vec4f highlightColor, GLFont* fontPointer = nullptr);

            static void drawRectangle(double x1, double y1, double x2, double y2, bool outline);
            
            static void drawCircle(double x, double y, double radius, bool outline);
            static void drawCircle(double x, double y, double innerRadius, double outerRadius);

            static void drawEllipse(double x, double y, double xRadius, double yRadius, bool outline);
            static void drawEllipse(double x, double y, double xRadius, double yRadius, double innerRadiusDistance);

            static void drawTriangle(Vec2f p1, Vec2f p2, Vec2f p3, bool outline);

            static void drawLine(double x1, double y1, double x2, double y2);
            static void drawBezierCurve(BezierCurve* b, int subdivisions);
            
            
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
            static GLShader* rectangleShader;
            static GLShader* textShader;
            static GLFont* activeFont;

            static GLModel drawModel;
            static bool hasInit;
            static GLGraphics singleton;
        };

    }
#endif