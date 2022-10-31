-- This script will move the energy bolt object towards the target.
-- Upon reaching target, it will trigger a screenshake.

--[IsComponent]
function C_MoveEnergyBolt()
    local var = {
        
        --[SerializeString]
        MoveEnergyBolt = "MoveEnergyBolt",
        

    }
    return function()
        return var
    end
end

--[IsSystem]
function S_MoveEnergyBolt(e)

    
end
