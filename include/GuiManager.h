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

namespace glib
{

	class GuiInstance : public Object
	{
	public:
		static const int MAX_PRIORITY_VALUE = 9999;
		static const int MIN_PRIORITY_VALUE = 0;
		static const int CANVAS_PRIORITY_VALUE = -10000;

		/**
		 * @brief Construct a new GuiInstance object
		 * 		GuiInstance is more of an abstract class that is designed to be extended through sub classes.
		 * 
		 */
		GuiInstance();

		/**
		 * @brief Construct a new GuiInstance object from another GuiInstance object
		 * 
		 * @param other 
		 */
		GuiInstance(const GuiInstance& other);

		/**
		 * @brief Copies a GuiInstance object
		 * 
		 * @param other 
		 */
		void operator=(const GuiInstance& other);

		/**
		 * @brief Copies a GuiInstance object
		 * 
		 * @param other 
		 */
		void copy(const GuiInstance& other);

		/**
		 * @brief Destroy the GuiInstance object
		 * 
		 */
		~GuiInstance();

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;

		/**
		 * @brief An update function for the GuiInstance. Must be overriden.
		 * 
		 */
		virtual void update();

		/**
		 * @brief An render function for the GuiInstance. Must be overriden.
		 * 
		 */
		virtual void render(Image* surf);

		/**
		 * @brief Adds a child to the GuiInstance.
		 * 		The child will be updated and rendered after the parent.
		 * 		A GuiInstance child can only have one parent.
		 * 		The parent is replaced by this function if the child already has a parent.
		 * 
		 * @param obj 
		 */
		void addChild(GuiInstance* obj);

		/**
		 * @brief Removes a child from the GuiInstance.
		 * 
		 * @param obj 
		 */
		void removeChild(GuiInstance* obj);

		/**
		 * @brief Gets a list of children objects.
		 * 
		 * @return std::vector<GuiInstance*> 
		 */
		std::vector<GuiInstance*> getChildren();

		/**
		 * @brief The base update function.
		 * 		Should not be modified as it handles certain function calls like onFocus.
		 * 
		 */
		const void baseUpdate();

		/**
		 * @brief The base render function.
		 * 		Should not be modified as it sets the correct render offsets if neccessary.
		 * 
		 */
		const void baseRender();

		/**
		 * @brief Sets the onActivate Function
		 * 
		 * @param func 
		 */
		void setOnActivateFunction(std::function<void(GuiInstance*)> func);

		/**
		 * @brief Sets the onVisible Function
		 * 
		 * @param func 
		 */
		void setOnVisibleFunction(std::function<void(GuiInstance*)> func);

		/**
		 * @brief Sets the onDeActivate Function
		 * 
		 * @param func 
		 */
		void setOnDeActivateFunction(std::function<void(GuiInstance*)> func);

		/**
		 * @brief Sets the onInvisible Function
		 * 
		 * @param func 
		 */
		void setOnInvisibleFunction(std::function<void(GuiInstance*)> func);

		/**
		 * @brief Sets the onChanged Function
		 * 
		 * @param func 
		 */
		void setOnChangedFunction(std::function<void(GuiInstance*)> func);

		/**
		 * @brief Sets the onFocus Function
		 * 
		 * @param func 
		 */
		void setOnFocusFunction(std::function<void(GuiInstance*)> func);

		/**
		 * @brief Sets the x and y offset pointers.
		 * 		This will offset the actual position of the object.
		 * 		Useful if its position is based on another objects position.
		 * 
		 * @param offX 
		 * @param offY 
		 */
		void setOffset(int* offX, int* offY);

		/**
		 * @brief Sets the x and y render offset pointers.
		 * 		This will offset the rendering position of the object.
		 * 		Useful when using the Canvas element as the correct position on screen may not be the same
		 * 		as the actual objects position.
		 * 
		 * @param offX 
		 * @param offY 
		 */
		void setRenderOffset(int* offX, int* offY);

		/**
		 * @brief Gets the Priority of the GuiInstance
		 * 		Affects the order of processing when the GuiManager processes all of the GuiInstances it has.
		 * 		Note that the GuiCanvas element has a special priority value that will always be above the other priority values.
		 * 
		 * @return int 
		 */
		int getPriority();

