#pragma once
#include<string>
#include<vector>
#include "Image.h"
#include "Graphics.h"


class GuiInstance : public Object
{
public:
	GuiInstance(std::string name = "", bool vis = false);
	~GuiInstance();

	//Object and Class Stuff
	const Class* getClass();
	static const Class* myClass;

	virtual void update();
	virtual void render(Image* surf);

	const void baseUpdate();

	void setOnActivateFunction(void (*func)(void));
	void setOnVisibleFunction(void (*func)(void));

	void setOffset(int* offX, int* offY);

protected:
	void setName(std::string name);
	std::string getName();
	void setVisible(bool is);
	bool getVisible();
	void setActive(bool is);
	bool getActive();

	int getOffsetX();
	int getOffsetY();

private:

	std::string name = "";
	bool visible = false;
	bool active = false;

	int* offX = nullptr;
	int* offY = nullptr;

	bool shouldCallA = false;
	bool shouldCallV = false;

	void (*onActivateFunction)(void);
	void (*onVisibleFunction)(void);
};

class GuiTextBox : public GuiInstance
{
public:
	GuiTextBox(int x, int y, int width, int height);
	~GuiTextBox();

	//Object and Class Stuff
	const Class* getClass();
	static const Class* myClass;

	void update();
	void render(Image* surf);

	void setOnEnterPressedFunction(void (*func)(void));
	void setOnKeyPressFunction(void (*func)(void));

	void setBackgroundColor(Color c);
	void setOutlineColor(Color c);
	void setActiveOutlineColor(Color c);

private:

	void (*onEnterPressedFunction)(void);
	void (*onKeyPressFunction)(void);

	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;

	Color backgroundColor = { 180, 180, 180, 255 };
	Color outlineColor = { 0, 0, 0, 255 };
	Color activeOutlineColor = { 0, 0, 255, 255 };

	std::string text = "";
};

class GuiRectangleButton : public GuiInstance
{
public:
	GuiRectangleButton(int x, int y, int width, int height);
	~GuiRectangleButton();

	//Object and Class Stuff
	const Class* getClass();
	static const Class* myClass;
	
	void update();
	void render(Image* surf);

	void setOnClickFunction(void (*func)(void));

	void setBackgroundColor(Color c);
	void setOutlineColor(Color c);
	void setActiveOutlineColor(Color c);

private:
	void (*onClickFunction)(void);

	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;

	Color backgroundColor = { 180, 180, 180, 255 };
	Color outlineColor = { 0, 0, 0, 255 };
	Color activeOutlineColor = { 0, 0, 255, 255 };

	std::string text = "";
};

class GuiCircleButton : public GuiInstance
{
public:
	GuiCircleButton(int x, int y, int radius);
	~GuiCircleButton();

	//Object and Class Stuff
	const Class* getClass();
	static const Class* myClass;

	void update();
	void render(Image* surf);

	void setOnClickFunction(void (*func)(void));

	void setBackgroundColor(Color c);
	void setOutlineColor(Color c);
	void setActiveOutlineColor(Color c);

private:
	void (*onClickFunction)(void);

	int x = 0;
	int y = 0;
	int radius = 0;

	Color backgroundColor = { 180, 180, 180, 255 };
	Color outlineColor = { 0, 0, 0, 255 };
	Color activeOutlineColor = { 0, 0, 255, 255 };

	std::string text = "";
};

class GuiManager : public Object
{
public:
	GuiManager(int* offsetX, int* offsetY);
	GuiManager(int* offsetX, int* offsetY, Image* surface);
	GuiManager(const GuiManager& k);
	~GuiManager();

	//Object and Class Stuff
	const Class* getClass();
	static const Class* myClass;

	void addElement(GuiInstance* k);
	void deleteElement(GuiInstance* k);

	void updateGuiElements();
	void renderGuiElements();

	void setImage(Image* surface);

	Image* getImage();

	size_t getSize();
private:
	std::vector<GuiInstance*> objects = std::vector<GuiInstance*>();
	Image* surf = nullptr;
	int* offX = new int;
	int* offY = new int;
};
