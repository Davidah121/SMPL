#pragma once

#include "GraphicsExt.h"
#include "ColorNameConverter.h"
#include "SimpleXml.h"
#include "SimpleHashMap.h"
#include <unordered_map>
#include <unordered_set>
#include "SmartMemory.h"
#include "ResourceManager.h"

namespace glib
{


    #pragma region BASE_GUI_CLASSES
    class GuiManager;
    class GuiItem;

    template<typename T>
    bool loadValueStuff(T& value, std::string data)
    {
        //check for %
        size_t indexOfPercent = data.find('%');
        if(indexOfPercent != SIZE_MAX)
        {
            //is percent
            try
            {
                value = (T)(GuiItem::PERCENTAGE_DIVIDE * (StringTools::toDouble(data.substr(indexOfPercent)) / 100.0));
            }
            catch(...) { }
            
            return true;
        }
        else
        {
            try
            {
                value = (T)StringTools::toDouble(data);
            }
            catch(...) { }
            return false;
        }
        return false;
    }
    Color loadColor(std::string data);

    class GuiResourceManager
	{
	public:
		~GuiResourceManager();

		void addSprite(SpriteInterface* data, std::string key, bool array, bool localMemory);
		void addFont(FontInterface* data, std::string key, bool array, bool localMemory);

		SmartMemory<SpriteInterface> getSprite(std::string key);
		SmartMemory<FontInterface> getFont(std::string key);
		
		void deleteSprite(std::string key);
		void deleteFont(std::string key);
		
		ResourceManager<SpriteInterface>* getSpriteResourceManager();
		ResourceManager<FontInterface>* getFontResourceManager();
		
		static GuiResourceManager& getResourceManager();
	private:
		GuiResourceManager();
		ResourceManager<SpriteInterface> spriteResources = ResourceManager<SpriteInterface>();
		ResourceManager<FontInterface> fontResources = ResourceManager<FontInterface>();

		static GuiResourceManager singleton;
	};

    /**
     * @brief GRect - A Rectangle. It has no additional properties and is meant as a replacement for Windows RECT for cross platform development.
     * A little different since it uses uint32_t instead of LONG
     */
    struct GRect
    {
        uint32_t left;
        uint32_t top;
        uint32_t right;
        uint32_t bottom;
    };

    //GuiItem - The base class that everything is built off of. It contains the static flag constants, 
    /**
     * @brief GuiItem - The base class that every other gui element should extend from.
     *      It contains the static flag constants, the type of item, position stuff, and some basic functions.
     *      It has 3 functions that must be implemented in any class that extends from it.
     *          virtual void layoutUpdate(int offX, int offY, int maximumWidth, int maximumHeight);
     *          virtual void update(SmartMemory<GuiManager>);
     *          virtual void render(SurfaceInterface*);
     * 
     *      With regards to position, position is affected by the layout so any offsets should be set by the margin if it is
     *          a layout. TrueX and TrueY refer to its location on screen after being adjusted by the parents position if it
     *          has a parent. These are updated by the function fixPosition() which is called during the preUpdate() function.
     * 
     */
    class GuiItem : public Object
    {
    public:
        static const unsigned char TYPE_EMPTY = 0;
        static const unsigned char TYPE_CONTENT = 1;
        static const unsigned char TYPE_LAYOUT = 2;

        static const uint16_t FLAG_MIN_WIDTH_PERCENTAGE = 0x01;
        static const uint16_t FLAG_MIN_HEIGHT_PERCENTAGE = 0x02;
        static const uint16_t FLAG_MAX_WIDTH_PERCENTAGE = 0x04;
        static const uint16_t FLAG_MAX_HEIGHT_PERCENTAGE = 0x08;
        static const uint16_t FLAG_X_PERCENTAGE = 0x10;
        static const uint16_t FLAG_Y_PERCENTAGE = 0x20;
        
        static const uint16_t FLAG_AUTO_LEFT_MARGIN = 0x40;
        static const uint16_t FLAG_AUTO_RIGHT_MARGIN = 0x80;
        static const uint16_t FLAG_AUTO_TOP_MARGIN = 0x100;
        static const uint16_t FLAG_AUTO_BOTTOM_MARGIN = 0x200;
        static const uint16_t FLAG_ABSOLUTE_POSITION = 0x400;

