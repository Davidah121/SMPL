#pragma once

namespace glib
{

	class Input
	{
	public:

		static const unsigned char KEY_BACKSPACE = 0x08;
		static const unsigned char KEY_TAB = 0x09;
		static const unsigned char KEY_ENTER = 0x0D;
		static const unsigned char KEY_SHIFT = 0x10;
		static const unsigned char KEY_CONTROL = 0x11;
		static const unsigned char KEY_ALT = 0x12;
		static const unsigned char KEY_PAUSE = 0x13;
		static const unsigned char KEY_CAPSLOCK = 0x14;
		static const unsigned char KEY_ESCAPE = 0x1B;
		static const unsigned char KEY_SPACE = 0x20;
		static const unsigned char KEY_PAGEUP = 0x21;
		static const unsigned char KEY_PAGEDOWN = 0x22;

		static const unsigned char KEY_END= 0x23;
		static const unsigned char KEY_HOME = 0x24;
		static const unsigned char KEY_LEFT = 0x25;
		static const unsigned char KEY_UP = 0x26;
		static const unsigned char KEY_RIGHT = 0x27;
		static const unsigned char KEY_DOWN = 0x28;
		static const unsigned char KEY_PRINTSCREEN = 0x2C;
		static const unsigned char KEY_INSERT = 0x2D;
		static const unsigned char KEY_DELETE = 0x2E;

		static const unsigned char KEY_NUMPAD0 = 0x60;
		static const unsigned char KEY_NUMPAD1 = 0x61;
		static const unsigned char KEY_NUMPAD2 = 0x62;
		static const unsigned char KEY_NUMPAD3 = 0x63;
		static const unsigned char KEY_NUMPAD4 = 0x64;
		static const unsigned char KEY_NUMPAD5 = 0x65;
		static const unsigned char KEY_NUMPAD6 = 0x66;
		static const unsigned char KEY_NUMPAD7 = 0x67;
		static const unsigned char KEY_NUMPAD8 = 0x68;
		static const unsigned char KEY_NUMPAD9 = 0x69;

		static const unsigned char KEY_NUMPAD_MULTIPLY = 0x6A;
		static const unsigned char KEY_NUMPAD_ADD = 0x6B;
		static const unsigned char KEY_NUMPAD_ENTER = 0x6C;
		static const unsigned char KEY_NUMPAD_SUBTRACT = 0x6D;
		static const unsigned char KEY_NUMPAD_DECIMAL = 0x6E;
		static const unsigned char KEY_NUMPAD_DIVIDE = 0x6F;

		static const unsigned char KEY_F1 = 0x70;
		static const unsigned char KEY_F2 = 0x71;
		static const unsigned char KEY_F3 = 0x72;
		static const unsigned char KEY_F4 = 0x73;
		static const unsigned char KEY_F5 = 0x74;
		static const unsigned char KEY_F6 = 0x75;
		static const unsigned char KEY_F7 = 0x76;
		static const unsigned char KEY_F8 = 0x77;
		static const unsigned char KEY_F9 = 0x78;
		static const unsigned char KEY_F10 = 0x79;
		static const unsigned char KEY_F11 = 0x7A;
		static const unsigned char KEY_F12 = 0x7B;
		static const unsigned char KEY_F13 = 0x7C;
		static const unsigned char KEY_F14 = 0x7D;
		static const unsigned char KEY_F15 = 0x7E;
		static const unsigned char KEY_F16 = 0x7F;
		static const unsigned char KEY_F17 = 0x80;
		static const unsigned char KEY_F18 = 0x81;
		static const unsigned char KEY_F19 = 0x82;
		static const unsigned char KEY_F20 = 0x83;
		static const unsigned char KEY_F21 = 0x84;
		static const unsigned char KEY_F22 = 0x85;
		static const unsigned char KEY_F23 = 0x86;
		static const unsigned char KEY_F24 = 0x87;

		static const unsigned char KEY_NUMLOCK = 0x90;
		static const unsigned char KEY_SCROLL_LOCK = 0x91;
		
		static const unsigned char KEY_LSHIFT = 0xA0;
		static const unsigned char KEY_RSHIFT = 0xA1;
		static const unsigned char KEY_LCONTROL = 0xA2;
		static const unsigned char KEY_RCONTROL = 0xA3;

		static const unsigned char KEY_PLUS = 0xBB;
		static const unsigned char KEY_COMMA = 0xBC;
		static const unsigned char KEY_MINUS = 0xBD;
		static const unsigned char KEY_PERIOD = 0xBE;

