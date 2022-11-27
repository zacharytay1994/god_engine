-- This script contains the attacking logic for Squinkies.
-- During the Squinkies' turn, if it is adjacent to the Player, it will do a melee attack.

--[IsComponent]
function C_EnemyAttackSquinky()
    local var = {

        -- cue to attempt a Charging Attack
        executeAttack = false,

        -- for turning Squinky in the direction it is attacking
        squinkyRotation = 0,

        -- damage for Charging Attack
        attackDamage = 2,

    }
    return function()
        return var
    end
end

--[IsSystem]
function S_EnemyAttackSquinky(e)
        
    -- print("[EnemyAttackSquinky.lua] Start of S_EnemyAttackSquinky()")
    
    -- get EnemyAttackDummee component
    local attackComponent = GetComponent(e, "C_EnemyAttackSquinky")
    
    -- if enemy is not cued to attack, then don't run this script
    if (attackComponent.executeAttack == false) then
        return
    end

    local enemyController = GetComponent(e, "C_EnemyController")

    -- stop the script here if Player entity does not exist as Dummee will have no one to attack :(
    local playerEntity = GetEntity("Player")
    if (playerEntity == -1) then
        print("[EnemyAttackSquinky.lua] ERROR: Player does not exist! Returning.")
        return
    end

    -- if Squinky is beside Player, then attack Player and reset variables
    if (EnemyAttackSquinkyAdjacentToPlayer(e, playerEntity)) then

        -- apply damage
        playerCharacterComponent = GetComponent(playerEntity, "C_Character")
        playerCharacterComponent.currentHP = playerCharacterComponent.currentHP - attackComponent.attackDamage
        
        -- turn Squinky to face Player
        GetTransform(e).rotation.y = attackComponent.squinkyRotation

        -- trigger screenshake
        screenShakeEntity = GetEntity("ScreenShake")
        if (screenShakeEntity ~= -1) then
            screenShakeComponent = GetComponent(screenShakeEntity, "C_ScreenShake")
            screenShakeComponent.duration = 0.25
            screenShakeComponent.doScreenShake = true
        end

        -- trigger sound effect
        InstancePrefab("SFX_Jab",0,0,0)

        -- trigger particles (if any)
            
        -- play attack animation (if any)

        -- reset variables
        attackComponent.executeAttack = false
        -- attackComponent.dummeeRotation = 0

        -- end the attackc
        enemyController.hasAttacked = true
        print("[EnemyAttackSquinky.lua] Squinky has completed Melee Attack!")
        
    -- else, cannot attack, just reset variables
    else
        
        -- reset variables
        attackComponent.executeAttack = false
        -- attackComponent.dummeeRotation = 0

        -- end the attackc
        enemyController.hasAttacked = true
        print("[EnemyAttackSquinky.lua] Squinky has not adjacent to Player, cannot attack!")
    end  
end

function EnemyAttackSquinkyAdjacentToPlayer(enemy, player)
    
    -- init result
    local result = false

    -- get attacker and defenders' locations
    local enemyGrid = GetGridCell(enemy)
    local playerGrid = GetGridCell(player)

    local attackComponent = GetComponent(enemy, "C_EnemyAttackSquinky")

    if (enemyGrid.y == playerGrid.y) then 

        -- enemy behind player
        if     (enemyGrid.x == playerGrid.x and enemyGrid.z == playerGrid.z - 1) then
            result = true 
            attackComponent.squinkyRotation = 0

        -- enemy in front of player
        elseif (enemyGrid.x == playerGrid.x and enemyGrid.z == playerGrid.z + 1) then
            result = true 
            attackComponent.squinkyRotation = 180

        -- enemy to player's left
        elseif (enemyGrid.z == playerGrid.z and enemyGrid.x == playerGrid.x - 1) then
            result = true 
            attackComponent.squinkyRotation = 90

        -- enemy to player's right
        elseif (enemyGrid.z == playerGrid.z and enemyGrid.x == playerGrid.x + 1) then
            result = true  
            attackComponent.squinkyRotation = 270
        end
    end

    return result
end