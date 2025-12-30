# Purpose
A cross platform GUI that can be converted to HTML/CSS/JS directly built upon the existing system.

There must be a means to translate the GUI and run it as if it were a webpage to circumvent the need for a rendering system and avoiding the need for more OS code / allowing the user to access different systems across other devices than the running system. I.E. running the application on a headless Linux computer but using it from your phone/other computer.

# How
In order to have this work, the needed basic GUI types must be able to create all forms of GUI. While they aren't as useful as a specific item, it does allow most things to be created

# Basic GUI Items
The basic items that are required to get other advanced gui items to work
- Image
    - width
    - height
    - src
- Text
    - font
    - size
    - style
        - bold
        - italics
        - strikethrough
        - underline
    - color
- LayoutList
    - style
        - vertical
        - horizontal
- LayoutGrid
- GridItem
    - row_span
    - column_span
- LayoutFixed
- ScrollableLayout

# Advanced GUI Items
These items are built upon existing items utilizing layouts and such. Often required unless you don't need iteraction
- Button
- Textbox

# Additional Notes
Some things such as how an object is placed, scaled, aspect ratio, etc. are ommitted here.
Style is ommitted as well but it is something that should be included/thought about

# General system rundown
Create Layouts that can hold additional items of any type. Place those items in those layouts. Let the layouts reposition everything according to their margins and padding. Auto Margin and Auto Padding allow for alignments (left, center, right, etc.)

Any object may be in focus. Those objects are the ones that should be considered for input. Meaning any object not in focus shouldn't interact with human input till they are in focus.

Objects may or may not need to be re-rendered and if so, the objects behind it may need to be re-rendered if transparency is a factor.
To simplify this process, assume all objects need to be re-rendered every frame something reports being changed. 

An advanced system can chose to calculate specifically what should be adjusted and limit it to the affected areas.

# Loading/Converting process
Converting between HTML/CSS/JS should be done simply. Pre-Written JS Code needed for button functionaility, etc. Converting must be done in software as converting in JS may run into serious issues with security.

Starting off simply. Title of the window can be written out as:
```HTML
<head>
    <title>INSERT TITLE</title>
<head>
```

Images can be written as:
```XML
<Sprite src="INSERT IMAGE" width="100" height="100">
```
vs
```HTML
<img src="INSERT FILE/LINK" style="width:100px; height:100px;">
```


Text can be written as:
```XML
<Text text="INSERT TEXT" font="FONT_NAME" color="COLOR"/>
```
vs
```HTML
<p style="font:FONT_NAME; color:COLOR;">INSERT TEXT</p>
```

LayoutList can be written as:
```XML
<!--VERTICAL LIST-->
<LayoutList direction="vertical">
    <Text text="item1"/>
    <Text text="item2"/>
</LayoutList>
<!--HORIZONAL LIST-->
<LayoutList direction="horizontal">
    <Text text="item1"/>
    <Text text="item2"/>
</LayoutList>
```
vs
```HTML
<!--VERTICAL LIST-->
<div>
    <p>item1</p>
    <p>item2</p>
</div>
<!--HORIZONAL LIST-->
<div>
    <p style="display:inline-block;">item1</p>
    <p style="display:inline-block;">item2</p>
</div>
```
Notice that the horizontal attribute of the list is something that is applied to the children and not the list itself. Also note that div is used instead of ul or anything like that.
Makes things simplier

LayoutGrid can be written as:
```XML
<LayoutGrid rows="2" columns="2">
    <Text text="item1"/>
    <Text text="item2"/>
    <Text text="item1"/>
    <Text text="item2"/>
</LayoutGrid>
```
vs
```HTML
<table>
    <tr>
        <td><p>item1</p></td>
        <td><p>item2</p></td>
    </tr>
    <tr>
        <td><p>item1</p></td>
        <td><p>item2</p></td>
    </tr>
</table>
```
Here using table, tr, td is probably required to have HTML to properly display it. Note that row isn't required in the base layout but results in less flexibility compared to HTML where a row does not need to use the same amount of columns. Ideally though, just don't do this or use span or empty objects that take up no area.

LayoutFixed can be written out as:
```XML
<LayoutFixed>
    <Text margin="{0, 0, 0, 0}" text="item1"/>
    <Text margin="{32, 32, 0, 0}" text="item2"/>
</LayoutFixed>
```
vs
```HTML
<div>
    <p style="position: absolute; margin-left: 0px; margin-top: 0px; ">item1</p>
    <p style="position: absolute; margin-left: 32px; margin-top: 32px; ">item2</p>
</div>
```
Just as with horizontal list, the fixed positioning is applied to the items and not the container.

Note that in order to simplify implementation, margins can only be applied to layouts but in practice, it doesn't matter much if underlying GuiItems have it

Note that at the very very start, a layout is placed. That layout is a fixed layout. 

To give an example of converting layouts:

