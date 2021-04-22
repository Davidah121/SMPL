#include "GuiManager.h"
#include "Input.h"
#include "MathExt.h"
#include "Sort.h"
#include "WndWindow.h"

#pragma region GUI_INSTANCE_CLASS

const Class* GuiInstance::myClass = new Class("GuiInstance", {Object::myClass});
const Class* GuiInstance::getClass()
{
	return GuiInstance::myClass;
}

GuiInstance::GuiInstance(std::string name, bool vis)
{
	this->name = name;
	visible = vis;
	shouldCallA = false;
	shouldCallV = false;
}

GuiInstance::~GuiInstance()
{
	children.clear();
	manager = nullptr;
}

void GuiInstance::update()
{
}

void GuiInstance::render(Image* surf)
{
}

void GuiInstance::addChild(GuiInstance* ins)
{
	ins->priority = this->priority + 1;
	ins->setOffset( &x, &y );
	children.push_back(ins);
}

std::vector<GuiInstance*> GuiInstance::getChildren()
{
	return children;
}

void GuiInstance::setName(std::string name)
{
	this->name = name;
}

std::string GuiInstance::getName()
{
	return name;
}

void GuiInstance::setVisible(bool is)
{
	visible = is;
	if (visible)
	{
		shouldCallV = true;
		shouldCallV2 = false;
	}
	else
	{
		shouldCallV2 = true;
		shouldCallV = false;
	}
}

bool GuiInstance::getVisible()
{
	return visible;
}

void GuiInstance::setActive(bool is)
{
	active = is;
	if (active)
	{
		shouldCallA = true;
		shouldCallA2 = false;
	}
	else
	{
		shouldCallA2 = true;
		shouldCallA = false;
	}
}

bool GuiInstance::getActive()
{
	return active;
}

int GuiInstance::getOffsetX()
{
	if(offX!=nullptr)
		return *offX;
	return 0;
}

int GuiInstance::getOffsetY()
{
	if(offY!=nullptr)
		return *offY;
	return 0;
}

void GuiInstance::setBaseX(int x)
{
	baseX = x;
}

int GuiInstance::getBaseX()
{
	return baseX;
}

void GuiInstance::setBaseY(int y)
{
	baseY = y;
}

int GuiInstance::getBaseY()
{
	return baseY;
}

int GuiInstance::getX()
{
	return x;
}

int GuiInstance::getY()
{
	return y;
}

void GuiInstance::setOnActivateFunction(std::function<void(GuiInstance*)> func)
{
	onActivateFunction = func;
}

void GuiInstance::setOnVisibleFunction(std::function<void(GuiInstance*)> func)
{
	onVisibleFunction = func;
}

void GuiInstance::setOnDeActivateFunction(std::function<void(GuiInstance*)> func)
{
	onDeActivateFunction = func;
}

void GuiInstance::setOnInvisibleFunction(std::function<void(GuiInstance*)> func)
{
	onInvisibleFunction = func;
}

void GuiInstance::setOnChangedFunction(std::function<void(GuiInstance*)> func)
{
	onChangedFunction = func;
}

void GuiInstance::setOffset(int* offX, int* offY)
{
	this->offX = offX;
	this->offY = offY;
}

const void GuiInstance::baseUpdate()
{
	x = getOffsetX() + baseX;
	y = getOffsetY() + baseY;
	
	if (shouldCallA)
		if(onActivateFunction!=nullptr)
			onActivateFunction(this);

	if (shouldCallV)
		if(onVisibleFunction!=nullptr)
			onVisibleFunction(this);
	
	if (shouldCallA2)
		if(onDeActivateFunction!=nullptr)
			onDeActivateFunction(this);

	if (shouldCallV2)
		if(onInvisibleFunction!=nullptr)
			onInvisibleFunction(this);

	shouldCallA = false;
	shouldCallV = false;

	shouldCallA2 = false;
	shouldCallV2 = false;
}

int GuiInstance::getPriority()
{
	return priority;
}

void GuiInstance::setManager(GuiManager* m)
{
	manager = m;
}

