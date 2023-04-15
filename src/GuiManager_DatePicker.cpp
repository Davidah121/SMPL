#include "GuiManager.h"
#include <ctime>

namespace glib
{
	std::string arrayOfMonths[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
	int arrayForDaysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	const Class GuiDatePicker::globalClass = Class("GuiDatePicker", {&GuiInstance::globalClass});

	GuiDatePicker::GuiDatePicker() : GuiInstance()
	{
		setClass(globalClass);
	}

	GuiDatePicker::GuiDatePicker(int x, int y, int width, int height, bool calendarChild) : GuiInstance()
	{
		setClass(globalClass);
		init(x, y, width, height, calendarChild);
	}
	
	GuiDatePicker::GuiDatePicker(const GuiDatePicker& other) : GuiInstance(other)
	{
		copy(other);
	}

	void GuiDatePicker::operator=(const GuiDatePicker& other)
	{
		GuiInstance::copy(other);
		copy(other);
	}

	void GuiDatePicker::copy(const GuiDatePicker& other)
	{
		setClass(globalClass);
		dispose();

		width = other.width;
		height = other.height;
		backgroundColor = other.backgroundColor;
		gridHeaderColor = other.gridHeaderColor;
		calendarIsChild = other.calendarIsChild;

		init(getBaseX(), getBaseY(), width, height, calendarIsChild);
	}

	GuiDatePicker::~GuiDatePicker()
	{
		dispose();
	}
	
	void GuiDatePicker::init(int x, int y, int width, int height, bool calendarChild)
	{
		setBaseX(x);
		setBaseY(y);
		calendarIsChild = calendarChild;

		this->width = width;
		this->height = height;

		this->gridWidth = width*2;
		this->gridHeight = width*2;
		
		//Get current date and convert to a string
		std::tm currentStoredDate = System::getCurrentDate();
		std::string dateString = StringTools::formatString("%02d/%02d/%04d", currentStoredDate.tm_mon+1, currentStoredDate.tm_mday, currentStoredDate.tm_year+1900);

		//setup
		mainContainer = GuiList(0, 0, false);
		mainContainer.setElementSpacing(12);

		//load sprites for later use
		leftButSprite = GuiSprite("./Resources/DefaultArrowLeft.png");
		rightButSprite = GuiSprite("./Resources/DefaultArrowRight.png");
		calendarButSprite = GuiSprite("./Resources/CogWheel.png");

		dateStringBox = GuiTextBox(0, 0, width, height);
		dateStringBox.getTextBlockElement()->setDefaultText("mm/dd/yyyy");
		dateStringBox.getTextBlockElement()->setText(dateString);
		dateStringBox.setBackgroundColor(backgroundColor);

		calendarButton = GuiRectangleButton(0, 0, 16, 16);
		calendarButton.setBackgroundColor(backgroundColor);
		calendarButton.setOutlineColor(Color{0,0,0,0});
		calendarButton.setFocusOutlineColor(Color{0,0,0,0});
		
		calendarButton.addChild(&calendarButSprite);

		calendarButton.setOnClickReleaseFunction( [this](glib::GuiInstance* ins) ->void{
			this->toggleCalendar();
		});

		mainContainer.addChild(&dateStringBox);
		mainContainer.addChild(&calendarButton);

		calendarStack = GuiList(0, 0, true); //next to the main textbox
		calendarStack.setElementSpacing(4);
		
		calendarStack.setBackgroundColor(backgroundColor);
		calendarStack.setOutlineColor({0,0,0,255});

		topHorizontalBar = GuiList(0, 0, false);
		topHorizontalBar.setElementSpacing(12);

		gridLeftBut = GuiRectangleButton(0, 0, 16, 16);
		gridLeftBut.setOutlineColor(Color{0,0,0,0});
		gridLeftBut.setFocusOutlineColor(Color{0,0,0,0});
		gridLeftBut.addChild(&leftButSprite);
		gridLeftBut.setOnClickReleaseFunction([this](glib::GuiInstance* ins) ->void{
			this->shiftLeftMonth();
		});

		gridCenterText = GuiTextBlock(0, 0, gridWidth-48, height);

		gridRightBut = GuiRectangleButton(0, 0, 16, 16);
		gridRightBut.setOutlineColor(Color{0,0,0,0});
		gridRightBut.setFocusOutlineColor(Color{0,0,0,0});
		gridRightBut.addChild(&rightButSprite);
		gridRightBut.setOnClickReleaseFunction([this](glib::GuiInstance* ins) ->void{
			this->shiftRightMonth();
		});

		//Set the header text of the grid to the current month and year
		gridCenterText.setText(arrayOfMonths[currentStoredDate.tm_mon] + " " + StringTools::toString(currentStoredDate.tm_yday+1900));
		
		topHorizontalBar.addChild(&gridLeftBut);
		topHorizontalBar.addChild(&gridCenterText);
		topHorizontalBar.addChild(&gridRightBut);

		topHorizontalBar.setBackgroundColor(gridHeaderColor);
		
		dateGrid = GuiGrid(0, 0);
		dateGrid.setMaxColumns(7);
		dateGrid.setMaxRows(7);
		dateGrid.setRowMajorOrder(false);
		dateGrid.setGridSpacing(4, 4);
		
		for(int i=0; i<7; i++)
		{
			gridText.push_back( new GuiTextBlock(0, 0, 24, 24) );
			dateGrid.addChild( gridText.back() );
		}

		gridText[0]->setText("sun");
		gridText[1]->setText("mon");
		gridText[2]->setText("tue");
		gridText[3]->setText("wed");
		gridText[4]->setText("thu");
		gridText[5]->setText("fri");
		gridText[6]->setText("sat");

		for(int i=0; i<42; i++)
		{
			GuiRectangleButton* newGridBut = new GuiRectangleButton(0, 0, 24, 24);
			newGridBut->setOnClickReleaseFunction( [this, i](glib::GuiInstance* ins) ->void{
				this->gridButtonPress(i+7);
			});

			gridButtons.push_back( newGridBut );
			gridText.push_back( new GuiTextBlock(0, 0, 24, 24) );
			
			// gridText.back()->setText( StringTools::toString(i) ); //Test value

			gridButtons.back()->addChild( gridText.back() );

			dateGrid.addChild( gridButtons.back() );
		}

		calendarStack.addChild(&topHorizontalBar);
		calendarStack.addChild(&dateGrid);

		mainContainer.setVisible(true);
		calendarStack.setVisible(false);
		calendarStack.setActive(false);

		addChild(&mainContainer);

		if(calendarIsChild)
		{
			mainContainer.addChild(&calendarStack);
		}
		else
		{
			//Calendar should be placed next to the list but not included in the bounds.
			//Treat it as a separate object.
			
			if(this->getManager() != nullptr)
				this->getManager()->addElement(&calendarStack);
		}


		setCalendarValues();
	}

	void GuiDatePicker::dispose()
	{
		for(size_t i=0; i<gridButtons.size(); i++)
		{
			if(gridButtons[i] != nullptr)
			{
				delete gridText[i];
				delete gridButtons[i];
			}
		}
		gridButtons.clear();
		gridText.clear();
	}

	void GuiDatePicker::update()
	{
		if(!calendarIsChild)
		{
			//add the calendar to the manager if it has not been done yet.
			if(this->getManager() != nullptr)
				this->getManager()->addElement(&calendarStack);
			
			//set the calendar stack to the right side of it.
			Box2D bounds = mainContainer.getBoundingBox();
			int newX = (int)MathExt::round(bounds.getLeftBound() + bounds.getWidth() + 16);
			int newY = (int)MathExt::round(bounds.getTopBound());

			calendarStack.setBaseX(newX);
			calendarStack.setBaseY(newY);
		}
	}

	void GuiDatePicker::render()
	{

	}
	
	void GuiDatePicker::setCalendarValues()
	{
		//Just don't set the previous month's or the next month's days
		//If the textbox has a date, attempt to use it.
		//Otherwise, use current time

		std::tm currentStoredDate;
		bool isLeapYear;
		bool isValid = true;

		std::string tempText = getDateString();
		if(tempText.empty())
		{
			isValid = false;
		}
		else
		{
			std::vector<std::string> splitString = StringTools::splitString(tempText, '/');

			if(splitString.size() == 3)
			{
				std::tm tempTime = {0, 0, 0, 0, 0, 0, 0, 0, 0};
				tempTime.tm_isdst = -1;
				try
				{
					tempTime.tm_mon = StringTools::toInt(splitString[0]) - 1;
					tempTime.tm_mday = StringTools::toInt(splitString[1]);
					tempTime.tm_year = StringTools::toInt(splitString[2]) - 1900;
				}
				catch(const std::exception& e)
				{
					isValid = false;
				}
				
				
				std::time_t asTimeT = std::mktime(&tempTime);
				if(asTimeT == -1)
					isValid = false;
				else
					currentStoredDate = System::convertTimeToDate(asTimeT);
			}
			else
			{
				isValid = false;
			}
		}

		if(!isValid)
		{
			currentStoredDate = System::getCurrentDate();
			std::string dateString = StringTools::formatString("%02d/%02d/%04d", currentStoredDate.tm_mon+1, currentStoredDate.tm_mday, currentStoredDate.tm_year+1900);
			
			setDateString(dateString);
		}

		std::string headerText = StringTools::formatString("%s %04d", arrayOfMonths[currentStoredDate.tm_mon].c_str(), currentStoredDate.tm_year+1900);
		gridCenterText.setText( headerText );

		//If feburary, check if leap year
		std::tm tempTime = {0, 0, 0, 0, 0, 0, 0, 0, 0};
		tempTime.tm_isdst = -1;
		tempTime.tm_mon = 1;
		tempTime.tm_mday = 29;
		tempTime.tm_year = currentStoredDate.tm_year;

		isLeapYear = std::mktime(&tempTime) != -1;

		//set things based on the currentStoredDate
		//check what column does the first day start on
		int daysFromStartAdjust = (currentStoredDate.tm_mday-1) % 7;
		int startColumn = currentStoredDate.tm_wday - daysFromStartAdjust;
		if(startColumn<0)
			startColumn+=7;

		int numDays = arrayForDaysInMonth[currentStoredDate.tm_mon];
		if(isLeapYear && currentStoredDate.tm_mon == 1)
			numDays++;
		
		for(size_t i=7; i<gridText.size(); i++)
		{
			gridText[i]->setText("");
			gridButtons[i-7]->setVisible(false);
		}

		for(int i=7+startColumn; i<7+startColumn+numDays; i++)
		{
			gridText[i]->setText( StringTools::toString( i - (7+startColumn) + 1 ) );
			gridButtons[i-7]->setVisible(true);
		}
	}

	void GuiDatePicker::gridButtonPress(int index)
	{
		//figure out the date based on the month and year.
		//Get day from the text of the button.
		if(index >= 0 && index <= (int)gridText.size())
		{
			std::string mDayText = gridText[index]->getText();
			if(mDayText.empty())
				return;
			
			int mDay = StringTools::toInt(mDayText);
			
			std::vector<std::string> headerTextSplit = StringTools::splitString(gridCenterText.getText(), ' ');
			if(headerTextSplit.size() == 2)
			{
				int mon = 0;
				int year = StringTools::toInt(headerTextSplit[1]);
				if(StringTools::equalsIgnoreCase<char>(headerTextSplit[0], "january"))
					mon = 1;
				else if(StringTools::equalsIgnoreCase<char>(headerTextSplit[0], "february"))
					mon = 2;
				else if(StringTools::equalsIgnoreCase<char>(headerTextSplit[0], "march"))
					mon = 3;
				else if(StringTools::equalsIgnoreCase<char>(headerTextSplit[0], "april"))
					mon = 4;
				else if(StringTools::equalsIgnoreCase<char>(headerTextSplit[0], "may"))
					mon = 5;
				else if(StringTools::equalsIgnoreCase<char>(headerTextSplit[0], "june"))
					mon = 6;
				else if(StringTools::equalsIgnoreCase<char>(headerTextSplit[0], "july"))
					mon = 7;
				else if(StringTools::equalsIgnoreCase<char>(headerTextSplit[0], "august"))
					mon = 8;
				else if(StringTools::equalsIgnoreCase<char>(headerTextSplit[0], "september"))
					mon = 9;
				else if(StringTools::equalsIgnoreCase<char>(headerTextSplit[0], "october"))
					mon = 10;
				else if(StringTools::equalsIgnoreCase<char>(headerTextSplit[0], "november"))
					mon = 11;
				else
					mon = 12;

				
				setDateString(StringTools::formatString("%02d/%02d/%04d", mon, mDay, year));
			}
		}

	}

	void GuiDatePicker::shiftRightMonth()
	{
		//Get the month and year from the header text.
		std::vector<std::string> headerTextSplit = StringTools::splitString(gridCenterText.getText(), ' ');
		if(headerTextSplit.size() == 2)
		{
			int mon = 0;
			int year = StringTools::toInt(headerTextSplit[1]);
			if(StringTools::equalsIgnoreCase<char>(headerTextSplit[0], "january"))
				mon = 1;
			else if(StringTools::equalsIgnoreCase<char>(headerTextSplit[0], "february"))
				mon = 2;
			else if(StringTools::equalsIgnoreCase<char>(headerTextSplit[0], "march"))
				mon = 3;
			else if(StringTools::equalsIgnoreCase<char>(headerTextSplit[0], "april"))
				mon = 4;
			else if(StringTools::equalsIgnoreCase<char>(headerTextSplit[0], "may"))
				mon = 5;
			else if(StringTools::equalsIgnoreCase<char>(headerTextSplit[0], "june"))
				mon = 6;
			else if(StringTools::equalsIgnoreCase<char>(headerTextSplit[0], "july"))
				mon = 7;
			else if(StringTools::equalsIgnoreCase<char>(headerTextSplit[0], "august"))
				mon = 8;
			else if(StringTools::equalsIgnoreCase<char>(headerTextSplit[0], "september"))
				mon = 9;
			else if(StringTools::equalsIgnoreCase<char>(headerTextSplit[0], "october"))
				mon = 10;
			else if(StringTools::equalsIgnoreCase<char>(headerTextSplit[0], "november"))
				mon = 11;
			else
				mon = 12;

			mon++;
			if(mon > 12)
			{
				mon = 1;
				year++;
			}
			
			setDateString(StringTools::formatString("%02d/%02d/%04d", mon, 1, year));
			setCalendarValues();
		}

	}

	void GuiDatePicker::shiftLeftMonth()
	{
		//Get the month and year from the header text.
		std::vector<std::string> headerTextSplit = StringTools::splitString(gridCenterText.getText(), ' ');
		if(headerTextSplit.size() == 2)
		{
			int mon = 0;
			int year = StringTools::toInt(headerTextSplit[1]);
			if(StringTools::equalsIgnoreCase<char>(headerTextSplit[0], "january"))
				mon = 1;
			else if(StringTools::equalsIgnoreCase<char>(headerTextSplit[0], "february"))
				mon = 2;
			else if(StringTools::equalsIgnoreCase<char>(headerTextSplit[0], "march"))
				mon = 3;
			else if(StringTools::equalsIgnoreCase<char>(headerTextSplit[0], "april"))
				mon = 4;
			else if(StringTools::equalsIgnoreCase<char>(headerTextSplit[0], "may"))
				mon = 5;
			else if(StringTools::equalsIgnoreCase<char>(headerTextSplit[0], "june"))
				mon = 6;
			else if(StringTools::equalsIgnoreCase<char>(headerTextSplit[0], "july"))
				mon = 7;
			else if(StringTools::equalsIgnoreCase<char>(headerTextSplit[0], "august"))
				mon = 8;
			else if(StringTools::equalsIgnoreCase<char>(headerTextSplit[0], "september"))
				mon = 9;
			else if(StringTools::equalsIgnoreCase<char>(headerTextSplit[0], "october"))
				mon = 10;
			else if(StringTools::equalsIgnoreCase<char>(headerTextSplit[0], "november"))
				mon = 11;
			else
				mon = 12;

			mon--;
			if(mon < 0)
			{
				mon = 12;
				year--;
			}
			
			setDateString(StringTools::formatString("%02d/%02d/%04d", mon, 1, year));
			setCalendarValues();
		}
	}
	
	void GuiDatePicker::toggleCalendar()
	{
		visibleCalendar = !visibleCalendar;
		calendarStack.setVisible( visibleCalendar );
		calendarStack.setActive( visibleCalendar );

		if(visibleCalendar)
			setCalendarValues();
	}

	std::string GuiDatePicker::getDateString()
	{
		return dateStringBox.getTextBlockElement()->getText();
	}

	void GuiDatePicker::setDateString(std::string text)
	{
		dateStringBox.getTextBlockElement()->setText(text);
		dateStringBox.setShouldRedraw(true);
	}

	void GuiDatePicker::solveBoundingBox()
	{
		//Do nothing as the grouping of all of the children's boxes is handled by the instance itself
		//Reset the bounds though.
		boundingBox = GuiInstance::getInvalidBox();
	}
	
	void GuiDatePicker::loadDataFromXML(SimpleHashMap<std::string, std::string>& attribs)
	{
		GuiInstance::loadDataFromXML(attribs);
		std::vector<std::string> possibleNames = { "width", "height", "backgroundcolor", "gridheadercolor", "calendarchild" };

		for(size_t i=0; i<possibleNames.size(); i++)
		{
			auto it = attribs.get(possibleNames[i]);
			if(it != nullptr)
			{
				if(possibleNames[i] == "width")
				{
					this->width = std::abs(StringTools::toInt(it->data));
				}
				else if(possibleNames[i] == "height")
				{
					this->height = std::abs(StringTools::toInt(it->data));
				}
				else if(possibleNames[i] == "backgroundcolor")
				{
					this->backgroundColor = ColorNameConverter::NameToColor(it->data);
				}
				else if(possibleNames[i] == "gridheadercolor")
				{
					this->gridHeaderColor = ColorNameConverter::NameToColor(it->data);
				}
				else if(possibleNames[i] == "calendarchild")
				{
					this->calendarIsChild = StringTools::equalsIgnoreCase<char>(it->data, "true");
				}

				attribs.remove(it);
			}
		}

		//Recreates the objects and sets all the stuff necessary.
		dispose();
		init(baseX, baseY, width, height, calendarIsChild);
	}

	GuiInstance* GuiDatePicker::loadFunction(SimpleHashMap<std::string, std::string>& attribs)
	{
		GuiDatePicker* ins = new GuiDatePicker();
		ins->loadDataFromXML(attribs);

		return ins;
	}
	void GuiDatePicker::registerLoadFunction()
	{
		GuiManager::registerLoadFunction("GuiDatePicker", GuiDatePicker::loadFunction);
	}
};