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

    -- Pressing 1, 2, 3, 4 does the same thing as clicking the Action Buttons.
    -- Leaving these here for quick access.
    
    playerComponent = GetComponent(e, "C_Player")

    if (CheckKeyPress(49)) then
        playerComponent.selectedAction = "FrontJab"
    end

    -- if (CheckKeyPress(50)) then
    --     playerComponent.selectedAction = "BigSweep"
    -- end

    if (CheckKeyPress(51)) then
        playerComponent.selectedAction = "EnergyBolt"
    end

    if (CheckKeyPress(52)) then 
        playerComponent.selectedAction = "Move"
    end

end