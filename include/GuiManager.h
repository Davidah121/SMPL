#pragma once
#include<string>
#include<vector>
#include "VectorGraphic.h"
#include "Image.h"
#include "Graphics.h"
#include "Shape.h"
#include "Sprite.h"

#include <functional>

//Should use SmartPointers for memory access to avoid accessing memory that has already been deleted.

class GuiInstance : public Object
{
public:
	GuiInstance();
	GuiInstance(const GuiInstance& other);
	void operator=(const GuiInstance& other);
	void copy(const GuiInstance& other);
	~GuiInstance();

	//Object and Class Stuff
	const Class* getClass();
	static const Class myClass;

	virtual void update();
	virtual void render(Image* surf);

	void addChild(GuiInstance* obj);
	void removeChild(GuiInstance* obj);
	std::vector<GuiInstance*> getChildren();

	const void baseUpdate();
	const void baseRender();

	void setOnActivateFunction(std::function<void(GuiInstance*)> func);
	void setOnVisibleFunction(std::function<void(GuiInstance*)> func);

	void setOnDeActivateFunction(std::function<void(GuiInstance*)> func);
	void setOnInvisibleFunction(std::function<void(GuiInstance*)> func);

	void setOnChangedFunction(std::function<void(GuiInstance*)> func);
	void setOnFocusFunction(std::function<void(GuiInstance*)> func);

	void setOffset(int* offX, int* offY);
	void setRenderOffset(int* offX, int* offY);

	int getPriority();

	void setBaseX(int x);
	int getBaseX();

	void setBaseY(int y);
	int getBaseY();

	int getX();
	int getY();

	void setVisible(bool is);
	bool getVisible();

	void setActive(bool is);
	bool getActive();

	void setFocus(bool is);
	bool getFocus();

	void setAlwaysFocus(bool is);
	bool getAlwaysFocus();

	void setCanvas(Image* m);
	Image* getCanvas();

protected:
	std::function<void(GuiInstance*)> onChangedFunction;

	int getOffsetX();
	int getOffsetY();

	int getRenderOffsetX();
	int getRenderOffsetY();

	int baseX = 0;
	int baseY = 0;

	int x = 0;
	int y = 0;

	int renderX = 0;
	int renderY = 0;

	void setPriority(int value);

	friend class GuiManager;

	void setManager(GuiManager* m);
	GuiManager* getManager();

private:

	void setParentVisible(bool is);
	void setParentActive(bool is);

	int priority = 0;

	bool parentVisible = true;
	bool parentActive = true;

	bool visible = true;
	bool active = true;
	bool focus = false;
	bool alwaysFocus = false;

	int* offX = nullptr;
	int* offY = nullptr;

	int* renderOffX = nullptr;
	int* renderOffY = nullptr;
	
	bool shouldCallA = false;
	bool shouldCallV = false;
	bool shouldCallA2 = false;
	bool shouldCallV2 = false;

	bool shouldCallF = false;

	std::function<void(GuiInstance*)> onFocusFunction;
	std::function<void(GuiInstance*)> onActivateFunction;
	std::function<void(GuiInstance*)> onVisibleFunction;

	std::function<void(GuiInstance*)> onDeActivateFunction;
	std::function<void(GuiInstance*)> onInvisibleFunction;

	std::vector<GuiInstance*> children = std::vector<GuiInstance*>();
	GuiManager* manager = nullptr;
	Image* canvas = nullptr;
};

class GuiContainer : public GuiInstance
{
public:
	GuiContainer();
	~GuiContainer();

	//Object and Class Stuff
	const Class* getClass();
	static const Class myClass;
	
	void update();
	void render(Image* surf);

private:
};

class GuiCanvas : public GuiInstance
{
public:
	GuiCanvas();
	GuiCanvas(int width, int height);
	~GuiCanvas();

	//Object and Class Stuff
	const Class* getClass();
	static const Class myClass;
	
	void update();
	void render(Image* surf);

	void setInstanceCanvas(GuiInstance* ins);

	Color getClearColor();
	void setClearColor(Color c);
private:
	Image myImage;
	Color clearColor = {0,0,0,0};
};

class GuiImage : public GuiInstance
{
public:
	GuiImage(Image* img = nullptr);
	~GuiImage();

	//Object and Class Stuff
	const Class* getClass();
	static const Class myClass;
	
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
	static const Class myClass;
	
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
	static const Class myClass;

	void update();
	void render(Image* surf);

	void setTextColor(Color c);
	Color getTextColor();

	void setHighlightColor(Color c);
	Color getHighlightColor();

	void setShouldHighlightText(bool v);
	bool getShouldHighlightText();

