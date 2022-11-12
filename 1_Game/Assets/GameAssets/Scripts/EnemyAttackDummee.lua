-- This script contains the attacking logic for Dummees.
-- During the Dummee's turn, if the Player is in Dummee's line of sight, Dummee will do a Charging Attack.

-- Charging Attack: Dummee will move towards the Player until it hits something.
--                  If it hits a character, the character will get pushed one tile, while Dummee will bounce back 3 tiles.
--                  If it hits a destructible rock, the rock will be destroyed, while Dummee will bounce back 3 tiles.
--                  If it hits an indestructible object (like a floor tile), Dummee will stop in its track instead of bouncing back.

-- TODO:
-- 1) Apply damage to victim
-- 2) Apply pushback to victim
-- 3) Apply recoil to Dummee

--[IsComponent]
function C_EnemyAttackDummee()
    local var = {

        -- cue to attempt a Charging Attack
        executeAttack = false,

        -- for turning the Dummee when it does the Charging Attack
        dummeeRotation = 0,

        -- the entity on the receiving end of the Charging Attack
        -- could be a floor tile, a rock, the player, or another Dummee
        victim = nil,

        -- to stop Dummee's charge
        stopCharge = false,

        -- amount of time to pause between each tile while charging
        chargeInterval = 0.3,

        -- for EnemyAttackDummeeMoveDummeeTowardsPlayer() to use for pausing
        accumTime = 0

    }
    return function()
        return var
    end
end

