-- Example Common UI Widget Script
-- This script demonstrates how to use Lua with Common UI widgets

-- Initialize a widget's runtime state and log its creation.
-- Sets the widget's `isActive` to false, `clickCount` to 0, and `widgetTitle` to "Lua Common UI Widget".
-- Also prints a construction message and the name of the underlying `widget.Widget` object.
-- @param widget The widget instance/table to initialize; expected to have a `Widget` field whose object exposes `GetName()`.
function OnWidgetConstructed(widget)
    print("Common UI Widget Constructed!")
    
    -- Initialize widget state
    widget.isActive = false
    widget.clickCount = 0
    widget.widgetTitle = "Lua Common UI Widget"
    
    -- Store a reference to the widget object
    local widgetObject = widget.Widget
    print("Widget Name: " .. widgetObject:GetName())
end

-- Marks the given widget as active and initializes its state.
-- @param widget The widget instance to activate; its `isActive` flag will be set to true and its click counter reset.
function OnWidgetActivated(widget)
    print("Widget Activated!")
    widget.isActive = true
    
    -- You can perform initialization here
    ResetCounter(widget)
end

-- Deactivates the widget by setting its `isActive` flag to false.
-- @param widget The widget table to deactivate; its `isActive` field will be set to `false`.
function OnWidgetDeactivated(widget)
    print("Widget Deactivated!")
    widget.isActive = false
end

-- Handle widget teardown and log its final state.
-- Prints a destruction message and the widget's final click count.
-- @param widget The widget instance whose final state is being logged.
function OnWidgetDestructed(widget)
    print("Widget Destructed!")
    print("Final click count: " .. widget.clickCount)
end

-- Handle a button click for the widget, incrementing its click counter when the widget is active.
-- Inactive widgets are not modified.
-- @param widget Table representing the widget; expected to have `isActive` (boolean) and `clickCount` (number) fields.
-- @return The updated click count after increment, or `0` if the widget was not active.
function OnButtonClicked(widget)
    if not widget.isActive then
        print("Widget is not active!")
        return 0
    end
    
    widget.clickCount = widget.clickCount + 1
    print("Button clicked! Count: " .. widget.clickCount)
    
    return widget.clickCount
end

-- Reset the widget's click counter to zero.
-- @param widget The widget table whose `clickCount` will be reset.
-- @return `true` if the counter was reset.
function ResetCounter(widget)
    widget.clickCount = 0
    print("Counter reset!")
    return true
end

-- Retrieve the widget's current title, active state, and click count.
-- @param widget The widget table whose state will be read.
-- @return table A table with fields:
--   `title` (string) — the widget's title,
--   `active` (boolean) — `true` if the widget is active, `false` otherwise,
--   `clicks` (number) — the current click count.
function GetWidgetInfo(widget)
    local info = {
        title = widget.widgetTitle,
        active = widget.isActive,
        clicks = widget.clickCount
    }
    return info
end

-- Updates the widget's title to the provided string.
-- @param widget The widget instance (table) whose title will be updated.
-- @param newTitle The new title string to assign.
-- @return The updated title string.
function UpdateTitle(widget, newTitle)
    widget.widgetTitle = newTitle
    print("Title updated to: " .. newTitle)
    return newTitle
end