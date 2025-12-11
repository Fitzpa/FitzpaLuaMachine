# Example Lua Scripts for Common UI and ViewModel Integration

This folder contains example Lua scripts demonstrating the integration of LuaMachine with Unreal Engine's UMG Common UI and ViewModel systems.

## Files

### ExampleCommonUIWidget.lua

Demonstrates how to create a Common UI widget controlled by Lua:

- **Lifecycle callbacks**: `OnWidgetConstructed`, `OnWidgetActivated`, `OnWidgetDeactivated`, `OnWidgetDestructed`
- **State management**: Storing widget-specific data in the widget table
- **Event handling**: `OnButtonClicked` function for button events
- **Custom methods**: `ResetCounter`, `GetWidgetInfo`, `UpdateTitle`

**Usage:**
1. Create a LuaState Blueprint and set the `LuaFilename` to `LuaScripts/ExampleCommonUIWidget.lua`
2. Create a LuaCommonUIWidget Blueprint and set its `Lua State` to your LuaState
3. Configure the widget's Lua function callbacks in the details panel
4. Call Lua functions from Blueprint using `Lua Call Function`

### ExampleViewModel.lua

Demonstrates how to implement the MVVM pattern with Lua as the data layer:

- **Initialization**: `InitializeViewModel` sets up initial player stats
- **Computed properties**: `OnGetProperty` provides calculated values like health percentage
- **Property validation**: `OnSetProperty` validates and clamps values
- **Business logic**: Functions for damage, healing, mana usage, gold, experience, and leveling
- **Complex systems**: Automatic level-up system and death/respawn mechanics

**Usage:**
1. Create a LuaState Blueprint and set the `LuaFilename` to `LuaScripts/ExampleViewModel.lua`
2. Create a LuaViewModelBridge Blueprint and set its `Lua State` to your LuaState
3. Call `InitializeLuaViewModel` on the ViewModel instance
4. Call `LuaCallFunction("InitializeViewModel")` to set up initial data
5. Bind UMG widgets to ViewModel properties (e.g., `playerHealth`, `playerName`)
6. Call business logic functions from gameplay code (e.g., `TakeDamage`, `AddGold`)
7. After modifying properties, call `LuaBroadcastFieldValueChanged` to update the UI

## Key Concepts

### Widget Table
In Common UI integration, the first argument to all Lua functions is the widget table, which contains:
- `Widget`: Reference to the actual UObject widget
- Any custom fields you add to the table

### ViewModel Table
In ViewModel integration, the first argument is the ViewModel table, which contains:
- `ViewModel`: Reference to the actual UObject ViewModel
- All your data properties and methods

### Property Notifications
When you modify ViewModel properties from Lua functions, remember to notify the UI:
```blueprint
// In Blueprint after calling a Lua function that modifies data
LuaBroadcastFieldValueChanged(PlayerViewModel, "playerHealth")
```

This triggers UI updates for any widgets bound to that property.

## Best Practices

1. **Initialize early**: Call initialization functions in `BeginPlay` or widget construction
2. **Always notify**: Call `LuaBroadcastFieldValueChanged` after property updates
3. **Validate input**: Use the `OnSetProperty` callback to validate and sanitize data
4. **Use computed properties**: Calculate derived values in `OnGetProperty` instead of storing them
5. **Keep functions focused**: Each Lua function should have a single, clear purpose
6. **Log for debugging**: Use `print()` to log important events and state changes

## Further Reading

- [Common UI with Lua Tutorial](../Tutorials/CommonUIWithLua.md)
- [ViewModel Integration Tutorial](../Tutorials/LuaViewModelIntegration.md)
- [LuaMachine Documentation](../README.md)
