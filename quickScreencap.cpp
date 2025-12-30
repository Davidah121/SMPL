#include <windows.h>
#include <limits>

void emulateKeyPress(int key)
{
    INPUT i;
    ZeroMemory(&i, sizeof(INPUT));
    
    i.type = INPUT_KEYBOARD;
    i.ki.wVk = key;
    i.ki.dwFlags = NULL;

    uint32_t err = SendInput(1, &i, sizeof(INPUT));

}

void emulateKeyRelease(int key)
{
    INPUT i;
    ZeroMemory(&i, sizeof(INPUT));

    i.type = INPUT_KEYBOARD;
    i.ki.wVk = key;
    i.ki.dwFlags = KEYEVENTF_KEYUP;

    uint32_t err = SendInput(1, &i, sizeof(INPUT));

}

int main()
{
    bool down = false;
    while(true)
    {
        if((GetAsyncKeyState(VK_LCONTROL) & 0x8000) && (GetAsyncKeyState(VK_RCONTROL) & 0x8000))
        {
            if(down==false)
            {
                down = true;
                Sleep(100); //required due to GetAsyncKeyState for some odd reason. Don't feel like figuring it out. 
                // Perhaps my compiler and its work arounds for the windows API
                emulateKeyPress(VK_SNAPSHOT);
                emulateKeyRelease(VK_SNAPSHOT);
            }
        }
        else
        {
            down = false;
        }

        Sleep(10);
    }
    return 0;
}