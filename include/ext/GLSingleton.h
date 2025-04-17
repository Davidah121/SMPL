#pragma once

#ifdef USE_OPENGL
    #include "TargetVer.h"

    #include <GL/glew.h>

    #include <GL/gl.h>
    #include <GL/glu.h>

    #ifdef __unix__

    #else
        #include <GL/wglew.h> //Windows extension
    #endif



class GLSingleton
{
public:

    static bool getInit()
    {
        return hasInit;
    }

    static void tryInit()
    {
        init();
    }

private:

    //should throw an error
    static void init()
    {
        if(!hasInit)
        {
            GLuint err = glewInit();
            if(err != GLEW_OK)
            {
                hasInit = false;
                return;
            }
            hasInit = true;
        }
    }

    static bool hasInit;
};

inline bool GLSingleton::hasInit = false;

#endif