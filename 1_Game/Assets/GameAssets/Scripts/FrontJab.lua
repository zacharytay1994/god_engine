-- This script will allow the player to perform a front jab attack.
-- This script will be attached to CombatManager.
-- CombatManager will run this script whenever the player uses a Front Jab attack.

-- TODO:
-- 1) 

--[IsComponent]
function C_FrontJab()
    local var = {
        --[SerializeString]
        PlayerAttack = "PlayerAttack",

        -- currently selected attack (refer to AttackList)
        selectedAttack = nil,

        -- target entity
        targetEntity = -1,

        -- reset variables
        resetVariables = false

        -- -- will be set to false while combat resolution is taking place
        -- canAttack = true
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_FrontJab(e)
    
    -- check if player is adjacent to enemy

    -- turn player to face enemy
    
    
    -- play attack animation (if any)

    -- activate screenshake

    -- trigger sound effect

    -- trigger particles (if any)

end