GuiManager* GuiInstance::getManager()
{
	return manager;
}

#pragma endregion

#pragma region GUI_CONTAINER_CLASS

const Class* GuiContainer::myClass = new Class("GuiContainer", {GuiInstance::myClass});
const Class* GuiContainer::getClass()
{
	return GuiContainer::myClass;
}

GuiContainer::GuiContainer()
{
	
}

GuiContainer::~GuiContainer()
{
	
}

void GuiContainer::update()
{
	
}

void GuiContainer::render(Image* surf)
{
	
}

#pragma endregion

#pragma region GUI_IMAGE_CLASS

const Class* GuiImage::myClass = new Class("GuiImage", {GuiInstance::myClass});
const Class* GuiImage::getClass()
{
	return GuiImage::myClass;
}

GuiImage::GuiImage(Image* img)
{
	this->img = img;
}

GuiImage::~GuiImage()
{
	
}

void GuiImage::update()
{
	
}

void GuiImage::render(Image* surf)
{
	if(surf!=nullptr)
	{
		if(img!=nullptr)
		{
			Graphics::drawImage(img, x, y, surf);
		}
	}
}

void GuiImage::setImage(Image* img)
{
	this->img = img;
}

Image* GuiImage::getImage()
{
	return img;
}

#pragma endregion

#pragma region GUI_SPRITE_CLASS

const Class* GuiSprite::myClass = new Class("GuiSprite", {GuiInstance::myClass});
const Class* GuiSprite::getClass()
{
	return GuiSprite::myClass;
}

GuiSprite::GuiSprite()
{

}

GuiSprite::~GuiSprite()
{
	
}

void GuiSprite::update()
{
	
	if(timer>=fps)
	{
		timer=0;
		index++;
		if(index>=img.getSize())
		{
			if(shouldLoop)
				index = 0;
		}
	}
	timer++;
}

void GuiSprite::render(Image* surf)
{
	if(surf!=nullptr)
	{
		if(img.getImage(index)!=nullptr)
		{
			Graphics::setColor(imgColor);
			Graphics::drawSprite(img.getImage(index), x, y, surf);
		}
	}
}

void GuiSprite::setSprite(Sprite img)
{
	this->img = img;
}

Sprite& GuiSprite::getSprite()
{
	return img;
}

void GuiSprite::setAnimationTimer(int framesPerSecond)
{
	fps = framesPerSecond;
}
int GuiSprite::getAnimationTimer()
{
	return fps;
}

void GuiSprite::setLoop(bool loop)
{
	shouldLoop = loop;
}

bool GuiSprite::getLoop()
{
	return shouldLoop;
}

void GuiSprite::setColor(Color c)
{
	imgColor = c;
}

Color GuiSprite::getColor()
{
	return imgColor;
}

#pragma endregion

#pragma region GUI_TEXTBLOCK_CLASS

const Class* GuiTextBlock::myClass = new Class("GuiTextBlock", {GuiInstance::myClass});
const Class* GuiTextBlock::getClass()
{
	return GuiTextBlock::myClass;
}

GuiTextBlock::GuiTextBlock(int x, int y, int width, int height)
{
	setBaseX(x);
	setBaseY(y);
	this->width = width;
	this->height = height;
}

GuiTextBlock::~GuiTextBlock()
{
	
}

void GuiTextBlock::update()
{
	
}

void GuiTextBlock::render(Image* surf)
{
	if(surf!=nullptr)
	{
		Font* oldFont = Graphics::getFont();

		if(textFont!=nullptr)
			Graphics::setFont(textFont);

		Graphics::setColor(textColor);
		Graphics::drawTextLimits(text, x, y, width, height, surf);

		Graphics::setFont(oldFont);
	}
}

void GuiTextBlock::setTextColor(Color c)
{
	textColor = c;
}

Color GuiTextBlock::getTextColor()
{
	return textColor;
}

std::string GuiTextBlock::getText()
{
	return text;
}

std::string& GuiTextBlock::getTextRef()
{
	return text;
}

void GuiTextBlock::setText(std::string s)
{
	text = s;
}