		/**
		 * @brief Set the Base X for the GuiInstance.
		 * 		The is the unmodified x value for the object.
		 * 
		 * @param x 
		 */
		void setBaseX(int x);

		/**
		 * @brief Get the Base X for the GuiInstance.
		 * 
		 * @return int 
		 */
		int getBaseX();

		/**
		 * @brief Set the Base Y for the GuiInstance.
		 * 		The is the unmodified y value for the object.
		 * 
		 * @param y 
		 */
		void setBaseY(int y);

		/**
		 * @brief Get the Base Y for the GuiInstance.
		 * 
		 * @return int 
		 */
		int getBaseY();

		/**
		 * @brief Gets the x value for the GuiInstance.
		 * 		This will be modified by any offsets used.
		 * 
		 * @return int 
		 */
		int getX();

		/**
		 * @brief Gets the y value for the GuiInstance.
		 * 		This will be modified by any offsets used.
		 * 
		 * @return int 
		 */
		int getY();

		/**
		 * @brief Sets if the GuiInstance is visible.
		 * 
		 * @param is 
		 */
		void setVisible(bool is);

		/**
		 * @brief Gets if the GuiInstance is visible.
		 * 
		 * @return true 
		 * @return false 
		 */
		bool getVisible();

		/**
		 * @brief Sets if the GuiInstance is active.
		 * 
		 * @param is 
		 */
		void setActive(bool is);

		/**
		 * @brief Gets if the GuiInstance is active.
		 * 
		 * @return true 
		 * @return false 
		 */
		bool getActive();

		/**
		 * @brief Sets if the GuiInstance is focused.
		 * 
		 * @param is 
		 */
		void setFocus(bool is);

		/**
		 * @brief Gets if the GuiInstance is focused.
		 * 
		 * @return true 
		 * @return false 
		 */
		bool getFocus();

		/**
		 * @brief Sets if the GuiInstance is always focused.
		 * 
		 * @param is 
		 */
		void setAlwaysFocus(bool is);

		/**
		 * @brief Gets if the GuiInstance is always focused.
		 * 
		 * @return true 
		 * @return false 
		 */
		bool getAlwaysFocus();

		/**
		 * @brief Sets the canvas for the GuiInstance.
		 * 		This will specify that all drawing must be done specifically to this image.
		 * 
		 * @param m 
		 */
		void setCanvas(Image* m);

		/**
		 * @brief Returns the pointer to the canvas for the GuiInstance if it has one.
		 * 
		 * @return Image* 
		 */
		Image* getCanvas();

	protected:
		std::function<void(GuiInstance*)> onChangedFunction;

		/**
		 * @brief Gets the offset x position.
		 * 
		 * @return int 
		 */
		int getOffsetX();

		/**
		 * @brief Gets the offset y position.
		 * 
		 * @return int 
		 */
		int getOffsetY();

		/**
		 * @brief Gets the rendering offset x position.
		 * 
		 * @return int 
		 */
		int getRenderOffsetX();

		/**
		 * @brief Gets the rendering offset y position.
		 * 
		 * @return int 
		 */
		int getRenderOffsetY();

		int baseX = 0;
		int baseY = 0;

		int x = 0;
		int y = 0;

		int renderX = 0;
		int renderY = 0;

		/**
		 * @brief Sets the Priority for the GuiInstance.
		 * 		Affects the order of processing when the GuiManager processes all of the GuiInstances it has.
		 * 		Note that the GuiCanvas element has a special priority value that will always be above the other priority values.
		 * 
		 * @param value 
		 */
		void setPriority(int value);

		friend class GuiManager;

		/**
		 * @brief Sets the GuiManager for the GuiInstance.
		 * 		Once it has been set to a GuiManager, the GuiManager will manage the update and rendering functions for the object.
		 * 
		 * @param m 
		 */
		void setManager(GuiManager* m);

		/**
		 * @brief Gets the GuiManager for the GuiInstance.
		 * 
		 * @return GuiManager* 
		 */
		GuiManager* getManager();

	private:
		void updatePriority();

		int priority = 0;

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
		GuiInstance* parent = nullptr;
	};

	class GuiContainer : public GuiInstance
	{
	public:
		/**
		 * @brief Construct a new GuiContainer object
		 * 		It does not do anything. Primarily for containing children.
		 * 
		 */
		GuiContainer();

