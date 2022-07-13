#pragma once
#include <mutex>
#include <queue>

namespace glib
{

	class Input
	{
	public:
		static const int NEGATIVE = 0x80000000;
		static const unsigned int KEY_BACKSPACE = 0x08 | NEGATIVE;
		static const unsigned int KEY_TAB = 0x09 | NEGATIVE;
		static const unsigned int KEY_ENTER = 0x0D | NEGATIVE;
		static const unsigned int KEY_SHIFT = 0x10 | NEGATIVE;
		static const unsigned int KEY_CONTROL = 0x11 | NEGATIVE;
		static const unsigned int KEY_ALT = 0x12 | NEGATIVE;
		static const unsigned int KEY_PAUSE = 0x13 | NEGATIVE;
		static const unsigned int KEY_CAPSLOCK = 0x14 | NEGATIVE;
		static const unsigned int KEY_ESCAPE = 0x1B | NEGATIVE;
		static const unsigned int KEY_SPACE = 0x20 | NEGATIVE;
		static const unsigned int KEY_PAGEUP = 0x21 | NEGATIVE;
		static const unsigned int KEY_PAGEDOWN = 0x22 | NEGATIVE;

		static const unsigned int KEY_END= 0x23 | NEGATIVE;
		static const unsigned int KEY_HOME = 0x24 | NEGATIVE;
		static const unsigned int KEY_LEFT = 0x25 | NEGATIVE;
		static const unsigned int KEY_UP = 0x26 | NEGATIVE;
		static const unsigned int KEY_RIGHT = 0x27 | NEGATIVE;
		static const unsigned int KEY_DOWN = 0x28 | NEGATIVE;
		static const unsigned int KEY_PRINTSCREEN = 0x2C | NEGATIVE;
		static const unsigned int KEY_INSERT = 0x2D | NEGATIVE;
		static const unsigned int KEY_DELETE = 0x2E | NEGATIVE;

		static const unsigned int KEY_NUMPAD0 = 0x60 | NEGATIVE;
		static const unsigned int KEY_NUMPAD1 = 0x61 | NEGATIVE;
		static const unsigned int KEY_NUMPAD2 = 0x62 | NEGATIVE;
		static const unsigned int KEY_NUMPAD3 = 0x63 | NEGATIVE;
		static const unsigned int KEY_NUMPAD4 = 0x64 | NEGATIVE;
		static const unsigned int KEY_NUMPAD5 = 0x65 | NEGATIVE;
		static const unsigned int KEY_NUMPAD6 = 0x66 | NEGATIVE;
		static const unsigned int KEY_NUMPAD7 = 0x67 | NEGATIVE;
		static const unsigned int KEY_NUMPAD8 = 0x68 | NEGATIVE;
		static const unsigned int KEY_NUMPAD9 = 0x69 | NEGATIVE;

		static const unsigned int KEY_NUMPAD_MULTIPLY = 0x6A | NEGATIVE;
		static const unsigned int KEY_NUMPAD_ADD = 0x6B | NEGATIVE;
		static const unsigned int KEY_NUMPAD_ENTER = 0x6C | NEGATIVE;
		static const unsigned int KEY_NUMPAD_SUBTRACT = 0x6D | NEGATIVE;
		static const unsigned int KEY_NUMPAD_DECIMAL = 0x6E | NEGATIVE;
		static const unsigned int KEY_NUMPAD_DIVIDE = 0x6F | NEGATIVE;

