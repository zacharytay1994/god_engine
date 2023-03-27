-- This script will allow the player to perform a Ground Smash attack.
-- This script will be attached to CombatManager.
-- CombatManager will run this script whenever the player uses a Ground Smash attack.

-- To use Ground Smash, the Player will need to click on the tile below Triton. Then, it will hit all enemies adjacent to Triton
-- and apply push back.

-- TODO:
-- 1) Make GroundSmash affect diagonal enemies.
-- 2) Retrieve the actual groundsmash damage based on dice color
-- 3) Allow groundsmash to be used by selecting triton

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
            
            -- check if the player has clicked the tile below Triton
            if (AttackGroundSmashCheckCorrectTileClicked(attackComponent.attacker, attackComponent.defender)) then
                
                -- passed the check, allow the rest of the script to run
                attackComponent.canAttack = true
                print("[AttackGroundSmash.lua] Player clicked on the tile below Triton! Proceeding with GroundSmash.")
            else
                
                -- failed the check, print fail message
                print("[AttackGroundSmash.lua] Player did not click on the tile below Triton! GroundSmash failed.")
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
        InstancePrefab("SFX_GroundSmash",0,0,0)
        
        -- trigger particles (if any)
        
        -- play attack animation (if any)

        print("[AttackGroundSmash.lua] End of GroundSmash!")
    end
    -- end of attack effects ---------------------------------------------------------------------------------------------
    
    -- reset variables (do not reset checkCompleted and canAttack here. They will be reset by PlayerAttack.lua)
    attackComponent.startCheck = false
    attackComponent.attacker = -1
    attackComponent.defender = -1
    attackComponent.playerRotation = 0
end

function AttackGroundSmashCheckCorrectTileClicked(player, clickedEntity)

    print("[AttackGroundSmash.lua] Checking if clickedEntity is the tile below Triton!")
    
    local playerGrid = GetGridCell(player)
    local clickedEntityGrid = GetGridCell(clickedEntity)

    if (playerGrid.x == clickedEntityGrid.x and playerGrid.y == clickedEntityGrid.y and playerGrid.z == clickedEntityGrid.z) then
        return true
    end

    -- if (playerGrid.x == clickedEntityGrid.x and playerGrid.y == clickedEntityGrid.y + 1 and playerGrid.z == clickedEntityGrid.z) then
    --     return true
    -- elseif (playerGrid.x == clickedEntityGrid.x and playerGrid.y == clickedEntityGrid.y and playerGrid.z == clickedEntityGrid.z) then
    --     return true
    -- end

    return false
end