		/**
		 * @brief Destroy the GuiContainer object
		 * 
		 */
		~GuiContainer();

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;
		
		/**
		 * @brief An update function that does not do anything
		 * 
		 */
		void update();

		/**
		 * @brief An render function that does not do anything
		 * 
		 * @param surf
		 */
		void render(Image* surf);

	private:
	};

	class GuiCustomObject : public GuiInstance
	{
	public:
		/**
		 * @brief Construct a new GuiCustomObject object
		 * 		Utilizes std::function to allow custom update and render functions without having to create
		 * 		a new class.
		 * 
		 */
		GuiCustomObject();

		/**
		 * @brief Destroy the GuiCustomObject object
		 * 
		 */
		~GuiCustomObject();

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;
		
		/**
		 * @brief Calls the update function that has been set.
		 * 
		 */
		void update();

		/**
		 * @brief Calls the render function that has been set.
		 * 
		 * @param surf
		 */
		void render(Image* surf);

		/**
		 * @brief Sets the Update Function for the GuiCustomObject
		 * 
		 * @param func 
		 */
		void setUpdateFunction(std::function<void()> func);

		/**
		 * @brief Sets the Render Function for the GuiCustomObject
		 * 
		 * @param func 
		 */
		void setRenderFunction(std::function<void(Image*)> func);
	private:
		std::function<void()> updateFunc;
		std::function<void(Image*)> renderFunc;
	};

	class GuiCanvas : public GuiInstance
	{
	public:
		/**
		 * @brief Construct a new GuiCanvas object
		 * 		A canvas object allows other objects to redirect their drawing to this
		 * 		object.
		 * 		Specifically, their render function draws to the image contained in this object.
		 * 		Default Image is created with no width or height (An invalid image).
		 */
		GuiCanvas();

		/**
		 * @brief Construct a new GuiCanvas object
		 * 		A canvas object allows other objects to redirect their drawing to this
		 * 		object.
		 * 		Specifically, their render function draws to the image contained in this object.
		 * 
		 * @param width
		 * 		Width of the image.
		 * @param height
		 * 		Height of the image.
		 */
		GuiCanvas(int width, int height);

		/**
		 * @brief Destroy the GuiCanvas object
		 * 		Deletes the image as well.
		 */
		~GuiCanvas();

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;
		
		/**
		 * @brief Updates the GuiCanvas object
		 * 		Specifically, clears the image.
		 */
		void update();

		/**
		 * @brief Draws the GuiCanvas's Image on the surface at the objects x and y location.
		 * 
		 * @param surf 
		 */
		void render(Image* surf);

		/**
		 * @brief Sets the specified GuiInstance's canvas to this canvas.
		 * 
		 * @param ins 
		 */
		void setInstanceCanvas(GuiInstance* ins);

		/**
		 * @brief Gets the Clear Color for the image.
		 * 
		 * @return Color 
		 */
		Color getClearColor();

		/**
		 * @brief Sets the Clear Color for the image.
		 * 
		 * @param c 
		 */
		void setClearColor(Color c);
	private:
		Image myImage;
		Color clearColor = {0,0,0,0};
	};

	class GuiImage : public GuiInstance
	{
	public:
		/**
		 * @brief Construct a new GuiImage object
		 * 		Draws an image.
		 * 
		 * @param img 
		 * 		Default is nullptr.
		 */
		GuiImage(Image* img = nullptr);

		/**
		 * @brief Destroy the GuiImage object
		 * 		DOES NOT delete the image pointer.
		 * 
		 */
		~GuiImage();

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;
		
		/**
		 * @brief Does not do anything
		 * 
		 */
		void update();

		/**
		 * @brief Draws the image on the surface at this object's x and y position
		 * 
		 * @param surf 
		 */
		void render(Image* surf);

		/**
		 * @brief Sets the Image pointer
		 * 
		 * @param img 
		 */
		void setImage(Image* img);

		/**
		 * @brief Gets the Image pointer
		 * 
		 * @return Image* 
		 */
		Image* getImage();

	private:
		Image* img;
	};

	class GuiSprite : public GuiInstance
	{
	public:
		/**
		 * @brief Construct a new GuiSprite object
		 * 		It contains multiple Images that can be animated.
		 * 		Will use a copy of the Sprite
		 */
		GuiSprite();

		/**
		 * @brief Destroy the GuiSprite object
		 * 
		 */
		~GuiSprite();

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;
		
