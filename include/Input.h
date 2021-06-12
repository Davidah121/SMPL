#pragma once

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

	static const unsigned char KEY_MULTIPLY = 0x6A;
	static const unsigned char KEY_ADD = 0x6B;
	static const unsigned char KEY_SEPARATOR = 0x6C;
	static const unsigned char KEY_SUBTRACT = 0x6D;
	static const unsigned char KEY_DECIMAL = 0x6E;
	static const unsigned char KEY_DIVIDE = 0x6F;

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
	
	static void pollInput();

	static bool getKeyPressed(unsigned char k);
	static bool getKeyDown(unsigned char k);
	static bool getKeyUp(unsigned char k);

	static bool getMousePressed(int v);
	static bool getMouseDown(int v);
	static bool getMouseUp(int v);

	static int getMouseScrollVertical();
	static int getMouseScrollHorizontal();

	static int getMouseX();
	static int getMouseY();

	static bool getKeyChanged();
	static bool getMouseMoved();

	static int getLastKeyPressed();
	static int getLastKeyDown();
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