void GuiTextBlock::setFont(Font* f)
{
	textFont = f;
}

Font* GuiTextBlock::getFont()
{
	return textFont;
}

void GuiTextBlock::setWidth(int v)
{
	width = v;
}
void GuiTextBlock::setHeight(int v)
{
	height = v;
}

int GuiTextBlock::getWidth()
{
	return width;
}
int GuiTextBlock::getHeight()
{
	return height;
}

#pragma endregion

#pragma region GUI_TEXTBOX_CLASS

const Class* GuiTextBox::myClass = new Class("GuiTextBox", {GuiInstance::myClass});
const Class* GuiTextBox::getClass()
{
	return GuiTextBox::myClass;
}

GuiTextBox::GuiTextBox(int x, int y, int width, int height) : GuiInstance()
{
	setBaseX(x);
	setBaseY(y);
	this->width = width;
	this->height = height;
	onEnterPressedFunction = nullptr;
	onKeyPressFunction = nullptr;

	textElement = GuiTextBlock(0,0,width,height);
	addChild( &textElement );
}

GuiTextBox::~GuiTextBox()
{
}

void GuiTextBox::setOnEnterPressedFunction(std::function<void(GuiInstance*)> func)
{
	onEnterPressedFunction = func;
}

void GuiTextBox::setOnKeyPressFunction(std::function<void(GuiInstance*)> func)
{
	onKeyPressFunction = func;
}

void GuiTextBox::setBackgroundColor(Color c)
{
	backgroundColor = c;
}

void GuiTextBox::setOutlineColor(Color c)
{
	outlineColor = c;
}

void GuiTextBox::setActiveOutlineColor(Color c)
{
	activeOutlineColor = c;
}

void GuiTextBox::render(Image* surf)
{
	if(surf!=nullptr)
	{
		//draw a rectangle
		Graphics::setColor(backgroundColor);
		surf->drawRect(x, y, x + width, y + height, false);

		if (getActive() == false)
			Graphics::setColor(outlineColor);
		else
			Graphics::setColor(activeOutlineColor);
		
		surf->drawRect(x, y, x + width, y + height, true);
	}
}

void GuiTextBox::update()
{
	if (getActive())
	{
		if (Input::getKeyChanged() && Input::getKeyPressed(0x0D)!=true )
		{
			//General Key
			if (onKeyPressFunction != nullptr)
			{
				onKeyPressFunction(this);
			}
		}

		if (Input::getKeyPressed(0x0D))
		{
			 //Enter key
			if (onEnterPressedFunction != nullptr)
			{
				onEnterPressedFunction(this);
			}
		}

		//Do something like this
		if( Input::getKeyPressed(0x08))
		{
			if(textElement.getTextRef().size()>0)
				textElement.getTextRef().pop_back();
		}

		if( StringTools::isValidChar( Input::getLastKeyPressed() ) )
		{
			if(Input::getKeyDown( VK_SHIFT ))
				textElement.getTextRef() += (char)Input::getLastKeyPressed();
			else
				textElement.getTextRef() += (char)std::tolower(Input::getLastKeyPressed());
		}
	}

	int mouseX = Input::getMouseX();
	int mouseY = Input::getMouseY();
	
	if(getManager()!=nullptr)
	{
		mouseX -= getManager()->getWindowX();
		mouseY -= getManager()->getWindowY();
	}

	if (Input::getMousePressed(Input::LEFT_MOUSE_BUTTON))
	{
		setActive(false);
		if (mouseX >= x && mouseX <= x + width)
		{
			if (mouseY >= y && mouseY <= y + height)
			{
				setActive(true);
			}
		}
	}

}

GuiTextBlock* GuiTextBox::getTextBlockElement()
{
	return &textElement;
}

void GuiTextBox::setWidth(int v)
{
	width = v;
}
void GuiTextBox::setHeight(int v)
{
	height = v;
}
int GuiTextBox::getWidth()
{
	return width;
}
int GuiTextBox::getHeight()
{
	return height;
}

#pragma endregion