```XML
<SimpleGUI>    
    <LayoutList>
        <!-- SECTION HEADER -->
        <LayoutList ID="WindowBar" margin="{16,16,0,0}" padding="{16,16,16,16}" border-color="red" border="{1,1,1,1}" direction="horizontal">
            <Button margin="{8,0,8,0}" background-color="gray" ID="WindowBar_Edit" min-width="96" min-height="24">
                <Text margin="{auto,auto,auto,auto}" ID="WindowBar_EditText" text="Insert"/>
            </Button>

            <Button margin="{8,0,8,0}" background-color="gray" ID="WindowBar_Search" min-width="96" min-height="24">
                <Text margin="{auto,auto,auto,auto}" text="Search"/>
            </Button>

            <Button margin="{8,0,8,0}" background-color="gray" ID="WindowBar_Upload" min-width="96" min-height="24">
                <Text margin="{auto,auto,auto,auto}" text="Upload Database"/>
            </Button>

            <Button margin="{8,0,8,0}" background-color="gray" ID="WindowBar_Help" min-width="96" min-height="24">
                <Text margin="{auto, auto, auto, auto}" text="Help"/>
            </Button>
            
            <LayoutList margin="{8,0,8,0}" direction="vertical">
                <Text margin="{auto, auto, auto, auto}" text="Inserting Entry" ID="WindowBar_Status1"/>
                <Text margin="{auto, auto, auto, auto}" text="Test2.0" ID="WindowBar_Status2"/>
            </LayoutList>
        </LayoutList>

        <!-- SECTION LEFT MAIN -->
        <LayoutList margin="{16, 64, 0, 0}" padding="{28, 28, 28, 28}">
            
            <LayoutList margin="{4, 0, 0, 0}" padding="{4, 4, 4, 4}" direction="horizontal">
                <Button ID="ModeSwitchID" min-width="16" min-height="16" visible="true">
                    <Sprite src="CancelButtonImg"/>
                </Button>
                <Text ID="MessageID" text="ID: NULL"/>
            </LayoutList>

        </LayoutList>
    </LayoutList>
</SimpleGUI>
```
vs
```HTML
<!DOCTYPE html>
<html>
    <body>
        <div>
            <!-- SECTION HEADER -->
            <div id="WindowBar" style="margin-left: 16px; margin-top: 16px; padding: 16px; border: 1px; border-color: red;">
                <button id="WindowBar_Edit" style="margin-left: 8px; margin-right: 8px; background-color: gray; min-width: 96px; min-height: 24px; display: inline-block;">
                    <p style="margin: auto;" id="WindowBar_EditText">Insert</p>
                </button>
                <button id="WindowBar_Edit" style="margin-left: 8px; margin-right: 8px; background-color: gray; min-width: 96px; min-height: 24px; display: inline-block;">
                    <p style="margin: auto;" id="WindowBar_EditText">Search</p>
                </button>
                <button id="WindowBar_Edit" style="margin-left: 8px; margin-right: 8px; background-color: gray; min-width: 96px; min-height: 24px; display: inline-block;">
                    <p style="margin: auto;" id="WindowBar_EditText">Upload Database</p>
                </button>
                <button id="WindowBar_Edit" style="margin-left: 8px; margin-right: 8px; background-color: gray; min-width: 96px; min-height: 24px; display: inline-block;">
                    <p style="margin: auto;" id="WindowBar_EditText">Help</p>
                </button>
                
                <div style="margin-left: 8px; margin-right: 8px; display: inline-block;">
                    <p style="margin: auto;">Inserting Entry</p>
                    <p style="margin: auto;">Test2.0</p>
                </div>
            </div>

            <!-- SECTION LEFT MAIN -->
            <div style="margin-left: 16px; margin-top: 64px; padding: 28px;">
                
                <div style="margin-left: 4px; padding: 4px;" direction="horizontal">
                    <button ID="ModeSwitchID" style="min-width: 16px; min-height: 16px; display: inline-block;">
                        x
                    </button>
                    <p style="display: inline-block;">ID: NULL</p>
                </div>
            </div>
        </div>
    </body>
</html>
```

# What about style?
Similar in idea. Style is handled as a JSON file for simple manual editing and processing

```JSON
{
    "DefaultText": {
        "padding": [1, 2, 1, 2],
        "margin": ["auto", 4, "auto" 8],
        "font-color": "red"
    },
    "global": {
        "fill": {
            "type": "flat",
            "color": "white"
        }
    }
}
```
vs
```CSS
.DefaultText: {
    padding: 2px 1px 2px 1px;
    margin: 4px auto 8px auto;
    color: red;
}
body {
    background-color: white;
}
```

# Loading style information and template information
Handled with the link tag that is currently not used. Will load in style information or existing gui information that can be reused.

Imagine a file like so with the name TemplateList.sgui
```XML
<SimpleGUI>
    <!--VERTICAL LIST-->
    <LayoutList direction="vertical">
        <Text text="item1"/>
        <Text text="item2"/>
    </LayoutList>
    <!--HORIZONAL LIST-->
    <LayoutList direction="horizontal">
        <Text text="item1"/>
        <Text text="item2"/>
    </LayoutList>
</SimpleGUI>
```

Another file could load this gui information as such assuming the file link provided is correct and locatable

```XML
<SimpleGUI>
    <link src="TemplateList.sgui">
</SimpleGUI>
```

A style could be loaded using the exact same idea but a different extension name is expected.

Converting this to HTML is simple for CSS style linking but for inserting existing html, javascript is needed and therefore will be skipped for now as buttons and such also need javascript support.


# New Things
- Remove unneccessary base classes
- Remove cleanup bad or messed up layout code
- Remove old method of loading resources and instead let the file reference specified be the only thing needed.
    - Makes it closer to HTML this way
- Add style format
- Add class for each object
    - Only used in loading
- Add importing / linking of existing GUI information or style information