	void setHighlightStart(int v);
	int getHighlightStart();
	void setHighlightEnd(int v);
	int getHighlightEnd();

	void setAllowLineBreaks(bool v);
	bool getAllowLineBreaks();

	std::string getText();
	std::string& getTextRef();
	void setText(std::string s);

	void setFont(Font* f);
	Font* getFont();

	void setWidth(int v);
	void setHeight(int v);

	int getWidth();
	int getHeight();

	void setOffsetX(int x);
	void setOffsetY(int y);

private:

	int width = 0;
	int height = 0;
	bool shouldHighlight = false;

	int startHighlight = -1;
	int endHighlight = -1;
	int offsetX = 0;
	int offsetY = 0;

	bool allowLineBreaks = false;

	Color textColor = { 0, 0, 0, 255 };
	Color highlightColor = { 72, 150, 255, 64 };
	Font* textFont = nullptr;

	std::string text = "";
};

class GuiTextBox : public GuiInstance
{
public:
	GuiTextBox(int x, int y, int width, int height);
	GuiTextBox(const GuiTextBox& other);
	void operator=(const GuiTextBox& other);
	void copy(const GuiTextBox& other);
	~GuiTextBox();

	//Object and Class Stuff
	const Class* getClass();
	static const Class myClass;

	void update();
	void render(Image* surf);

	void setOnEnterPressedFunction(std::function<void(GuiInstance*)> func);
	void setOnKeyPressFunction(std::function<void(GuiInstance*)> func);

	void setBackgroundColor(Color c);
	void setOutlineColor(Color c);
	void setFocusOutlineColor(Color c);
	void setCursorBlinkColor(Color c);

	GuiTextBlock* getTextBlockElement();

	void setCursorBlinkTimer(int timeInFrames);
	void setCursorWidth(int w);

	void setWidth(int v);
	void setHeight(int v);

	int getWidth();
	int getHeight();
private:
	
	std::function<void(GuiInstance*)> onEnterPressedFunction;
	std::function<void(GuiInstance*)> onKeyPressFunction;

	void keyInput();
	void mouseInput();
	void selectionCleanup();

	int width = 0;
	int height = 0;

	int cursorLocation = 0;
	int selectStart = -1;
	int selectEnd = -1;
	
	int startStringIndex = 0;

	bool hold = false;
	bool cursorBlink = false;
	int cursorBlinkTimer = 0;
	int cursorBlinkMaxTime = 30;
	int cursorWidth = 1;

	Color backgroundColor = { 180, 180, 180, 255 };
	Color outlineColor = { 0, 0, 0, 255 };
	Color focusOutlineColor = { 0, 0, 255, 255 };

	Color cursorBlinkColor = {0,0,0,255};

	GuiTextBlock textElement = GuiTextBlock(0,0,0,0);
};

class GuiShape : public GuiInstance
{
public:
	GuiShape(int x, int y, VectorGraphic* svg);
	GuiShape(const GuiShape& other);
	void operator=(const GuiShape& other);
	void copy(const GuiShape& other);
	~GuiShape();

	//Object and Class Stuff
	const Class* getClass();
	static const Class myClass;
	
	void update();
	void render(Image* surf);

	void setOnClickFunction(std::function<void(GuiInstance*)> func);

	void setBackgroundColor(Color c);
	void setOutlineColor(Color c);
	void setFocusOutlineColor(Color c);

private:
	
	std::function<void(GuiInstance*)> onClickFunction;

	CombinationShape* collisionMap = nullptr;
	VectorGraphic* shape = nullptr;

	Color backgroundColor = { 180, 180, 180, 255 };
	Color outlineColor = { 0, 0, 0, 255 };
	Color focusOutlineColor = { 0, 0, 255, 255 };
};

class GuiRectangleButton : public GuiInstance
{
public:
	GuiRectangleButton(int x, int y, int width, int height);
	GuiRectangleButton(const GuiRectangleButton& other);
	void operator=(const GuiRectangleButton& other);
	void copy(const GuiRectangleButton& other);
	~GuiRectangleButton();

	//Object and Class Stuff
	const Class* getClass();
	static const Class myClass;
	
	void update();
	void render(Image* surf);

	void setOnClickFunction(std::function<void(GuiInstance*)> func);
	void setOnClickHoldFunction(std::function<void(GuiInstance*)> func);

	void setBackgroundColor(Color c);
	void setOutlineColor(Color c);
	void setFocusOutlineColor(Color c);

	void setFocusBackgroundColor(Color c);
	void setHoverColor(Color c);

	void setWidth(int v);
	void setHeight(int v);