		static const unsigned char KEY_FORWARD_SLASH = 0xBF;
		static const unsigned char KEY_TILDE = 0xC0;
		static const unsigned char KEY_SEMICOLON = 0xBA;
		static const unsigned char KEY_LEFT_SQUARE_BRACKET = 0xDB;
		static const unsigned char KEY_BACK_SLASH = 0xDC;
		static const unsigned char KEY_RIGHT_SQUARE_BRACKET = 0xDD;
		static const unsigned char KEY_SINGLE_QUOTE = 0xDE;
		
		/**
		 * @brief Polls input from the the Keyboard and Mouse using the OS tools.
		 * 		Note that mouse scroll wheel uses the currently active window to update.
		 * 		This may be changed.
		 * 		pollInput should be called to update the status of each key.
		 */
		static void pollInput();

		/**
		 * @brief Returns whether the specified key has just been pressed.
		 * 		Last until the next pollInput is called even if the key is being held down.
		 * 
		 * @param k 
		 * 		The key the check. Use either the constants or the uppercase letters and numbers
		 * 		to reference a key.
		 * @return true 
		 * @return false 
		 */
		static bool getKeyPressed(unsigned char k);

		/**
		 * @brief Returns whether the specified key is being held down.
		 * 		Last as long as the key is held down.
		 * 
		 * @param k 
		 * 		The key the check. Use either the constants or the uppercase letters and numbers
		 * 		to reference a key.
		 * @return true 
		 * @return false 
		 */
		static bool getKeyDown(unsigned char k);

		/**
		 * @brief Returns whether the specified key has just been released.
		 * 		Last until the next pollInput is called.
		 * 
		 * @param k 
		 * 		The key the check. Use either the constants or the uppercase letters and numbers
		 * 		to reference a key.
		 * @return true 
		 * @return false 
		 */
		static bool getKeyUp(unsigned char k);

		/**
		 * @brief Returns whether the specified mouse button has just been pressed.
		 * 		Last until the next pollInput is called even if the button is still being held down.
		 * 
		 * @param v
		 * 		The mouse button to check. Use the constants to access the correct button.
		 * @return true 
		 * @return false 
		 */
		static bool getMousePressed(int v);

		/**
		 * @brief Returns whether the specified mouse button is being held down.
		 * 		Last as long as the button is being held down.
		 * 
		 * @param v
		 * 		The mouse button to check. Use the constants to access the correct button.
		 * @return true 
		 * @return false 
		 */
		static bool getMouseDown(int v);

		/**
		 * @brief Returns whether the specified mouse button has just been released.
		 * 		Last until the next pollInput is called.
		 * 
		 * @param v
		 * 		The mouse button to check. Use the constants to access the correct button.
		 * @return true 
		 * @return false 
		 */
		static bool getMouseUp(int v);

		/**
		 * @brief Returns the mouse scroll wheel value in the vertical direction.
		 * 		Can return a positive or negative value which indicates the intensity and direction.
		 * 		Requires an active window to be in use.
		 * 
		 * @return int 
		 */
		static int getMouseScrollVertical();

		/**
		 * @brief Returns the mouse scroll wheel value in the horizontal direction.
		 * 		Can return a positive or negative value which indicates the intensity and direction.
		 * 		Requires an active window to be in use.
		 * 
		 * @return int 
		 */
		static int getMouseScrollHorizontal();

		/**
		 * @brief Gets the mouse x position.
		 * 
		 * @return int 
		 */
		static int getMouseX();

		/**
		 * @brief Gets the mouse y position.
		 * 
		 * @return int 
		 */
		static int getMouseY();

		/**
		 * @brief Gets whether a key state has changed from the previous poll input.
		 * 
		 * @return true 
		 * @return false 
		 */
		static bool getKeyChanged();

		/**
		 * @brief Gets whether the mouse has moved from the previous poll input.
		 * 
		 * @return true 
		 * @return false 
		 */
		static bool getMouseMoved();

		/**
		 * @brief Gets the last key that was pressed.
		 * 
		 * @return int 
		 */
		static int getLastKeyPressed();

		/**
		 * @brief Gets the last key that was held down.
		 * 
		 * @return int 
		 */
		static int getLastKeyDown();

		/**
		 * @brief Gets the last key that was released.
		 * 
		 * @return int 
		 */
		static int getLastKeyReleased();

		const static int LEFT_MOUSE_BUTTON = 0x00;
		const static int MIDDLE_MOUSE_BUTTON = 0x01;
		const static int RIGHT_MOUSE_BUTTON = 0x02;

	private:
		static bool keyState[256];
		static bool mouseState[3];

		static bool preKeyState[256];
		static bool preMouseState[3];
		
		static int verticalScrollValue;
		static int horizontalScrollValue;

		static int nVerticalScrollValue;
		static int nHorizontalScrollValue;

		static int mouseX;
		static int mouseY;

		static bool keyChanged;
		static bool mouseMoved;
		static bool mouseClicked;

		static int lastKeyPressed;
		static int lastKeyDown;
		static int lastKeyUp;
	};

}