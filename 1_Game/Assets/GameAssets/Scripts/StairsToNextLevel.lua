--[IsComponent]
function C_StairsToNextLevel()
    local var = {
		trigger = false
	}
	
	return function()
		return var
	end
end

playerHP = 10
playerCurrentHP = 10
playerStamina = 7
playerCurrentStamina = 7
SwitchScene = false
--[IsSystem]
function S_StairsToNextLevel(e)

    local player = GetEntity("Player")
    local player_grid_cell = GetGridCell(player)
    local stairs = GetComponent(e, "C_StairsToNextLevel")
    local characterComponent = GetComponent(player, "C_Character")
    local cell = GetGridCell(e)

    playerHP = characterComponent.maxHP
    playerCurrentHP = characterComponent.currentHP
    playerStamina = characterComponent.maxStamina
    playerCurrentStamina = characterComponent.currentStamina
    
    if(stairs.trigger == false)
    then
        if(player_grid_cell.x == cell.x and player_grid_cell.y == cell.y + 1 and player_grid_cell.z == cell.z)
        then
            ChangeScene("Level_2",true)
            stairs.trigger = true
            SwitchScene = true
        end
    end

end

function OnLoad_StairsToNextLevel(e)
    -- local player = GetEntity("Player")
    -- local characterComponent = GetComponent(player, "C_Character")

    -- characterComponent.maxHP = playerHP
    -- characterComponent.currentHP = playerCurrentHP
    -- characterComponent.maxStamina = playerStamina
    -- characterComponent.currentStamina = playerCurrentStamina
    -- print("stats loaded")
end