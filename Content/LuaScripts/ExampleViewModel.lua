-- Example ViewModel Script
-- This script demonstrates how to use Lua with UMG ViewModels

-- Initialize the ViewModel data
function InitializeViewModel(viewModel)
    print("Initializing ViewModel...")
    
    -- Player stats
    viewModel.playerName = "Hero"
    viewModel.playerLevel = 1
    viewModel.playerExperience = 0
    viewModel.experienceToNextLevel = 100
    
    -- Player resources
    viewModel.playerHealth = 100
    viewModel.playerMaxHealth = 100
    viewModel.playerMana = 50
    viewModel.playerMaxMana = 50
    
    -- Player currency
    viewModel.gold = 0
    viewModel.gems = 0
    
    print("ViewModel initialized!")
end

-- Called when a property is requested (optional)
function OnGetProperty(viewModel, propertyName)
    -- Computed properties
    if propertyName == "healthPercent" then
        return viewModel.playerHealth / viewModel.playerMaxHealth
    elseif propertyName == "manaPercent" then
        return viewModel.playerMana / viewModel.playerMaxMana
    elseif propertyName == "experiencePercent" then
        return viewModel.playerExperience / viewModel.experienceToNextLevel
    elseif propertyName == "levelDisplay" then
        return "Level " .. viewModel.playerLevel
    elseif propertyName == "healthDisplay" then
        return viewModel.playerHealth .. " / " .. viewModel.playerMaxHealth
    elseif propertyName == "manaDisplay" then
        return viewModel.playerMana .. " / " .. viewModel.playerMaxMana
    end
    
    -- Return nil to use default behavior
    return nil
end

-- Called when a property is set (optional)
function OnSetProperty(viewModel, propertyName, value)
    -- Validate health changes
    if propertyName == "playerHealth" then
        local clampedHealth = math.max(0, math.min(value, viewModel.playerMaxHealth))
        viewModel.playerHealth = clampedHealth
        
        if clampedHealth <= 0 then
            print("Player died!")
            OnPlayerDeath(viewModel)
        end
        
        return true -- Handled
    end
    
    -- Validate mana changes
    if propertyName == "playerMana" then
        local clampedMana = math.max(0, math.min(value, viewModel.playerMaxMana))
        viewModel.playerMana = clampedMana
        return true -- Handled
    end
    
    -- Validate experience changes
    if propertyName == "playerExperience" then
        viewModel.playerExperience = value
        
        -- Check for level up
        while viewModel.playerExperience >= viewModel.experienceToNextLevel do
            LevelUp(viewModel)
        end
        
        return true -- Handled
    end
    
    return false -- Use default behavior
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

function UseMana(viewModel, amount)
    if viewModel.playerMana >= amount then
        viewModel.playerMana = viewModel.playerMana - amount
        print("Used " .. amount .. " mana. Remaining: " .. viewModel.playerMana)
        return true
    else
        print("Not enough mana!")
        return false
    end
end

function RestoreMana(viewModel, amount)
    local newMana = viewModel.playerMana + amount
    viewModel.playerMana = math.min(viewModel.playerMaxMana, newMana)
    
    print("Restored " .. amount .. " mana. Mana: " .. viewModel.playerMana)
    
    return viewModel.playerMana
end

function AddGold(viewModel, amount)
    viewModel.gold = viewModel.gold + amount
    print("Added " .. amount .. " gold. Total: " .. viewModel.gold)
    return viewModel.gold
end

function SpendGold(viewModel, amount)
    if viewModel.gold >= amount then
        viewModel.gold = viewModel.gold - amount
        print("Spent " .. amount .. " gold. Remaining: " .. viewModel.gold)
        return true
    else
        print("Not enough gold!")
        return false
    end
end

function AddExperience(viewModel, amount)
    viewModel.playerExperience = viewModel.playerExperience + amount
    print("Gained " .. amount .. " experience. Total: " .. viewModel.playerExperience)
    
    -- Level up check is handled in OnSetProperty
    
    return viewModel.playerExperience
end

function LevelUp(viewModel)
    viewModel.playerLevel = viewModel.playerLevel + 1
    viewModel.playerExperience = viewModel.playerExperience - viewModel.experienceToNextLevel
    
    -- Increase stats
    viewModel.playerMaxHealth = viewModel.playerMaxHealth + 10
    viewModel.playerMaxMana = viewModel.playerMaxMana + 5
    
    -- Full heal on level up
    viewModel.playerHealth = viewModel.playerMaxHealth
    viewModel.playerMana = viewModel.playerMaxMana
    
    -- Increase experience needed for next level
    viewModel.experienceToNextLevel = math.floor(viewModel.experienceToNextLevel * 1.5)
    
    print("LEVEL UP! Now level " .. viewModel.playerLevel)
    print("Max Health: " .. viewModel.playerMaxHealth)
    print("Max Mana: " .. viewModel.playerMaxMana)
    print("Experience to next level: " .. viewModel.experienceToNextLevel)
    
    return viewModel.playerLevel
end

function OnPlayerDeath(viewModel)
    print("Player has died! Resetting...")
    
    -- Reset to level 1
    viewModel.playerLevel = 1
    viewModel.playerExperience = 0
    viewModel.experienceToNextLevel = 100
    
    -- Reset stats
    viewModel.playerMaxHealth = 100
    viewModel.playerMaxMana = 50
    viewModel.playerHealth = viewModel.playerMaxHealth
    viewModel.playerMana = viewModel.playerMaxMana
    
    -- Lose half the gold
    viewModel.gold = math.floor(viewModel.gold / 2)
    
    print("Player respawned at level 1")
end

-- Utility function to get complete player stats
function GetPlayerStats(viewModel)
    return {
        name = viewModel.playerName,
        level = viewModel.playerLevel,
        health = viewModel.playerHealth,
        maxHealth = viewModel.playerMaxHealth,
        mana = viewModel.playerMana,
        maxMana = viewModel.playerMaxMana,
        experience = viewModel.playerExperience,
        experienceToNext = viewModel.experienceToNextLevel,
        gold = viewModel.gold,
        gems = viewModel.gems
    }
end
