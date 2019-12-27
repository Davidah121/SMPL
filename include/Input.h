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

	static int getMouseX();
	static int getMouseY();

	static bool getKeyChanged();
	static bool getMouseMoved();

	const static int LEFT_MOUSE_BUTTON = 0;
	const static int MIDDLE_MOUSE_BUTTON = 1;
	const static int RIGHT_MOUSE_BUTTON = 2;

private:
	static bool* keyState;
	static bool* mouseState;

	static bool* preKeyState;
	static bool* preMouseState;

	static int mouseX;
	static int mouseY;

	static bool keyChanged;
	static bool mouseMoved;
	static bool mouseClicked;
};

