-- C_Character contains basic data that all characters (players and enemies) will need, such as maxHP,
-- currentHP, maxStamina, currentStamina, strength, defence. 
-- Note that stamina is refreshed by TurnOrderManager.

-- local util = require "util"

--[IsComponent]
function C_Character()
    local var = {     
        -- for identifying the type of character 
        -- e.g. Player, Squinky, Dummee, etc.
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

        -- will be true when it's this character's turn
        --[SerializeBool]
        isActive = false,
        
        -- enemyController will set this to true when the enemy ends it turns.
        -- this will, in turn, inform TurnOrderManager to go to next turn.
        endTurn = false,

        -- set to true when character's HP hits zero. Makes TurnOrderManager skip this character's turn,
        -- and then call RemoveInstance() on this entity at the end of the round.
        --[SerializeBool]
        isDead = false,

        -- Frozen, Immobilised, Berserk, etc.
        statusAilment = nil
    };
    return function()
        return var
    end
end

isAlive = true
--[IsSystem]
function S_Character(e)
    
    -- getting TurnOrderManager component
    local turnOrderManagerEntity = GetEntity("TurnOrderManager") 
    local turnOrderManagerComponent = nil
    if (turnOrderManagerEntity ~= -1) then
        turnOrderManagerComponent = GetComponent(turnOrderManagerEntity, "C_TurnOrderManager")  
    else
        print("[Character.lua] ERROR: TurnOrderManager entity does not exist! Returning.")
        return
    end
    
    -- getting required components
    local entityDataComponent = GetEntityData(e)
    local characterComponent = GetComponent(e, "C_Character")

    -- don't run this script if the character is already dead
    if (characterComponent.isDead) then
        return
    end

    -- set character to dead if HP falls to zero and below
    if (characterComponent.currentHP <= 0) then 
        
        print("[Character.lua]", EntityName(e), entityDataComponent.id, "has died, hiding body.")

        -- RefreshEnemyForecast()
        -- print("[Character.lua] Refreshing EnemyForecast because an enemy died.")

        -- hide the character below the map for now
        GetTransform(e).position.y = -100
        GetGridCell(e).y = -100
        
        -- set character to dead
        characterComponent.isDead = true

        -- Note: this causes the game to crash when EnemyForecast tries to clear indicatorsList.
        -- remove forecast indicator ---------------------------------------------------------------------
        -- if (characterComponent.characterType == "Dummee" or characterComponent.characterType == "Squinky") then   
        --     local entity = e   --- need this line if not the script will stop running for the player entity
        --     local enemyControllerComponent = GetComponent(entity, "C_EnemyController")
        --     if (enemyControllerComponent.indicatorEntity ~= nil) then
        --         print("[Character.lua] Removing indicatorEntity for character who just died!")
        --         RemoveInstance(enemyControllerComponent.indicatorEntity)
        --         enemyControllerComponent.indicatorEntity = nil
        --     else
        --         print("enemyController.indicatorEntity is nil")
        --     end
        -- end
        --------------------------------------------------------------------------------------------------
         
        if (characterComponent.characterType == "Player" and characterComponent.isDead == true) 
        then
            isAlive = false
            ChangeScene("WinloseScreen",true)
        end
        -- RemoveInstance will be called by TurnOrderManager at the end of current round
    end

    if (characterComponent.endTurn) then
        characterComponent.endTurn = false
        turnOrderManagerComponent.nextTurn = true
        characterComponent.isActive = false
        return
    end
    
    -- set character to active if its ID matches TurnOrderManager.currentTurn
    if (entityDataComponent.id == turnOrderManagerComponent.currentTurn) then
        characterComponent.isActive = true
    else
        characterComponent.isActive = false
    end

    

    -- CHEATS -----------------------------------------------------------------------------------------------------------------------
    -- press J to set all enemy HP to zero
    if (CheckKeyPress(74)) then
        
        enemyList = EntitiesWithScriptComponent("C_EnemyController")
        
        for i = 1, #enemyList do
            GetComponent(enemyList[i], "C_Character").currentHP = 0
        end
        print("[Character.lua]", #enemyList, "enemies set to 0 HP!")
    end

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

    -- press Y to refresh enemy forecasts
    if (CheckKeyPress(89)) then
        RefreshEnemyForecast()
    end

    

    if (characterComponent.isActive) then
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
    -- END OF CHEATS ----------------------------------------------------------------------------------------------------------------   
end