#pragma region GUI_RECTANGLE_BUTTON

const Class* GuiRectangleButton::myClass = new Class("GuiRectangleButton", {GuiInstance::myClass});
const Class* GuiRectangleButton::getClass()
{
	return GuiRectangleButton::myClass;
}

GuiRectangleButton::GuiRectangleButton(int x, int y, int width, int height) : GuiInstance()
{
	setBaseX(x);
	setBaseY(y);
	this->width = width;
	this->height = height;
	onClickFunction = nullptr;
}

GuiRectangleButton::~GuiRectangleButton()
{
}

void GuiRectangleButton::update()
{
	int mouseX = Input::getMouseX();
	int mouseY = Input::getMouseY();
	
	if(getManager()!=nullptr)
	{
		mouseX -= getManager()->getWindowX();
		mouseY -= getManager()->getWindowY();
	}

	if (Input::getMousePressed(Input::LEFT_MOUSE_BUTTON))
	{
		setActive(false);
		if (mouseX >= x && mouseX <= x + width)
		{
			if (mouseY >= y && mouseY <= y + height)
			{
				setActive(true);

				if (onClickFunction != nullptr)
					onClickFunction(this);
			}
		}
	}

	if(Input::getMouseDown(Input::LEFT_MOUSE_BUTTON) && getActive())
	{
		if (onClickHoldFunction != nullptr)
			onClickHoldFunction(this);
	}
}

void GuiRectangleButton::render(Image* surf)
{
	if(surf!=nullptr)
	{
		//draw a rectangle
		Graphics::setColor(backgroundColor);
		surf->drawRect(x, y, x + width, y + height, false);

		if (getActive() == false)
			Graphics::setColor(outlineColor);
		else
			Graphics::setColor(activeOutlineColor);

		surf->drawRect(x, y, x + width, y + height, true);
	}
}

void GuiRectangleButton::setOnClickFunction(std::function<void(GuiInstance*)> func)
{
	onClickFunction = func;
}

void GuiRectangleButton::setOnClickHoldFunction(std::function<void(GuiInstance*)> func)
{
	onClickHoldFunction = func;
}

void GuiRectangleButton::setBackgroundColor(Color c)
{
	backgroundColor = c;
}

void GuiRectangleButton::setOutlineColor(Color c)
{
	outlineColor = c;
}

void GuiRectangleButton::setActiveOutlineColor(Color c)
{
	activeOutlineColor = c;
}

void GuiRectangleButton::setWidth(int v)
{
	width = v;
}
void GuiRectangleButton::setHeight(int v)
{
	height = v;
}
int GuiRectangleButton::getWidth()
{
	return width;
}
int GuiRectangleButton::getHeight()
{
	return height;
}

#pragma endregion

#pragma region GUI_CIRCLE_BUTTON

const Class* GuiCircleButton::myClass = new Class("GuiCircleButton", {GuiInstance::myClass});
const Class* GuiCircleButton::getClass()
{
	return GuiCircleButton::myClass;
}

GuiCircleButton::GuiCircleButton(int x, int y, int radius) : GuiInstance()
{
	setBaseX(x);
	setBaseY(y);
	this->radius = radius;
	onClickFunction = nullptr;
}

GuiCircleButton::~GuiCircleButton()
{
}

void GuiCircleButton::update()
{
	int mouseX = Input::getMouseX();
	int mouseY = Input::getMouseY();
	
	if(getManager()!=nullptr)
	{
		mouseX -= getManager()->getWindowX();
		mouseY -= getManager()->getWindowY();
	}

	if (Input::getMousePressed(Input::LEFT_MOUSE_BUTTON))
	{
		setActive(false);
		double radSqr = MathExt::sqr(radius);
		double otherRad = MathExt::sqr((double)mouseX - x) + MathExt::sqr((double)mouseY - y);

		if (otherRad <= radSqr)
		{
			setActive(true);
			if (onClickFunction != nullptr)
				onClickFunction(this);
		}
	}

	if(Input::getMouseDown(Input::LEFT_MOUSE_BUTTON) && getActive())
	{
		if (onClickHoldFunction != nullptr)
			onClickHoldFunction(this);
	}
}

