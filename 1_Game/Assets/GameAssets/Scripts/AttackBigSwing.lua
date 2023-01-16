-- This script will allow the player to perform a Big Swing attack.
-- This script will be attached to CombatManager.
-- CombatManager will run this script whenever the player uses a Big Swing attack.

-- To use Big Swing, the Player will have to select an adjacent tile. The attack will then hit
-- that tile, as well as the tiles to its left and right.

-- TODO:
-- 1) Disallow selection of BigSwing before selection of target.
-- 2) Highlight potentially affected tiles when hovering over target

--[IsComponent]
function C_BigSwing()
    local var = {
        --[SerializeString]
        AttackName = "BigSwing",

        -- THE VARIABLES IN THIS SECTION IS REQUIRED BY ALL ATTACK SCRIPTS!!! -----------------------------------------
        -- the attacking entity, aka the player
        attacker = -1,
        
        -- the target tile
        defender = -1,
        
        -- cue to start check
        startCheck = false,
        
        -- marks the end of the check (used by PlayerAttack.lua)
        checkCompleted = false,
        
        -- if canAttack is true at the end of the checking process, then can go ahead with the attack
        canAttack = false,
        
        -- error message (why the attack failed)
        errorMessage = "[AttackBigSwing.lua] Selected tile is not adjacent to Player.",
        
        -- used to turn player to face the target
        playerRotation = 0,
        -- END OF REQUIRED SECTION ------------------------------------------------------------------------------------

        -- the enemy standing on the target tile
        middleEnemy = nil,

        -- the enemy standing on the left of the target tile
        leftEnemy = nil,

        -- the enemy standing on the right of the target tile
        rightEnemy = nil,

        -- the tile left of the target tile 
        leftTileX = nil,
        leftTileZ = nil,

        -- the tile right of the target tile 
        rightTileX = nil,
        rightTileZ = nil,

        playerY = nil
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_BigSwing(e)
    
    -- getting C_BigSwing component
    local attackComponent = GetComponent(e, "C_BigSwing")
    
    -- getting C_Player component to check selected action
    local playerEntity = GetEntity("Player")
    local playerComponent 
    if (playerEntity ~= -1) then
        playerComponent = GetComponent(playerEntity, "C_Player")
    end
    
    -- if BigSwing is currently the selected attack, highlight the tiles that will be affected by Big Swing --------------
    if (playerComponent.selectedAction == "BigSwing") then
        
        -- highlight tiles

    end
    
    -- checking if player is able to use attack against the enemy --------------------------------------------------------
    if (attackComponent.startCheck == true) then
        
        print("[AttackBigSwing.lua] Starting check!")
        
        -- run the check only once per attack
        attackComponent.startCheck = false

        if (attackComponent.attacker == -1 or attackComponent.defender == -1) then 

            print("[AttackBigSwing.lua] ERROR: Either attacker or defender is -1.")

        else
            
            -- check if selected tile is adjacent to Player
            if (CheckTargetTileAdjacentToPlayer(attackComponent.attacker, attackComponent.defender, e) == true) then
                
                -- passed the check, allow the rest of the script to run
                attackComponent.canAttack = true
                print("[AttackBigSwing.lua] Adjacent check passed!")
            else
                
                -- failed the check, print fail message
                print("[AttackBigSwing.lua] Adjacent check failed.")
            end
            
            -- this will allow PlayerAttack.lua to proceed
            attackComponent.checkCompleted = true

        end   
    end
    -- end of checking if player is able to use attack against the enemy -------------------------------------------------

    -- if can use attack, turn the player and activate effects -----------------------------------------------------------
    if (attackComponent.canAttack == true) then 
        
        -- turn player to face enemy 
        -- transformComponent = GetTransform(attackComponent.attacker)
        -- transformComponent.rotation.y = attackComponent.playerRotation
        local child_model = Child(attackComponent.attacker, 0)
        local child_model_transform = GetTransform(child_model)
        child_model_transform.rotation.y = attackComponent.playerRotation
        -- print("[AttackFrontJab.lua] Setting Player model's y-rotation to:", attackComponent.playerRotation)
        
        -- activate screenshake
        screenShakeEntity = GetEntity("ScreenShake")
        if (screenShakeEntity ~= -1) then
            screenShakeComponent = GetComponent(screenShakeEntity, "C_ScreenShake")
            screenShakeComponent.duration = 0.25
            screenShakeComponent.doScreenShake = true
        end
        
        -- trigger sound effect
        InstancePrefab("SFX_Wave",0,0,0)

        -- set playerAttackComponent.targetEntity to middleEnemy
        local playerAttackComponent = GetComponent(playerEntity, "C_PlayerAttack")

        if (attackComponent.middleEnemy ~= nil) then
            playerAttackComponent.targetEntity = attackComponent.middleEnemy
        else
            -- reset variables from here instead of PlayerAttack.lua
            attackComponent.canAttack = false
            playerComponent.playerAttacked = true
            playerAttackComponent.resetVariables = true
            attackComponent.checkCompleted = false
        end

        -- deal damage to leftEnemy and rightEnemy
        if (attackComponent.leftEnemy ~= nil) then
            local leftEnemyChar = GetComponent(attackComponent.leftEnemy, "C_Character")
            leftEnemyChar.currentHP = leftEnemyChar.currentHP - 2
        end
        if (attackComponent.rightEnemy ~= nil) then
            local rightEnemyChar = GetComponent(attackComponent.rightEnemy, "C_Character")
            rightEnemyChar.currentHP = rightEnemyChar.currentHP - 2
        end
        
        -- trigger particles (if any)
        
        -- play attack animation (if any)

        print("[AttackBigSwing.lua] END.")
    end
    -- end of attack effects ---------------------------------------------------------------------------------------------
    
    -- reset variables (do not reset checkCompleted and canAttack here. They will be reset by PlayerAttack.lua)
    attackComponent.startCheck = false
    attackComponent.attacker = -1
    attackComponent.defender = -1
    attackComponent.playerRotation = 0
    attackComponent.middleEnemy = nil
    attackComponent.leftEnemy = nil
    attackComponent.rightEnemy = nil
    attackComponent.leftTileX = nil
    attackComponent.leftTileZ = nil
    attackComponent.rightTileX = nil
    attackComponent.rightTileZ = nil
    attackComponent.playerY = nil
end

function CheckTargetTileAdjacentToPlayer(attacker, defender, e)
    
    print("[AttackBigSwing.lua] Starting CheckTargetTileAdjacentToPlayer().")
    
    -- init result
    result = false

    -- get attacker and target tiles' locations
    local attackerGrid = GetGridCell(attacker)
    local tileGrid = GetGridCell(defender)

    -- get rotation variable
    local attackComponent = GetComponent(e, "C_BigSwing")
    attackComponent.playerY = attackerGrid.y

    -- reference to the direction of attack (relative to Triton)
    local attackDirection = nil
    
    if (attackerGrid.y == tileGrid.y + 1 or attackerGrid.y == tileGrid.y) then 

        -- enemy behind player
        if (attackerGrid.x == tileGrid.x and attackerGrid.z == tileGrid.z + 1) then
            result = true 
            attackComponent.playerRotation = 180
            attackComponent.leftTileX = attackerGrid.x - 1
            attackComponent.leftTileZ = attackerGrid.z - 1
            attackComponent.rightTileX = attackerGrid.x + 1
            attackComponent.rightTileZ = attackerGrid.z - 1
            print("[AttackBigSwing.lua] Targeting Player's back!")

        -- enemy in front of player
        elseif (attackerGrid.x == tileGrid.x and attackerGrid.z == tileGrid.z - 1) then
            result = true 
            attackComponent.playerRotation = 0
            attackComponent.leftTileX = attackerGrid.x + 1
            attackComponent.leftTileZ = attackerGrid.z + 1
            attackComponent.rightTileX = attackerGrid.x - 1
            attackComponent.rightTileZ = attackerGrid.z + 1
            print("[AttackBigSwing.lua] Targeting Player's front!")

        -- enemy to player's left
        elseif (attackerGrid.z == tileGrid.z and attackerGrid.x == tileGrid.x - 1) then
            result = true 
            attackComponent.playerRotation = 90
            attackComponent.leftTileX = attackerGrid.x + 1
            attackComponent.leftTileZ = attackerGrid.z - 1
            attackComponent.rightTileX = attackerGrid.x + 1
            attackComponent.rightTileZ = attackerGrid.z + 1
            print("[AttackBigSwing.lua] Targeting Player's left!")

        -- enemy to player's right
        elseif(attackerGrid.z == tileGrid.z and attackerGrid.x == tileGrid.x + 1) then
            result = true 
            attackComponent.playerRotation = 270
            attackComponent.leftTileX = attackerGrid.x - 1
            attackComponent.leftTileZ = attackerGrid.z + 1
            attackComponent.rightTileX = attackerGrid.x - 1
            attackComponent.rightTileZ = attackerGrid.z - 1
            print("[AttackBigSwing.lua] Targeting Player's right!")
        end
    end

    -- find middleEnemy, leftEnemy, rightEnemy entities
    local characterList = EntitiesWithScriptComponent("C_Character")
    for i = 1, #characterList do
    
        currentCharGrid = GetGridCell(characterList[i])
        
        if     (currentCharGrid.x == attackComponent.leftTileX and currentCharGrid.z == attackComponent.leftTileZ and currentCharGrid.y == attackComponent.playerY) then
            -- this enemy is on the left tile
            attackComponent.leftEnemy = characterList[i]
        elseif (currentCharGrid.x == attackComponent.rightTileX and currentCharGrid.z == attackComponent.rightTileZ and currentCharGrid.y == attackComponent.playerY) then
            -- this enemy is on the right tile
            attackComponent.rightEnemy = characterList[i]
        elseif (currentCharGrid.x == tileGrid.x and currentCharGrid.z == tileGrid.z and currentCharGrid.y == attackComponent.playerY) then
            -- this enemy is on the middle tile
            attackComponent.middleEnemy = characterList[i]
        end
    end

    if (attackComponent.leftEnemy == nil and 
        attackComponent.rightEnemy == nil and
        attackComponent.middleEnemy == nil) then
        print("[AttackBigSwing.lua] No enemies in the attack range! Stopping attack.")
        result = false
    end

    return result
end