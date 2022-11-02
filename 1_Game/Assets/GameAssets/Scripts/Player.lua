-- This script stores data such as currently selected action.
-- Probably will not have a system.

-- TODO:
-- 1) 

--[IsComponent]
function C_Player()
    local var = {
        --[SerializeString]
        Player = "Player",

        -- certain systems will run / not run based on currently selected action
        selectedAction = nil
    };
    return function()
        return var
    end
end

--[IsSystem]
function S_Player(e)  

    -- clicking on UI buttons will replace these placeholder functions!!!
    
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