function AttackGroundSmashApplyDamagePushback(player)

    print("[AttackGroundSmash.lua] Applying damage to enemies hit by GroundSmash!")
    
    local hitEnemies = { }
    
    -- find all characters in range of attack
    local characterList = EntitiesWithScriptComponent("C_Character")
    for i = 1, #characterList do
        if (AttackGroundSmashCheckPlayerAdjacentToEnemy(player, characterList[i])) then 
            hitEnemies[#hitEnemies + 1] = characterList[i]
            print("[AttackGroundSmash.lua] GroundSmash has hit", EntityName(characterList[i]), GetEntityData(characterList[i]).id)
        end
    end

    if (#hitEnemies < 1) then 
        print("[AttackGroundSmash.lua] GroundSmash did not hit any enemies!")
    end

    -- local combatManagerEntity = GetEntity("CombatManager")
    -- local combatManagerComponent = nil
    -- if (combatManagerEntity ~= -1) then
    --     combatManagerComponent = GetComponent(combatManagerEntity, "C_CombatManager")
    -- end
    -- local groundSmashDamage = combatManagerComponent.attackType[2]

    -- NEED TO GET DICE COLOR HERE, FOR NOW JUST SET DAMAGE AS 2
    local groundSmashDamage = 2

    for j = 1, #hitEnemies do
    
        -- apply damage to all characters
        local enemyCharacterComponent = GetComponent(hitEnemies[j], "C_Character")
        enemyCharacterComponent.currentHP = enemyCharacterComponent.currentHP - groundSmashDamage
        print("[AttackGroundSmash.lua] Applied", groundSmashDamage, "damage to", EntityName(hitEnemies[j]))

        -- apply pushback to all characters while ensuring proper landing
        AttackGroundSmashApplyPushback(player, hitEnemies[j])
        print("AttackGroundSmash.lua] Applied pushback to", EntityName(hitEnemies[j]), "and character behind it.")
    end
end

function AttackGroundSmashCheckPlayerAdjacentToEnemy(player, enemy)
    
    -- init result
    local result = false

    -- get attacker and defenders' locations
    local attackerGrid = GetGridCell(player)
    local defenderGrid = GetGridCell(enemy)

    if (attackerGrid.y == defenderGrid.y) then 

        -- enemy behind player
        if (attackerGrid.x == defenderGrid.x and attackerGrid.z == defenderGrid.z - 1) then
            result = true 

        -- enemy in front of player
        elseif (attackerGrid.x == defenderGrid.x and attackerGrid.z == defenderGrid.z + 1) then
            result = true 

        -- enemy to player's left
        elseif (attackerGrid.z == defenderGrid.z and attackerGrid.x == defenderGrid.x - 1) then
            result = true 

        -- enemy to player's right
        elseif(attackerGrid.z == defenderGrid.z and attackerGrid.x == defenderGrid.x + 1) then
            result = true 

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

    -- add the enemy directly hit by GroundSmash to affectedEntities first
    affectedEntities[#affectedEntities + 1] = enemy

    while (loopCounter <= 8) do
    
        -- update the currentGrid to check for entities
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
    local tileList = EntitiesWithScriptComponent("C_FloorTile")
    --local rockList = 

    for k = 1, #affectedEntities do
        for l = 1, #tileList do
        
            -- if there is a floortile, then don't do anything more, return. can't pushback anyone in affectedEntity because the floortile is blocking.
            if (affectedEntities[k] == tileList[l]) then  
                print("[AttackGroundSmash.lua] There is a floortile in the pushback chain behind", EntityName(affectedEntities[k]), ", cannot apply pushback.")
                return
            end
        end

        -- -- if there is a destructible rock, then remove all entities from that rock onwards (including that rock) and destroy that rock.
        -- for m = 1, #rockList do
        -- end
    end
    
    -- code reach here means there is no rock / floortile, apply pushback to affectedEntities + ensure proper landing.
    print("[AttackGroundSmash.lua] Nothing blocking pushback, applying pushback to all entities behind", EntityName(affectedEntities[1]))

    -- for each affectedEntity, apply pushback here and then check proper landing
    for n = 1, #affectedEntities do 
        local affectedEntityGrid = GetGridCell(affectedEntities[n])
        affectedEntityGrid.x = affectedEntityGrid.x + pushbackDirection[1]
        affectedEntityGrid.y = affectedEntityGrid.y + pushbackDirection[2]
        affectedEntityGrid.z = affectedEntityGrid.z + pushbackDirection[3]
        AttackGroundSmashEnsureProperLanding(affectedEntities[n])
    end
end

-- checks if there is a tile below character after recoil
-- if no tile below then this will deal fall damage / kill the character accordingly
-- input entity MUST HAVE C_CHARACTER COMPONENT
function AttackGroundSmashEnsureProperLanding(e)

    local tileList = EntitiesWithScriptComponent("C_FloorTile")
    local tilesBelow = { }
    local entityGrid = GetGridCell(e)

    for i = 1, #tileList do
    
        local currentTileGrid = GetGridCell(tileList[i])

        if (currentTileGrid.x == entityGrid.x and currentTileGrid.y == entityGrid.y - 1 and currentTileGrid.z == entityGrid.z) then
            print("[AttackGroundSmash.lua] Pushed character has safely landed onto a floor tile.")
            return
        elseif (currentTileGrid.x == entityGrid.x and currentTileGrid.y < entityGrid.y - 1 and currentTileGrid.z == entityGrid.z) then
            tilesBelow[#tilesBelow + 1] = tileList[i]
        end
    end

    -- Note: if code reaches here then it means pushed character is not in a safe position

    local characterComponent = GetComponent(e, "C_Character")
    local tileToFallOnto = nil
    
    -- if there are not tilesBelow then pushed character has fallen off the map, kill it
    if (#tilesBelow == 0) then
        characterComponent.currentHP = 0
        print("[AttackGroundSmash.lua] Pushed character has fallen off the map and died.")
        return
    
    -- else, find the highest tile in tilesBelow and place the pushed character there, applying fall damage
    else
        for j = 1, #tilesBelow do
              
            local currentTileBelowGrid = GetGridCell(tilesBelow[j])

            -- if tileToFallOnto is nil then simply assign currentTile to tileToFallOnto
            if (tileToFallOnto == nil) then
                tileToFallOnto = tilesBelow[j]
            
            -- else if currentTileGrid is higher than tileToFallOnto, then overwrite tileToFallOnto
            elseif (currentTileBelowGrid.y > GetGridCell(tileToFallOnto).y) then
                tileToFallOnto = tilesBelow[j]
            end
        end

        -- place the entity on top of the grid
        entityGrid.y = GetGridCell(tileToFallOnto).y + 1
        
        local combatManagerEntity = GetEntity("CombatManager")
        local combatManagerComponent = nil
        if (combatManagerEntity ~= -1) then
            combatManagerComponent = GetComponent(combatManagerEntity, "C_CombatManager")
        else
            print("[AttackGroundSmash.lua] ERROR: Cannot find CombatManager! Unable to deal fall damage.")
        end
        characterComponent.currentHP = characterComponent.currentHP - combatManagerComponent.fallDamage
        print("[AttackGroundSmash.lua]", EntityName(e), "has fallen off a ledge and received fall damage.")
    end

    EnemyAttackDummeeCheckCharacterBelow(e)

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
                print("[AttackGroundSmash.lua]", EntityName(characterList[i]), "has been squashed to death by", EntityName(e), ".")
            end
        end     
    end
end