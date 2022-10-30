-- This script contains the AI for the small and fast enemy fish, controlling it's:
-- 1) Movement
-- 2) Attack
-- 3) Ending turn

--[IsComponent]
function C_SmallFastFish()
    local var = {
        --[SerializeString]
        SmallFastFish = "SmallFastFish",

        -- 1 is move right beside player, 2 is move to a certain distance from player, 0 is keep still
        movementType = 1
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_SmallFastFish(e)
    
    -- check if its this enemy's turn

    -- check if enemy needs to move (already beside player?)

    -- check if able to perform attack

    -- once moved and attacked, end turn

end