		/**
		 * @brief Updates the GuiSprite object
		 * 		Swaps to the next image in the sprite based on the delay time and how much time has passed.
		 * 
		 */
		void update();

		/**
		 * @brief Draws the GuiSprite object
		 * 
		 * @param surf 
		 */
		void render(Image* surf);

		/**
		 * @brief Sets the Sprite
		 * 
		 * @param img 
		 */
		void setSprite(Sprite img);

		/**
		 * @brief Gets the Sprite
		 * 		Note that it is a reference and not a copy.
		 * 
		 * @return Sprite& 
		 */
		Sprite& getSprite();

		/**
		 * @brief Sets the x scale for the sprite.
		 * 
		 * @param v 
		 */
		void setXScale(double v);

		/**
		 * @brief Sets the y scale for the sprite.
		 * 
		 * @param v 
		 */
		void setYScale(double v);
		
		/**
		 * @brief Gets the x scale for the sprite.
		 * 
		 * @return double 
		 */
		double getXScale();

		/**
		 * @brief Gets the y scale for the sprite.
		 * 
		 * @return double 
		 */
		double getYScale();

		/**
		 * @brief Sets the Color for the sprite.
		 * 
		 * @param c 
		 */
		void setColor(Color c);

		/**
		 * @brief Gets the Color for the sprite.
		 * 
		 * @return Color 
		 */
		Color getColor();

		/**
		 * @brief Force resets the sprite back to index 0.
		 * 
		 */
		void reset();

	private:
		time_t lastUpdateTime = 0;
		int index = 0;

		double xScale = 1;
		double yScale = 1;
		Sprite img;
		Color imgColor = {255,255,255,255};
	};

	class GuiTextBlock : public GuiInstance
	{
	public:
		/**
		 * @brief Construct a new GuiTextBlock object
		 * 		It draws text at some location with a specified max width and height.
		 * 		Allows for linebreaks as well.
		 * 
		 * @param x 
		 * @param y 
		 * @param width 
		 * @param height 
		 */
		GuiTextBlock(int x, int y, int width, int height);

		/**
		 * @brief Destroy the GuiTextBlock object
		 * 
		 */
		~GuiTextBlock();

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;

		/**
		 * @brief Updates the GuiTextBlock object.
		 * 		Does nothing
		 * 
		 */
		void update();

		/**
		 * @brief Renders the GuiTextBlock object.
		 * 
		 * @param surf 
		 */
		void render(Image* surf);

		/**
		 * @brief Sets the Text Color
		 * 
		 * @param c 
		 */
		void setTextColor(Color c);

		/**
		 * @brief Gets the Text Color
		 * 
		 * @return Color
		 */
		Color getTextColor();

		/**
		 * @brief Sets the Highlight Color
		 * 		The color for any highlighted section of text.
		 * 
		 * @param c 
		 */
		void setHighlightColor(Color c);

		/**
		 * @brief Gets the Highlight Color
		 * 		The color for any highlighted section of text.
		 * 
		 * @return Color 
		 */
		Color getHighlightColor();

		/**
		 * @brief Sets whether the text should be allowed to have highlighted sections.
		 * 
		 * @param v 
		 */
		void setShouldHighlightText(bool v);

		/**
		 * @brief Gets whether the text should be allowed to have highlighted sections.
		 * 
		 * @return true
		 * @return false
		 */
		bool getShouldHighlightText();

		/**
		 * @brief Sets the position to start the highlighted section of text
		 * 
		 * @param v 
		 */
		void setHighlightStart(int v);

		/**
		 * @brief Gets the position of the start of the highlighted section of text
		 * 
		 * @return int 
		 */
		int getHighlightStart();

		/**
		 * @brief Sets the position to end the highlighted section of text
		 * 
		 * @param v 
		 */
		void setHighlightEnd(int v);

		/**
		 * @brief Gets the position of the end of the highlighted section of text
		 * 
		 * @return int 
		 */
		int getHighlightEnd();

		/**
		 * @brief Sets whether to allow linebreaks in the text.
		 * 
		 * @param v 
		 */
		void setAllowLineBreaks(bool v);

		/**
		 * @brief Gets whether linebreaks are allowed in the text.
		 * 
		 * @return true 
		 * @return false 
		 */
		bool getAllowLineBreaks();

