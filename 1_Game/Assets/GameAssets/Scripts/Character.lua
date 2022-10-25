-- C_Character contains basic data that all characters (players and enemies) will need, such as maxHP,
-- currentHP, maxStamina, currentStamina, strength, defence. 

-- TODO: 
-- 1) need to access entity's name to print along with their ID number for easier identification
-- 2) refresh character's stamina at the start of a new turn

-- IMPORTANT NOTE: STAMINA MUST ONLY BE REFRESHED AT THE START OF CHARACTER'S TURN, NOT AFTER IT ENDS
-- this is because their remaining stamina must be retained in order to calculate next turn's turn order.

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

    -- checking whether turnOrderManager is nil
    if (turnOrderManagerEntity ~= -1) then
    
        -- getting TurnOrderManager component
        local turnOrderManagerComponent = GetComponent(turnOrderManagerEntity, "C_TurnOrderManager")       
        -- getting this character's ID number
        local entityDataComponent = GetEntityData(e)

        -- only run the rest of this script if it is currently this character's turn
        if (entityDataComponent.id == turnOrderManagerComponent.currentTurn) then
        
            -- press L to check character's coordinates on the grid
            if (CheckKeyPress(76) == true) then
                local cell = GetGridCell(e) -- note to self: cell ~= nil
                print(cell.x, cell.y, cell.z)
            end

            -- press I to print character's ID number
            if (CheckKeyPress(73) == true) then           
                -- if (entityDataComponent ~= nil) then 
                --     print("entityData is not nil")
                -- else
                --     print("entityData is nil")
                -- end
                print(entityDataComponent.id)
            end
        end
    end
end