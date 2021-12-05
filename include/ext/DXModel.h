#pragma once

#include "MathExt.h"
#include "ext/DXSingleton.h"
#include "SimpleFile.h"

namespace glib
{

    class DXModel
    {
    public:
        DXModel();
        ~DXModel();

        void draw();

    private:
        ID3D11Buffer* modelData = nullptr;
    };

}