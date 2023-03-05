#pragma once
#include <Windows.h>
#include <vector>

namespace glib
{
    class TouchSimulator
    {
    public:
        static const int INPUTS_ALLOWED = 10;
        static const int L_THUMB = 0;
        static const int L_INDEX = 1;
        static const int L_MIDDLE = 2;
        static const int L_RING = 3;
        static const int L_PINKY = 4;
        static const int R_THUMB = 5;
        static const int R_INDEX = 6;
        static const int R_MIDDLE = 7;
        static const int R_RING = 8;
        static const int R_PINKY = 9;
        
        static void init();
        static void injectDown(int inputID, int x, int y);
        static void injectUpdate(int inputID, int x, int y);
        static void injectUp(int inputID, int x, int y);
        
    private:
        static std::vector<POINTER_TOUCH_INFO> inputs;
    };
}