		/**
		 * @brief Gets a copy of the text displayed
		 * 
		 * @return std::string 
		 */
		std::string getText();

		/**
		 * @brief Gets a reference to the text displayed
		 * 
		 * @return std::string& 
		 */
		std::string& getTextRef();

		/**
		 * @brief Sets the Text displayed
		 * 
		 * @param s 
		 */
		void setText(std::string s);

		/**
		 * @brief Sets a specific font to use.
		 * 		If nullptr, the default font set in the Graphics class will be used.
		 * 
		 * @param f 
		 */
		void setFont(Font* f);

		/**
		 * @brief Gets the Font used.
		 * 		If nullptr, the default font set in the Graphics class is being used.
		 * 
		 * @return Font* 
		 */
		Font* getFont();

		/**
		 * @brief Sets the Width of the text block
		 * 
		 * @param v 
		 */
		void setWidth(int v);

		/**
		 * @brief Sets the Height of the text block
		 * 
		 * @param v 
		 */
		void setHeight(int v);

		/**
		 * @brief Gets the Width of the text block
		 * 
		 * @return int 
		 */
		int getWidth();

		/**
		 * @brief Gets the Height of the text block
		 * 
		 * @return int 
		 */
		int getHeight();

		/**
		 * @brief Sets the Offset X position of the text.
		 * 
		 * @param x 
		 */
		void setOffsetX(int x);

		/**
		 * @brief Sets the Offset Y position of the text.
		 * 
		 * @param y 
		 */
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
		/**
		 * @brief Construct a new GuiTextBox object
		 * 		Allows a user to type information into the box.
		 * 
		 * @param x 
		 * 		X value of the box
		 * @param y 
		 * 		Y value of the box
		 * @param width 
		 * 		Width of the box
		 * @param height 
		 * 		Height of the box
		 */
		GuiTextBox(int x, int y, int width, int height);

		/**
		 * @brief Construct a new GuiTextBox object from another GuiTextBox object
		 * 
		 * @param other 
		 */
		GuiTextBox(const GuiTextBox& other);

		/**
		 * @brief Copies a GuiTextBox object
		 * 
		 * @param other 
		 */
		void operator=(const GuiTextBox& other);

		/**
		 * @brief Copies a GuiTextBox object
		 * 
		 * @param other 
		 */
		void copy(const GuiTextBox& other);

		/**
		 * @brief Destroy the GuiTextBox object
		 * 
		 */
		~GuiTextBox();

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;

		/**
		 * @brief Updates the GuiTextBox object
		 * 
		 */
		void update();

		/**
		 * @brief Draws the GuiTextBox object
		 * 
		 * @param surf 
		 */
		void render(Image* surf);

		/**
		 * @brief Sets the OnEnterPressed Function
		 * 
		 * @param func 
		 */
		void setOnEnterPressedFunction(std::function<void(GuiInstance*)> func);

		/**
		 * @brief Sets the OnKeyPressed Function
		 * 
		 * @param func 
		 */
		void setOnKeyPressFunction(std::function<void(GuiInstance*)> func);

		/**
		 * @brief Sets the Background Color for the text box
		 * 
		 * @param c 
		 */
		void setBackgroundColor(Color c);

		/**
		 * @brief Sets the Outline Color for the text box
		 * 
		 * @param c 
		 */
		void setOutlineColor(Color c);

		/**
		 * @brief Sets the Focus Outline Color for the text box
		 * 
		 * @param c 
		 */
		void setFocusOutlineColor(Color c);

		/**
		 * @brief Sets the Cursor Blink Color for the text box
		 * 
		 * @param c 
		 */
		void setCursorBlinkColor(Color c);

		/**
		 * @brief Gets the TextBlock Element which contains the text data for the text box.
		 * 
		 * @return GuiTextBlock* 
		 */
		GuiTextBlock* getTextBlockElement();

		/**
		 * @brief Sets the Cursor Blink Timer in frames.
		 * 		(May be changed to time in microseconds)
		 * 
		 * @param timeInFrames 
		 */
		void setCursorBlinkTimer(int timeInFrames);

		/**
		 * @brief Sets the width of the Cursor
		 * 
		 * @param w 
		 */
		void setCursorWidth(int w);

		/**
		 * @brief Sets the Width of the text box
		 * 
		 * @param v 
		 */
		void setWidth(int v);