void GuiCircleButton::render(Image* surf)
{
	if(surf!=nullptr)
	{
		//draw a circle
		Graphics::setColor(backgroundColor);
		surf->drawCircle(x, y, radius, false);

		if (getActive() == false)
			Graphics::setColor(outlineColor);
		else
			Graphics::setColor(activeOutlineColor);

		surf->drawCircle(x, y, radius, true);
	}
}

void GuiCircleButton::setOnClickFunction(std::function<void(GuiInstance*)> func)
{
	onClickFunction = func;
}

void GuiCircleButton::setOnClickHoldFunction(std::function<void(GuiInstance*)> func)
{
	onClickHoldFunction = func;
}

void GuiCircleButton::setBackgroundColor(Color c)
{
	backgroundColor = c;
}

void GuiCircleButton::setOutlineColor(Color c)
{
	outlineColor = c;
}

void GuiCircleButton::setActiveOutlineColor(Color c)
{
	activeOutlineColor = c;
}

void GuiCircleButton::setRadius(int v)
{
	radius = v;
}
int GuiCircleButton::getRadius()
{
	return radius;
}

#pragma endregion

#pragma region GUI_SCROLL_BAR

const Class* GuiScrollBar::myClass = new Class("GuiScrollBar", {GuiScrollBar::myClass});
const Class* GuiScrollBar::getClass()
{
	return GuiScrollBar::myClass;
}

GuiScrollBar::GuiScrollBar(int startX, int startY, int endX, int endY) : GuiInstance()
{
	setBaseX(x);
	setBaseY(y);
	
	//must satisfy the min width and height
	this->startX = startX;
	this->startY = startY;
	this->endX = endX;
	this->endY = endY;
	
	onSlideFunction = nullptr;

	int width = this->endX-this->startX;
	int height = this->endY-this->startY;

	int buttonWidth = max(width, minWidth);
	int buttonHeight = max(height, minHeight);

	if(isHorizontal)
		buttonElement = GuiRectangleButton(this->startX, this->startY - buttonHeight/2, buttonWidth, buttonHeight);
	else
		buttonElement = GuiRectangleButton(this->startX - buttonWidth/2, this->startY, buttonWidth, buttonHeight);

	std::function<void(GuiInstance*)> f = [this](GuiInstance* a) -> void {this->holdButtonFunction(a);};
	std::function<void(GuiInstance*)> f2 = [this](GuiInstance* a) -> void { this->setActive(true);};
	std::function<void(GuiInstance*)> f3 = [this](GuiInstance* a) -> void { this->setActive(false);};

	buttonElement.setOnClickHoldFunction( f );
	buttonElement.setOnActivateFunction( f2 );
	buttonElement.setOnDeActivateFunction( f3 );
	addChild( &buttonElement );

	std::function<void(GuiInstance*)> scrollDecrease = [this](GuiInstance* a) -> void{
		this->decreaseScroll();
	};

	std::function<void(GuiInstance*)> scrollIncrease = [this](GuiInstance* a) -> void{
		this->increaseScroll();
	};
	
	if(isHorizontal)
	{
		decreaseButtonElement = GuiRectangleButton(this->startX-buttonHeight, this->startY - buttonHeight/2, buttonHeight, buttonHeight);
		increaseButtonElement = GuiRectangleButton(this->endX, this->startY - buttonHeight/2, buttonHeight, buttonHeight);
	}
	else
	{
		decreaseButtonElement = GuiRectangleButton(this->startX - buttonWidth/2, this->startY-buttonWidth, buttonWidth, buttonWidth);
		increaseButtonElement = GuiRectangleButton(this->startX - buttonWidth/2, this->endY, buttonWidth, buttonWidth);
	}

	decreaseButtonElement.setOnClickFunction(scrollDecrease);
	increaseButtonElement.setOnClickFunction(scrollIncrease);
	
	addChild( &decreaseButtonElement );
	addChild( &increaseButtonElement );
}

