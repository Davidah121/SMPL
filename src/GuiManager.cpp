#include "GuiManager.h"
#include "Input.h"
#include "MathExt.h"
#include "Sort.h"
#include "WndWindow.h"

#pragma region GUI_INSTANCE_CLASS

const Class GuiInstance::myClass = Class("GuiInstance", {&Object::myClass});
const Class* GuiInstance::getClass()
{
	return &GuiInstance::myClass;
}

GuiInstance::GuiInstance()
{
	shouldCallA = false;
	shouldCallV = false;
}

GuiInstance::GuiInstance(const GuiInstance& other)
{
	copy(other);
}

void GuiInstance::operator=(const GuiInstance& other)
{
	copy(other);
}

void GuiInstance::copy(const GuiInstance& other)
{
	baseX = other.baseX;
	baseY = other.baseY;

	x = other.x;
	y = other.y;

	renderX = other.renderX;
	renderY = other.renderY;

	priority = other.priority;

	visible = other.visible;
	active = other.active;
	focus = other.focus;
	alwaysFocus = other.alwaysFocus;

	offX = other.offX;
	offY = other.offY;

	renderOffX = other.renderOffX;
	renderOffY = other.renderOffY;
	
	shouldCallA = other.shouldCallA;
	shouldCallV = other.shouldCallV;
	shouldCallA2 = other.shouldCallA2;
	shouldCallV2 = other.shouldCallV2;

	shouldCallF = other.shouldCallF;

	//children = other.children;
	
	if(other.parent!=nullptr)
	{
		other.parent->addChild(this);
	}
	
	for(GuiInstance* child : children)
	{
		child->parent = nullptr;
		addChild(child);
	}

	manager = other.manager;
	canvas = other.canvas;
	
	onFocusFunction = other.onFocusFunction;
	onActivateFunction = other.onActivateFunction;
	onVisibleFunction = other.onVisibleFunction;

	onDeActivateFunction = other.onDeActivateFunction;
	onInvisibleFunction = other.onInvisibleFunction;

	onChangedFunction = other.onChangedFunction;
}

GuiInstance::~GuiInstance()
{
	if(parent!=nullptr)
	{
		parent->removeChild(this);
	}
	if(manager!=nullptr)
	{
		manager->deleteElement(this);
	}

	for(GuiInstance* ins : children)
	{
		ins->parent = nullptr;
		ins->setOffset(nullptr, nullptr);
	}

	parent = nullptr;
	children.clear();
	manager = nullptr;

	renderOffX = nullptr;
	renderOffY = nullptr;

	offX = nullptr;
	offY = nullptr;
}

void GuiInstance::update()
{
}

void GuiInstance::render(Image* surf)
{
}

void GuiInstance::addChild(GuiInstance* ins)
{
	if(ins->parent!=nullptr)
	{
		ins->parent->removeChild(ins);
	}
	
	ins->parent = this;
	ins->updatePriority();
	ins->setOffset( &x, &y );
	children.push_back(ins);

	if(manager!=nullptr && manager!=ins->manager)
	{
		//add to manager
		manager->addElement(ins);
	}
}

void GuiInstance::removeChild(GuiInstance* ins)
{
	std::vector<GuiInstance*> m = std::vector<GuiInstance*>();
	for(int i=0; i<children.size(); i++)
	{
		if(children[i] != ins)
		{
			m.push_back(children[i]);
		}
	}

	children = m;

	ins->parent = nullptr;
}

std::vector<GuiInstance*> GuiInstance::getChildren()
{
	return children;
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
	bool finalVisibility = (parent!=nullptr) ? (visible && parent->getVisible()) : visible;
	return finalVisibility;
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
	bool finalActivity = (parent!=nullptr) ? (active && parent->getActive()) : active;
	return finalActivity;
}

void GuiInstance::setFocus(bool is)
{
	if(!alwaysFocus)
	{
		if(!focus && is)
		{
			shouldCallF = true;
		}
		focus = is;
	}
	else
	{
		focus = true;
	}
}

bool GuiInstance::getFocus()
{
	return focus;
}

void GuiInstance::setAlwaysFocus(bool is)
{
	alwaysFocus = is;
	setFocus( getFocus() ); //Will set to true if alwaysFocus is true.
}