        static const int32_t PERCENTAGE_DIVIDE = 0xFFFF;

        GuiItem();
        ~GuiItem();
        
		//Object and Class Stuff
		static const Class globalClass;

        /**
         * @brief This functions purpose is to adjust the TrueX and TrueY values for this
         *          item. The x and y values along with its parent position are used to derive the
         *          trueX and trueY values.
         *      This can be called anytime but it is called inside of the preUpdate function for
         *          itself and children.
         * 
         */
        void fixPosition();

        /**
         * @brief Quick function to reset position values for subsequent calls.
         *      Sets x, y, trueX, trueY, width, and height = 0
         * 
         */
        void resetPosition();

        /**
         * @brief Calls the fixPosition() function.
         *      It can be overriden by extended classes to do additional functionality.
         *      It should still call the fixPosition() function though.
         * 
         */
        virtual void preUpdate();

        /**
         * @brief Called when the GuiItem is being adjusted by a layout. This
         *          is called before any final adjustments are done to the position such
         *          as centering or right alignment.
         *      The maximum width and maximum height are accurate and should be used to
         *          further adjust the object.
         *      The object does not have to abide by the maximum width and height provided
         *          as the allowed width or height may be too small but it could have negative
         *          affects on the layout.
         * 
         * @param offX 
         * @param offY 
         * @param maximumWidth 
         * @param maximumHeight 
         */
        virtual void layoutUpdate(int offX, int offY, int maximumWidth, int maximumHeight);

        /**
         * @brief Called to update the object.
         *      Should be called every frame the GUI is updated.
         *      Should have a GuiManager passed through it but it can be nullptr.
         *          If it is nullptr, somethings will not work properly.
         *      Typically, it is called through the GuiManager which belongs to a window.
         *          This allows it to have its variables properly set.
         *          Input is polled through Input::pollInput() before calling these functions.
         * 
         *      If the object has children, they should be manually updated through this method.
         *          All layouts have this functionality by default.
         * @param manager 
         */
        virtual void update(SmartMemory<GuiManager> manager);

        /**
         * @brief Called to draw the object.
         *      Should be called whenever the object needs to be redrawn.
         *          (Currently all of the time)
         *      Does not need a surface interface passed through it as it will be bound
         *          before hand.
         *          (Will be removed)
         *      
         *      If the object has children, they should be manually rendered through this method.
         *          All layouts have this functionality by default.
         * @param surface 
         */
        virtual void render();
        
        bool getFocused(SmartMemory<GuiManager> manager);
        void setFocused(SmartMemory<GuiManager> manager, bool f);
        
        bool isColliding(int x, int y);

        int getType();

        std::string getNameID();

        void setParent(SmartMemory<GuiItem> p);
        SmartMemory<GuiItem> getParent();

        int getTrueX();
        int getTrueY();

        int32_t x = 0;
        int32_t y = 0;

        int32_t width = 0;
        int32_t height = 0;

        /**
		 * @brief Loads data from an Xml Attribute.
		 * 		This allows a large list of attributes to be defined
		 * 		and allow each class to deal with them in their own way.
		 * 		
		 * 		Will remove data from the hashmap if it has been processed.
		 * 
		 * @param attrib 
		 */
		void loadDataFromXML(SimpleHashMap<std::string, std::string>& attribs, SmartMemory<GuiManager> manager);

    protected:
        virtual void onChildRemoved(SmartMemory<GuiItem> removedChild);

        SmartMemory<GuiItem> parent = nullptr;
        int type = 0;

    private:
        friend GuiManager;
        int32_t trueX = 0;
        int32_t trueY = 0;

        std::string nameID = "";
    };

