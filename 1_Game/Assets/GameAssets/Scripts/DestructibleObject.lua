-- This script is to be added to destructible objects like IceTiles / some rocks.
-- For now it doesn't do anything by itself, the attack scripts will check if the
-- target has this script attached. If yes then the target gets destroyed.

-- TODO:
-- 1) Apply fall damage when a character falls due to ice tile melting

--[IsComponent]
function C_DestructibleObject()
    local var = {

        --[SerializeString]
        Name = "DestructibleObject"

    }
    return function()
        return var
    end
end

--[IsSystem]
function S_DestructibleObject(e)
end