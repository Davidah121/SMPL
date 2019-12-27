#include "GuiManager.h"
#include "Input.h"
#include "MathExt.h"

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
}

void GuiInstance::update()
{
}

void GuiInstance::render(Image* surf)
{
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
	}
}

bool GuiInstance::getActive()
{
	return active;
}

int GuiInstance::getOffsetX()
{
	return *offX + 8;
}

int GuiInstance::getOffsetY()
{
	return *offY + 32;
}

void GuiInstance::setOnActivateFunction(void (*func)(void))
{
	onActivateFunction = func;
}

void GuiInstance::setOnVisibleFunction(void (*func)(void))
{
	onVisibleFunction = func;
}

void GuiInstance::setOffset(int* offX, int* offY)
{
	this->offX = offX;
	this->offY = offY;
}

const void GuiInstance::baseUpdate()
{
	if (shouldCallA)
		if(onActivateFunction!=nullptr)
			onActivateFunction();

	if (shouldCallV)
		if(onVisibleFunction!=nullptr)
			onVisibleFunction();

	shouldCallA = false;
	shouldCallV = false;
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
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	onEnterPressedFunction = nullptr;
	onKeyPressFunction = nullptr;
}

GuiTextBox::~GuiTextBox()
{
}

void GuiTextBox::setOnEnterPressedFunction(void(*func)(void))
{
	onEnterPressedFunction = func;
}

void GuiTextBox::setOnKeyPressFunction(void (*func)(void))
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
	//draw a rectangle
	Graphics::setColor(backgroundColor);
	surf->drawRect(x, y, x + width, y + height, false);

	if (getActive() == false)
		Graphics::setColor(outlineColor);
	else
		Graphics::setColor(activeOutlineColor);

	surf->drawRect(x, y, x + width, y + height, true);
}

void GuiTextBox::update()
{
	if (getActive())
	{
		if (Input::getKeyChanged())
		{
			if (onKeyPressFunction != nullptr)
			{
				onKeyPressFunction();
			}
		}

		if (Input::getKeyPressed(0x0D)) // Enter key
		{
			if (onEnterPressedFunction != nullptr)
			{
				onEnterPressedFunction();
			}
		}
	}

	if (Input::getMousePressed(Input::LEFT_MOUSE_BUTTON))
	{
		setActive(false);
		if (Input::getMouseX() - getOffsetX() >= x && Input::getMouseX() - getOffsetX() <= x + width)
		{
			if (Input::getMouseY() - getOffsetY() >= y && Input::getMouseY() - getOffsetY() <= y + height)
			{
				setActive(true);
			}
		}
	}

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
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	onClickFunction = nullptr;
}

GuiRectangleButton::~GuiRectangleButton()
{
}

void GuiRectangleButton::update()
{
	
	if (Input::getMousePressed(Input::LEFT_MOUSE_BUTTON))
	{
		setActive(false);
		if (Input::getMouseX() - getOffsetX() >= x && Input::getMouseX() - getOffsetX() <= x + width)
		{

			if (Input::getMouseY() - getOffsetY() >= y && Input::getMouseY() - getOffsetY() <= y + height)
			{
				setActive(true);

				if (onClickFunction != nullptr)
					onClickFunction();
			}
		}
	}
}

void GuiRectangleButton::render(Image* surf)
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

void GuiRectangleButton::setOnClickFunction(void(*func)(void))
{
	onClickFunction = func;
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

#pragma endregion

#pragma region GUI_CIRCLE_BUTTON

const Class* GuiCircleButton::myClass = new Class("GuiCircleButton", {GuiInstance::myClass});
const Class* GuiCircleButton::getClass()
{
	return GuiCircleButton::myClass;
}

GuiCircleButton::GuiCircleButton(int x, int y, int radius) : GuiInstance()
{
	this->x = x;
	this->y = y;
	this->radius = radius;
	onClickFunction = nullptr;
}

GuiCircleButton::~GuiCircleButton()
{
}

void GuiCircleButton::update()
{
	if (Input::getMousePressed(Input::LEFT_MOUSE_BUTTON))
	{
		setActive(false);
		double radSqr = MathExt::sqr(radius);
		double otherRad = MathExt::sqr((double)Input::getMouseX() - getOffsetX() - x) + MathExt::sqr((double)Input::getMouseY() - getOffsetY() - y);

		if (otherRad <= radSqr)
		{
			setActive(true);
			if (onClickFunction != nullptr)
				onClickFunction();
		}
	}
}

void GuiCircleButton::render(Image* surf)
{
	//draw a rectangle
	Graphics::setColor(backgroundColor);
	surf->drawCircle(x, y, radius, false);

	if (getActive() == false)
		Graphics::setColor(outlineColor);
	else
		Graphics::setColor(activeOutlineColor);

	surf->drawCircle(x, y, radius, true);
}

void GuiCircleButton::setOnClickFunction(void(*func)(void))
{
	onClickFunction = func;
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

#pragma endregion

#pragma region GUI_MANAGER

const Class* GuiManager::myClass = new Class("GuiManager", {Object::myClass});
const Class* GuiManager::getClass()
{
	return GuiManager::myClass;
}

GuiManager::GuiManager(int* offsetX, int* offsetY)
{
	offX = offsetX;
	offY = offsetY;
}

GuiManager::GuiManager(int* offsetX, int* offsetY, Image* img)
{
	offX = offsetX;
	offY = offsetY;
	surf = img;
}

GuiManager::GuiManager(const GuiManager& k)
{
	surf = k.surf;
	objects = std::vector<GuiInstance*>(k.objects);
}

GuiManager::~GuiManager()
{
	for (int i = 0; i < objects.size(); i++)
	{
		delete objects[i];
	}
	objects.clear();
}

void GuiManager::addElement(GuiInstance* k)
{
	k->setOffset(offX, offY);
	objects.push_back(k);
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

	delete k;
}

void GuiManager::updateGuiElements()
{
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

#pragma endregion