    /**
     * @brief GuiContent - A GuiItem that is just content. This content can be anything but it does not have
     *          children. It does not have any layout features so it must be placed inside of a layout to have
     *          the option of margin and padding.
     *      All content (images, text, etc.) should extend from this class. They can then be placed inside of different
     *         layouts to properly be positioned.
     * 
     *      Much like GuiItem, it has 3 functions that should be implemented in any class that extends from it.
     *          virtual void layoutUpdate(int offX, int offY, int maximumWidth, int maximumHeight);
     *              offX and offY are the desired x and y positions for this item. maximum width and height can be used to wrap text or scale.
     * 
     *          virtual void update(SmartMemory<GuiManager>);
     *          virtual void render(SurfaceInterface*);
     * 
     *      Content Width and Height must be set by the extended class. If set after the layout has been updated, it could lead to
     *          improper rendering. 
     * 
     *      Layout is updated before calling the update function.
     *      The first update is used for things like initialization.
     *      Rendering is not done on the first update but on subsequent calls.
     */
    class GuiContent : public GuiItem
    {
    public:
        GuiContent();
        ~GuiContent();
        
		//Object and Class Stuff
		static const Class globalClass;

        int contentWidth = 0;
        int contentHeight = 0;

        void loadDataFromXML(SimpleHashMap<std::string, std::string>& attribs, SmartMemory<GuiManager> manager);
    };

    /**
     * @brief GuiEmpty - A empty gui item. This can be used as a placeholder.
     *      It contains no information or rendering code.
     *      This is equivalent to a GuiContent with no rendering or update functionality.
     * 
     */
    class GuiEmpty : public GuiItem
    {
    public:
        GuiEmpty();
        ~GuiEmpty();
        
		//Object and Class Stuff
		static const Class globalClass;
        
        void layoutUpdate(int offX, int offY, int maximumWidth, int maximumHeight);
        void update();
        void render();

    	void loadDataFromXML(SimpleHashMap<std::string, std::string>& attribs, SmartMemory<GuiManager> manager);
		static SmartMemory<GuiItem> loadFunction(SimpleHashMap<std::string, std::string>& attributes, SmartMemory<GuiManager> manager);
    };

    /**
     * @brief GuiLayout - A gui item that contains children. These children are other GuiItems that
     *      may or maynot be other GuiLayouts.
     *      GuiLayouts have margin, padding, and a border.
     *          Margin represents the distance from other items in the parent layout.
     *          Padding represents the distance from the border to the content inside this layout.
     *          Border represents the size of the border edges.
     *      GuiLayouts have a background color and a border color as well.
     *      GuiLayouts can be repositioned inside of other layouts using alignment flags.
     *          Alignment Left
     *          Alignment Center
     *          Alignment Right
     *      Along with those flags, width, height, and position can all be percentage based. Width and Height can be limited to
     *          a minimum and maximum as well though these do not have to be followed by classes that extend GuiLayout
     *      
     *      Lastly, each layout has both a column span and a row span. This was added for the table layout so that every layout can access it.
     *      Ideally all flags would be implemented by the extended class however, it is not required. 
     *          A layout should specify what is or isn't supported.
     * 
     *      Any layout that does not have a parent is positioned in an absolute manner instead of a relative manner.
     */
    class GuiLayout : public GuiItem
    {
    public:
        GuiLayout();
        ~GuiLayout();
        
		//Object and Class Stuff
		static const Class globalClass;
        
        void preUpdate();
        void update(SmartMemory<GuiManager> manager);
        void baseRender();
        void render();
        void addChild(SmartMemory<GuiItem> c);
        void removeChild(SmartMemory<GuiItem> c);
        
        void setLeftAlign();
        void setTopAlign();
        void setCenterAlign(bool vertical);
        void setBottomAlign();
        void setRightAlign();
        void setMinWidthPercentage(bool t);
        void setMaxWidthPercentage(bool t);
        void setMinHeightPercentage(bool t);
        void setMaxHeightPercentage(bool t);
        void setXPercentage(bool t);
        void setYPercentage(bool t);
        void setAbsolutePosition(bool t);

        virtual void layoutUpdate(int offX, int offY, int maximumWidth, int maximumHeight);

        void setMargin(GRect r);
        void setPadding(GRect r);
        void setBorder(GRect r);
        
        void setBorderColor(glib::Color c);
        void setBackgroundColor(glib::Color c);

