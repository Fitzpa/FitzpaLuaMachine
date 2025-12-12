# Quick Start: Common UI and ViewModel with Lua

This guide provides a quick overview of the new Common UI and ViewModel features in LuaMachine.

## What's New

LuaMachine now supports:
- **Common UI Integration**: Control Common UI widgets with Lua scripts
- **MVVM Pattern**: Implement Model-View-ViewModel pattern with Lua as the data layer
- **Reactive UI**: Automatic UI updates when Lua data changes
- **Two-way Binding**: Synchronize data between Lua and UMG widgets

## When to Use Each Feature

### Use LuaCommonUIWidget When:
- You want to script widget behavior and UI logic in Lua
- You need to respond to Common UI lifecycle events (activation, deactivation)
- You want to handle UI interactions through Lua functions
- You're building modular, reusable UI components

### Use LuaViewModelBridge When:
- You want to separate UI presentation from data/logic
- You need data binding between Lua and UMG widgets
- You want reactive UI that updates automatically when data changes
- You're implementing complex data-driven UIs

### Use Both Together When:
- Building complex UI systems with both scripted behavior and data binding
- Creating data-driven Common UI widgets
- Implementing full-featured UI frameworks in Lua

## 5-Minute Setup: Common UI Widget

1. **Create a LuaState Blueprint**
   - Inherit from `LuaState`
   - Set `Lua Filename` to your script (e.g., `LuaScripts/ExampleCommonUIWidget.lua`)

2. **Create a Widget Blueprint**
   - Inherit from `LuaCommonUIWidget`
   - Set `Lua State` to your LuaState class
   - Set lifecycle callbacks:
     - `On Constructed Lua Function`: `"OnWidgetConstructed"`
     - `On Activated Lua Function`: `"OnWidgetActivated"`

3. **Write Your Lua Script**
   ```lua
   function OnWidgetConstructed(widget)
       widget.counter = 0
       print("Widget constructed!")
   end
   
   function OnWidgetActivated(widget)
       widget.counter = widget.counter + 1
       print("Widget activated, count: " .. widget.counter)
   end
   
   function OnButtonClick(widget)
       print("Button clicked!")
       return true
   end
   ```

4. **Call Lua from Blueprint**
   - In your widget's Event Graph
   - Use `Lua Call Function` node to call Lua functions
   - Example: On Button Click → `Lua Call Function("OnButtonClick")`

## 5-Minute Setup: ViewModel

1. **Create a LuaState Blueprint**
   - Inherit from `LuaState`
   - Set `Lua Filename` to your script (e.g., `LuaScripts/ExampleViewModel.lua`)

2. **Create a ViewModel Blueprint**
   - Inherit from `LuaViewModelBridge`
   - Set `Lua State` to your LuaState class

3. **Write Your Lua Script**
   ```lua
   function InitializeViewModel(viewModel)
       viewModel.playerName = "Hero"
       viewModel.playerHealth = 100
       viewModel.playerMaxHealth = 100
   end
   
   function TakeDamage(viewModel, amount)
       viewModel.playerHealth = viewModel.playerHealth - amount
       return viewModel.playerHealth
   end
   ```

4. **Initialize in Blueprint**
   ```blueprint
   BeginPlay:
       ViewModel = Construct BP_PlayerViewModel
       ViewModel.InitializeLuaViewModel()
       ViewModel.LuaCallFunction("InitializeViewModel")
   ```

5. **Bind in UMG Widget**
   - Add ViewModel to widget: `PlayerVM` (BP_PlayerViewModel)
   - Bind text/properties to `PlayerVM.playerHealth`, `PlayerVM.playerName`, etc.

6. **Update from Gameplay**
   ```blueprint
   PlayerTakeDamage:
       Args = [25] // damage amount
       ViewModel.LuaCallFunction("TakeDamage", Args)
       ViewModel.LuaBroadcastFieldValueChanged("playerHealth")
   ```

## Key Concepts

### Lua Table as State Container
Both features use Lua tables to store widget/viewmodel state:
- First argument to Lua functions is always the table
- Store all your data in the table
- Access via dot notation: `widget.myValue` or `viewModel.myData`

### Property Change Notifications
For ViewModel:
- After modifying properties, call `LuaBroadcastFieldValueChanged`
- This triggers UI updates for bound widgets
- Example: `LuaBroadcastFieldValueChanged("playerHealth")`

### Return Value Conventions

**LuaCommonUIWidget**: Functions can return any value
- Returned values are accessible in Blueprint
- Use for computed values, status codes, etc.

**LuaViewModelBridge OnSetProperty**: Return values control behavior
- `true`: Property was handled successfully (UI notified)
- `false`: Property change rejected (no change occurs)
- `nil`: Use default behavior (store in table)

## Example Files

Check the `/Content/LuaScripts/` directory for complete examples:
- `ExampleCommonUIWidget.lua` - Common UI widget with lifecycle callbacks
- `ExampleViewModel.lua` - Full ViewModel with player stats and game logic
- `README.md` - Detailed explanation of the examples

## Complete Tutorials

For in-depth guides:
- [Common UI with Lua Tutorial](../Tutorials/CommonUIWithLua.md)
- [ViewModel Integration Tutorial](../Tutorials/LuaViewModelIntegration.md)

## Troubleshooting

**Widget callbacks not firing:**
- Verify `Lua State` is set in widget details
- Check Lua script is loaded (look in Output Log)
- Ensure function names match exactly (case-sensitive)

**ViewModel bindings not updating:**
- Call `LuaBroadcastFieldValueChanged` after property changes
- Verify ViewModel is initialized: `InitializeLuaViewModel()`
- Check property names match in both Lua and UMG

**"Table not initialized" errors:**
- For widgets: Called before `NativeConstruct`
- For ViewModels: Call `InitializeLuaViewModel()` first

## Best Practices

1. **Initialize early**: Set up ViewModels in BeginPlay or widget construction
2. **Always notify**: Call `LuaBroadcastFieldValueChanged` after ViewModel updates
3. **Keep functions focused**: One function, one purpose
4. **Use computed properties**: Calculate derived values in `OnGetProperty`
5. **Validate in OnSetProperty**: Use callbacks to validate and sanitize data
6. **Log for debugging**: Use `print()` in Lua to debug issues

## Support

- Check the [main README](../README.md) for general LuaMachine documentation
- Visit the [LuaMachine Discord](https://discord.gg/eTXDfEU) for community support
- Review [existing tutorials](../Tutorials/) for related patterns
