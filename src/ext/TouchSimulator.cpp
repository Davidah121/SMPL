#include "ext/TouchSimulator.h"

#ifdef _WIN32

namespace smpl
{
    std::vector<POINTER_TOUCH_INFO> TouchSimulator::inputs = std::vector<POINTER_TOUCH_INFO>(TouchSimulator::INPUTS_ALLOWED); //10 inputs since 10 fingers
    std::vector<bool> TouchSimulator::updatePoints = std::vector<bool>(TouchSimulator::INPUTS_ALLOWED); //10 inputs since 10 fingers
    std::vector<bool> TouchSimulator::setToUp = std::vector<bool>(TouchSimulator::INPUTS_ALLOWED); //10 inputs since 10 fingers

    void TouchSimulator::init()
    {
        InitializeTouchInjection(TouchSimulator::INPUTS_ALLOWED, TOUCH_FEEDBACK_DEFAULT);
        for(int i=0; i<inputs.size(); i++)
        {
            memset(&inputs[i], 0, sizeof(POINTER_TOUCH_INFO)); //Zero Memory

            inputs[i].touchFlags = TOUCH_FLAG_NONE;
            inputs[i].touchMask = TOUCH_MASK_CONTACTAREA; //Could add pressure
            
            inputs[i].pointerInfo.pointerType = PT_TOUCH;
            inputs[i].pointerInfo.pointerId = i;
            inputs[i].pointerInfo.pointerFlags = POINTER_FLAG_UP;
            updatePoints[i] = false;
            setToUp[i] = false;
        }

    }

    void TouchSimulator::setDown(int inputID, int x, int y)
    {
        if(inputID >= 0 && inputID < TouchSimulator::INPUTS_ALLOWED)
        {
            inputs[inputID].pointerInfo.ptPixelLocation.x = x;
            inputs[inputID].pointerInfo.ptPixelLocation.y = y;
            inputs[inputID].rcContact.left = x-2;
            inputs[inputID].rcContact.right = x+2;
            inputs[inputID].rcContact.top = y-2;
            inputs[inputID].rcContact.bottom = y+2;

            if((inputs[inputID].pointerInfo.pointerFlags & POINTER_FLAG_UPDATE))
                inputs[inputID].pointerInfo.pointerFlags = POINTER_FLAG_UPDATE | POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT;
            else
                inputs[inputID].pointerInfo.pointerFlags = POINTER_FLAG_DOWN | POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT;
            
            updatePoints[inputID] = true;
            setToUp[inputID] = false;
        }
    }

    void TouchSimulator::setUp(int inputID)
    {
        if(inputID >= 0 && inputID < TouchSimulator::INPUTS_ALLOWED && !updatePoints[inputID])
        {
            setToUp[inputID] = true;
        }
    }

    void TouchSimulator::internalSetUp(int inputID)
    {
        if(inputID >= 0 && inputID < TouchSimulator::INPUTS_ALLOWED && setToUp[inputID] == true)
        {
            if(inputs[inputID].pointerInfo.pointerFlags != POINTER_FLAG_UP)
            {
                if(!(inputs[inputID].pointerInfo.pointerFlags & POINTER_FLAG_UPDATE))
                    inputs[inputID].pointerInfo.pointerFlags = POINTER_FLAG_CANCELED | POINTER_FLAG_UP;
                else
                    inputs[inputID].pointerInfo.pointerFlags = POINTER_FLAG_UP;
                updatePoints[inputID] = true;
            }
        }
    }

    bool TouchSimulator::inject()
    {
        for(int i=0; i<inputs.size(); i++)
        {
            internalSetUp(i);
        }
        //collect inputs that need to be updated
        std::vector<POINTER_TOUCH_INFO> validInputs;
        for(int i=0; i<inputs.size(); i++)
        {
            if(updatePoints[i])
                validInputs.push_back(inputs[i]);
        }

        if(validInputs.size() > 0)
        {
            BOOL err = InjectTouchInput(validInputs.size(), validInputs.data());
            
            if(err == FALSE)
            {
                if(GetLastError() == ERROR_NOT_READY)
                {
                    //retry once
                    err = InjectTouchInput(validInputs.size(), validInputs.data());
                }
            }

            //set all inputs with POINTER_DOWN to be POINTER_UPDATE
            for(int i=0; i<inputs.size(); i++)
            {
                if((inputs[i].pointerInfo.pointerFlags & POINTER_FLAG_DOWN))
                    inputs[i].pointerInfo.pointerFlags = POINTER_FLAG_UPDATE | POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT;
                
                updatePoints[i] = false;
            }

            if(err == FALSE)
            {
                //RESET
                for(int i=0; i<inputs.size(); i++)
                {
                    memset(&inputs[i], 0, sizeof(POINTER_TOUCH_INFO)); //Zero Memory

                    inputs[i].touchFlags = TOUCH_FLAG_NONE;
                    inputs[i].touchMask = TOUCH_MASK_CONTACTAREA; //Could add pressure
                    
                    inputs[i].pointerInfo.pointerType = PT_TOUCH;
                    inputs[i].pointerInfo.pointerId = i;
                    inputs[i].pointerInfo.pointerFlags = POINTER_FLAG_UP;
                    updatePoints[i] = false;
                    setToUp[i] = false;
                }
            }

            return err;
        }
        return true;
    }
};

#endif