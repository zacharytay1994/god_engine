-- C_Character contains basic data that all characters (players and enemies) will need, such as maxHP,
-- currentHP, maxStamina, currentStamina, strength, defence. 

-- TODO: 
-- 1) 
-- 2)

-- IMPORTANT NOTE: STAMINA MUST ONLY BE REFRESHED AT THE START OF CHARACTER'S TURN, NOT AFTER IT ENDS
-- this is because their remaining stamina must be retained in order to calculate next turn's turn order.

-- local util = require "util"

--[IsComponent]
function C_Character()
    local var = {     
        -- for identifying the type of character 
        -- e.g. Player, Squinky, etc
        --[SerializeString]
        characterType = "",
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
        --[SerializeBool]
        isDead = false,

        -- StateMoveEnemy will set this to true once it is finished. then the enemy-specific script will reset this.
        moved = false

        -- name of icon texture (for turn order UI)
        -- iconName = ""
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
    
        -- getting required components
        local turnOrderManagerComponent = GetComponent(turnOrderManagerEntity, "C_TurnOrderManager")       
        local entityDataComponent = GetEntityData(e)
        local characterComponent = GetComponent(e, "C_Character")

        if (characterComponent.currentHP <= 0) then 
            
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
        
        -- press J to set all enemy HP to zero
        if (CheckKeyPress(74)) then
            
            enemyList = EntitiesWithScriptComponent("C_StateMoveEnemy")
            
            for i = 1, #enemyList do
                GetComponent(enemyList[i], "C_Character").currentHP = 0
            end
            print(#enemyList, "enemies set to 0 HP!")
        end

        -- only run the rest of this script if it is currently this character's turn
        if (entityDataComponent.id == turnOrderManagerComponent.currentTurn) then
        
            -- refresh stamina will be refreshed by TurnOrderManager, after it builds the turnQueue

            -- press T to reset player stamina
            if (CheckKeyPress(84) and EntityName(e) == "Player") then
                characterComponent.currentStamina = characterComponent.maxStamina
                print("[Character.lua] Player's stamina refreshed! Back to", characterComponent.currentStamina)
            end

            -- -- press Y to unselect the Move button
            -- if (CheckKeyPress(89)) then
            --     GetComponent(e, "C_Player").selectedAction = nil
            --     print("Releasing MoveButton. Player's selectedAction is:", GetComponent(e, "C_Player").selectedAction)
            -- end
            
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

            -- press 0 (quick debug)
            if (CheckKeyPress(48) == true) then   
                -- util.doSomething()
                print(GetComponent(e, "C_Player").selectedAction)
            end
        end
    end
end