		/**
		 * @brief Sets the Height of the text box
		 * 
		 * @param v 
		 */
		void setHeight(int v);

		/**
		 * @brief Gets the Width of the text box
		 * 
		 * @return int 
		 */
		int getWidth();

		/**
		 * @brief Gets the Height of the text box
		 * 
		 * @return int 
		 */
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
		/**
		 * @brief Construct a new GuiRectangleButton object
		 * 
		 * @param x 
		 * @param y 
		 * @param width 
		 * @param height 
		 */
		GuiRectangleButton(int x, int y, int width, int height);

		/**
		 * @brief Construct a new GuiRectangleButton object from another GuiRectangleButton
		 * 
		 * @param other 
		 */
		GuiRectangleButton(const GuiRectangleButton& other);

		/**
		 * @brief Copies a GuiRectangleButton object
		 * 
		 * @param other 
		 */
		void operator=(const GuiRectangleButton& other);

		/**
		 * @brief Copies a GuiRectangleButton object
		 * 
		 * @param other 
		 */
		void copy(const GuiRectangleButton& other);

		/**
		 * @brief Destroy the GuiRectangleButton object
		 * 
		 */
		~GuiRectangleButton();

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;
		
		/**
		 * @brief Updates the GuiRectangleButton
		 * 
		 */
		void update();

		/**
		 * @brief Renders the GuiRectangleButton
		 * 
		 * @param surf
		 */
		void render(Image* surf);

		/**
		 * @brief Sets the OnClickFunction for the button
		 * 
		 * @param func 
		 */
		void setOnClickFunction(std::function<void(GuiInstance*)> func);

		/**
		 * @brief Sets the OnClickHoldFunction for the button
		 * 
		 * @param func 
		 */
		void setOnClickHoldFunction(std::function<void(GuiInstance*)> func);

		/**
		 * @brief Sets the OnClickReleaseFunction for the button
		 * 
		 * @param func 
		 */
		void setOnClickReleaseFunction(std::function<void(GuiInstance*)> func);

		/**
		 * @brief Sets the Background Color for the button
		 * 
		 * @param c 
		 */
		void setBackgroundColor(Color c);

		/**
		 * @brief Sets the Outline Color for the button
		 * 
		 * @param c 
		 */
		void setOutlineColor(Color c);

		/**
		 * @brief Sets the Focus Outline Color for the button
		 * 
		 * @param c 
		 */
		void setFocusOutlineColor(Color c);

		/**
		 * @brief Sets the Focus Background Color for the button
		 * 
		 * @param c 
		 */
		void setFocusBackgroundColor(Color c);

		/**
		 * @brief Sets the Hover Color for the button
		 * 
		 * @param c 
		 */
		void setHoverColor(Color c);

		/**
		 * @brief Sets the Width of the button
		 * 
		 * @param v 
		 */
		void setWidth(int v);

		/**
		 * @brief Sets the Height of the button
		 * 
		 * @param v 
		 */
		void setHeight(int v);

		/**
		 * @brief Gets the Width of the button
		 * 
		 * @return int 
		 */
		int getWidth();

		/**
		 * @brief Gets the Height of the button
		 * 
		 * @return int 
		 */
		int getHeight();

	private:
		std::function<void(GuiInstance*)> onClickFunction;
		std::function<void(GuiInstance*)> onClickHoldFunction;
		std::function<void(GuiInstance*)> onClickReleaseFunction;

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
		/**
		 * @brief Construct a new GuiScrollBar object
		 * 		It creates a horizontal or vertical scroll bar.
		 * 		By default, it is vertical.
		 * 		The scrolling area will be sized according to the startX,startY,endX,endY values. The button for scrolling will be based on that.
		 * 		Can use up to 3 GuiRectangleButtons as children.
		 * 
		 * @param startX 
		 * 		Start x of the scrolling area
		 * @param startY 
		 * 		Start y of the scrolling area
		 * @param endX 
		 * 		End x of the scrolling area
		 * @param endY 
		 * 		End y of the scrolling area
		 */
		GuiScrollBar(int startX, int startY, int endX, int endY);

		/**
		 * @brief Construct a new GuiScrollBar object from an existing one
		 * 
		 * @param other 
		 */
		GuiScrollBar(const GuiScrollBar& other);