GuiScrollBar::~GuiScrollBar()
{
}

void GuiScrollBar::decreaseScroll()
{
	setCurrentStep( currStep - 1 );
	if(onSlideFunction!=nullptr)
	{
		onSlideFunction(this);
	}
}

void GuiScrollBar::increaseScroll()
{
	setCurrentStep( currStep + 1 );
	if(onSlideFunction!=nullptr)
	{
		onSlideFunction(this);
	}
}

void GuiScrollBar::update()
{
	if(buttonElement.getActive() || increaseButtonElement.getActive() || decreaseButtonElement.getActive())
	{
		if(!getActive())
			setActive(true);
	}
	else
	{
		if(getActive())
			setActive(false);
	}

	if(getActive())
	{
		if(isHorizontal)
		{
			if(Input::getKeyPressed(VK_LEFT) || Input::getMouseScrollVertical() > 0 || Input::getMouseScrollHorizontal() < 0)
			{
				decreaseScroll();
			}
			else if (Input::getKeyPressed(VK_RIGHT) || Input::getMouseScrollVertical() < 0 || Input::getMouseScrollHorizontal() > 0)
			{
				increaseScroll();
			}
		}
		else
		{
			if(Input::getKeyPressed(VK_UP) || Input::getMouseScrollVertical() > 0)
			{
				decreaseScroll();
			}
			else if (Input::getKeyPressed(VK_DOWN) || Input::getMouseScrollVertical() < 0)
			{
				increaseScroll();
			}
		}
	}
}

void GuiScrollBar::holdButtonFunction(GuiInstance* ins)
{
	//ins is the rectangle button. Can be disregarded.
	int mouseX = Input::getMouseX();
	int mouseY = Input::getMouseY();
	
	if(getManager()!=nullptr)
	{
		mouseX -= getManager()->getWindowX();
		mouseY -= getManager()->getWindowY();
	}

	if(Input::getMousePressed(Input::LEFT_MOUSE_BUTTON))
	{
		preMouseX = mouseX;
		preMouseY = mouseY;
		preButtonX = buttonElement.getBaseX();
		preButtonY = buttonElement.getBaseY();
	}

	mouseX -= preMouseX;
	mouseY -= preMouseY;

	if(!isHorizontal)
	{
		//adjust so that the end of the button does not pass the bounds
		int butHeight = buttonElement.getHeight();

		//determine button position
		int butY = MathExt::clamp(preButtonY + mouseY, startY, endY-butHeight);
		buttonElement.setBaseY( butY );

		//determine current step
		double percentage = (double)(butY - startY) / (endY - startY - butHeight);
		double t = percentage*steps;

		currStep = MathExt::clamp((int)MathExt::round(t), 0, steps);
	}
	else
	{
		//adjust so that the end of the button does not pass the bounds
		int butWidth = buttonElement.getWidth();

		//determine button position
		int butX = MathExt::clamp(preButtonX + mouseX, startX, endX-butWidth);
		buttonElement.setBaseX( butX );

		//determine current step
		double percentage = (double)(butX - startX) / (endX - startX - butWidth);
		double t = percentage*steps;

		currStep = MathExt::clamp((int)MathExt::round(t), 0, steps);
	}

	if(onSlideFunction!=nullptr)
	{
		onSlideFunction(this);
	}
}

void GuiScrollBar::render(Image* surf)
{
	if(surf!=nullptr)
	{
		//draw the background bar
		Graphics::setColor(backgroundColor);
		surf->drawRect(startX, startY, endX, endY, false);

		Graphics::setColor(outlineColor);
		surf->drawRect(startX, startY, endX, endY, true);
	}
}

