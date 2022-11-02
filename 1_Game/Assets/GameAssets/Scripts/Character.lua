-- C_Character contains basic data that all characters (players and enemies) will need, such as maxHP,
-- currentHP, maxStamina, currentStamina, strength, defence. 

-- TODO: 
-- 1) 
-- 2)

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
        defence = 10,

        -- set to true when character's HP hits zero. Makes TurnOrderManager skip this character's turn
        isDead = false,

        -- StateMoveEnemy will set this to true once it is finished. then the enemy-specific script will reset this.
        moved = false
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

        -- press T to reset player stamina
        if (CheckKeyPress(84) and EntityName(e) == "Player") then
            characterComponent.currentStamina = characterComponent.maxStamina
            print("Player's stamina refreshed! Back to", characterComponent.currentStamina)
        end

        -- press T to reset player stamina
        if (CheckKeyPress(89)) then
            GetComponent(GetEntity("MoveButton"), "C_ButtonOnHoverPopup").resetSize = true
            GetComponent(e, "C_Player").selectedAction = nil
            print("Releasing MoveButton. Player's selectedAction is:", GetComponent(e, "C_Player").selectedAction)
        end

        -- breaks the game
        -- press K to remove enemy entity
        if (CheckKeyPress(75) and EntityName(e) == "Enemy") then
            RemoveInstance(e)
        end

        -- press J to set all enemy HP to zero
        if (CheckKeyPress(74)) then
            
            enemyList = EntitiesWithScriptComponent("C_EnemyEndTurn")
            
            for i = 1, #enemyList do
                GetComponent(enemyList[i], "C_Character").currentHP = 0
            end
        end
        
        if (characterComponent.currentHP <= 0) then 

            print("enemy hP is zero")
            
            -- hide the character below the map
            GetTransform(e).position.y = -100
            GetGridCell(e).y = -100
            
            -- set character to dead
            characterComponent.isDead = true

            -- RemoveInstance will be called by TurnOrderManager (near the end of the script)

        end

        if (characterComponent.isDead) then
            return
        end

        -- only run the rest of this script if it is currently this character's turn
        if (entityDataComponent.id == turnOrderManagerComponent.currentTurn) then
        
            -- refresh stamina will be refreshed by TurnOrderManager, after it builds the turnQueue
            
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