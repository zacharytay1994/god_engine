-- C_Character contains basic data that all characters (players and enemies) will need, such as maxHP,
-- currentHP, maxStamina, currentStamina, strength, defence. 

-- TODO: 
-- 1) need to access entity's name to print along with their ID number for easier identification
-- 2) refresh character's stamina at the start of a new turn
-- 3) destroy character if HP <= 0

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
        maxStamina = 7,
        --[SerializeInt]
        currentStamina = 7,
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
        -- getting this character's C_Character component
        local characterComponent = GetComponent(e, "C_Character")

        -- breaks the game
        -- press K to remove enemy entity
        if (CheckKeyPress(75) and EntityName(e) == "Enemy") then
            RemoveInstance(e)
        end
        
        -- -- breaks the game
        -- if (characterComponent.currentHP <= 0) then 
        --     RemoveInstance(e)
        -- end

        -- only run the rest of this script if it is currently this character's turn
        if (entityDataComponent.id == turnOrderManagerComponent.currentTurn) then
        
            -- press X to check character's coordinates on the grid
            if (CheckKeyPress(88) == true) then
                local cell = GetGridCell(e)
                print(EntityName(e), "GridCell location:", cell.x, cell.y, cell.z)
                local position = GetTransform(e).position
                print(EntityName(e), "Transform position:", position.x, position.y, position.z)
            end

            -- press C to print character's ID number
            if (CheckKeyPress(67) == true) then           
                
                local cell = GetGridCell(e)
                
                print("[ACTIVE CHARACTER'S DETAILS]")
                print("Name:      ", EntityName(e))
                print("ID no.     ", entityDataComponent.id)
                print("Current location:", cell.x, cell.y, cell.z)
                print("maxHP:    ", characterComponent.maxHP)
                print("currentHP:", characterComponent.currentHP)
                print("maxStamina:", characterComponent.maxStamina)
                print("currentStamina:", characterComponent.currentStamina)
                print("strength:", characterComponent.strength)
                print("defence:", characterComponent.defence)
                print("\n\n")
            end
        end
    end
end