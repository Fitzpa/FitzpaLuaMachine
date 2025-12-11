# Using UMG ViewModels with Lua in LuaMachine

This tutorial demonstrates how to integrate Unreal Engine's Model-View-ViewModel (MVVM) pattern with Lua scripting using the LuaMachine plugin.

## Overview

The LuaMachine plugin now supports UMG ViewModels, enabling:
- Data binding between Lua scripts and UI elements
- Two-way synchronization of data changes
- Separation of UI logic from presentation
- Reactive UI updates when Lua data changes

## Prerequisites

- LuaMachine plugin installed and configured
- ModelViewViewModel plugin enabled in your project (enabled by default in UE5.1+)
- Basic understanding of Lua scripting and MVVM concepts

## Understanding the MVVM Pattern with Lua

In the MVVM pattern:
- **Model**: Your game data (can be managed by Lua)
- **View**: Your UMG widgets
- **ViewModel**: The bridge between Model and View (LuaViewModelBridge)

The `LuaViewModelBridge` class allows you to:
- Store data in Lua tables
- Expose that data to UMG widgets through property binding
- Automatically update the UI when Lua data changes
- Respond to UI changes in Lua

## Creating a Lua ViewModel

### Step 1: Create a LuaState

Create a Blueprint class inheriting from `LuaState` (e.g., `BP_ViewModelLuaState`).

Configure it:
1. Set `Lua Open Libs` to `true`
2. Set `LuaFilename` to your Lua script (e.g., `PlayerViewModel.lua`)

### Step 2: Create a ViewModel Blueprint

Create a Blueprint class inheriting from `LuaViewModelBridge` (e.g., `BP_PlayerViewModel`).

In the Blueprint:
1. Set `Lua State` to your `BP_ViewModelLuaState` class
2. Configure optional callbacks:
   - `On Get Property Lua Function`: Function called when a property is read (e.g., "OnGetProperty")
   - `On Set Property Lua Function`: Function called when a property is written (e.g., "OnSetProperty")

### Step 3: Create the Lua Script

Create your Lua script in the `Content` directory (e.g., `PlayerViewModel.lua`):

```lua
-- PlayerViewModel.lua

-- Initialize the ViewModel data
function InitializeViewModel(viewModel)
    viewModel.playerName = "Hero"
    viewModel.playerHealth = 100
    viewModel.playerMaxHealth = 100
    viewModel.playerLevel = 1
    viewModel.playerGold = 0
    
    print("ViewModel initialized")
end

-- Called when a property is requested (optional)
function OnGetProperty(viewModel, propertyName)
    -- You can add custom logic here, like computed properties
    if propertyName == "healthPercent" then
        return viewModel.playerHealth / viewModel.playerMaxHealth
    end
    
    -- Return nil to use default behavior (get from table)
    return nil
end

-- Called when a property is set (optional)
function OnSetProperty(viewModel, propertyName, value)
    -- You can add validation or side effects here
    if propertyName == "playerHealth" then
        -- Clamp health between 0 and max
        local clampedHealth = math.max(0, math.min(value, viewModel.playerMaxHealth))
        viewModel.playerHealth = clampedHealth
        
        -- Also update the health percent binding
        local vmObject = viewModel.ViewModel
        if vmObject then
            -- Notify that healthPercent has also changed
            -- This would be done in Blueprint using LuaNotifyViewModelPropertyChanged
        end
        
        return true -- Handled
    end
    
    return false -- Use default behavior (set in table)
end

-- Business logic functions

function TakeDamage(viewModel, amount)
    local newHealth = viewModel.playerHealth - amount
    viewModel.playerHealth = math.max(0, newHealth)
    
    print("Player took " .. amount .. " damage. Health: " .. viewModel.playerHealth)
    
    return viewModel.playerHealth
end

function Heal(viewModel, amount)
    local newHealth = viewModel.playerHealth + amount
    viewModel.playerHealth = math.min(viewModel.playerMaxHealth, newHealth)
    
    print("Player healed " .. amount .. ". Health: " .. viewModel.playerHealth)
    
    return viewModel.playerHealth
end

function AddGold(viewModel, amount)
    viewModel.playerGold = viewModel.playerGold + amount
    print("Added " .. amount .. " gold. Total: " .. viewModel.playerGold)
    return viewModel.playerGold
end

function LevelUp(viewModel)
    viewModel.playerLevel = viewModel.playerLevel + 1
    viewModel.playerMaxHealth = viewModel.playerMaxHealth + 10
    viewModel.playerHealth = viewModel.playerMaxHealth -- Full heal on level up
    
    print("Level up! Now level " .. viewModel.playerLevel)
    
    return viewModel.playerLevel
end
```