		/**
		 * @brief Copies a GuiScrollBar
		 * 
		 * @param other 
		 */
		void operator=(const GuiScrollBar& other);

		/**
		 * @brief Copies a GuiScrollBar
		 * 
		 * @param other 
		 */
		void copy(const GuiScrollBar& other);

		/**
		 * @brief Destroy the GuiScrollBar object
		 * 
		 */
		~GuiScrollBar();

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;

		/**
		 * @brief Updates the GuiScrollBar
		 * 
		 */
		void update();

		/**
		 * @brief Renders the GuiScrollBar
		 * 
		 * @param surf 
		 */
		void render(Image* surf);

		/**
		 * @brief Sets the onSlideFunction.
		 * 		Occurs when the scroll bar moves from its current position for any reason
		 * 
		 * @param func 
		 */
		void setOnSlideFunction( std::function<void(GuiInstance*)> func);

		/**
		 * @brief Sets the Background Color of the scroll bar
		 * 
		 * @param c 
		 */
		void setBackgroundColor(Color c);

		/**
		 * @brief Gets the Background Color of the scroll bar
		 * 
		 * @return Color 
		 */
		Color getBackgroundColor();

		/**
		 * @brief Sets the Outline Color of the scroll bar
		 * 
		 * @param c 
		 */
		void setOutlineColor(Color c);

		/**
		 * @brief Gets the Outline Color of the scroll bar
		 * 
		 * @return Color 
		 */
		Color getOutlineColor();

		/**
		 * @brief Sets the Start X of the scroll bar area
		 * 
		 * @param x 
		 */
		void setStartX(int x);

		/**
		 * @brief Gets the Start X of the scroll bar area
		 * 
		 * @return int 
		 */
		int getStartX();

		/**
		 * @brief Sets the Start Y of the scroll bar area
		 * 
		 * @param y 
		 */
		void setStartY(int y);

		/**
		 * @brief Gets the Start Y of the scroll bar area
		 * 
		 * @return int 
		 */
		int getStartY();

		/**
		 * @brief Sets the End X of the scroll bar area
		 * 
		 * @param x 
		 */
		void setEndX(int x);

		/**
		 * @brief Gets the End X of the scroll bar area
		 * 
		 * @return int 
		 */
		int getEndX();

		/**
		 * @brief Sets the End Y of the scroll bar area
		 * 
		 * @param y 
		 */
		void setEndY(int y);

		/**
		 * @brief Gets the End Y of the scroll bar area
		 * 
		 * @return int 
		 */
		int getEndY();

		/**
		 * @brief Sets the Minimum width of the scroll bar button
		 * 
		 * @param w 
		 */
		void setMinWidth(int w);

		/**
		 * @brief Gets the Minimum width of the scroll bar button
		 * 
		 * @return int 
		 */
		int getMinWidth();

		/**
		 * @brief Sets the Minimum height of the scroll bar button
		 * 
		 * @param w 
		 */
		void setMinHeight(int h);

		/**
		 * @brief Gets the Minimum height of the scroll bar button
		 * 
		 * @return int 
		 */
		int getMinHeight();

		/**
		 * @brief Sets the amount of steps in the scroll bar.
		 * 		This controls where the scroll bar can be / how much to move when the buttons are pressed.
		 * 		
		 * @param s 
		 */
		void setSteps(int s);

		/**
		 * @brief Gets the amount of steps in the scroll bar.
		 * 
		 * @return int 
		 */
		int getSteps();

		/**
		 * @brief Sets the Current Step for the scroll bar.
		 * 		Sets where the scroll bar is relative to the maximum allowed steps.
		 * 		Must be less than or equal to the maximum allowed steps
		 * 
		 * @param s 
		 */
		void setCurrentStep(int s);

		/**
		 * @brief Gets the Current Step for the scroll bar.
		 * 
		 * @return int 
		 */
		int getCurrentStep();

		/**
		 * @brief Sets whether the scroll bar is vertical or horizontal.
		 * 		By default, it is vertical.
		 * 
		 * @param v 
		 */
		void setHorizontalBar(bool v);

		/**
		 * @brief Gets whether the scroll bar is vertical or horizontal.
		 * 
		 * @return true 
		 * @return false 
		 */
		bool getHorizontalBar();

