-- This script will allow the player to perform a Ground Smash attack.
-- This script will be attached to CombatManager.
-- CombatManager will run this script whenever the player uses a Ground Smash attack.

-- To use Ground Smash, the Player will need to click on Triton. Then, it will hit all enemies adjacent to Triton
-- and apply push back.

-- TODO:
-- 1) Make GroundSmash affect diagonal enemies.
-- 2) Complete the pushback function

--[IsComponent]
function C_GroundSmash()
    local var = {
        --[SerializeString]
        AttackName = "GroundSmash",

        -- THE VARIABLES IN THIS SECTION IS REQUIRED BY ALL ATTACK SCRIPTS!!! -----------------------------------------
        -- the attacking entity, aka the player
        attacker = -1,
        
        -- the defending entity, aka the enemy
        defender = -1,
        
        -- cue to start check
        startCheck = false,
        
        -- marks the end of the check (used by PlayerAttack.lua)
        checkCompleted = false,
        
        -- if canAttack is true at the end of the checking process, then can go ahead with the attack
        canAttack = false,
        
        -- error message (why the attack failed)
        errorMessage = "[AttackGroundSmash.lua] Nothing to report.",
        
        -- used to turn player to face the target
        playerRotation = 0
        --END OF REQUIRED SECTION -------------------------------------------------------------------------------------
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_GroundSmash(e)
    
    attackComponent = GetComponent(e, "C_GroundSmash")
    
    -- checking if player is able to use attack against the enemy --------------------------------------------------------
    if (attackComponent.startCheck == true) then
        
        -- run the check only once per attack
        attackComponent.startCheck = false

        if (attackComponent.attacker == -1 or attackComponent.defender == -1) then 

            print("[AttackGroundSmash.lua] ERROR: Either attacker or defender is -1.")

        else
            
            -- check if attacker == defender because the player needs to click on itself to use ground smash
            if (attackComponent.attacker == attackComponent.defender) then
                
                -- passed the check, allow the rest of the script to run
                attackComponent.canAttack = true
                print("[AttackGroundSmash.lua] Player clicked on Triton! Proceeding with GroundSmash.")
            else
                
                -- failed the check, print fail message
                print("[AttackGroundSmash.lua] Player did not click on Triton! GroundSmash failed.")
            end
            
            -- this will allow PlayerAttack.lua to proceed
            attackComponent.checkCompleted = true

        end   
    end
    -- end of checking if player is able to use attack against the enemy -------------------------------------------------
    
    -- if can use attack, turn the player and activate effects -----------------------------------------------------------
    if (attackComponent.canAttack == true) then 
        
        AttackGroundSmashApplyDamagePushback(attackComponent.attacker)

        -- activate screenshake
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
    end
    -- end of attack effects ---------------------------------------------------------------------------------------------
    
    -- reset variables (do not reset checkCompleted and canAttack here. They will be reset by PlayerAttack.lua)
    attackComponent.startCheck = false
    attackComponent.attacker = -1
    attackComponent.defender = -1
    attackComponent.playerRotation = 0
end

function AttackGroundSmashApplyDamagePushback(e)

    local hitEnemies = { }
    
    -- find all characters in range of attack
    local characterList = EntitiesWithScriptComponent("C_Character")
    for i = 1, #characterList do
        if (AttackGroundSmashCheckPlayerAdjacentToEnemy(e, characterList[i])) then 
            hitEnemies[#hitEnemies + 1] = characterList[i]
        end
    end

    local combatManagerEntity = GetEntity("CombatManager")
    local attackList = nil
    if (combatManagerEntity ~= -1) then
        attackList = GetComponent(combatManagerEntity, "C_CombatManager")
    end
    
    local groundSmashDamage = combatManagerComponent.attackType[2]

    for j = 1, #hitEnemies do
    
        -- apply damage to all characters
        local enemyCharacterComponent = GetComponent(hitEnemies[j], "C_Character")
        enemyCharacterComponent.currentHP = enemyCharacterComponent.curerntHP - groundSmashDamage

        -- TODO: apply pushback to all characters while ensuring proper landing
    
    end
end

function AttackGroundSmashCheckPlayerAdjacentToEnemy(player, enemy)
    
    -- init result
    result = false

    -- get attacker and defenders' locations
    attackerGrid = GetGridCell(attacker)
    defenderGrid = GetGridCell(defender)

    -- get rotation variable
    attackComponent = GetComponent(e, "C_FrontJab")

    if (attackerGrid.y == defenderGrid.y) then 

        -- enemy behind player
        if (attackerGrid.x == defenderGrid.x and attackerGrid.z == defenderGrid.z - 1) then
            result = true 
            attackComponent.playerRotation = 0

        -- enemy in front of player
        elseif (attackerGrid.x == defenderGrid.x and attackerGrid.z == defenderGrid.z + 1) then
            result = true 
            attackComponent.playerRotation = 180

        -- enemy to player's left
        elseif (attackerGrid.z == defenderGrid.z and attackerGrid.x == defenderGrid.x - 1) then
            result = true 
            attackComponent.playerRotation = 90

        -- enemy to player's right
        elseif(attackerGrid.z == defenderGrid.z and attackerGrid.x == defenderGrid.x + 1) then
            result = true 
            attackComponent.playerRotation = 270
        end
    end

    return result
end

function AttackGroundSmashApplyPushback(player, enemy)

    print("[AttackGroundSmash.lua] Applying pushback!")

    -- determine pushback direction
    local playerGrid = GetGridCell(player)
    local enemyGrid = GetGridCell(enemy)
    local pushbackDirection = { 0, 0, 0 }
    pushbackDirection[1] = enemyGrid.x - playerGrid.x
    pushbackDirection[2] = enemyGrid.y - playerGrid.y
    pushbackDirection[3] = enemyGrid.z - playerGrid.z

    -- using pushback direction, check if there are any other entities directly behind this enemy
    local currentGrid = { enemyGrid.x, enemyGrid.y, enemyGrid.z }
    local loopCounter = 1 -- arbitrary condition for the while loop
    local entitiesWithGridCell = EntitiesWithEngineComponent("GridCell")
    local affectedEntity = nil
    local affectedEntities = { }

    while (loopCounter <= 8) do
    
        currentGrid[1] = currentGrid[1] + pushbackDirection[1]
        currentGrid[2] = currentGrid[2] + pushbackDirection[2]
        currentGrid[3] = currentGrid[3] + pushbackDirection[3]

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
            
            -- increment loopCounter
            loopCounter = loopCounter + 1

            -- reset affectedEntity
            affectedEntity = nil
        
        -- else if no affectedEntity was found, then the pushback chain breaks and we can break the while loop.
        else
            break
        end
    end

    -- TODO: Check for any floor tiles or destructible rocks in the affectedEntities list, 
    -- and remove all entities behind the first found floor tile or destructible rock entity.
    print("[AttackGroundSmash.lua] TODO: remove non-character entities from affectedEntities. Game will crash if pushback is applied to non-character entities.")

    -- affectedEntities now contain all entities with a GridCell component, that form a chain behind the initial enemy

    -- check affectedEntities for any floortiles / destructible rocks
    
    -- if there is a destructible rock, then remove all entities from that rock onwards (including that rock) and destroy that rock, 
    -- and apply pushback to affectedEntities + ensure proper landing

    -- if there is a floortile, then don't do anything more, return. can't pushback anyone in affectedEntity because the floortile is blocking.

    -- if there is no rock / floortile, then apply pushback to affectedEntities + ensure proper landing

end

function AttackGroundSmashEnsureProperLanding(entity)
end