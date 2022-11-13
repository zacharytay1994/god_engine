-- This script contains the attacking logic for Dummees.
-- During the Dummee's turn, if the Player is in Dummee's line of sight, Dummee will do a Charging Attack.

-- Charging Attack: Dummee will move towards the Player until it hits something.
--                  If it hits a character, the character will get pushed one tile, while Dummee will bounce back 3 tiles.
--                  If it hits a destructible rock, the rock will be destroyed, while Dummee will bounce back 3 tiles.
--                  If it hits an indestructible object (like a floor tile), Dummee will stop in its track instead of bouncing back.

-- TODO:
-- 1) Destroy destructible rocks
-- 2) Apply pushback to victim

--[IsComponent]
function C_EnemyAttackDummee()
    local var = {

        -- cue to attempt a Charging Attack
        executeAttack = false,

        -- to stop Dummee's charge
        stopCharge = false,

        damageApplied = false,

        recoilComplete = false,

        -- for turning the Dummee when it does the Charging Attack
        dummeeRotation = 0,

        -- the entity on the receiving end of the Charging Attack
        -- could be a floor tile, a rock, the player, or another Dummee
        victim = nil,

        -- for EnemyAttackDummeeMoveDummeeTowardsPlayer() to use for pausing
        accumTime = 0,

        recoilDestination = nil,

        recoilPath = { },

        -- amount of time to pause between each tile while charging
        chargeInterval = 0.3,

        

        chargeDamage = 2

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
    
        -- apply damage to victim (if applicable) OR destroy rock (if applicable)
        if (attackComponent.damageApplied == false) then
            EnemyAttackDummeeApplyDamage(e, attackComponent.victim)

            -- trigger screenshake
            local screenShakeEntity = GetEntity("ScreenShake")
            if (screenShakeEntity ~= -1) then
                local screenShakeComponent = GetComponent(screenShakeEntity, "C_ScreenShake")
                screenShakeComponent.duration = 0.25
                screenShakeComponent.doScreenShake = true
            end

            -- trigger sound effect
            InstancePrefab("SFX_Jab",0,0,0)

            -- set recoil destination (doesn't do anything if victim is a floor tile)
            EnemyAttackDummeeSetRecoilDestination(e, attackComponent.victim)

            -- apply push to victim (if applicable)

        end 
           
        if (attackComponent.recoilComplete == false) then
            EnemyAttackDummeeMoveToRecoilDestination(e)
        else
            -- reset variables
            attackComponent.executeAttack = false
            attackComponent.stopCharge = false
            attackComponent.damageApplied = false
            attackComponent.recoilComplete = false
            attackComponent.dummeeRotation = 0
            attackComponent.victim = nil
            attackComponent.accumTime = 0
            attackComponent.recoilPath = { }
            
            -- end the attack
            enemyController.hasAttacked = true
            print("[EnemyAttackDummee.lua] Dummee has completed Charging Attack!")
        end    
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
        return result
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

    print("[EnemyAttackDummee.lua] Charging towards Player!")
    attackComponent.accumTime = 0
end

function EnemyAttackDummeeApplyDamage(dummee, victim)

    local characterList = EntitiesWithScriptComponent("C_Character")

    for i = 1, #characterList do
        
        if (characterList[i] == victim) then
            
            local attackComponent = GetComponent(dummee, "C_EnemyAttackDummee")
            local victimCharacterComponent = GetComponent(victim, "C_Character")
            victimCharacterComponent.currentHP = victimCharacterComponent.currentHP - attackComponent.chargeDamage
            print("[EnemyAttackDummee.lua]", EntityName(victim), "currentHP after getting charged is", victimCharacterComponent.currentHP)
            GetComponent(dummee, "C_EnemyAttackDummee").damageApplied = true
            return
        end
    end

    -- break rocks here 

end

function EnemyAttackDummeeSetRecoilDestination(dummee, victim)

    print("[EnemyAttackDummee.lua] Figuring out recoil destination!")
    
    -- check whether victim is a floor tile
    local tileList = EntitiesWithScriptComponent("C_FloorTile")

    for i = 1, #tileList do
        if (tileList[i] == victim) then
            print("[EnemyAttackDummee.lua] Charge Attack victim is a floor tile! No recoil applied.")
            return
        end
    end

    -- Note: if code reaches here, then victim is defo not a floor tile and we can apply recoil to Dummee.
    local dummeeGrid = GetGridCell(dummee)
    local currentX = dummeeGrid.x
    local currentY = dummeeGrid.y
    local currentZ = dummeeGrid.z
    
    local attackComponent = GetComponent(dummee, "C_EnemyAttackDummee")

    -- player is behind Dummee
    if (attackComponent.dummeeRotation == 180) then
                
        -- make a list of all GridCells to check against (the 3 tiles (globally) in front of Dummee)
        local recoilGridZ = dummeeGrid.z + 3
        while (currentZ ~= recoilGridZ) do
            currentZ = currentZ + 1
            attackComponent.recoilPath[#attackComponent.recoilPath + 1] = { currentX, currentY, currentZ } 
        end

    -- player is in front of Dummee
    elseif (attackComponent.dummeeRotation == 0) then
        
        -- make a list of all GridCells to check against (the 3 tiles (globally) behind Dummee)
        local recoilGridZ = dummeeGrid.z - 3
        while (currentZ ~= recoilGridZ) do
            currentZ = currentZ - 1
            attackComponent.recoilPath[#attackComponent.recoilPath + 1] = { currentX, currentY, currentZ } 
        end

    -- player is to Dummee's right
    elseif (attackComponent.dummeeRotation == 270) then
        
        -- make a list of all GridCells to check against (the 3 tiles (globally) left of Dummee)
        local recoilGridX = dummeeGrid.x + 3
        while (currentX ~= recoilGridX) do
            currentX = currentX + 1
            attackComponent.recoilPath[#attackComponent.recoilPath + 1] = { currentX, currentY, currentZ } 
        end

    -- player is to Dummee's left
    elseif (attackComponent.dummeeRotation == 90) then

        -- make a list of all GridCells to check against (the 3 tiles (globally) right of Dummee)
        local recoilGridX = dummeeGrid.x - 3
        while (currentX ~= recoilGridX) do
            currentX = currentX - 1
            attackComponent.recoilPath[#attackComponent.recoilPath + 1] = { currentX, currentY, currentZ } 
        end
    end

    -- if there are any objects in Dummee's recoil path, then set recoil destination as the tile right in front of that object.
    local gridCellList = EntitiesWithEngineComponent("GridCell")
    local collidedIndex = -1

    for j = 1, #attackComponent.recoilPath do
        for k = 1, #gridCellList do
            local currentGridCell = GetGridCell(gridCellList[k])
            -- print(attackComponent.recoilPath[j][1], attackComponent.recoilPath[j][2], attackComponent.recoilPath[j][3], "vs", currentGridCell.x, currentGridCell.y, currentGridCell.z)
            
            if (attackComponent.recoilPath[j][1] == currentGridCell.x and attackComponent.recoilPath[j][2] == currentGridCell.y and attackComponent.recoilPath[j][3] == currentGridCell.z) then
                collidedIndex = j
                break
            end
        end

        if (collidedIndex ~= -1) then
            break
        end
    end

    -- if collidedIndex == -1 then there are not obstacles in Dummee's recoil path
    if (collidedIndex == -1) then
        attackComponent.recoilDestination = attackComponent.recoilPath[3]
        print("[EnemyAttackDummee.lua] There are no obstacles in Dummee's recoil path.")
    
    -- else, there is an obstacle, and Dummee's recoil will end at collidedIndex - 1
    else
        if (collidedIndex - 1 <= 0) then
            print("[EnemyAttackDummee.lua] There is an obstacle right behind Dummee, cannot bounce back.")
            attackComponent.recoilComplete = true
        else
            print("[EnemyAttackDummee.lua] There is an obstacle", j, "tile(s) behind Dummee.") 
            attackComponent.recoilDestination = attackComponent.recoilPath[collidedIndex - 1]
        end
    end
end

function EnemyAttackDummeeMoveToRecoilDestination(dummee)

    -- getting EnemyAttackDummee component
    local attackComponent = GetComponent(dummee, "C_EnemyAttackDummee")

    -- get attacker and defenders' locations
    local dummeeGrid = GetGridCell(dummee)
    local recoilGrid = attackComponent.recoilDestination

    -- Note: impossible to do smooth charging animation because Dummee position is locked to the grid.

    if (attackComponent.accumTime < attackComponent.chargeInterval) then
        attackComponent.accumTime = attackComponent.accumTime + GetDeltaTime()
        return
    end

    for i = 1, #attackComponent.recoilPath do
    
        dummeeGrid.x = attackComponent.recoilPath[i][1]
        dummeeGrid.y = attackComponent.recoilPath[i][2]
        dummeeGrid.z = attackComponent.recoilPath[i][3]

        if (dummeeGrid.x == recoilGrid[1] and dummeeGrid.y == recoilGrid[2] and dummeeGrid.z == recoilGrid[3]) then
            attackComponent.recoilComplete = true
            print("[EnemyAttackDummee.lua] Recoil destination reached!")
            break
        end
    end

    attackComponent.accumeTime = 0
end