        uint16_t getFlags();
        GRect getMargin();
        GRect getPadding();
        GRect getBorder();
        glib::Color getBorderColor();
        glib::Color getBackgroundColor();
        
        void setMinWidth(uint16_t v);
        void setMaxWidth(uint16_t v);
        void setMinHeight(uint16_t v);
        void setMaxHeight(uint16_t v);

        void setColSpan(uint16_t c);
        void setRowSpan(uint16_t r);
        
        uint16_t getColSpan();
        uint16_t getRowSpan();
        
        void loadDataFromXML(SimpleHashMap<std::string, std::string>& attribs, SmartMemory<GuiManager> manager);
    protected:
        friend GuiItem;
        uint16_t flags = 0;
        void loadRectStuff(GRect& input, std::string data, bool autoAllowed);

        uint16_t maxWidth = 0xFFFF;
        uint16_t maxHeight = 0xFFFF;
        uint16_t minWidth = 0;
        uint16_t minHeight = 0;

        std::vector<SmartMemory<GuiItem>> children;
        GRect margin = {0,0,0,0};
        GRect padding = {0,0,0,0};
        GRect border = {0,0,0,0};

        glib::Color borderColor = {0,0,0,0};
        glib::Color backgroundColor = {0,0,0,0};

        uint16_t colSpan = 1;
        uint16_t rowSpan = 1;
    };

    /**
     * @brief GuiLayoutFixed - A special GuiLayout where everything is positioned in an absolute manner.
     *      The position and size of the other children does not affect each other. This allows objects to overlap.
     *      The position of a child is based on its margin + any offsets like the position of this layout, padding, and border.
     * 
     *      This layout is considered special since it is almost equivalent to its children not having a parent.
     * 
     */
    class GuiLayoutFixed : public GuiLayout
    {
    public:
        GuiLayoutFixed();
        ~GuiLayoutFixed();
        
		//Object and Class Stuff
		static const Class globalClass;
        
        void processLayout(int offX, int offY, int maximumWidth, int maximumHeight);

        void loadDataFromXML(SimpleHashMap<std::string, std::string>& attribs, SmartMemory<GuiManager> manager);
        static SmartMemory<GuiItem> loadFunction(SimpleHashMap<std::string, std::string>& attributes, SmartMemory<GuiManager> manager);
    private:
    };

    /**
     * @brief GuiLayoutList - A predefined GuiLayout where everything is laid out as a list.
     *      The position and size of the other children does affect each other so objects will not overlap.
     *      The size of the layout is dynamically adjusted to fit the children and if enabled, the children may wrap around
     *          to prevent extending pass the maximum allowed size. If wrapping around does not fix that issue, it will just
     *          allow it to extend pass the maximum size.
     * 
     *      This layout has 2 options. Whether it is a horizontal or vertical list and whether it is allowed to wrap around.
     * 
     */
    class GuiLayoutList : public GuiLayout
    {
    public:
        static const bool DIRECTION_VERTICAL = false;
        static const bool DIRECTION_HORIZONTAL = true;

        GuiLayoutList(bool direction = DIRECTION_VERTICAL, bool wrap = true);
        ~GuiLayoutList();
        
		//Object and Class Stuff
		static const Class globalClass;

        void setDirection(bool d);
        void setWrap(bool w);

        void layoutUpdate(int offX, int offY, int maximumWidth, int maximumHeight);
        
        void loadDataFromXML(SimpleHashMap<std::string, std::string>& attribs, SmartMemory<GuiManager> manager);
        static SmartMemory<GuiItem> loadFunction(SimpleHashMap<std::string, std::string>& attributes, SmartMemory<GuiManager> manager);
    private:
        bool direction = DIRECTION_VERTICAL;
        bool wrap = true;
        
        void fixAlignmentAndLayout(int autoMarginWidth, int autoMarginHeight, int startIndex, int endIndex);
    };

