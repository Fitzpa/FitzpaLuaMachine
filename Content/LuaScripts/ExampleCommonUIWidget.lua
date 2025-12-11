-- Example Common UI Widget Script
-- This script demonstrates how to use Lua with Common UI widgets

-- Called when the widget is constructed
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

-- Called when the widget is activated
function OnWidgetActivated(widget)
    print("Widget Activated!")
    widget.isActive = true
    
    -- You can perform initialization here
    ResetCounter(widget)
end

-- Called when the widget is deactivated  
function OnWidgetDeactivated(widget)
    print("Widget Deactivated!")
    widget.isActive = false
end

-- Called when the widget is destructed
function OnWidgetDestructed(widget)
    print("Widget Destructed!")
    print("Final click count: " .. widget.clickCount)
end

-- Custom function: Handle button click
function OnButtonClicked(widget)
    if not widget.isActive then
        print("Widget is not active!")
        return 0
    end
    
    widget.clickCount = widget.clickCount + 1
    print("Button clicked! Count: " .. widget.clickCount)
    
    return widget.clickCount
end

-- Custom function: Reset counter
function ResetCounter(widget)
    widget.clickCount = 0
    print("Counter reset!")
    return true
end

-- Custom function: Get widget info
function GetWidgetInfo(widget)
    local info = {
        title = widget.widgetTitle,
        active = widget.isActive,
        clicks = widget.clickCount
    }
    return info
end

-- Custom function: Update title
function UpdateTitle(widget, newTitle)
    widget.widgetTitle = newTitle
    print("Title updated to: " .. newTitle)
    return newTitle
end
