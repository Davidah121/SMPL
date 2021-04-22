#pragma once
class Input
{
public:
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

	const static int LEFT_MOUSE_BUTTON = 0;
	const static int MIDDLE_MOUSE_BUTTON = 1;
	const static int RIGHT_MOUSE_BUTTON = 2;

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

