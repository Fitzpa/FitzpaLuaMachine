-- Example ViewModel Script
-- This script demonstrates how to use Lua with UMG ViewModels

-- Populate the provided ViewModel table with default player identity, stats, resources, and currency fields.
-- @param viewModel The ViewModel table to initialize. The function sets: `playerName`, `playerLevel`, `playerExperience`, `experienceToNextLevel`, `playerHealth`, `playerMaxHealth`, `playerMana`, `playerMaxMana`, `gold`, and `gems`.
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

-- Provide computed or formatted values for select derived ViewModel properties.
-- Recognized property names and returned values:
--   - "healthPercent": number (playerHealth / playerMaxHealth)
--   - "manaPercent": number (playerMana / playerMaxMana)
--   - "experiencePercent": number (playerExperience / experienceToNextLevel)
--   - "levelDisplay": string ("Level X")
--   - "healthDisplay": string ("current / max")
--   - "manaDisplay": string ("current / max")
-- Returns nil to indicate default table-based property lookup when the name is not recognized.
-- @param viewModel The ViewModel table containing player state fields.
-- @param propertyName The name of the requested property.
-- @return number|string|nil The computed value for known properties, or `nil` to fall back to default behavior.
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

-- Handles assignments to reactive ViewModel properties, validating and applying special-case updates.
-- @param viewModel The ViewModel table whose property is being set.
-- @param propertyName The name of the property being assigned (e.g., "playerHealth", "playerMana", "playerExperience").
-- @param value The new value for the property.
-- @return `true` if the assignment was handled (value applied and UI should be notified), `false` if the change is rejected, or `nil` to indicate default table assignment should be used.
-- Notes: Setting `playerHealth` may trigger player death handling; setting `playerExperience` may trigger one or more level-ups.
function OnSetProperty(viewModel, propertyName, value)
    -- This function can return:
    -- true: Property was handled by this function (UI will be notified)
    -- false: Property change is rejected (no change will occur)
    -- nil or non-boolean: Use default behavior (store in table)
    
    -- Validate health changes
    if propertyName == "playerHealth" then
        local clampedHealth = math.max(0, math.min(value, viewModel.playerMaxHealth))
        viewModel.playerHealth = clampedHealth
        
        if clampedHealth <= 0 then
            print("Player died!")
            OnPlayerDeath(viewModel)
        end
        
        return true -- Property was handled successfully
    end
    
    -- Validate mana changes
    if propertyName == "playerMana" then
        local clampedMana = math.max(0, math.min(value, viewModel.playerMaxMana))
        viewModel.playerMana = clampedMana
        return true -- Property was handled successfully
    end
    
    -- Validate experience changes
    if propertyName == "playerExperience" then
        viewModel.playerExperience = value
        
        -- Check for level up
        while viewModel.playerExperience >= viewModel.experienceToNextLevel do
            LevelUp(viewModel)
        end
        
        return true -- Property was handled successfully
    end
    
    return nil -- Use default behavior (set in table)
end

-- Reduces the player's health by the specified damage amount.
-- @param amount The amount of damage to apply.
-- @return The player's updated health after applying damage, floored at 0.

function TakeDamage(viewModel, amount)
    local newHealth = viewModel.playerHealth - amount
    viewModel.playerHealth = math.max(0, newHealth)
    
    print("Player took " .. amount .. " damage. Health: " .. viewModel.playerHealth)
    
    return viewModel.playerHealth
end

-- Restores the player's health by the given amount, capped at the player's maximum health.
-- @param viewModel The view model containing player state.
-- @param amount The amount of health to restore.
-- @return The player's updated health after healing.
function Heal(viewModel, amount)
    local newHealth = viewModel.playerHealth + amount
    viewModel.playerHealth = math.min(viewModel.playerMaxHealth, newHealth)
    
    print("Player healed " .. amount .. ". Health: " .. viewModel.playerHealth)
    
    return viewModel.playerHealth
end

-- Attempts to consume a specified amount of mana from the view model.
-- If the view model has enough mana, reduces viewModel.playerMana by `amount`.
-- @param viewModel The ViewModel table containing player state.
-- @param amount The amount of mana to consume.
-- @return `true` if the mana was sufficient and deducted, `false` otherwise.
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

-- Restores the player's mana by the specified amount, clamped to the player's maximum mana.
-- @param amount The amount of mana to add to the current mana (will be capped at max mana).
-- @return The player's mana after restoration.
function RestoreMana(viewModel, amount)
    local newMana = viewModel.playerMana + amount
    viewModel.playerMana = math.min(viewModel.playerMaxMana, newMana)
    
    print("Restored " .. amount .. " mana. Mana: " .. viewModel.playerMana)
    
    return viewModel.playerMana
end

-- Increases the player's gold by the specified amount.
-- @param amount The amount of gold to add.
-- @return The updated total gold.
function AddGold(viewModel, amount)
    viewModel.gold = viewModel.gold + amount
    print("Added " .. amount .. " gold. Total: " .. viewModel.gold)
    return viewModel.gold
end

-- Attempts to deduct a specified amount of gold from the view model's balance.
-- If sufficient gold exists, reduces viewModel.gold by the amount and logs the transaction.
-- @param amount The amount of gold to spend.
-- @return `true` if the amount was deducted, `false` otherwise.
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

-- Adds the specified amount of experience to the player's current experience total.
-- @param amount The experience points to add.
-- @return The player's updated experience total.
function AddExperience(viewModel, amount)
    viewModel.playerExperience = viewModel.playerExperience + amount
    print("Gained " .. amount .. " experience. Total: " .. viewModel.playerExperience)
    
    -- Level up check is handled in OnSetProperty
    
    return viewModel.playerExperience
end

-- Apply a level increase to the player and update related stats on the view model.
-- This mutates the viewModel: increments `playerLevel`, subtracts the current `experienceToNextLevel`
-- from `playerExperience`, increases `playerMaxHealth` and `playerMaxMana`, restores `playerHealth`
-- and `playerMana` to their new maxima, and increases `experienceToNextLevel` (multiplied by 1.5 and floored).
-- @param viewModel The view model table representing the player; its level, experience, health, mana, and thresholds are updated.
-- @return The player's new level (number).
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

-- Reset the player's state to initial respawn values after death.
-- Sets playerLevel to 1, playerExperience to 0, and experienceToNextLevel to 100.
-- Restores max stats to playerMaxHealth = 100 and playerMaxMana = 50, and fills current health/mana to those maxima.
-- Reduces gold to half its current value, rounded down.
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

-- Provides a table snapshot of the player's current stats.
-- @return A table with the following keys:
--   name (string) - playerName,
--   level (number) - playerLevel,
--   health (number) - playerHealth,
--   maxHealth (number) - playerMaxHealth,
--   mana (number) - playerMana,
--   maxMana (number) - playerMaxMana,
--   experience (number) - playerExperience,
--   experienceToNext (number) - experienceToNextLevel,
--   gold (number) - gold,
--   gems (number) - gems.
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