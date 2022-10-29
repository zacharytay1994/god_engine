-- This script will allow characters to perform an attack

--[IsComponent]
function C_CombatManager()
    local var = {
        --[SerializeString]
        CharacterAttack = "CharacterAttack",

        -- currently selected attack (refer to AttackList for the ID number)
        selectedAttack = 0,

        -- target entity
        targetEntity = -1,

        -- will be set to false while combat resolution is taking place
        canAttack = true
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_CombatManager(e)
    
    -- only run this script if its the character's turn
    
    -- select an attack 

    -- select a target

    -- if (attack ~= nil and target ~= nil) then initiate combat

    -- pass this entity, attacktype, and target to CombatManager

    -- CombatManager will deduct the target's HP accordingly

end