	int getWidth();
	int getHeight();

private:
	std::function<void(GuiInstance*)> onClickFunction;
	std::function<void(GuiInstance*)> onClickHoldFunction;

	int width = 0;
	int height = 0;

	bool hover = false;

	Color backgroundColor = { 180, 180, 180, 255 };
	Color hoverColor = {200, 200, 200, 255};
	Color focusBackgroundColor = {225, 225, 225, 255};

	Color outlineColor = { 0, 0, 0, 255 };
	Color focusOutlineColor = { 0, 0, 255, 255 };
};

class GuiCircleButton : public GuiInstance
{
public:
	GuiCircleButton(int x, int y, int radius);
	GuiCircleButton(const GuiCircleButton& other);
	void operator=(const GuiCircleButton& other);
	void copy(const GuiCircleButton& other);
	~GuiCircleButton();

	//Object and Class Stuff
	const Class* getClass();
	static const Class myClass;

	void update();
	void render(Image* surf);

	void setOnClickFunction(std::function<void(GuiInstance*)> func);
	void setOnClickHoldFunction(std::function<void(GuiInstance*)> func);

	void setBackgroundColor(Color c);
	void setOutlineColor(Color c);
	void setFocusOutlineColor(Color c);

	void setRadius(int v);
	int getRadius();

private:
	std::function<void(GuiInstance*)> onClickFunction;
	std::function<void(GuiInstance*)> onClickHoldFunction;

	int radius = 0;

	Color backgroundColor = { 180, 180, 180, 255 };
	Color outlineColor = { 0, 0, 0, 255 };
	Color focusOutlineColor = { 0, 0, 255, 255 };
};

class GuiScrollBar : public GuiInstance
{
public:
	GuiScrollBar(int startX, int startY, int endX, int endY);
	GuiScrollBar(const GuiScrollBar& other);
	void operator=(const GuiScrollBar& other);
	void copy(const GuiScrollBar& other);
	~GuiScrollBar();

	//Object and Class Stuff
	const Class* getClass();
	static const Class myClass;

	void update();
	void render(Image* surf);

	void setOnSlideFunction( std::function<void(GuiInstance*)> func);

	void setBackgroundColor(Color c);
	Color getBackgroundColor();

	void setOutlineColor(Color c);
	Color getOutlineColor();

	void setStartX(int x);
	int getStartX();

	void setStartY(int y);
	int getStartY();

	void setEndX(int x);
	int getEndX();

	void setEndY(int y);
	int getEndY();

	void setMinWidth(int w);
	int getMinWidth();

	void setMinHeight(int h);
	int getMinHeight();

	void setSteps(int s);
	int getSteps();

	void setCurrentStep(int s);
	int getCurrentStep();

	void setHorizontalBar(bool v);
	bool getHorizontalBar();

	void setShowScrollButtons(bool v);
	bool getShowScrollButtons();

	void increaseScroll();
	void decreaseScroll();

	GuiRectangleButton* getButtonElement();
	GuiRectangleButton* getDecreaseButtonElement();
	GuiRectangleButton* getIncreaseButtonElement();

private:

	int startX = 0;
	int endX = 0;
	int startY = 0;
	int endY = 0;

	int steps = 0;
	int currStep = 0;

	int minWidth = 12;
	int minHeight = 12;

	bool isHorizontal = false;
	bool showScrollButtons = true;

	//holdButtonFunction variables
	std::function<void(GuiInstance*)> onSlideFunction;
	void holdButtonFunction(GuiInstance* a);
	int preMouseX = 0;
	int preMouseY = 0;
	int preButtonX = 0;
	int preButtonY = 0;

	Color backgroundColor = { 180, 180, 180, 255 };
	Color outlineColor = { 0, 0, 0, 255 };

	GuiRectangleButton buttonElement = GuiRectangleButton(0,0,0,0);

	GuiRectangleButton decreaseButtonElement = GuiRectangleButton(0,0,0,0);
	GuiRectangleButton increaseButtonElement = GuiRectangleButton(0,0,0,0);
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
	static const Class myClass;

	void addElement(GuiInstance* k);
	void deleteElement(GuiInstance* k);
	std::vector<GuiInstance*> getElements();

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

	Color getBackgroundColor();
	void setBackgroundColor(Color c);

private:
	void sortElements();

	std::vector<GuiInstance*> objects = std::vector<GuiInstance*>();
	
	bool ownsImage = false;
	Image* surf = nullptr;
	int windowX = 0;
	int windowY = 0;

	Color backgroundColor = { 0xA2, 0xB9, 0xBC, 0xFF };
};