### Step 4: Initialize the ViewModel in Blueprint

In your game's initialization (e.g., Player Controller's `BeginPlay`):

```blueprint
// Create the ViewModel instance
BP_PlayerViewModel PlayerViewModel = Construct Object from Class(BP_PlayerViewModel)

// Initialize the Lua table
PlayerViewModel.InitializeLuaViewModel()

// Call the initialization function
FLuaValue InitArgs = (empty array)
PlayerViewModel.LuaCallFunction("InitializeViewModel", InitArgs)

// Store the ViewModel for later use (e.g., as a variable)
```

### Step 5: Create a Widget with ViewModel Binding

Create a UMG widget (e.g., `WBP_PlayerHUD`).

In the widget designer:
1. Add text blocks for player name, health, level, and gold
2. Add a progress bar for health

In the widget's ViewModel settings:
1. Click "Add ViewModel"
2. Select your `BP_PlayerViewModel` class
3. Name it "PlayerVM"

### Step 6: Bind Widget Properties to ViewModel

For each widget element, bind to the ViewModel:

**Player Name Text:**
- Binding: `PlayerVM.playerName` (One Way To Widget)

**Health Text:**
- Binding: Create a function binding that formats the health
- Or use: `PlayerVM.playerHealth` (One Way To Widget)

**Health Progress Bar:**
- Percent: Binding to a computed property or function
- In Blueprint, create a function:
  ```blueprint
  GetHealthPercent():
      Health = PlayerViewModel.LuaGetProperty("playerHealth")
      MaxHealth = PlayerViewModel.LuaGetProperty("playerMaxHealth")
      return Health / MaxHealth
  ```

**Player Level Text:**
- Binding: `PlayerVM.playerLevel` (One Way To Widget)

**Player Gold Text:**
- Binding: `PlayerVM.playerGold` (One Way To Widget)

## Updating ViewModel from Gameplay

When game events occur, update the ViewModel and notify the UI:

```blueprint
// Player takes damage
DamageAmount = 25
Args = Array with one LuaValue (DamageAmount)
PlayerViewModel.LuaCallFunction("TakeDamage", Args)

// Notify UI that health changed
PlayerViewModel.LuaBroadcastFieldValueChanged("playerHealth")
```

## Updating ViewModel from Lua

You can also update ViewModel properties directly from Lua and trigger UI updates:

```lua
-- In your Lua script
function UpdatePlayerStats(viewModel, newHealth, newGold)
    viewModel.playerHealth = newHealth
    viewModel.playerGold = newGold
    
    -- In Blueprint, you would call LuaBroadcastFieldValueChanged
    -- after this function returns to notify the UI
    return true
end
```

Then in Blueprint:
```blueprint
UpdatePlayerStats(75, 1000)
LuaBroadcastFieldValueChanged("playerHealth")
LuaBroadcastFieldValueChanged("playerGold")
```

## Using Blueprint Functions

The plugin provides helper functions for working with ViewModels:

**LuaSetViewModelProperty:**
```blueprint
LuaSetViewModelProperty(PlayerViewModel, "playerHealth", 50)
```

**LuaGetViewModelProperty:**
```blueprint
FLuaValue HealthValue = LuaGetViewModelProperty(PlayerViewModel, "playerHealth")
int Health = Conv_LuaValueToInt(HealthValue)
```

**LuaNotifyViewModelPropertyChanged:**
```blueprint
// After updating a property, notify the UI
LuaNotifyViewModelPropertyChanged(PlayerViewModel, "playerHealth")
```