		/**
		 * @brief Sets whether to show the scroll bar buttons.
		 * 		By default, it is set to true.
		 * 
		 * @param v 
		 */
		void setShowScrollButtons(bool v);

		/**
		 * @brief Gets whether to show the scroll bar buttons.
		 * 
		 * @return true 
		 * @return false 
		 */
		bool getShowScrollButtons();

		/**
		 * @brief Scrolls the bar forward by 1 step.
		 * 		Equivalent to pressing the increase button once.
		 * 
		 */
		void increaseScroll();

		/**
		 * @brief Scrolls the bar backwards by 1 step.
		 * 		Equivalent to pressing the decrease button once.
		 * 
		 */
		void decreaseScroll();

		/**
		 * @brief Gets the Button Element used for the scroll bar button
		 * 
		 * @return GuiRectangleButton* 
		 */
		GuiRectangleButton* getButtonElement();

		/**
		 * @brief Gets the Button Element used for the scroll bar decrease button
		 * 
		 * @return GuiRectangleButton* 
		 */
		GuiRectangleButton* getDecreaseButtonElement();

		/**
		 * @brief Gets the Button Element used for the scroll bar increase button
		 * 
		 * @return GuiRectangleButton* 
		 */
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
		/**
		 * @brief Construct a new GuiManager object
		 * 		Manages a list of GuiInstances updating and rendering them onto a Image.
		 * 		Constructs a Image of default size 320x240
		 */
		GuiManager();

		/**
		 * @brief Construct a new GuiManager object
		 * 		Manages a list of GuiInstances updating and rendering them onto a Image.
		 * 
		 * @param width 
		 * @param height 
		 */
		GuiManager(int width, int height);

		/**
		 * @brief Destroy the GuiManager object
		 * 		If the GuiManager owns the image, it will be deleted as well.
		 * 		All GuiInstances managed will have their GuiManager pointer set to nullptr.
		 */
		~GuiManager();

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;

		/**
		 * @brief Adds a GuiInstance to be managed.
		 * 
		 * @param k 
		 */
		void addElement(GuiInstance* k);

		/**
		 * @brief Removes a GuiInstance from the list.
		 * 
		 * @param k 
		 */
		void deleteElement(GuiInstance* k);

		/**
		 * @brief Gets a list of all the GuiInstances managed
		 * 
		 * @return std::vector<GuiInstance*> 
		 */
		std::vector<GuiInstance*> getElements();

		/**
		 * @brief Updates all of the GuiInstances
		 * 
		 */
		void updateGuiElements();

		/**
		 * @brief Renders all of the GuiInstances
		 * 
		 */
		void renderGuiElements();

		/**
		 * @brief Gets the Image for the GuiManager
		 * 
		 * @return Image* 
		 */
		Image* getImage();

		/**
		 * @brief Gets the amount of GuiInstances in the list.
		 * 
		 * @return size_t 
		 */
		size_t getSize();

		/**
		 * @brief Resizes the Image for the GuiManager.
		 * 
		 * @param width 
		 * @param height 
		 */
		void resizeImage(int width, int height);

		/**
		 * @brief Sets the x location for the window.
		 * 		Used for offsetting mouse x position.
		 * 
		 * @param v 
		 */
		void setWindowX(int v);

		/**
		 * @brief Sets the y location for the window.
		 * 		Used for offsetting mouse y position.
		 * 
		 * @param v 
		 */
		void setWindowY(int v);

		/**
		 * @brief Gets the x location for the window.
		 * 
		 * @return int 
		 */
		int getWindowX();

		/**
		 * @brief Gets the y location for the window.
		 * 
		 * @return int 
		 */
		int getWindowY();

		/**
		 * @brief Gets the Background Color for the Image
		 * 		The image is cleared to this color before any rendering occurs
		 * 
		 * @return Color 
		 */
		Color getBackgroundColor();

		/**
		 * @brief Sets the Background Color for the Image
		 * 		The image is cleared to this color before any rendering occurs
		 * 
		 * @param c 
		 */
		void setBackgroundColor(Color c);

	private:
		void sortElements();

		std::vector<GuiInstance*> objects = std::vector<GuiInstance*>();
		
		Image surf;

		int windowX = 0;
		int windowY = 0;

		Color backgroundColor = { 0xA2, 0xB9, 0xBC, 0xFF };
	};

} //NAMESPACE glib END