    /**
     * @brief GuiLayoutTable - A predefined GuiLayout where everything is laid out as a table.
     *      In a table, a row shares the same height across and a column shares the same width down.
     *      Also, it is possible for a single child to occupy more than 1 cell. This is done through the
     *          colspan and rowspan.
     * 
     *      The number of columns allowed per row must be set and defaults to 1.
     *          Empty GuiItems can be used to occupy unneeded cells. Spanning may also be used but results in a different appearance.
     *      Minimum width/height and maximum width/height is not directly supported and may not work as expected.
     *      
     *      It is possible to show the border that make up the table. This can be set using the setShowInnerGrid() function.
     *      Also, the table can have additional padding for the cells.
     */
    class GuiLayoutTable : public GuiLayout
    {
    public:
        GuiLayoutTable(uint16_t numColumns);
        ~GuiLayoutTable();
        
		//Object and Class Stuff
		static const Class globalClass;

        void setShowInnerGrid(bool v);
        void setNumberOfColumns(uint16_t c);
        void setInnerTablePadding(uint16_t p);

        void render();
        void layoutUpdate(int offX, int offY, int maximumWidth, int maximumHeight);
        
        void loadDataFromXML(SimpleHashMap<std::string, std::string>& attribs, SmartMemory<GuiManager> manager);
        static SmartMemory<GuiItem> loadFunction(SimpleHashMap<std::string, std::string>& attributes, SmartMemory<GuiManager> manager);
    private:
        static const uint8_t COLSPAN = 0b0001;
        static const uint8_t ROWSPAN = 0b0010;

        bool showInnerGrid = false;
        uint16_t columns = 1;
        uint16_t innerTablePadding = 0;

        //Just used for tables and rendering the edges in the table.
        std::vector<int> tableWidthPerCol;
        std::vector<int> tableHeightPerRow;

        std::vector< std::vector<uint8_t> > tableCellSkip;
    };


    #pragma endregion

    #pragma region STANDARD_GUI_ELEMENTS

    class GuiButton : public GuiLayoutList
    {
    public:
        GuiButton();
        ~GuiButton();
        
		//Object and Class Stuff
		static const Class globalClass;

        void update(SmartMemory<GuiManager> manager);

        void setOnClickFunction(std::function<void()> onClick);
        void setOnMouseInFunction(std::function<void()> onMouseIn);
        void setOnMouseOutFunction(std::function<void()> onMouseOut);

        void addChild(SmartMemory<GuiItem> child);
        
        void loadDataFromXML(SimpleHashMap<std::string, std::string>& attribs, SmartMemory<GuiManager> manager);
        static SmartMemory<GuiItem> loadFunction(SimpleHashMap<std::string, std::string>& attributes, SmartMemory<GuiManager> manager);
    private:
        bool hovering = false;
        bool depressed = false;
        std::function<void()> onClickFunc = nullptr;
        std::function<void()> onMouseInFunc = nullptr;
        std::function<void()> onMouseOutFunc = nullptr;
    };

    class GuiText : public GuiContent
    {
    public:
        GuiText();
        ~GuiText();
        
		//Object and Class Stuff
		static const Class globalClass;

        void layoutUpdate(int offX, int offY, int maximumWidth, int maximumHeight);
        void update(SmartMemory<GuiManager> manager);
        void render();

        void setText(std::string s);
        std::string getText();
        void setColor(Color c);
        void setHighlightColor(Color c);

        int getHighlightStartIndex();
        int getHighlightEndIndex();

        void increaseSelectIndex();
        void decreaseSelectIndex();
        void setSelectIndex(int index);
        GRect getCaretBox();

        void setSelectable(bool s);
        void setCanWrap(bool w);
        
        void loadDataFromXML(SimpleHashMap<std::string, std::string>& attribs, SmartMemory<GuiManager> manager);
        static SmartMemory<GuiItem> loadFunction(SimpleHashMap<std::string, std::string>& attributes, SmartMemory<GuiManager> manager);
    private:
        bool isSelectable = false;
        bool canWrap = false;
        bool selecting = false;
        int highlightStartIndex = -1;
        int highlightEndIndex = -1;
        int maxWidth = -1;
        int maxHeight = -1;
        std::string text;
        Color fontColor = {0,0,0,255};
        Color highlightColor = {64,128,255,128};
    };