--[IsSystem]
function S_EnemyAttackDummee(e)
        
    -- get EnemyAttackDummee component
    local attackComponent = GetComponent(e, "C_EnemyAttackDummee")
    
    -- if enemy is not cued to attack, then don't run this script
    if (attackComponent.executeAttack == false) then
        return
    end

    local enemyController = GetComponent(e, "C_EnemyController")

    -- stop the script here if Player entity does not exist as Dummee will have no one to attack :(
    local playerEntity = GetEntity("Player")
    if (playerEntity == -1) then
        print("[EnemyAttackDummee.lua] ERROR: Player does not exist! Returning.")
        return
    end

    -- stop the script if Dummee and Player are not in the same lane
    if (EnemyAttackDummeeSameLane(e, playerEntity) == false) then
        attackComponent.executeAttack = false
        enemyController.hasAttacked = true
        print("[EnemyAttackDummee.lua] Dummee is not in the same lane as Player, cannot use Charging Attack!")
        return
    end

    -- checking for obstacles between Dummee and Player 
    if (attackComponent.victim == nil) then
        print("[EnemyAttackDummee.lua] Checking for obstacles between Dummee and Player!")
        attackComponent.victim = EnemyAttackDummeeFirstVictim(e, playerEntity)
        print("[EnemyAttackDummee.lua] victim is", EntityName(attackComponent.victim))
    end

    -- start moving Dummee, stop when it hits the first obstacle in its path
    EnemyAttackDummeeMoveDummeeTowardsPlayer(e, playerEntity)

    if (attackComponent.stopCharge) then
    
        -- apply damage to victim (if applicable)
    
        -- apply push to victim (if applicable)

        -- apply recoil to Dummee (if applicable)
        
        -- trigger screenshake
        local screenShakeEntity = GetEntity("ScreenShake")
        if (screenShakeEntity ~= -1) then
            local screenShakeComponent = GetComponent(screenShakeEntity, "C_ScreenShake")
            screenShakeComponent.duration = 0.25
            screenShakeComponent.doScreenShake = true
        end

        -- trigger sound effect
        InstancePrefab("SFX_Jab",0,0,0)

        -- reset variables
        attackComponent.executeAttack = false
        attackComponent.dummeeRotation = 0
        attackComponent.victim = nil
        attackComponent.stopCharge = false
        attackComponent.accumTime = 0
        
        -- end the attack
        enemyController.hasAttacked = true
        print("[EnemyAttackDummee.lua] Dummee has completed Charging Attack!")
    end
end

function EnemyAttackDummeeSameLane(dummee, player)
    
    -- init result
    local result = false

    -- get attacker and defenders' locations
    local dummeeGrid = GetGridCell(dummee)
    local playerGrid = GetGridCell(player)

    -- getting EnemyAttackDummee component
    local attackComponent = GetComponent(dummee, "C_EnemyAttackDummee")

    if (dummeeGrid.y ~= playerGrid.y) then 
        print("[EnemyAttackDummee.lua] Dummee and Player are on different y-axis! Cannot use Charging Attack! Returning.")
        return
    end

    -- Dummee and player on the same x-axis
    if (dummeeGrid.x == playerGrid.x) then

        -- player is behind Dummee
        if (dummeeGrid.z > playerGrid.z) then 
            result = true 
            attackComponent.dummeeRotation = 180

        -- player is in front of Dummee
        else
            result = true 
            attackComponent.dummeeRotation = 0
        end

    -- enemy and player on the same z-axis
    elseif (dummeeGrid.z == playerGrid.z) then

        -- player is to Dummee's right
        if (dummeeGrid.x > playerGrid.x) then
            result = true 
            attackComponent.dummeeRotation = 270    
        
        -- player is to Dummee's left
        else
            result = true 
            attackComponent.dummeeRotation = 90
        end   
    else
        attackComponent.errorMessage = "[EnemyAttackDummee.lua] Cannot use Charging Attack as Dummee is not in same lane as Player."
    end

    return result
end

function EnemyAttackDummeeFirstVictim(dummee, player)
    
    -- init result
    local result = nil

    -- get attacker and defenders' locations
    local dummeeGrid = GetGridCell(dummee)
    local playerGrid = GetGridCell(player)

    -- getting EnemyAttackDummee component
    local attackComponent = GetComponent(dummee, "C_EnemyAttackDummee")

    local affectedGridList = { }
    local currentX = dummeeGrid.x
    local currentZ = dummeeGrid.z

    local entitiesWithGridCell = EntitiesWithEngineComponent("GridCell")

    if (attackComponent.dummeeRotation == 180) then
                
        -- make a list of all GridCells to check against (GridCells between Dummee and Player)
        while (currentZ ~= playerGrid.z) do
            currentZ = currentZ - 1
            affectedGridList[#affectedGridList + 1] = { currentX, currentZ } 
        end

    elseif (attackComponent.dummeeRotation == 0) then
        
        -- make a list of all GridCells to check against (GridCells between Dummee and Player)
        while (currentZ ~= playerGrid.z) do
            currentZ = currentZ + 1
            affectedGridList[#affectedGridList + 1] = { currentX, currentZ } 
        end

    elseif (attackComponent.dummeeRotation == 270) then
        
        -- make a list of all GridCells to check against (GridCells between Dummee and Player)
        while (currentX ~= playerGrid.x) do
            currentX = currentX - 1
            affectedGridList[#affectedGridList + 1] = { currentX, currentZ } 
        end

    elseif (attackComponent.dummeeRotation == 90) then

        -- make a list of all GridCells to check against (GridCells between Dummee and Player)
        while (currentX ~= playerGrid.x) do
            currentX = currentX + 1
            affectedGridList[#affectedGridList + 1] = { currentX, currentZ } 
        end
    end

    -- for each GridCell, check against all Entities with GridCell
    for i = 1, #affectedGridList do
        for j = 1, #entitiesWithGridCell do
        
            if (affectedGridList[i][1] == GetGridCell(entitiesWithGridCell[j]).x and
                affectedGridList[i][2] == GetGridCell(entitiesWithGridCell[j]).z and
                dummeeGrid.y           == GetGridCell(entitiesWithGridCell[j]).y) then
            
                -- the first Entity that gets matched will be the victim
                return entitiesWithGridCell[j]
            end
        end
    end
end

function EnemyAttackDummeeMoveDummeeTowardsPlayer(dummee, player)
    
    -- getting EnemyAttackDummee component
    local attackComponent = GetComponent(dummee, "C_EnemyAttackDummee")

    if (attackComponent.stopCharge) then
        return
    end
    
    -- get attacker and defenders' locations
    local dummeeGrid = GetGridCell(dummee)
    local playerGrid = GetGridCell(player)

    -- Note: impossible to do smooth charging animation because Dummee position is locked to the grid.

    if (attackComponent.accumTime < attackComponent.chargeInterval) then
        attackComponent.accumTime = attackComponent.accumTime + GetDeltaTime()
        return
    end
    
    if (attackComponent.dummeeRotation == 180) then
                
        if (dummeeGrid.z > playerGrid.z + 1) then
            dummeeGrid.z = dummeeGrid.z - 1
        else
            attackComponent.stopCharge = true 
        end

    elseif (attackComponent.dummeeRotation == 0) then
                
        if (dummeeGrid.z < playerGrid.z - 1) then
            dummeeGrid.z = dummeeGrid.z + 1
        else
            attackComponent.stopCharge = true 
        end

    elseif (attackComponent.dummeeRotation == 270) then
        
        if (dummeeGrid.x > playerGrid.x + 1) then
            dummeeGrid.x = dummeeGrid.x - 1
        else
            attackComponent.stopCharge = true 
        end

    elseif (attackComponent.dummeeRotation == 90) then

        if (dummeeGrid.x < playerGrid.x - 1) then
            dummeeGrid.x = dummeeGrid.x + 1
        else
            attackComponent.stopCharge = true 
        end
    end

    attackComponent.accumeTime = 0
end