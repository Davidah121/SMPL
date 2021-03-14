#pragma once
#include<string>
#include<vector>
#include "VectorGraphic.h"
#include "Image.h"
#include "Graphics.h"
#include "Shape.h"
#include "Sprite.h"


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

	void addChild(GuiInstance* obj);
	std::vector<GuiInstance*> getChildren();

	const void baseUpdate();

	void setOnActivateFunction(void (*func)(void));
	void setOnVisibleFunction(void (*func)(void));

	void setOffset(int* offX, int* offY);

	int getPriority();

	void setBaseX(int x);
	int getBaseX();

	void setBaseY(int y);
	int getBaseY();

	int getX();
	int getY();

protected:
	void setName(std::string name);
	std::string getName();
	void setVisible(bool is);
	bool getVisible();
	void setActive(bool is);
	bool getActive();

	int getOffsetX();
	int getOffsetY();

	int baseX = 0;
	int baseY = 0;

	int x = 0;
	int y = 0;

	friend class GuiManager;

	void setManager(GuiManager* m);
	GuiManager* getManager();
private:

	int priority = 0;
	std::string name = "";
	bool visible = false;
	bool active = false;

	int* offX = nullptr;
	int* offY = nullptr;
	
	bool shouldCallA = false;
	bool shouldCallV = false;

	void (*onActivateFunction)(void) = nullptr;
	void (*onVisibleFunction)(void) = nullptr;

	std::vector<GuiInstance*> children = std::vector<GuiInstance*>();
	GuiManager* manager = nullptr;
};

class GuiContainer : public GuiInstance
{
public:
	GuiContainer();
	~GuiContainer();

	//Object and Class Stuff
	const Class* getClass();
	static const Class* myClass;
	
	void update();
	void render(Image* surf);

private:
};

class GuiImage : public GuiInstance
{
public:
	GuiImage(Image* img = nullptr);
	~GuiImage();

	//Object and Class Stuff
	const Class* getClass();
	static const Class* myClass;
	
	void update();
	void render(Image* surf);

	void setImage(Image* img);
	Image* getImage();

private:
	Image* img;
};

class GuiSprite : public GuiInstance
{
public:
	GuiSprite();
	~GuiSprite();

	//Object and Class Stuff
	const Class* getClass();
	static const Class* myClass;
	
	void update();
	void render(Image* surf);

	void setSprite(Sprite img);
	Sprite& getSprite();

	void setAnimationTimer(int framesPerSecond);
	int getAnimationTimer();

	void setLoop(bool loop);
	bool getLoop();

	void setColor(Color c);
	Color getColor();

private:
	int timer = 0;
	int index = 0;
	int fps = 0;
	bool shouldLoop = false;
	Sprite img;
	Color imgColor = {255,255,255,255};
};

class GuiTextBlock : public GuiInstance
{
public:
	GuiTextBlock(int x, int y, int width, int height);
	~GuiTextBlock();

	//Object and Class Stuff
	const Class* getClass();
	static const Class* myClass;

	void update();
	void render(Image* surf);

	void setTextColor(Color c);
	Color getTextColor();

	std::string getText();
	std::string& getTextRef();
	void setText(std::string s);

	void setFont(Font* f);
	Font* getFont();

	void setWidth(int v);
	void setHeight(int v);

	int getWidth();
	int getHeight();

private:

	int width = 0;
	int height = 0;

	Color textColor = { 0, 0, 0, 255 };
	Font* textFont = nullptr;

	std::string text = "";
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

	GuiTextBlock* getTextBlockElement();

	void setWidth(int v);
	void setHeight(int v);

	int getWidth();
	int getHeight();
private:

	void (*onEnterPressedFunction)(void) = nullptr;
	void (*onKeyPressFunction)(void) = nullptr;

	int width = 0;
	int height = 0;

	Color backgroundColor = { 180, 180, 180, 255 };
	Color outlineColor = { 0, 0, 0, 255 };
	Color activeOutlineColor = { 0, 0, 255, 255 };

	GuiTextBlock textElement = GuiTextBlock(0,0,0,0);
};

class GuiShape : public GuiInstance
{
public:
	GuiShape(int x, int y, VectorGraphic* svg);
	~GuiShape();

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
	
	void (*onClickFunction)(void) = nullptr;

	CombinationShape* collisionMap = nullptr;
	VectorGraphic* shape = nullptr;

	Color backgroundColor = { 180, 180, 180, 255 };
	Color outlineColor = { 0, 0, 0, 255 };
	Color activeOutlineColor = { 0, 0, 255, 255 };
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

	void setWidth(int v);
	void setHeight(int v);

	int getWidth();
	int getHeight();

private:
	void (*onClickFunction)(void) = nullptr;

	int width = 0;
	int height = 0;

	Color backgroundColor = { 180, 180, 180, 255 };
	Color outlineColor = { 0, 0, 0, 255 };
	Color activeOutlineColor = { 0, 0, 255, 255 };
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

	void setRadius(int v);
	int getRadius();

private:
	void (*onClickFunction)(void) = nullptr;

	int radius = 0;

	Color backgroundColor = { 180, 180, 180, 255 };
	Color outlineColor = { 0, 0, 0, 255 };
	Color activeOutlineColor = { 0, 0, 255, 255 };
};

class GuiManager : public Object
{
public:
	GuiManager();
	GuiManager(Image* surface, bool ownership);
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

	void resizeImage(int width, int height);

	void setWindowX(int v);
	void setWindowY(int v);

	int getWindowX();
	int getWindowY();

private:
	void sortElements();

	std::vector<GuiInstance*> objects = std::vector<GuiInstance*>();
	bool ownsImage = false;
	Image* surf = nullptr;
	int windowX = 0;
	int windowY = 0;
};