void GuiScrollBar::setHorizontalBar(bool v)
{
	isHorizontal = v;

	int width = this->endX-this->startX;
	int height = this->endY-this->startY;

	int buttonWidth = max(width, minWidth);
	int buttonHeight = max(height, minHeight);

	if(isHorizontal)
	{
		int nY = startY - ((buttonHeight/2) - (height/2));
		buttonElement.setBaseY(nY);

		decreaseButtonElement.setBaseX(this->startX-buttonHeight);
		decreaseButtonElement.setBaseY(this->startY-buttonHeight/2);
		decreaseButtonElement.setWidth(buttonHeight);
		decreaseButtonElement.setHeight(buttonHeight);
		
		increaseButtonElement.setBaseX(this->endX);
		increaseButtonElement.setBaseY(this->startY-buttonHeight/2);
		increaseButtonElement.setWidth(buttonHeight);
		increaseButtonElement.setHeight(buttonHeight);
	}
	else
	{
		int nX = startX - ((buttonWidth/2) - (width/2));
		buttonElement.setBaseX(nX);

		decreaseButtonElement.setBaseX(nX);
		decreaseButtonElement.setBaseY(this->startY-buttonWidth);
		decreaseButtonElement.setWidth(buttonWidth);
		decreaseButtonElement.setHeight(buttonWidth);
		
		increaseButtonElement.setBaseX(nX);
		increaseButtonElement.setBaseY(this->endY);
		increaseButtonElement.setWidth(buttonWidth);
		increaseButtonElement.setHeight(buttonWidth);
	}
}

bool GuiScrollBar::getHorizontalBar()
{
	return isHorizontal;
}

void GuiScrollBar::setShowScrollButtons(bool v)
{
	decreaseButtonElement.setVisible(v);
	increaseButtonElement.setVisible(v);
	showScrollButtons = v;
}

bool GuiScrollBar::getShowScrollButtons()
{
	return showScrollButtons;
}

void GuiScrollBar::setOnSlideFunction(std::function<void(GuiInstance*)> func)
{
	onSlideFunction = func;
}

void GuiScrollBar::setBackgroundColor(Color c)
{
	backgroundColor = c;
}

Color GuiScrollBar::getBackgroundColor()
{
	return backgroundColor;
}

void GuiScrollBar::setOutlineColor(Color c)
{
	outlineColor = c;
}

Color GuiScrollBar::getOutlineColor()
{
	return outlineColor;
}

void GuiScrollBar::setStartX(int x)
{
	startX = x;
}

int GuiScrollBar::getStartX()
{
	return startX;
}

void GuiScrollBar::setStartY(int y)
{
	startY = y;
}

int GuiScrollBar::getStartY()
{
	return startY;
}

void GuiScrollBar::setEndX(int x)
{
	endX = x;
}

int GuiScrollBar::getEndX()
{
	return endX;
}

void GuiScrollBar::setEndY(int y)
{
	endY = y;
}

int GuiScrollBar::getEndY()
{
	return endY;
}

void GuiScrollBar::setMinWidth(int w)
{
	minWidth = w;
}

int GuiScrollBar::getMinWidth()
{
	return minWidth;
}

void GuiScrollBar::setMinHeight(int h)
{
	minHeight = h;
}

int GuiScrollBar::getMinHeight()
{
	return minHeight;
}

void GuiScrollBar::setSteps(int s)
{
	steps = max(s, 0);
	setCurrentStep( MathExt::clamp(currStep, 0, steps) );

	if(isHorizontal)
	{
		int width = (endX-startX);
		
		if(steps!=0)
			width = (int)MathExt::round((double)width/steps);

		buttonElement.setWidth( max(width, minWidth) );
	}
	else
	{
		int height = (endY-startY);

		if(steps!=0)
			height = (int)MathExt::round((double)height/steps);

		buttonElement.setHeight( max(height, minHeight) );
	}
}

int GuiScrollBar::getSteps()
{
	return steps;
}

void GuiScrollBar::setCurrentStep(int s)
{
	currStep = MathExt::clamp(s, 0, steps);

	if(isHorizontal)
	{
		//adjust so that the end of the button does not pass the bounds
		int butWidth = buttonElement.getWidth();

		//determine percentage
		double percentage = (double)currStep/steps;
		double t = percentage*(endX-startX-butWidth) + startX;

		int xPos = MathExt::clamp((int)MathExt::round(t), startX, endX-butWidth);

		buttonElement.setBaseX( xPos );
	}
	else
	{
		//adjust so that the end of the button does not pass the bounds
		int butHeight = buttonElement.getHeight();

		//determine percentage
		double percentage = (double)currStep/steps;
		double t = percentage*(endY-startY-butHeight) + startY;

		int yPos = MathExt::clamp((int)MathExt::round(t), startY, endY-butHeight);

		buttonElement.setBaseY( yPos );
	}
}

