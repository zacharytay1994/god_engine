-- This script contains the AI for the small and fast enemy fish, controlling it's:
-- 1) Movement
-- 2) Attack
-- 3) Any special behaviour

-- The suicide bomber will make a beeline towards the player and explode once it's right beside the player,
-- dealing massive damage to the player and killing itself. 
-- Leaves a crater in its wake.

--[IsComponent]
function C_SuicideBomber()
    local var = {
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
function S_SuicideBomber(e)
    
    --print("[SmallFastFish.lua] Start")
    
    -- getting turn order manager
    local turnOrderManagerEntity = GetEntity("TurnOrderManager")
    local turnOrderManagerComponent
    if (turnOrderManagerEntity ~= -1) then
        turnOrderManagerComponent = GetComponent(turnOrderManagerEntity, "C_TurnOrderManager")
    end

    local enemyComponent = GetComponent(e, "C_SuicideBomber")
    local entityDataComponent = GetEntityData(e)

    local playerEntity = GetEntity("Player")
    if (playerEntity == -1) then
        print("[EnemySuicideBomber.lua] ERROR: Player does not exist! Returning.")
        return
    end

    -- check if its this enemy's turn
    if (entityDataComponent.id == turnOrderManagerComponent.currentTurn) then
    
        local characterComponent = GetComponent(e, "C_Character")

        -- if enemy has not moved or finished moving, then continue moving
        if (enemyComponent.hasMoved == false) then

            -- wait for StateMoveEnemy.lua to finish.
            -- StateMoveEnemy.lua will set C_Character.moved to true

            if (characterComponent.moved) then
                print("[EnemySuicideBomber.lua] Done moving.")
                enemyComponent.hasMoved = true
                characterComponent.moved = false
            end

        elseif (enemyComponent.hasAttacked == false) then
        
            if (CheckEnemyAdjacentToPlayer(e, playerEntity)) then
                
                local playerCharacterComponent = GetComponent(playerEntity, "C_Character")
                
                --attack player
                print("[EnemySuicideBomber.lua] Player's HP before getting hit is", playerCharacterComponent.currentHP)
                print("[EnemySuicideBomber.lua] SuicideBomber exploding!")

                -- damage the player
                playerCharacterComponent.currentHP = playerCharacterComponent.currentHP - 5
                
                -- kill off the suicide bomber
                characterComponent.currentHP = 0

                -- leave a crater
                local enemyGrid = GetGridCell(e)
                local tileList = EntitiesWithScriptComponent("C_FloorTile")
                for i = 1, #tileList do
                    local tileGrid = GetGridCell(tileList[i])
                    
                    if (tileGrid.x == enemyGrid.x and tileGrid.y == enemyGrid.y - 1 and tileGrid.z == enemyGrid.z) then
                        -- remove the tile below the suicide bomber
                        RemoveInstance(tileList[i])    
                        print("[EnemySuicideBomber.lua] Removed the tile below suicide bomber")
                        break
                    end

                end

                -- activate screenshake
                local screenShakeEntity = GetEntity("ScreenShake")
                if (screenShakeEntity ~= -1) then
                    local screenShakeComponent = GetComponent(screenShakeEntity, "C_ScreenShake")
                    screenShakeComponent.duration = 0.25
                    screenShakeComponent.doScreenShake = true
                end

                print("[EnemySuicideBomber.lua] Player's HP after getting hit is", playerCharacterComponent.currentHP)
            end

            -- set hasAttacked to true
            enemyComponent.hasAttacked = true

        elseif (enemyComponent.hasPerformedSpecialBehaviour == false) then

            -- special action code here
            print("[EnemySuicideBomber.lua] Pretend to perform special action.")

            -- set to true
            enemyComponent.hasPerformedSpecialBehaviour = true
        end

        -- once all actions are performed, end turn
        if (enemyComponent.hasMoved and enemyComponent.hasAttacked and enemyComponent.hasPerformedSpecialBehaviour) then
            
            -- reset variables 
            enemyComponent.hasMoved = false
            enemyComponent.hasAttacked = false
            enemyComponent.hasPerformedSpecialBehaviour = false
            print("[EnemySuicideBomber.lua] Resetting hasMoved, hasAttacked, hasPerformedSpecialBehaviour.")

            -- may need to move this to StateMoveEnemy.lua 
            GetComponent(e, "C_Character").moved = false
            
            -- set next turn to true
            turnOrderManagerComponent.nextTurn = true

            print("[EnemySuicideBomber.lua] Ending enemy turn!")
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