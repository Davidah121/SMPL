#pragma once
#include <Windows.h>

namespace glib
{
    class MouseSimulator
    {
    public:
        
		static const int MOUSE_WHEEL_UP = 0;
		static const int MOUSE_WHEEL_DOWN = 1;
		static const int MOUSE_WHEEL_LEFT = 2;
		static const int MOUSE_WHEEL_RIGHT = 3;

		/**
		 * @brief Emulates a mouse press.
		 * 		The button is considered down until it is released.
		 * 		May not work in other OS.
		 * 
		 * @param button 
		 */
		static void emulateMousePress(int button, int x=-1, int y=-1);

		/**
		 * @brief Emulates a mouse release.
		 * 		May not work in other OS.
		 * 
		 * @param key 
		 */
		static void emulateMouseRelease(int button, int x=-1, int y=-1);

		/**
		 * @brief Emulates the mouse wheel moving.
		 * 		
		 * @param wheel 
		 * 		Valid values are:
		 * 			MOUSE_WHEEL_UP
		 * 			MOUSE_WHEEL_DOWN
		 * 			MOUSE_WHEEL_LEFT
		 * 			MOUSE_WHEEL_RIGHT
		 * @param amount 
		 * 		Any positive or negative value.
		 */
		static void emulateMouseWheel(int wheel, int amount);

		/**
		 * @brief Sets the Mouse Position
		 * 
		 * @param x 
		 * @param y 
		 */
		static void setMousePosition(int x, int y);

    private:
    };
}