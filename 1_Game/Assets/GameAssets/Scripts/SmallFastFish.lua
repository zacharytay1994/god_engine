-- This script contains the AI for the small and fast enemy fish, controlling it's:
-- 1) Movement
-- 2) Attack
-- 3) Any special behaviour

--[IsComponent]
function C_SmallFastFish()
    local var = {
        --[SerializeString]
        EnemySmallFastFish = "SmallFastFish",

        -- will be set by StateMoveEnemy.lua
        hasMoved = false,
        
        -- will be set by specific enemy script
        hasAttacked = false,

        -- will be set by specific enemy script
        hasPerformedSpecialBehaviour = false

        -- 1 is move right beside player, 2 is move to a certain distance from player, 0 is keep still
        -- movementType = 1
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_SmallFastFish(e)
    
    --print("[SmallFastFish.lua] Start")
    
    local turnOrderManagerEntity = GetEntity("TurnOrderManager")
    local turnOrderManagerComponent
    if (turnOrderManagerEntity ~= -1) then
        turnOrderManagerComponent = GetComponent(turnOrderManagerEntity, "C_TurnOrderManager")
    end

    local enemyComponent = GetComponent(e, "C_SmallFastFish")
    local entityDataComponent = GetEntityData(e)

    -- print(entityDataComponent.id)
    -- print(turnOrderManagerComponent.currentTurn)

    -- check if its this enemy's turn
    if (entityDataComponent.id == turnOrderManagerComponent.currentTurn) then
    
        local characterComponent = GetComponent(e, "C_Character")

        -- if enemy has not moved or finished moving, then continue moving
        if (enemyComponent.hasMoved == false) then

            -- wait for StateMoveEnemy.lua to finish.
            -- StateMoveEnemy.lua will set C_Character.moved to true

            --print("[SmallFastFish.lua] characterComponent.moved", characterComponent.moved)

            if (characterComponent.moved) then
                print("[EnemySmallFastFish.lua] Done moving.")
                enemyComponent.hasMoved = true
                characterComponent.moved = false
            end

        elseif (enemyComponent.hasAttacked == false) then
        
            if (CheckEnemyAdjacentToPlayer(e, GetEntity("Player"))) then
                
                local playerCharacterComponent = GetComponent(GetEntity("Player"), "C_Character")
                
                --attack player
                print("[SmallFastFish.lua] Player's HP before getting hit is", playerCharacterComponent.currentHP)
                print("[SmallFastFish.lua] SmallFastFish attacks player!")

                playerCharacterComponent.currentHP = playerCharacterComponent.currentHP - 1

                -- activate screenshake
                print("triggering screenshake!")
                screenShakeEntity = GetEntity("ScreenShake")
                if (screenShakeEntity ~= -1) then
                    screenShakeComponent = GetComponent(screenShakeEntity, "C_ScreenShake")
                    screenShakeComponent.duration = 0.25
                    screenShakeComponent.doScreenShake = true
                end

                print("[SmallFastFish.lua] Player's HP after getting hit is", playerCharacterComponent.currentHP)

            end

            -- set hasAttacked to true
            enemyComponent.hasAttacked = true

        elseif (enemyComponent.hasPerformedSpecialBehaviour == false) then

            -- special action code here
            print("[SmallFastFishSmallFastFish.lua] Pretend to perform special action.")

            -- set to true
            enemyComponent.hasPerformedSpecialBehaviour = true
        end

        -- once all actions are performed, end turn
        if (enemyComponent.hasMoved and enemyComponent.hasAttacked and enemyComponent.hasPerformedSpecialBehaviour) then
            
            -- reset variables 
            enemyComponent.hasMoved = false
            enemyComponent.hasAttacked = false
            enemyComponent.hasPerformedSpecialBehaviour = false
            
            -- set next turn to true
            turnOrderManagerComponent.nextTurn = true
        end
    end

end

function CheckEnemyAdjacentToPlayer(enemy, player)
    
    -- init result
    result = false

    -- get attacker and defenders' locations
    enemyGrid = GetGridCell(enemy)
    playerGrid = GetGridCell(player)

    if (enemyGrid.y == playerGrid.y) then 

        -- enemy behind player
        if     (enemyGrid.x == playerGrid.x and enemyGrid.z == playerGrid.z - 1) then
            result = true 

        -- enemy in front of player
        elseif (enemyGrid.x == playerGrid.x and enemyGrid.z == playerGrid.z + 1) then
            result = true 

        -- enemy to player's left
        elseif (enemyGrid.z == playerGrid.z and enemyGrid.x == playerGrid.x - 1) then
            result = true 

        -- enemy to player's right
        elseif (enemyGrid.z == playerGrid.z and enemyGrid.x == playerGrid.x + 1) then
            result = true  
        end
    end

    return result
end

-- helper function template
-- function HelperFunction(attacker, defender, e)
    
--     -- init result
--     result = false

--     return result
-- end