int GuiScrollBar::getCurrentStep()
{
	return currStep;
}

GuiRectangleButton* GuiScrollBar::getButtonElement()
{
	return &buttonElement;
}

GuiRectangleButton* GuiScrollBar::getDecreaseButtonElement()
{
	return &decreaseButtonElement;
}

GuiRectangleButton* GuiScrollBar::getIncreaseButtonElement()
{
	return &increaseButtonElement;
}

#pragma endregion

#pragma region GUI_MANAGER

const Class* GuiManager::myClass = new Class("GuiManager", {Object::myClass});
const Class* GuiManager::getClass()
{
	return GuiManager::myClass;
}

GuiManager::GuiManager()
{
}

GuiManager::GuiManager(Image* img, bool ownership)
{
	ownsImage = ownership;
	surf = img;
}

GuiManager::GuiManager(const GuiManager& k)
{
	ownsImage = false;
	surf = k.surf;
	objects = std::vector<GuiInstance*>(k.objects);
}

GuiManager::~GuiManager()
{
	objects.clear();

	if(ownsImage)
	{
		if(surf!=nullptr)
		{
			delete surf;
		}
	}

	surf = nullptr;
}

void GuiManager::addElement(GuiInstance* k)
{
	if(k->getManager() == nullptr)
	{
		k->setManager(this);
		objects.push_back(k);

		for(GuiInstance* c : k->getChildren())
		{
			if(c->getManager() == nullptr)
			{
				addElement(c);
			}
		}
	}
}

void GuiManager::deleteElement(GuiInstance* k)
{
	int startIndex = -1;
	int orgSize = getSize();

	for (int i = 0; i < orgSize; i++)
	{
		if (objects[i] == k)
		{
			startIndex = i;
			break;
		}
	}

	if (startIndex >= 0)
	{
		for (int i = startIndex; i < orgSize - 1; i++)
		{
			objects[i] = objects[i + 1];
		}
		objects.pop_back();
	}

	//delete children as well
	for(GuiInstance* o : k->getChildren())
	{
		deleteElement(o);
	}
}

void GuiManager::sortElements()
{
	Sort::insertionSort<GuiInstance*>(objects.data(), objects.size(), [](GuiInstance* a, GuiInstance* b)->bool{
		return a->getPriority() < b->getPriority();
	});
}

void GuiManager::updateGuiElements()
{
	sortElements();
	Input::pollInput();
	for (int i = 0; i < objects.size(); i++)
	{
		if (objects[i] != nullptr)
		{
			objects[i]->baseUpdate();
			objects[i]->update();
		}
	}
}

void GuiManager::renderGuiElements()
{
	if (surf != nullptr)
	{
		Graphics::setColor({ 0xA2, 0xB9, 0xBC, 0xFF });
		surf->clearImage();
		for (int i = 0; i < objects.size(); i++)
		{
			if (objects[i] != nullptr)
			{
				objects[i]->render(surf);
			}
		}
	}
}

void GuiManager::setImage(Image* surface)
{
	surf = surface;
}

Image* GuiManager::getImage()
{
	return surf;
}

size_t GuiManager::getSize()
{
	return objects.size();
}

void GuiManager::resizeImage(int width, int height)
{
	if(ownsImage)
	{
		if(surf!=nullptr)
		{
			delete surf;
		}
		surf = new Image(width, height);
	}
}

void GuiManager::setWindowX(int v)
{
	windowX = v;
}

void GuiManager::setWindowY(int v)
{
	windowY = v;
}

int GuiManager::getWindowX()
{
	return windowX;
}

int GuiManager::getWindowY()
{
	return windowY;
}

#pragma endregion

