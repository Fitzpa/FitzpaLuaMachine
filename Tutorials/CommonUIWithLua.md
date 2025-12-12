# Using Common UI with Lua in LuaMachine

This tutorial demonstrates how to integrate Unreal Engine's Common UI system with Lua scripting using the LuaMachine plugin.

## Overview

The LuaMachine plugin now supports UMG Common UI, allowing you to:
- Create Common UI widgets controlled by Lua scripts
- Bind Lua functions to widget lifecycle events (activation, deactivation, construction)
- Call Lua functions to update widget state and respond to user input
- Access and modify widget properties from Lua

## Prerequisites

- LuaMachine plugin installed and configured
- Common UI plugin enabled in your project
- Basic understanding of Lua scripting and Common UI concepts

## Creating a Lua-Enabled Common UI Widget

### Step 1: Create a LuaState

First, create a Blueprint class that inherits from `LuaState`. Name it something like `BP_CommonUILuaState`.

In the `BP_CommonUILuaState` Blueprint:
1. Set `Lua Open Libs` to `true`
2. Optionally set a `LuaFilename` to automatically load your Lua script

### Step 2: Create a Common UI Widget Blueprint

Create a Blueprint class that inherits from `LuaCommonUIWidget`. Name it something like `WBP_MyLuaWidget`.

In the widget designer, add your UI elements (buttons, text blocks, etc.).

### Step 3: Configure Lua Integration

In the widget's details panel:
1. Set `Lua State` to your `BP_CommonUILuaState` class
2. Configure the Lua function callbacks:
   - `On Constructed Lua Function`: Name of the Lua function to call when widget is constructed (e.g., "OnWidgetConstructed")
   - `On Activated Lua Function`: Name of the Lua function to call when widget is activated (e.g., "OnWidgetActivated")
   - `On Deactivated Lua Function`: Name of the Lua function to call when widget is deactivated (e.g., "OnWidgetDeactivated")

### Step 4: Create the Lua Script

Create a Lua file in your project's `Content` directory (e.g., `CommonUIExample.lua`):

```lua
-- CommonUIExample.lua

function OnWidgetConstructed(widget)
    print("Widget constructed!")
    
    -- Store some initial state in the widget table
    widget.counter = 0
    widget.title = "My Lua Widget"
    
    -- You can access the widget object itself
    local widgetObject = widget.Widget
    print("Widget name: " .. widgetObject:GetName())
end

function OnWidgetActivated(widget)
    print("Widget activated!")
    print("Current counter: " .. widget.counter)
    
    -- You can update the widget state
    widget.counter = widget.counter + 1
end

function OnWidgetDeactivated(widget)
    print("Widget deactivated!")
end

-- Custom function that can be called from Blueprint
function UpdateWidgetTitle(widget, newTitle)
    widget.title = newTitle
    print("Widget title updated to: " .. newTitle)
    return true
end

-- Custom function to handle button click
function OnButtonClicked(widget)
    widget.counter = widget.counter + 1
    print("Button clicked! Counter is now: " .. widget.counter)
    
    return widget.counter
end
```

### Step 5: Set the Lua Filename

In your `BP_CommonUILuaState` Blueprint, set the `LuaFilename` field to `CommonUIExample.lua`.

### Step 6: Call Lua Functions from Blueprints

In your widget's Event Graph, you can call Lua functions:

1. Add a Button widget to your UI
2. In the Event Graph, create an `OnClicked` event for the button
3. Call `Lua Call Function` node:
   - Target: `Self`
   - Name: `"OnButtonClicked"`
   - Args: (empty array)

The function will be called with the widget table as the first argument automatically.

## Using Widget Properties from Lua

You can pre-populate the widget's Lua table with values from Blueprints:

In your widget's details panel, expand the `Table` property and add key-value pairs:
- Key: `"max_counter"`, Value: Integer `10`
- Key: `"debug_mode"`, Value: Bool `true`

These will be accessible in your Lua script:

```lua
function OnWidgetActivated(widget)
    if widget.debug_mode then
        print("Debug mode enabled")
    end
    
    if widget.counter >= widget.max_counter then
        print("Counter reached maximum!")
    end
end
```

## Getting and Setting Fields

You can also use the Blueprint functions to get and set fields:

```blueprint
// Get a field
FLuaValue CounterValue = LuaGetField("counter");
int32 Counter = Conv_LuaValueToInt(CounterValue);

// Set a field
FLuaValue NewValue = Conv_IntToLuaValue(42);
LuaSetField("counter", NewValue);
```

## Advanced: Accessing UMG Widgets from Lua

You can expose your UMG widgets to Lua by adding them to the widget table:

In your widget's `OnConstruct` event:

```blueprint
// Create a reference to a text block
FLuaValue TextBlockValue = Conv_ObjectToLuaValue(MyTextBlock);
LuaSetField("TitleText", TextBlockValue);
```

Then in Lua:

```lua
function UpdateTitle(widget, newTitle)
    local textBlock = widget.TitleText
    if textBlock then
        -- Call UMG functions through reflection
        -- Note: You'll need to use the reflection API for this
        widget.title = newTitle
    end
end
```

## Best Practices

1. **Initialize widget state in OnConstructed**: Use the construction callback to set up initial state and references.

2. **Use the widget table for state management**: Store all your widget-specific data in the widget table passed to your functions.

3. **Handle nil gracefully**: Always check if values exist before using them:
   ```lua
   function SafeUpdate(widget)
       if widget and widget.counter then
           widget.counter = widget.counter + 1
       end
   end
   ```

4. **Keep Lua functions focused**: Create small, focused functions for specific tasks rather than large monolithic functions.

5. **Use logging for debugging**: The `print()` function in Lua automatically logs to Unreal's Output Log.

## Example: Complete Counter Widget

Here's a complete example of a counter widget:

**Lua Script (CounterWidget.lua):**
```lua
function OnWidgetConstructed(widget)
    widget.counter = 0
    widget.maxCount = 10
end

function OnButtonIncrement(widget)
    if widget.counter < widget.maxCount then
        widget.counter = widget.counter + 1
        return widget.counter
    end
    return widget.counter
end

function OnButtonReset(widget)
    widget.counter = 0
    return 0
end

function GetCounterPercent(widget)
    return widget.counter / widget.maxCount
end
```

**Blueprint Setup:**
1. Create `WBP_CounterWidget` inheriting from `LuaCommonUIWidget`
2. Add a text block to display the counter
3. Add an "Increment" button
4. Add a "Reset" button
5. Set `Lua State` to your `BP_CommonUILuaState`
6. Set `On Constructed Lua Function` to `"OnWidgetConstructed"`

**Event Graph:**
- On Increment Button Clicked:
  - Call `Lua Call Function` with Name: `"OnButtonIncrement"`
  - Convert result to integer
  - Update text block with the counter value

- On Reset Button Clicked:
  - Call `Lua Call Function` with Name: `"OnButtonReset"`
  - Update text block to show 0

## Troubleshooting

**Widget not receiving Lua callbacks:**
- Verify that `Lua State` is set in the widget
- Ensure the Lua script is loaded (check the Output Log)
- Confirm the function names match exactly (Lua is case-sensitive)

**Lua errors:**
- Enable `bLogError` in your widget to see detailed error messages
- Check the Output Log for Lua errors
- Verify your Lua syntax using a Lua linter

**Cannot access widget properties:**
- Make sure the widget table is initialized (called after construction)
- Verify you're passing the correct arguments to your Lua functions

## Next Steps

- Learn about [ViewModel integration with Lua](./LuaViewModelIntegration.md)
- Explore the [Reflection API](../Docs/ReflectionAPI.md) for advanced UObject manipulation
- Check out [LuaComponent](../Docs/LuaComponent.md) for actor-based Lua scripting
