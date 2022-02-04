#pragma once
#include "MathExt.h"
#include "ext/DXSingleton.h"
#include "ext/DXWindow.h"
#include "ext/DXTexture.h"
#include "ext/DXSurface.h"
#include "ext/DXShader.h"
#include "ext/DXModel.h"
#include "ext/DXSprite.h"
#include "ext/DXFont.h"


namespace glib
{

    class DXGraphics
    {
    public:
        static const int COLOR_BUFFER = 0b0001;
        static const int DEPTH_BUFFER = 0b0010;
        static const int STENCIL_BUFFER = 0b0100;

        static const int DEPTH_LESS = D3D11_COMPARISON_LESS;
        static const int DEPTH_LEQUAL = D3D11_COMPARISON_LESS_EQUAL;

        static const int DEPTH_GREATER = D3D11_COMPARISON_GREATER;
        static const int DEPTH_GEQUAL = D3D11_COMPARISON_GREATER_EQUAL;

        static const int DEPTH_EQUAL= D3D11_COMPARISON_EQUAL;
        static const int DEPTH_NEQUAL= D3D11_COMPARISON_NOT_EQUAL;

        static const int DEPTH_NEVER= D3D11_COMPARISON_NEVER;
        static const int DEPTH_ALWAYS = D3D11_COMPARISON_ALWAYS;

        static const int CULL_FRONT = D3D11_CULL_FRONT;
        static const int CULL_BACK = D3D11_CULL_BACK;

        static const bool CULL_COUNTER_CLOCKWISE = true;
        static const bool CULL_CLOCKWISE = false;
        
        
        static void init();

        static void clear(int clearCodes);
        static void setClearColor(Vec4f color);
        static Vec4f getClearColor();

        static void setDrawColor(Vec4f color);
        static Vec4f getDrawColor();

        static void enableDepthTest();
        static void disableDepthTest();

        static void setDepthTestFunction(int type);

        static void enableFaceCulling(int type);
        static void disableFaceCulling();

        static void enableBlending();
        static void disableBlending();
        static void setBlendFunction(int src, int dest);

        static void setOrthoProjection(int width, int height);
        static void setOrthoProjection(Mat4f mat);

        static void setFont(DXFont* font);
        static DXFont* getFont();

        static void drawSurface(double x1, double y1, double x2, double y2, DXSurface* s);
        static void drawTexture(double x1, double y1, double x2, double y2, DXTexture* tex);
        static void drawTexture(double x, double y, DXTexture* tex);
        
        static void drawSprite(double x1, double y1, double x2, double y2, DXSprite* sprite, int index);
        static void drawSprite(double x, double y, DXSprite* sprite, int index);

        static void drawText(std::string text, double x, double y, DXFont* fontPointer = nullptr);
        static void drawText(std::wstring text, double x, double y, DXFont* fontPointer = nullptr);

        static void drawTextLimits(std::string text, double x, double y, double maxWidth, double maxHeight, bool useLineBreaks, DXFont* fontPointer = nullptr);
        static void drawTextLimits(std::wstring text, double x, double y, double maxWidth, double maxHeight, bool useLineBreaks, DXFont* fontPointer = nullptr);

        static void drawRectangle(double x1, double y1, double x2, double y2, bool outline);
        static void drawCircle(double x, double y, double radius);
        static void drawTriangle(Vec2f p1, Vec2f p2, Vec2f p3, bool outline);

        static void drawLine(double x1, double y1, double x2, double y2);
        static void drawBezierCurve(BezierCurve* b, int subdivisions);
        
        
    private:
        DXGraphics();
        ~DXGraphics();

        static void createDepthStencilState();
        static void createRasterState();
        static void createBlendState();

        static Vec4f clearColor;
        static Vec4f drawColor;
        static Mat4f orthoMat;

        //Note that the following shaders will be implemented in the resource folder
        //This may change to being literal strings
        static DXShader* textureShader;
        static DXShader* circleShader;
        static DXShader* rectangleShader;
        static DXShader* textShader;
        static DXFont* activeFont;

        static DXModel drawModel;

        static DXGraphics singleton;

        static D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
        static D3D11_RASTERIZER_DESC rasterDesc;
        static D3D11_BLEND_DESC blendDesc;
    };

}