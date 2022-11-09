-- This script stores data such as currently selected action.
-- Probably will not have a system.

-- This component is used by:
-- 1) ActionButton.lua

-- TODO:
-- 1) 

--[IsComponent]
function C_Player()
    local var = {
        --[SerializeString]
        Player = "Player",

        -- certain systems will run / not run based on currently selected action
        selectedAction = nil,

        -- used by ActionButton.lua to set button texture to empty after player uses the attack
        playerAttacked = false
    };
    return function()
        return var
    end
end

--[IsSystem]
function S_Player(e)  

    -- Pressing 1, 2, 3, 4, etc does the same thing as clicking the Action Buttons.
    -- Leaving these here for quick access.

    -- Currently some attacks are hardcoded (e.g. BigSwing). Before un-hardcoding them:
    -- 1) Need Action Button Texture
    -- 2) Need Dice to return a number between 0 - 8
    -- More detail in ActionButton.lua
    
    playerComponent = GetComponent(e, "C_Player")

    if (CheckKeyPress(49)) then
        playerComponent.selectedAction = "FrontJab"
    end

    if (CheckKeyPress(50)) then
        playerComponent.selectedAction = "BigSwing"
        playerAttackComponent.selectedAttack = attackList[10]
            print("[PlayerAttack.lua] Selected Player attack:", playerAttackComponent.selectedAttack[1], 
                        "Base damage:", playerAttackComponent.selectedAttack[2], 
                        "Special property:", playerAttackComponent.selectedAttack[3], "\n")
    end

    if (CheckKeyPress(51)) then
        playerComponent.selectedAction = "EnergyBolt"
    end

    if (CheckKeyPress(52)) then 
        playerComponent.selectedAction = "Move"
    end

end