    class GuiSprite : public GuiContent
    {
    public:
        GuiSprite();
        ~GuiSprite();
        
		//Object and Class Stuff
		static const Class globalClass;

        void layoutUpdate(int offX, int offY, int maximumWidth, int maximumHeight);
        void update(SmartMemory<GuiManager> manager);
        void render();

        void setSprite(SmartMemory<SpriteInterface> spr);
        void setColor(Color c);
        void setShouldScale(bool s);
        void setShouldMaintainAspectRatio(bool ar);
        void setShouldAnimate(bool a);
        void setShouldLoop(bool l);

        void setSpriteIndex(int i);
        
        void loadDataFromXML(SimpleHashMap<std::string, std::string>& attribs, SmartMemory<GuiManager> manager);
        static SmartMemory<GuiItem> loadFunction(SimpleHashMap<std::string, std::string>& attributes, SmartMemory<GuiManager> manager);
    private:
        bool shouldMaintainAspectRatio = false;
        bool isScalable = false;
        bool autoAnimate = false;
        bool shouldLoop = false;
        int spriteIndex = 0;

        size_t lastUpdateTime = 0;
        SmartMemory<SpriteInterface> spr = nullptr;

        int maxWidth = -1;
        int maxHeight = -1;
        Color imgColor = {255,255,255,255};
    };
    
    class GuiCanvas : public GuiContent
    {
    public:

    private:

    };

    class GuiTextBox : public GuiContent
    {
    public:
        GuiTextBox();
        ~GuiTextBox();

        //Object and Class Stuff
		static const Class globalClass;

        void preUpdate();
        void layoutUpdate(int offX, int offY, int maximumWidth, int maximumHeight);
        void update(SmartMemory<GuiManager> manager);
        void render();

        std::string getText();

        GuiLayoutList& getBoxLayout();
        GuiText& getTextElement();

        void setAllowLineBreaks(bool lb);
        void setDefaultText(std::string s);
        
        void loadDataFromXML(SimpleHashMap<std::string, std::string>& attribs, SmartMemory<GuiManager> manager);
        static SmartMemory<GuiItem> loadFunction(SimpleHashMap<std::string, std::string>& attributes, SmartMemory<GuiManager> manager);
    private:
        void keyboardInput();
        bool allowLineBreaks = false;
        bool textEmpty = true;
        std::string defaultText = "";
        GuiLayoutList boxLayout = GuiLayoutList();
        GuiText textElement = GuiText();
    };

    class GuiScrollBar : public GuiContent
    {

    };

    class GuiDatePicker : public GuiContent
    {

    };

    #pragma endregion

    #pragma region GUI_MANAGER

    class GuiManager : public Object
    {
    public:
        GuiManager(unsigned char type);
        GuiManager(unsigned char type, int width, int height);
        ~GuiManager();

        //Class Stuff
        static const Class globalClass;

        void disposeObjects();

        void addElement(SmartMemory<GuiItem> k);
        void removeElement(SmartMemory<GuiItem> k);

        void setID(SmartMemory<GuiItem> k, std::string s);
        void addToDisposeList(SmartMemory<GuiItem> k);
        void setObjectInFocus(SmartMemory<GuiItem> k);
        bool getObjectInFocus(SmartMemory<GuiItem> k);

        void setBackgroundColor(Color c);

        /**
		 * @brief Updates all of the GuiInstances
		 * 
		 */
		void updateGuiElements();

		/**
		 * @brief Renders all of the GuiInstances
		 * 
		 */
		bool renderGuiElements();

        /**
		 * @brief Gets the Surface for the GuiManager
		 * 
		 * @return SurfaceInterface* 
		 */
		SurfaceInterface* getSurface();
        
		/**
		 * @brief Resizes the Image for the GuiManager.
		 * 
		 * @param width 
		 * @param height 
		 */
		void resizeImage(int width, int height);

		/**
		 * @brief Forces a redraw of everything that is visible.
		 * 		By default, only objects that request a redraw will be redrawn along with potential
		 * 		objects that it could affect with its redraw.
		 * 
		 */
		void invalidateImage();

