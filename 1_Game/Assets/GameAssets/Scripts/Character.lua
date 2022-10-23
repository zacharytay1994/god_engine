-- C_Character contains basic data that all characters (players and enemies) will need, such as maxHP,
-- currentHP, maxStamina, currentStamina, strength, defence. 

-- TODO: 
-- 1) need to access character's EntityData component to check its ID against TurnOrderManager.currentTurn

--[IsComponent]
function C_Character()
    local var = {
        --[SerializeInt]
        maxHP = 10,
        --[SerializeInt]
        currentHP = 10,
        --[SerializeInt]
        maxStamina = 10,
        --[SerializeInt]
        currentStamina = 10,
        --[SerializeInt]
        strength = 10 ,
        --[SerializeInt]
        defence = 10
    };
    return function()
        return var
    end
end

--[IsSystem]
function S_Character(e)
    
    -- getting TurnOrderManager entity to check whether it's this character's turn
    local turnOrderManagerEntity = GetEntity("TurnOrderManager") 
    local turnOrderManagerComponent = GetComponent(turnOrderManagerEntity, "C_TurnOrderManager")

    -- local characterEntityData = GetEntityData(e)

    -- if (characterEntityData.ID == turnOrderManagerComponent.currentTurn) then
        -- run the rest of this script
    
        -- press L to check character's coordinates on the grid
        if (CheckKeyDown(76) == true) then
            local cell = GetGridCell(e)
            print(cell.x, cell.y, cell.z)
        end

    --end
end