		static const unsigned int KEY_F1 = 0x70 | NEGATIVE;
		static const unsigned int KEY_F2 = 0x71 | NEGATIVE;
		static const unsigned int KEY_F3 = 0x72 | NEGATIVE;
		static const unsigned int KEY_F4 = 0x73 | NEGATIVE;
		static const unsigned int KEY_F5 = 0x74 | NEGATIVE;
		static const unsigned int KEY_F6 = 0x75 | NEGATIVE;
		static const unsigned int KEY_F7 = 0x76 | NEGATIVE;
		static const unsigned int KEY_F8 = 0x77 | NEGATIVE;
		static const unsigned int KEY_F9 = 0x78 | NEGATIVE;
		static const unsigned int KEY_F10 = 0x79 | NEGATIVE;
		static const unsigned int KEY_F11 = 0x7A | NEGATIVE;
		static const unsigned int KEY_F12 = 0x7B | NEGATIVE;
		static const unsigned int KEY_F13 = 0x7C | NEGATIVE;
		static const unsigned int KEY_F14 = 0x7D | NEGATIVE;
		static const unsigned int KEY_F15 = 0x7E | NEGATIVE;
		static const unsigned int KEY_F16 = 0x7F | NEGATIVE;
		static const unsigned int KEY_F17 = 0x80 | NEGATIVE;
		static const unsigned int KEY_F18 = 0x81 | NEGATIVE;
		static const unsigned int KEY_F19 = 0x82 | NEGATIVE;
		static const unsigned int KEY_F20 = 0x83 | NEGATIVE;
		static const unsigned int KEY_F21 = 0x84 | NEGATIVE;
		static const unsigned int KEY_F22 = 0x85 | NEGATIVE;
		static const unsigned int KEY_F23 = 0x86 | NEGATIVE;
		static const unsigned int KEY_F24 = 0x87 | NEGATIVE;

		static const unsigned int KEY_NUMLOCK = 0x90 | NEGATIVE;
		static const unsigned int KEY_SCROLL_LOCK = 0x91 | NEGATIVE;
		
		static const unsigned int KEY_LSHIFT = 0xA0 | NEGATIVE;
		static const unsigned int KEY_RSHIFT = 0xA1 | NEGATIVE;
		static const unsigned int KEY_LCONTROL = 0xA2 | NEGATIVE;
		static const unsigned int KEY_RCONTROL = 0xA3 | NEGATIVE;

		static const unsigned int KEY_PLUS = 0xBB | NEGATIVE;
		static const unsigned int KEY_COMMA = 0xBC | NEGATIVE;
		static const unsigned int KEY_MINUS = 0xBD | NEGATIVE;
		static const unsigned int KEY_PERIOD = 0xBE | NEGATIVE;

		static const unsigned int KEY_FORWARD_SLASH = 0xBF | NEGATIVE;
		static const unsigned int KEY_TILDE = 0xC0 | NEGATIVE;
		static const unsigned int KEY_SEMICOLON = 0xBA | NEGATIVE;
		static const unsigned int KEY_LEFT_SQUARE_BRACKET = 0xDB | NEGATIVE;
		static const unsigned int KEY_BACK_SLASH = 0xDC | NEGATIVE;
		static const unsigned int KEY_RIGHT_SQUARE_BRACKET = 0xDD | NEGATIVE;
		static const unsigned int KEY_SINGLE_QUOTE = 0xDE | NEGATIVE;
		
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
		static bool getKeyPressed(int k);

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
		static bool getKeyDown(int k);

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
		static bool getKeyUp(int k);

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

		/**
		 * @brief Force clears the characters in the buffer.
		 * 		These are the character received from the active window.
		 * 		The character buffer is normalily cleared after it is read at least once and pollInput has been called.
		 * 
		 */
		void clearCharactersTyped();

		/**
		 * @brief Gets all the Characters Typed left in the buffer.
		 * 		Does not clear the buffer. The buffer is cleared after the pollInput() function is called
		 * 			and the character buffer has been read once.
		 * 
		 * @return std::queue<int> 
		 */
		static std::queue<int> getCharactersTyped();

		static void adjustVerticalScroll(int v);
		static void adjustHorizontalScroll(int v);
		static void adjustCurrCharVal(unsigned int v1, unsigned int v2);
		

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

		static bool canClearBuffer;
		static std::queue<int> charBuffer;
		static std::mutex inputMutex;
	};

}