## Advanced: Two-Way Binding

You can create two-way bindings where UI changes update the ViewModel:

### Example: Editable Player Name

In your widget:
1. Add an Editable Text Box
2. Set it to Two-Way binding with `PlayerVM.playerName`

When the user changes the name:
```blueprint
OnTextChanged:
    NewName = GetText()
    LuaSetViewModelProperty(PlayerViewModel, "playerName", NewName)
    LuaNotifyViewModelPropertyChanged(PlayerViewModel, "playerName")
```

In Lua, handle the change:
```lua
function OnSetProperty(viewModel, propertyName, value)
    if propertyName == "playerName" then
        -- Validate the name
        if value and value:len() > 0 then
            viewModel.playerName = value
            print("Player name changed to: " .. value)
            return true
        end
        return false -- Reject empty names
    end
    
    return false -- Use default behavior
end
```

## Complete Example: Inventory System

Here's a complete example implementing a simple inventory:

**InventoryViewModel.lua:**
```lua
function InitializeInventory(viewModel)
    viewModel.items = {}
    viewModel.maxSlots = 20
    viewModel.itemCount = 0
end

function AddItem(viewModel, itemName, quantity)
    if viewModel.itemCount >= viewModel.maxSlots then
        print("Inventory full!")
        return false
    end
    
    -- Check if item already exists
    for i, item in ipairs(viewModel.items) do
        if item.name == itemName then
            item.quantity = item.quantity + quantity
            print("Added " .. quantity .. " " .. itemName)
            return true
        end
    end
    
    -- Add new item
    table.insert(viewModel.items, {name = itemName, quantity = quantity})
    viewModel.itemCount = viewModel.itemCount + 1
    print("Added new item: " .. itemName .. " x" .. quantity)
    
    return true
end

function RemoveItem(viewModel, itemName, quantity)
    for i, item in ipairs(viewModel.items) do
        if item.name == itemName then
            item.quantity = item.quantity - quantity
            
            if item.quantity <= 0 then
                table.remove(viewModel.items, i)
                viewModel.itemCount = viewModel.itemCount - 1
                print("Removed all " .. itemName)
            else
                print("Removed " .. quantity .. " " .. itemName)
            end
            
            return true
        end
    end
    
    return false
end

function GetItemCount(viewModel, itemName)
    for i, item in ipairs(viewModel.items) do
        if item.name == itemName then
            return item.quantity
        end
    end
    return 0
end

function GetInventorySlotsFree(viewModel)
    return viewModel.maxSlots - viewModel.itemCount
end
```

## Best Practices

1. **Initialize ViewModels early**: Create and initialize ViewModels during game initialization.

2. **Always notify on changes**: When updating ViewModel properties from Lua functions, always call `LuaBroadcastFieldValueChanged` afterward.

3. **Use computed properties**: For derived values (like health percentage), calculate them in the `OnGetProperty` function.

4. **Validate data in OnSetProperty**: Use the `OnSetProperty` callback to validate and sanitize data before storing it.

5. **Keep ViewModels focused**: Each ViewModel should represent a specific domain (player stats, inventory, quest log, etc.).

6. **Use tables for complex data**: Store arrays and nested objects as Lua tables within your ViewModel.

## Troubleshooting

**Bindings not updating:**
- Ensure you're calling `LuaBroadcastFieldValueChanged` after updating properties
- Verify the property name matches exactly
- Check that the ViewModel is properly initialized

**Nil values in Lua:**
- Make sure `InitializeLuaViewModel` is called before accessing properties
- Initialize all properties in your Lua initialization function

**Performance issues:**
- Avoid calling `LuaBroadcastFieldValueChanged` too frequently
- Batch property updates when possible
- Use One-Way bindings instead of Two-Way when UI doesn't need to modify data

## Next Steps

- Combine with [Common UI integration](./CommonUIWithLua.md) for complete UI solutions
- Learn about [LuaComponent](../Docs/LuaComponent.md) for actor-based data management
- Explore the [Reflection API](../Docs/ReflectionAPI.md) for advanced UObject manipulation
