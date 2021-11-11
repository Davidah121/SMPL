#include "ext/GLGraphics.h"

namespace glib
{
    Vec4f GLGraphics::clearColor = Vec4f();

    void GLGraphics::clear(int clearCodes)
    {
        glClear(clearCodes);
    }
    
    void GLGraphics::setClearColor(Vec4f color)
    {
        clearColor = color;
        glClearColor((float)color.x, (float)color.y, (float)color.z, (float)color.w);
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

    void GLGraphics::setFaceCullingType(int type)
    {
        glCullFace(type);
    }

}