bool GuiInstance::getAlwaysFocus()
{
	return alwaysFocus;
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

int GuiInstance::getRenderOffsetX()
{
	if(renderOffX!=nullptr)
		return *renderOffX;
	return 0;
}

int GuiInstance::getRenderOffsetY()
{
	if(renderOffY!=nullptr)
		return *renderOffY;
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

void GuiInstance::setOnFocusFunction(std::function<void(GuiInstance*)> func)
{
	onFocusFunction = func;
}

void GuiInstance::setOffset(int* offX, int* offY)
{
	this->offX = offX;
	this->offY = offY;
}

void GuiInstance::setRenderOffset(int* offX, int* offY)
{
	this->renderOffX = offX;
	this->renderOffY = offY;
}

const void GuiInstance::baseRender()
{
	renderX = baseX + getOffsetX() - getRenderOffsetX();
	renderY = baseY + getOffsetY() - getRenderOffsetY();
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

	if(shouldCallF)
		if(onFocusFunction!=nullptr)
			onFocusFunction(this);
	
	shouldCallA = false;
	shouldCallV = false;

	shouldCallA2 = false;
	shouldCallV2 = false;

	shouldCallF = false;
}

int GuiInstance::getPriority()
{
	return priority;
}

void GuiInstance::setPriority(int value)
{
	if(value!=CANVAS_PRIORITY_VALUE)
	{
		priority = MathExt::clamp(value, GuiInstance::MIN_PRIORITY_VALUE, GuiInstance::MAX_PRIORITY_VALUE);
	}
	else
	{
		priority = MathExt::abs(GuiInstance::CANVAS_PRIORITY_VALUE);
	}
	
	for(GuiInstance* child : children)
	{
		child->updatePriority();
	}
}

void GuiInstance::updatePriority()
{
	if(parent!=nullptr)
		priority = max(priority, parent->priority+1);
	
	for(GuiInstance* insChildren : children)
	{
		insChildren->updatePriority();
	}
}

void GuiInstance::setManager(GuiManager* m)
{
	manager = m;
}

GuiManager* GuiInstance::getManager()
{
	return manager;
}

void GuiInstance::setCanvas(Image* m)
{
	canvas = m;
}

Image* GuiInstance::getCanvas()
{
	return canvas;
}

#pragma endregion

#pragma region GUI_CONTAINER_CLASS

const Class GuiContainer::myClass = Class("GuiContainer", {&GuiInstance::myClass});
const Class* GuiContainer::getClass()
{
	return &GuiContainer::myClass;
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

#pragma region GUI_CANVAS_CLASS

const Class GuiCanvas::myClass = Class("GuiCanvas", {&GuiInstance::myClass});
const Class* GuiCanvas::getClass()
{
	return &GuiCanvas::myClass;
}

GuiCanvas::GuiCanvas()
{
}

GuiCanvas::GuiCanvas(int width, int height)
{
	myImage = Image(width, height);
}

GuiCanvas::~GuiCanvas()
{

}

void GuiCanvas::update()
{

	Graphics::setColor(clearColor);
	myImage.clearImage();
}

void GuiCanvas::render(Image* surf)
{
	surf->drawImage(&myImage, renderX, renderY);
}

void GuiCanvas::setClearColor(Color c)
{
	clearColor = c;
}

Color GuiCanvas::getClearColor()
{
	return clearColor;
}

void GuiCanvas::setInstanceCanvas(GuiInstance* ins)
{
	ins->setCanvas(&myImage);
	ins->setRenderOffset(&x, &y);
	setPriority( GuiInstance::CANVAS_PRIORITY_VALUE );

	for(GuiInstance* o : ins->getChildren())
	{
		setInstanceCanvas(o);
	}
}

#pragma endregion

#pragma region GUI_IMAGE_CLASS

const Class GuiImage::myClass = Class("GuiImage", {&GuiInstance::myClass});
const Class* GuiImage::getClass()
{
	return &GuiImage::myClass;
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
			Graphics::drawImage(img, renderX, renderY, surf);
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

const Class GuiSprite::myClass = Class("GuiSprite", {&GuiInstance::myClass});
const Class* GuiSprite::getClass()
{
	return &GuiSprite::myClass;
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
			Graphics::drawSprite(img.getImage(index), renderX, renderY, surf);
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

const Class GuiTextBlock::myClass = Class("GuiTextBlock", {&GuiInstance::myClass});
const Class* GuiTextBlock::getClass()
{
	return &GuiTextBlock::myClass;
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

		Graphics::setClippingRect( Box2D(renderX, renderY, renderX+width, renderY+height) );
		Graphics::drawTextLimits(text, renderX+offsetX, renderY+offsetY, width-offsetX, height-offsetY, allowLineBreaks, surf);

		if(shouldHighlight && endHighlight>0 && startHighlight>0)
		{
			int actualS, actualE;

			actualS = MathExt::clamp(startHighlight, 0, text.size());
			actualE = MathExt::clamp(endHighlight, 0, text.size());

			if(actualS > actualE)
			{
				int swap = actualS;
				actualS = actualE;
				actualE = swap;
			}
			
			Font* currFont = Graphics::getFont();
			std::string startString, endString;
			startString = text.substr(0, actualS);
			endString = text.substr(actualE, text.size());

			int startWidth = currFont->getWidthOfString(startString);
			int endWidth = currFont->getWidthOfString(endString);

			int totalWidth = currFont->getWidthOfString(text);
			
			totalWidth -= (startWidth + endWidth);

			Graphics::setColor(highlightColor);
			int sX = renderX + startWidth;
			Graphics::drawRect(sX+offsetX, renderY+offsetY, sX+totalWidth+offsetX, renderY+offsetY+currFont->getFontSize(), false, surf);
		}
		
		Graphics::resetClippingPlane();
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

void GuiTextBlock::setHighlightColor(Color c)
{
	highlightColor = c;
}
Color GuiTextBlock::getHighlightColor()
{
	return highlightColor;
}

void GuiTextBlock::setShouldHighlightText(bool v)
{
	shouldHighlight = v;
}
bool GuiTextBlock::getShouldHighlightText()
{
	return shouldHighlight;
}

void GuiTextBlock::setHighlightStart(int v)
{
	startHighlight = v;
}
int GuiTextBlock::getHighlightStart()
{
	return startHighlight;
}
void GuiTextBlock::setHighlightEnd(int v)
{
	endHighlight = v;
}
int GuiTextBlock::getHighlightEnd()
{
	return endHighlight;
}

void GuiTextBlock::setAllowLineBreaks(bool v)
{
	allowLineBreaks = v;
}

bool GuiTextBlock::getAllowLineBreaks()
{
	return allowLineBreaks;
}

void GuiTextBlock::setOffsetX(int x)
{
	offsetX = x;
}

void GuiTextBlock::setOffsetY(int y)
{
	offsetY = y;
}

#pragma endregion

#pragma region GUI_TEXTBOX_CLASS

const Class GuiTextBox::myClass = Class("GuiTextBox", {&GuiInstance::myClass});
const Class* GuiTextBox::getClass()
{
	return &GuiTextBox::myClass;
}

GuiTextBox::GuiTextBox(int x, int y, int width, int height) : GuiInstance()
{
	setBaseX(x);
	setBaseY(y);
	this->width = width;
	this->height = height;
	onEnterPressedFunction = nullptr;
	onKeyPressFunction = nullptr;

	textElement = GuiTextBlock(0, 0, width, height);
	textElement.setShouldHighlightText(true);
	addChild( &textElement );
}

GuiTextBox::GuiTextBox(const GuiTextBox& other) : GuiInstance(other)
{
	copy(other);
}

void GuiTextBox::operator=(const GuiTextBox& other)
{
	GuiInstance::copy(other);
	copy(other);
}

void GuiTextBox::copy(const GuiTextBox& other)
{
	onEnterPressedFunction = other.onEnterPressedFunction;
	onKeyPressFunction = other.onKeyPressFunction;

	width = other.width;
	height = other.height;

	backgroundColor = other.backgroundColor;
	outlineColor = other.outlineColor;
	focusOutlineColor = other.focusOutlineColor;

	textElement = other.textElement;

	removeChild((GuiInstance*)&other.textElement);
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

void GuiTextBox::setFocusOutlineColor(Color c)
{
	focusOutlineColor = c;
}

void GuiTextBox::setCursorBlinkColor(Color c)
{
	cursorBlinkColor = c;
}

void GuiTextBox::setCursorBlinkTimer(int timeInFrames)
{
	cursorBlinkMaxTime = timeInFrames;
}

void GuiTextBox::setCursorWidth(int w)
{
	cursorWidth = w;
}

void GuiTextBox::render(Image* surf)
{
	if(surf!=nullptr)
	{
		//draw a rectangle
		Graphics::setColor(backgroundColor);
		surf->drawRect(renderX, renderY, renderX + width, renderY + height, false);

		if (getFocus() == false)
			Graphics::setColor(outlineColor);
		else
			Graphics::setColor(focusOutlineColor);
		
		surf->drawRect(renderX, renderY, renderX + width, renderY + height, true);

		if(getFocus())
		{
			if(!cursorBlink)
			{
				//Font Stuff
				Graphics::setColor(cursorBlinkColor);
				std::string testText = textElement.getTextRef().substr(startStringIndex, cursorLocation-startStringIndex);
				Font* f = (textElement.getFont() != nullptr) ? textElement.getFont() : Graphics::getFont();
				int startOfCursorLine = f->getWidthOfString(testText);

				surf->drawRect(renderX+startOfCursorLine, renderY, renderX+startOfCursorLine+cursorWidth, renderY+f->getFontSize(), false);
			}

			cursorBlinkTimer++;
			if(cursorBlinkTimer>=cursorBlinkMaxTime)
			{
				cursorBlinkTimer = 0;
				cursorBlink = !cursorBlink;
			}
		}
	}
}

void GuiTextBox::keyInput()
{
	if (getFocus())
	{
		if (Input::getKeyChanged() && Input::getKeyPressed(Input::KEY_ENTER)!=true )
		{
			//General Key
			if (onKeyPressFunction != nullptr)
			{
				onKeyPressFunction(this);
			}
		}

		if (Input::getKeyPressed(Input::KEY_ENTER) || Input::getKeyPressed(Input::KEY_NUMPAD_ENTER))
		{
			//Enter key
			if (onEnterPressedFunction != nullptr)
			{
				onEnterPressedFunction(this);
			}
		}
		else if (Input::getKeyPressed(Input::KEY_LEFT))
		{
			//Move cursor back
			cursorLocation = MathExt::clamp(cursorLocation-1, 0, textElement.getTextRef().size());
			selectStart = cursorLocation;
			selectEnd = cursorLocation;
		}
		else if (Input::getKeyPressed(Input::KEY_RIGHT))
		{
			//Move cursor forward
			cursorLocation = MathExt::clamp(cursorLocation+1, 0, textElement.getTextRef().size());
			selectStart = cursorLocation;
			selectEnd = cursorLocation;
		}
		else if( Input::getKeyPressed(Input::KEY_BACKSPACE))
		{
			std::string nString;
			int actualS, actualE;
			if(selectStart<selectEnd)
			{
				actualS = selectStart;
				actualE = selectEnd;
			}
			else
			{
				actualS = selectEnd;
				actualE = selectStart;
			}
			
			if(actualS==actualE)
			{
				actualS--;
			}

			for(int i=0; i<actualS; i++)
			{
				nString += textElement.getTextRef()[i];
			}

			for(int i=actualE; i<textElement.getTextRef().size(); i++)
			{
				nString += textElement.getTextRef()[i];
			}

			textElement.setText(nString);

			cursorLocation = actualS;
			selectStart = cursorLocation;
			selectEnd = cursorLocation;
		}
		else if( Input::getKeyPressed(Input::KEY_DELETE))
		{
			std::string nString;
			int actualS, actualE;
			if(selectStart<selectEnd)
			{
				actualS = selectStart;
				actualE = selectEnd;
			}
			else
			{
				actualS = selectEnd;
				actualE = selectStart;
			}
			
			if(actualS==actualE)
			{
				actualE++;
			}

			for(int i=0; i<actualS; i++)
			{
				nString += textElement.getTextRef()[i];
			}

			for(int i=actualE; i<textElement.getTextRef().size(); i++)
			{
				nString += textElement.getTextRef()[i];
			}

			textElement.setText(nString);

			cursorLocation = actualS;
			selectStart = cursorLocation;
			selectEnd = cursorLocation;
		}
		else
		{
			int lastKey = Input::getLastKeyPressed();
			int charVal = lastKey;

			//convert char to ascii
			int value = StringTools::convertKeyToAscii(lastKey, Input::getKeyDown(VK_SHIFT));

			if(value!=-1)
			{
				int minSelect = min(selectStart, selectEnd);
				int maxSelect = max(selectStart, selectEnd);
				
				std::string prefix = textElement.getText().substr(0, minSelect);
				std::string suffix = textElement.getText().substr(maxSelect);

				textElement.setText( (prefix + (char)value) + suffix );
				cursorLocation = minSelect+1;
				selectStart = cursorLocation;
				selectEnd = cursorLocation;
			}
		}
	}
}

void GuiTextBox::mouseInput()
{
	Font* f = (textElement.getFont() != nullptr) ? textElement.getFont() : Graphics::getFont();
	int mouseX = Input::getMouseX();
	int mouseY = Input::getMouseY();
	
	if(getManager()!=nullptr)
	{
		mouseX -= getManager()->getWindowX();
		mouseY -= getManager()->getWindowY();
	}
	
	if (Input::getMousePressed(Input::LEFT_MOUSE_BUTTON))
	{
		setFocus(false);

		selectEnd = -1;
		selectStart = -1;
		cursorLocation = 0;

		if (mouseX >= x && mouseX <= x + width)
		{
			if (mouseY >= y && mouseY <= y + height)
			{
				setFocus(true);
				hold = true;

				//set cursor pos and start selection
				int testX = mouseX-x;
				std::string testText = textElement.getText();

				int totalWidth = 0;
				bool found = false;
				for(int i=startStringIndex; i<testText.size(); i++)
				{
					FontCharInfo fci = f->getFontCharInfo(testText[i]);
					totalWidth += fci.horizAdv;
					if(testX<totalWidth)
					{
						cursorLocation = i;
						selectStart = i;
						selectEnd = i;
						found = true;
						break;
					}
				}

				if(!found)
				{
					cursorLocation = testText.size();
					selectStart = testText.size();
				}
			}
		}
	}

	if (Input::getMouseUp(Input::LEFT_MOUSE_BUTTON))
	{
		hold = false;
	}

	if(hold == true)
	{
		//selection stuff
		//set cursor pos and end selection
		int testX = mouseX-x;
		std::string testText = textElement.getText();

		int totalWidth = 0;
		bool found = false;
		for(int i=startStringIndex; i<testText.size(); i++)
		{
			FontCharInfo fci = f->getFontCharInfo(testText[i]);
			totalWidth += fci.horizAdv;
			if(testX<totalWidth)
			{
				cursorLocation = i;
				selectEnd = i;
				found = true;
				break;
			}
		}

		if(!found)
		{
			cursorLocation = testText.size();
			selectEnd = testText.size();
		}
	}
}

void GuiTextBox::selectionCleanup()
{
	Font* f = (textElement.getFont() != nullptr) ? textElement.getFont() : Graphics::getFont();

	textElement.setHighlightStart(selectStart);
	textElement.setHighlightEnd(selectEnd);

	if(startStringIndex>=cursorLocation)
	{
		startStringIndex = cursorLocation-1;
	}

	if(startStringIndex<0)
		startStringIndex = 0;

	std::string testStringWidth = textElement.getTextRef().substr(startStringIndex, cursorLocation-startStringIndex);
	int testWidth = f->getWidthOfString(testStringWidth);

	if(testWidth >= width)
	{
		startStringIndex++;
	}

	int offX = f->getWidthOfString( textElement.getTextRef().substr(0, startStringIndex) );
	textElement.setOffsetX(-offX);
}

void GuiTextBox::update()
{
	selectEnd = MathExt::clamp(selectEnd, 0, textElement.getTextRef().size());
	selectStart = MathExt::clamp(selectStart, 0, textElement.getTextRef().size());
	cursorLocation = MathExt::clamp(cursorLocation, 0, textElement.getTextRef().size());
	startStringIndex = MathExt::clamp(startStringIndex, 0, textElement.getTextRef().size());
	
	keyInput();
	mouseInput();
	selectionCleanup();
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

const Class GuiRectangleButton::myClass = Class("GuiRectangleButton", {&GuiInstance::myClass});
const Class* GuiRectangleButton::getClass()
{
	return &GuiRectangleButton::myClass;
}

GuiRectangleButton::GuiRectangleButton(int x, int y, int width, int height) : GuiInstance()
{
	setBaseX(x);
	setBaseY(y);
	this->width = width;
	this->height = height;
	onClickFunction = nullptr;
}

GuiRectangleButton::GuiRectangleButton(const GuiRectangleButton& other) : GuiInstance(other)
{
	copy(other);
}

void GuiRectangleButton::operator=(const GuiRectangleButton& other)
{
	GuiInstance::copy(other);
	copy(other);
}

void GuiRectangleButton::copy(const GuiRectangleButton& other)
{
	onClickFunction = other.onClickFunction;
	onClickHoldFunction = other.onClickHoldFunction;

	width = other.width;
	height = other.height;

	hover = other.hover;

	backgroundColor = other.backgroundColor;
	hoverColor = other.hoverColor;
	focusBackgroundColor = other.focusBackgroundColor;

	outlineColor = other.outlineColor;
	focusOutlineColor = other.focusOutlineColor;
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

	
	if (mouseX >= x && mouseX <= x + width)
	{
		if (mouseY >= y && mouseY <= y + height)
		{
			hover = true;

			if(Input::getMousePressed(Input::LEFT_MOUSE_BUTTON))
			{
				setFocus(true);
				if (onClickFunction != nullptr)
					onClickFunction(this);
			}
		}
		else
		{
			hover = false;
		}
	}
	else
	{
		hover = false;
	}

	if(Input::getMouseUp(Input::LEFT_MOUSE_BUTTON))
	{
		if(hover && getFocus())
		{
			if(onClickReleaseFunction != nullptr)
				onClickReleaseFunction(this);
		}
		setFocus(false);
	}

	if(getFocus())
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
		if(!getFocus())
		{
			if(!hover)
				Graphics::setColor(backgroundColor);
			else
				Graphics::setColor(hoverColor);
		}
		else
		{
			Graphics::setColor(focusBackgroundColor);
		}
		
		surf->drawRect(renderX, renderY, renderX + width, renderY + height, false);

		if (getFocus() == false)
			Graphics::setColor(outlineColor);
		else
			Graphics::setColor(focusOutlineColor);

		surf->drawRect(renderX, renderY, renderX + width, renderY + height, true);
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

void GuiRectangleButton::setOnClickReleaseFunction(std::function<void(GuiInstance*)> func)
{
	onClickReleaseFunction = func;
}

void GuiRectangleButton::setBackgroundColor(Color c)
{
	backgroundColor = c;
}

void GuiRectangleButton::setOutlineColor(Color c)
{
	outlineColor = c;
}

void GuiRectangleButton::setFocusOutlineColor(Color c)
{
	focusOutlineColor = c;
}

void GuiRectangleButton::setFocusBackgroundColor(Color c)
{
	focusBackgroundColor = c;
}

void GuiRectangleButton::setHoverColor(Color c)
{
	hoverColor = c;
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

const Class GuiCircleButton::myClass = Class("GuiCircleButton", {&GuiInstance::myClass});
const Class* GuiCircleButton::getClass()
{
	return &GuiCircleButton::myClass;
}

GuiCircleButton::GuiCircleButton(int x, int y, int radius) : GuiInstance()
{
	setBaseX(x);
	setBaseY(y);
	this->radius = radius;
	onClickFunction = nullptr;
}

GuiCircleButton::GuiCircleButton(const GuiCircleButton& other) : GuiInstance(other)
{
	copy(other);
}

void GuiCircleButton::operator=(const GuiCircleButton& other)
{
	GuiInstance::copy(other);
	copy(other);
}

void GuiCircleButton::copy(const GuiCircleButton& other)
{
	onClickFunction = other.onClickFunction;
	onClickHoldFunction = other.onClickHoldFunction;

	radius = other.radius;

	backgroundColor = other.backgroundColor;
	outlineColor = other.outlineColor;
	focusOutlineColor = other.focusOutlineColor;
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
		setFocus(false);
		double radSqr = MathExt::sqr(radius);
		double otherRad = MathExt::sqr((double)mouseX - x) + MathExt::sqr((double)mouseY - y);

		if (otherRad <= radSqr)
		{
			setFocus(true);
			if (onClickFunction != nullptr)
				onClickFunction(this);
		}
	}

	if(Input::getMouseDown(Input::LEFT_MOUSE_BUTTON) && getFocus())
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
		surf->drawCircle(renderX, renderY, radius, false);

		if (getFocus() == false)
			Graphics::setColor(outlineColor);
		else
			Graphics::setColor(focusOutlineColor);

		surf->drawCircle(renderX, renderY, radius, true);
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

void GuiCircleButton::setFocusOutlineColor(Color c)
{
	focusOutlineColor = c;
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

const Class GuiScrollBar::myClass = Class("GuiScrollBar", {&GuiScrollBar::myClass});
const Class* GuiScrollBar::getClass()
{
	return &GuiScrollBar::myClass;
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


	//location of button center == (startX+endX)/2
	//startButX = startX + ??
	//startButX2 = startX + ?? + buttonWidth

	int butX = startX;
	int butY = startY;

	if(isHorizontal)
	{
		if(buttonHeight != height)
		{
			butY -= buttonHeight/2;
		}
	}
	else
	{
		if(buttonWidth != width)
		{
			butX -= buttonWidth/2;
		}
	}

	buttonElement = GuiRectangleButton(butX, butY, buttonWidth, buttonHeight);

	std::function<void(GuiInstance*)> f = [this](GuiInstance* a) -> void {this->holdButtonFunction(a);};
	std::function<void(GuiInstance*)> f2 = [this](GuiInstance* a) -> void { this->setFocus(true);};
	std::function<void(GuiInstance*)> f3 = [this](GuiInstance* a) -> void { this->setFocus(false);};

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
		decreaseButtonElement = GuiRectangleButton(this->startX-buttonHeight, butY, buttonHeight, buttonHeight);
		increaseButtonElement = GuiRectangleButton(this->endX, butY, buttonHeight, buttonHeight);
	}
	else
	{
		decreaseButtonElement = GuiRectangleButton(butX, this->startY-buttonWidth, buttonWidth, buttonWidth);
		increaseButtonElement = GuiRectangleButton(butX, this->endY, buttonWidth, buttonWidth);
	}

	decreaseButtonElement.setOnClickFunction(scrollDecrease);
	increaseButtonElement.setOnClickFunction(scrollIncrease);
	
	addChild( &decreaseButtonElement );
	addChild( &increaseButtonElement );
}

GuiScrollBar::GuiScrollBar(const GuiScrollBar& other) : GuiInstance(other)
{
	copy(other);
}

void GuiScrollBar::operator=(const GuiScrollBar& other)
{
	GuiInstance::copy(other);
	copy(other);
}

void GuiScrollBar::copy(const GuiScrollBar& other)
{
	startX = other.startX;
	endX = other.endX;
	startY = other.startY;
	endY = other.endY;

	steps = other.steps;
	currStep = other.currStep;

	minWidth = other.minWidth;
	minHeight = other.minHeight;

	isHorizontal = other.isHorizontal;
	showScrollButtons = other.showScrollButtons;

	//holdButtonFunction variables
	onSlideFunction = other.onSlideFunction;
	preMouseX = other.preMouseX;
	preMouseY = other.preMouseY;
	preButtonX = other.preButtonX;
	preButtonY = other.preButtonY;

	backgroundColor = other.backgroundColor;
	outlineColor = other.outlineColor;

	buttonElement = other.buttonElement;

	decreaseButtonElement = other.decreaseButtonElement;
	increaseButtonElement = other.increaseButtonElement;

	//Recreate functions for the button elements to refer to this object and not the other
	std::function<void(GuiInstance*)> f = [this](GuiInstance* a) -> void {this->holdButtonFunction(a);};
	std::function<void(GuiInstance*)> f2 = [this](GuiInstance* a) -> void { this->setFocus(true);};
	std::function<void(GuiInstance*)> f3 = [this](GuiInstance* a) -> void { this->setFocus(false);};

	buttonElement.setOnClickHoldFunction( f );
	buttonElement.setOnActivateFunction( f2 );
	buttonElement.setOnDeActivateFunction( f3 );

	std::function<void(GuiInstance*)> scrollDecrease = [this](GuiInstance* a) -> void{
		this->decreaseScroll();
	};

	std::function<void(GuiInstance*)> scrollIncrease = [this](GuiInstance* a) -> void{
		this->increaseScroll();
	};

	decreaseButtonElement.setOnClickFunction(scrollDecrease);
	increaseButtonElement.setOnClickFunction(scrollIncrease);

	addChild(&buttonElement);
	addChild(&decreaseButtonElement);
	addChild(&increaseButtonElement);
}

GuiScrollBar::~GuiScrollBar()
{
}

void GuiScrollBar::decreaseScroll()
{
	setCurrentStep( currStep - 1 );
}

void GuiScrollBar::increaseScroll()
{
	setCurrentStep( currStep + 1 );

}

void GuiScrollBar::update()
{
	if(buttonElement.getFocus() || increaseButtonElement.getFocus() || decreaseButtonElement.getFocus())
	{
		if(!getFocus())
			setFocus(true);
	}
	else
	{
		if(getFocus())
			setFocus(false);
	}

	if(getFocus())
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

	int nSX = startX + x;
	int nSY = startY + y;
	int nEX = endX + x;
	int nEY = endY + y;

	int oldStep = currStep;

	if(!isHorizontal)
	{
		//adjust so that the end of the button does not pass the bounds
		int butHeight = buttonElement.getHeight();

		//determine button position
		int butY = MathExt::clamp(preButtonY + mouseY, nSY, nEY-butHeight);
		buttonElement.setBaseY( butY );

		//determine current step
		double percentage = (double)(butY - nSY) / (nEY - nSY - butHeight);
		double t = percentage*steps;

		currStep = MathExt::clamp((int)MathExt::round(t), 0, steps);
	}
	else
	{
		//adjust so that the end of the button does not pass the bounds
		int butWidth = buttonElement.getWidth();

		//determine button position
		int butX = MathExt::clamp(preButtonX + mouseX, nSX, nEX-butWidth);
		buttonElement.setBaseX( butX );

		//determine current step
		double percentage = (double)(butX - nSX) / (nEX - nSX - butWidth);
		double t = percentage*steps;

		currStep = MathExt::clamp((int)MathExt::round(t), 0, steps);
	}

	if(oldStep!=currStep)
	{
		if(onSlideFunction!=nullptr)
		{
			onSlideFunction(this);
		}
	}
}

void GuiScrollBar::render(Image* surf)
{
	if(surf!=nullptr)
	{
		//draw the background bar
		Graphics::setColor(backgroundColor);
		surf->drawRect(renderX+startX, renderY+startY, renderX+endX, renderY+endY, false);

		Graphics::setColor(outlineColor);
		surf->drawRect(renderX+startX, renderY+startY, renderX+endX, renderY+endY, true);
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
		
		width = (int)MathExt::round((double)width/(steps+1));

		buttonElement.setWidth( max(width, minWidth) );
	}
	else
	{
		int height = (endY-startY);

		height = (int)MathExt::round((double)height/(steps+1));

		buttonElement.setHeight( max(height, minHeight) );
	}
}

int GuiScrollBar::getSteps()
{
	return steps;
}

void GuiScrollBar::setCurrentStep(int s)
{
	int oldStep = currStep;
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

	if(oldStep!=currStep)
	{
		if(onSlideFunction!=nullptr)
		{
			onSlideFunction(this);
		}
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

const Class GuiManager::myClass = Class("GuiManager", {&Object::myClass});
const Class* GuiManager::getClass()
{
	return &GuiManager::myClass;
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
	for(GuiInstance* ins : objects)
	{
		if(ins!=nullptr)
			ins->manager = nullptr;
	}
	
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
	if(k!=nullptr)
	{
		if(k->getManager() == nullptr)
		{
			k->setManager(this);
			objects.push_back(k);

			for(int i=0; i<k->getChildren().size(); i++)
			{
				GuiInstance* c = k->getChildren().at(i);
				if(c->getManager() == nullptr)
				{
					addElement(c);
				}
			}
		}
	}
}

void GuiManager::deleteElement(GuiInstance* k)
{
	if(k->manager == this)
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

		k->manager = nullptr;

		//delete children as well
		for(GuiInstance* o : k->getChildren())
		{
			deleteElement(o);
		}
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
	//Assume that pollInput() was already called
	sortElements();

	//remove objects that are not active or whose parents aren't active
	std::vector<GuiInstance*> currActiveObjects = std::vector<GuiInstance*>();
	for(int i=0; i<objects.size(); i++)
	{
		if (objects[i] != nullptr)
		{
			if(objects[i]->getActive())
			{
				currActiveObjects.push_back(objects[i]);
			}
		}
	}

	//UPDATE
	for(GuiInstance* obj : currActiveObjects)
	{
		obj->baseUpdate();
		obj->update();
	}
}

void GuiManager::renderGuiElements()
{
	std::vector<GuiInstance*> currVisibleObjects = std::vector<GuiInstance*>();
	for(int i=0; i<objects.size(); i++)
	{
		if (objects[i] != nullptr)
		{
			if(objects[i]->getVisible())
			{
				currVisibleObjects.push_back(objects[i]);
			}
		}
	}

	//RENDER
	if (surf != nullptr)
	{
		Graphics::setColor(backgroundColor);
		surf->clearImage();
	}

	for (GuiInstance* obj : currVisibleObjects)
	{
		obj->baseRender();

		if(obj->canvas != nullptr)
		{
			obj->render(obj->canvas);
		}
		else
		{
			if(surf!=nullptr)
				obj->render(surf);
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

Color GuiManager::getBackgroundColor()
{
	return backgroundColor;
}

void GuiManager::setBackgroundColor(Color c)
{
	backgroundColor = c;
}

#pragma endregion