		/**
		 * @brief Forces the GuiManager to always redraw everything that is visible every frame.
		 * 
		 * @param v
		 */
		void alwaysInvalidateImage(bool v);

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
		 * @brief Gets the Mouse X position relative to the window
		 * 
		 * @return int 
		 */
		int getMouseX();

		/**
		 * @brief Gets the Mouse Y position relative to the window
		 * 
		 * @return int 
		 */
		int getMouseY();

        /**
		 * @brief Gets whether the GuiManager is in focus or not.
		 * 		This is equivalent to the window being focused.
		 * 
		 * @return true 
		 * @return false 
		 */
		bool getFocus();

		/**
		 * @brief Sets whether the GuiManager is in focus or not.
		 * 		If it is attached to a SimpleWindow, that window will call this function when the window is focused.
		 * 		Otherwise, call this function when the GuiManager should be in focus.
		 * 
		 * @param v 
		 */
		void setFocus(bool v);

        /**
		 * @brief Gets the Graphics Interface Mode used by the GuiManager.
		 * 
		 * @return int 
		 */
		int getGraphicsInterfaceMode();

        GuiLayoutFixed& getRootLayout();

        /**
		 * @brief Gets a list of GuiInstances with the specified name/id.
		 * 
		 * @param name 
		 * @return std::vector<HashPair<std::string, SmartMemory<GuiItem>>*> 
		 */
		std::vector< HashPair<std::string, SmartMemory<GuiItem>>* > getItemsByName(std::string name);

        /**
		 * @brief Loads a series of GuiInstances from a file. All instances loaded from the file are managed by the
		 * 		GuiManager and will be freed when no longer needed.
		 * 		A valid load function must be registered in order to load a particular instance from the file.
		 * 	
		 * @param f 
		 */
		void loadElementsFromFile(File f);

		void loadElementsFromXML(SimpleXml& f);

		/**
		 * @brief Adds a load function to the list of valid load functions that can be used when loading data from a file.
		 * 		The function must take in a unordered_map (hashmap) of parameters to be set.
		 * 		Not all parameters must be set and some are expected to be passed to the parent class.
		 * 
		 *  	The function must return a pointer of newly created memory as it will be added to a delete later list.
         *      The raw pointer should not be deleted by the programmer as it will mess with the Smart Memory Implementation used.
		 * 
		 * @param className 
		 * @param func 
		 */
		static void registerLoadFunction(std::string className, std::function<SmartMemory<GuiItem>(SimpleHashMap<std::string, std::string>&, SmartMemory<GuiManager>)> func);

		/**
		 * @brief Adds some default loading functions to be used when loading elements from a file.
		 * 		They are for the following classes:
		 * 			GuiContainer
		 * 			GuiGrid
		 * 			GuiList
		 * 			GuiContextMenu
		 * 			GuiRectangleButton
		 * 			GuiSprite
		 * 			GuiTextBlock
		 * 			GuiTextBox
		 */
		static void initDefaultLoadFunctions();

    private:
        // void addToRemovedObjectBox(SmartMemory<GuiItem> k);
		// void updateBounds(SmartMemory<GuiItem> k, int& redrawCount);
		bool loadElement(XmlNode* node, SmartMemory<GuiItem> parent);
        
        GuiLayoutFixed rootLayout = GuiLayoutFixed();
		std::vector<SmartMemory<GuiItem>> shouldDelete = std::vector<SmartMemory<GuiItem>>();
		static std::unordered_map<std::string, std::function<SmartMemory<GuiItem>(SimpleHashMap<std::string, std::string>&, SmartMemory<GuiManager>)> > elementLoadingFunctions;
		SimpleHashMap<std::string, SmartMemory<GuiItem>> objectsByName = SimpleHashMap<std::string, SmartMemory<GuiItem>>();

		int windowX = 0;
		int windowY = 0;
		bool isInFocus = false;
        SmartMemory<GuiItem> objectInFocus;
        Color backgroundColor = {0,0,0,0};

		Vec2f expectedSize;

		SurfaceInterface* surf;
		unsigned char graphicsInterfaceMode = GraphicsInterface::TYPE_DEFAULT;
    };

    #pragma endregion
}