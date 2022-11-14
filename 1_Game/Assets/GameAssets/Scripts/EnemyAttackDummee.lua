-- This script contains the attacking logic for Dummees.
-- During the Dummee's turn, if the Player is in Dummee's line of sight, Dummee will do a Charging Attack.

-- Charging Attack: Dummee will move towards the Player until it hits something.
--                  If it hits a character, the character will get pushed one tile, while Dummee will bounce back 3 tiles.
--                  If it hits a destructible rock, the rock will be destroyed, while Dummee will bounce back 3 tiles.
--                  If it hits an indestructible object (like a floor tile), Dummee will stop in its track instead of bouncing back.

-- A lot of redundant code, will optimize when there's time. Focusing on pushing out features first.

-- TODO:
-- 1) Destroy destructible rocks (when charging into them) or when another character is pushed against them
-- 2) Destructible rocks and Floor Tiles will break the pushback chain
-- 3) Remove Destructible rocks and Floor Tiles from affectedEntities before applying pushback
-- 4) Deal with characters falling onto each other
-- 5) Check if Dummee recoils to a safe location or if it will fall
-- 6) Make Dummee face where he is charging

--[IsComponent]
function C_EnemyAttackDummee()
    local var = {

        -- cue to attempt a Charging Attack
        executeAttack = false,

        -- to stop Dummee's charge
        stopCharge = false,

        -- to check if damage from Charging Attack has been applied to target
        damageApplied = false,

        -- to check if Dummee has fully bounced back after Charging
        recoilComplete = false,

        -- for turning the Dummee when it does the Charging Attack
        dummeeRotation = 0,

        -- the entity on the receiving end of the Charging Attack
        -- could be a floor tile, a rock, the player, or another Dummee
        victim = nil,

        -- for EnemyAttackDummeeMoveDummeeTowardsPlayer() to use for pausing
        accumTime = 0,

        -- recoilDestination is a "vector3" containing the ending coordinates for Dummee after bouncing back
        -- e.g. { 1, 1, 1 }
        recoilDestination = nil,

        -- an array of "vector3"s containing the coordinates for all gridcells in Dummee's recoil path
        recoilPath = { },

        -- amount of time to pause between each tile while charging
        chargeInterval = 0.3,

        -- damage for Charging Attack
        chargeDamage = 2,

        -- only do the lane check once per turn
        laneChecked = false,

        sameLane = false
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

    if (attackComponent.laneChecked == false) then
        attackComponent.sameLane = EnemyAttackDummeeSameLane(e, playerEntity)
    end

    -- stop the script if Dummee and Player are not in the same lane
    if (attackComponent.laneChecked == true and attackComponent.sameLane == false) then
        attackComponent.executeAttack = false
        enemyController.hasAttacked = true
        attackComponent.laneChecked = false
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
    EnemyAttackDummeeMoveDummeeTowardsPlayer(e, attackComponent.victim)

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

            -- apply push to victim (doesn't do anything if victim is a floor tile)
            EnemyAttackDummeeApplyPushback(e, attackComponent.victim)

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
            attackComponent.recoilDestination = nil
            attackComponent.recoilPath = { }
            attackComponent.laneChecked = false
            attackComponent.sameLane = false
            
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
        attackComponent.laneChecked = true
        return result
    end

    -- Dummee and player on the same x-axis
    if (dummeeGrid.x == playerGrid.x) then

        -- player is behind Dummee
        if (dummeeGrid.z > playerGrid.z) then 
            result = true 
            attackComponent.dummeeRotation = 180
            print("[EnemyAttackDummee.lua] Player is behind Dummee, dummeeRotation = 180.")

        -- player is in front of Dummee
        else
            result = true 
            attackComponent.dummeeRotation = 0
            print("[EnemyAttackDummee.lua] Player is in front of Dummee, dummeeRotation = 0.")
        end

    -- enemy and player on the same z-axis
    elseif (dummeeGrid.z == playerGrid.z) then

        -- player is to Dummee's right
        if (dummeeGrid.x > playerGrid.x) then
            result = true 
            attackComponent.dummeeRotation = 270    
            print("[EnemyAttackDummee.lua] Player is to Dummee's right, dummeeRotation = 270.")
        
        -- player is to Dummee's left
        else
            result = true 
            attackComponent.dummeeRotation = 90
            print("[EnemyAttackDummee.lua] Player is to Dummee's left, dummeeRotation = 90.")
        end   
    else
        attackComponent.errorMessage = "[EnemyAttackDummee.lua] Cannot use Charging Attack as Dummee is not in same lane as Player."
    end

    attackComponent.laneChecked = true

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
                
        print("[EnemyAttackDummee.lua] Player behind Dummee.")
        -- make a list of all GridCells to check against (GridCells between Dummee and Player)
        while (currentZ ~= playerGrid.z) do
            currentZ = currentZ - 1
            affectedGridList[#affectedGridList + 1] = { currentX, currentZ } 
        end

    elseif (attackComponent.dummeeRotation == 0) then
        
        print("[EnemyAttackDummee.lua] Player in front of Dummee.")
        -- make a list of all GridCells to check against (GridCells between Dummee and Player)
        while (currentZ ~= playerGrid.z) do
            currentZ = currentZ + 1
            affectedGridList[#affectedGridList + 1] = { currentX, currentZ } 
        end
    
    elseif (attackComponent.dummeeRotation == 270) then
        
        print("[EnemyAttackDummee.lua] Player is to Dummee's right.")
        -- make a list of all GridCells to check against (GridCells between Dummee and Player)
        while (currentX ~= playerGrid.x) do
            currentX = currentX - 1
            affectedGridList[#affectedGridList + 1] = { currentX, currentZ } 
        end

    elseif (attackComponent.dummeeRotation == 90) then

        print("[EnemyAttackDummee.lua] Player is to Dummee's right.")
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

    print("[EnemyAttackDummee.lua] Charging towards victim!")
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

    local attackComponent = GetComponent(dummee, "C_EnemyAttackDummee")
    
    -- check whether victim is a floor tile
    local tileList = EntitiesWithScriptComponent("C_FloorTile")
    for i = 1, #tileList do
        if (tileList[i] == victim) then
            print("[EnemyAttackDummee.lua] Charge Attack victim is a floor tile! No recoil applied.")
            attackComponent.recoilComplete = true
            return
        end
    end

    -- Note: if code reaches here, then victim is defo not a floor tile and we can apply recoil to Dummee.
    local dummeeGrid = GetGridCell(dummee)
    local currentX = dummeeGrid.x
    local currentY = dummeeGrid.y
    local currentZ = dummeeGrid.z
    
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

    -- print("[EnemyAttackDummee.lua] recoilGrid:", recoilGrid[1], recoilGrid[2], recoilGrid[3])

    -- Note: impossible to do smooth charging animation because Dummee position is locked to the grid.

    -- if (attackComponent.accumTime < attackComponent.chargeInterval) then
    --     attackComponent.accumTime = attackComponent.accumTime + GetDeltaTime()
    --     return
    -- end

    for i = 1, #attackComponent.recoilPath do
    
        dummeeGrid.x = attackComponent.recoilPath[i][1]
        dummeeGrid.y = attackComponent.recoilPath[i][2]
        dummeeGrid.z = attackComponent.recoilPath[i][3]

        if (dummeeGrid.x == recoilGrid[1] and dummeeGrid.y == recoilGrid[2] and dummeeGrid.z == recoilGrid[3]) then
            attackComponent.recoilComplete = true
            print("[EnemyAttackDummee.lua] Recoil destination reached!")
            break
        end

        -- attackComponent.accumTime = 0
    end

    -- TODO: CHECK WHETHER DUMMEE HAS LANDED SAFELY AFTER RECOIL
    
end

function EnemyAttackDummeeApplyPushback(dummee, victim)

    print("[EnemyAttackDummee.lua] Applying pushback!")

    -- check whether victim is a floor tile
    local tileList = EntitiesWithScriptComponent("C_FloorTile")
    for i = 1, #tileList do
        if (tileList[i] == victim) then
            print("[EnemyAttackDummee.lua] Charge Attack victim is a floor tile! No pushback applied.")
            return
        end
    end

    local attackComponent = GetComponent(dummee, "C_EnemyAttackDummee")

    local entitiesWithGridCell = EntitiesWithEngineComponent("GridCell")
    local affectedEntity = nil
    local affectedEntities = { }
    local victimGrid = GetGridCell(victim)
    local currentGrid = { victimGrid.x, victimGrid.y, victimGrid.z }
    local loopCounter = 1 -- arbitrary condition for the while loop
    local pushbackDirection = { 0, 0, 0 }

    -- add the victim to affectedEntities first, before checking if there's anything behind the player
    affectedEntities[#affectedEntities + 1] = victim

    -- Player behind Dummee
    if (attackComponent.dummeeRotation == 180) then
                
        -- set currentGrid to be the gridcell behind the victim
        currentGrid[3] = currentGrid[3] - 1

        -- set pushbackDirection
        pushbackDirection[3] = -1
        
        -- while loop will break once no affectedEntity is found in an iteration
        while (loopCounter <= 8) do

            for i = 1, #entitiesWithGridCell do
                
                -- could be any entity on the map with a GridCell component
                local currentEntityGrid = GetGridCell(entitiesWithGridCell[i])
                
                -- if that entity's gridcell matches currentGrid then it will be affected by pushback
                if (currentEntityGrid.x == currentGrid[1] and currentEntityGrid.y == currentGrid[2] and currentEntityGrid.z == currentGrid[3]) then
                    
                    -- mark this entity as affected
                    affectedEntity = entitiesWithGridCell[i]      

                    -- break the for-loop as no other entity can possibly have the same grid coordinates
                    break
                end
            end

            -- if an affectedEntity was found, then add to affectedEntities and continue checking behind the affectedEntity
            if (affectedEntity ~= nil) then
                
                -- add affectedEntity to the list
                affectedEntities[#affectedEntities + 1] = affectedEntity

                -- set currentGrid to be the gridcell behind the last affectedEntity
                currentGrid[3] = currentGrid[3] - 1

                -- increment loopCounter
                loopCounter = loopCounter + 1

                -- reset affectedEntity
                affectedEntity = nil
            
            -- else if no affectedEntity was found, then the pushback chain breaks and we can break the while loop.
            else
                break
            end   
        end

    -- Player in front of Dummee
    elseif (attackComponent.dummeeRotation == 0) then
        
        -- set currentGrid to be the gridcell behind the victim
        currentGrid[3] = currentGrid[3] + 1

        -- set pushbackDirection
        pushbackDirection[3] = 1
        
        -- while loop will break once no affectedEntity is found in an iteration
        while (loopCounter <= 8) do

            for i = 1, #entitiesWithGridCell do
                
                -- could be any entity on the map with a GridCell component
                local currentEntityGrid = GetGridCell(entitiesWithGridCell[i])
                
                -- if that entity's gridcell matches currentGrid then it will be affected by pushback
                if (currentEntityGrid.x == currentGrid[1] and currentEntityGrid.y == currentGrid[2] and currentEntityGrid.z == currentGrid[3]) then
                    
                    -- mark this entity as affected
                    affectedEntity = entitiesWithGridCell[i]      

                    -- break the for-loop as no other entity can possibly have the same grid coordinates
                    break
                end
            end

            -- if an affectedEntity was found, then add to affectedEntities and continue checking behind the affectedEntity
            if (affectedEntity ~= nil) then
                
                -- add affectedEntity to the list
                affectedEntities[#affectedEntities + 1] = affectedEntity

                -- set currentGrid to be the gridcell behind the last affectedEntity
                currentGrid[3] = currentGrid[3] + 1

                -- increment loopCounter
                loopCounter = loopCounter + 1

                -- reset affectedEntity
                affectedEntity = nil
            
            -- else if no affectedEntity was found, then the pushback chain breaks and we can break the while loop.
            else
                break
            end   
        end

    -- Player to Dummee's right
    elseif (attackComponent.dummeeRotation == 270) then
        
        -- set currentGrid to be the gridcell behind the victim
        currentGrid[1] = currentGrid[1] - 1

        -- set pushbackDirection
        pushbackDirection[1] = -1
        
        -- while loop will break once no affectedEntity is found in an iteration
        while (loopCounter <= 8) do

            for i = 1, #entitiesWithGridCell do
                
                -- could be any entity on the map with a GridCell component
                local currentEntityGrid = GetGridCell(entitiesWithGridCell[i])
                
                -- if that entity's gridcell matches currentGrid then it will be affected by pushback
                if (currentEntityGrid.x == currentGrid[1] and currentEntityGrid.y == currentGrid[2] and currentEntityGrid.z == currentGrid[3]) then
                    
                    -- mark this entity as affected
                    affectedEntity = entitiesWithGridCell[i]      

                    -- break the for-loop as no other entity can possibly have the same grid coordinates
                    break
                end
            end

            -- if an affectedEntity was found, then add to affectedEntities and continue checking behind the affectedEntity
            if (affectedEntity ~= nil) then
                
                -- add affectedEntity to the list
                affectedEntities[#affectedEntities + 1] = affectedEntity

                -- set currentGrid to be the gridcell behind the last affectedEntity
                currentGrid[1] = currentGrid[1] - 1

                -- increment loopCounter
                loopCounter = loopCounter + 1

                -- reset affectedEntity
                affectedEntity = nil
            
            -- else if no affectedEntity was found, then the pushback chain breaks and we can break the while loop.
            else
                break
            end   
        end

    -- Player to Dummee's left
    elseif (attackComponent.dummeeRotation == 90) then

        -- set currentGrid to be the gridcell behind the victim
        currentGrid[1] = currentGrid[1] + 1

        -- set pushbackDirection
        pushbackDirection[1] = 1
        
        -- while loop will break once no affectedEntity is found in an iteration
        while (loopCounter <= 8) do

            for i = 1, #entitiesWithGridCell do
                
                -- could be any entity on the map with a GridCell component
                local currentEntityGrid = GetGridCell(entitiesWithGridCell[i])
                
                -- if that entity's gridcell matches currentGrid then it will be affected by pushback
                if (currentEntityGrid.x == currentGrid[1] and currentEntityGrid.y == currentGrid[2] and currentEntityGrid.z == currentGrid[3]) then
                    
                    -- mark this entity as affected
                    affectedEntity = entitiesWithGridCell[i]      

                    -- break the for-loop as no other entity can possibly have the same grid coordinates
                    break
                end
            end

            -- if an affectedEntity was found, then add to affectedEntities and continue checking behind the affectedEntity
            if (affectedEntity ~= nil) then
                
                -- add affectedEntity to the list
                affectedEntities[#affectedEntities + 1] = affectedEntity

                -- set currentGrid to be the gridcell behind the last affectedEntity
                currentGrid[1] = currentGrid[1] + 1

                -- increment loopCounter
                loopCounter = loopCounter + 1

                -- reset affectedEntity
                affectedEntity = nil
            
            -- else if no affectedEntity was found, then the pushback chain breaks and we can break the while loop.
            else
                break
            end   
        end
    end

    -- TODO: Check for any floor tiles or destructible rocks in the affectedEntities list, 
    -- and remove all entities behind the first found floor tile or destructible rock entity.
    print("[EnemyAttackDummee.lua] TODO: remove non-character entities from affectedEntities. Game will crash if pushback is applied to non-character entities.")

    -- for each entity in affectedEntities, push back 1 tile
    for k = 1, #affectedEntities do
        local currentEntityGrid = GetGridCell(affectedEntities[k])
        currentEntityGrid.x = currentEntityGrid.x + pushbackDirection[1]
        currentEntityGrid.y = currentEntityGrid.y + pushbackDirection[2]
        currentEntityGrid.z = currentEntityGrid.z + pushbackDirection[3]
    end

    -- tileList is an array of an array of tiles that are above / below the corresponding entity in the affectedEntities list
    local tileList = { }
    
    -- for each entity in affectedEntities, if they are not standing on a tile, check if there is a tile below them (aka same x and z axis but different y axis)
    -- if there is a tile somewhere below them, drop them down there and apply fall damage
    -- if there is no tile at all below them, then kill them off
    local allTiles = EntitiesWithScriptComponent("C_FloorTile")
    for l = 1, #affectedEntities do

        local currentEntityGrid = GetGridCell(affectedEntities[l])
        local tileArray = { }

        for m = 1, #allTiles do
            local currentTileGrid = GetGridCell(allTiles[m])
            if (currentTileGrid.x == currentEntityGrid.x and currentTileGrid.z == currentEntityGrid.z) then
            
                tileArray[#tileArray + 1] = allTiles[m]
            end
        end
        tileList[#tileList + 1] = tileArray
    end

    -- for each affectedEntity, check if there are any tiles below them.
    -- if there aren't any, then reduce their HP to zero.
    -- if there are, then check if there is a tile right below them.
    -- if there is, then do nothing.
    -- if there isn't, drop them onto the next highest tile (that's below them) and apply fall damage.
    for n = 1, #affectedEntities do
    
        local currentEntityGrid = GetGridCell(affectedEntities[n])
        local tileToFallOnto = nil
        local willFall = true

        -- if there are no other tiles along the y-axis then reduce their HP to zero.
        if (#tileList[n] == 0) then
            GetComponent(affectedEntities[n], "C_Character").currentHP = 0
            print("[EnemyAttackDummee.lua]", EntityName(affectedEntities[n]), "was pushed into the void and died.")
        
        -- else if there are tiles along the y-axis
        else

            -- go through all tiles in tileList to see if character is safely standing on a tile
            for o = 1, #tileList[n] do
                
                local currentTileGrid = GetGridCell(tileList[n][o])

                -- if there is a tile right below this character, then it shall not fall
                if (currentTileGrid.y == currentEntityGrid.y - 1) then
                    willFall = false
                    break

                -- else, find the tile with highest y-position that is below this character
                else
                    -- if currentTile is below the character
                    if (currentTileGrid.y < currentEntityGrid.y) then
                    
                        -- if tileToFallOnto is nil then simply assign currentTile to tileToFallOnto
                        if (tileToFallOnto == nil) then
                            tileToFallOnto = tileList[n][o]
                        
                        -- else if currentTileGrid is higher than tileToFallOnto, then overwrite tileToFallOnto
                        elseif (currentTileGrid.y > GetGridCell(tileToFallOnto).y) then
                            tileToFallOnto = tileList[n][o]
                        end
                    end
                end            
            end

            -- if char willFall then make it fall onto tileToFallOnto
            if (willFall) then
                
                -- the current affectedEntity will squash any characters below them, killing them
                EnemyAttackDummeeCheckCharacterBelow(affectedEntities[n])
                  
                -- move the character down
                currentEntityGrid.y = GetGridCell(tileToFallOnto).y + 1

                -- deal fall damage
                local affectedCharacterComponent = GetComponent(affectedEntities[n], "C_Character")
                local combatManagerEntity = GetEntity("CombatManager")
                local combatManagerComponent = nil
                if (combatManagerEntity ~= -1) then
                    combatManagerComponent = GetComponent(combatManagerEntity, "C_CombatManager")
                else
                    print("[EnemyAttackDummee.lua] ERROR: Cannot find CombatManager! Unable to deal fall damage.")
                end
                 
                affectedCharacterComponent.currentHP = affectedCharacterComponent.currentHP - combatManagerComponent.fallDamage
                print("[EnemyAttackDummee.lua]", EntityName(affectedEntities[n]), "was pushed off a ledge and suffered fall damage.")
            end
        end
    end
end

-- checks if there is a character below the entity being pushed back
-- if there is then the character below will get squashed to death
function EnemyAttackDummeeCheckCharacterBelow(e)

    local characterList = EntitiesWithScriptComponent("C_Character")
    local entityGrid = GetGridCell(e)

    for i = 1, #characterList do
        
        if (characterList[i] ~= e) then
            
            local characterGrid = GetGridCell(characterList[i])

            if (characterGrid.y < entityGrid.y) then
                GetComponent(characterList[i], "C_Character").currentHP = 0
                print("[EnemyAttackDummee.lua]", EntityName(characterList[i]), "has been squashed to death by", EntityName(e), ".")
            end
        end     
    end
end


-- checks if there is a tile below Dummee after recoil
-- if no tile below then this will deal fall damage / kill Dummee accordingly
function EnemyAttackDummeeCheckDummeeSafeLanding(e)

end