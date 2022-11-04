-- This script will allow the player to perform a front jab attack.
-- This script will be attached to CombatManager.
-- CombatManager will run this script whenever the player uses a Front Jab attack.

-- Front jab can only be used when player is directly adjacent to the target.

-- TODO:
-- 1) Trigger sound effects / particles

--[IsComponent]
function C_FrontJab()
    local var = {
        --[SerializeString]
        AttackName = "FrontJab",

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
        errorMessage = "[AttackFrontJab.lua] Player not adjacent to enemy.",
        
        -- used to turn player to face the target
        playerRotation = 0
        --END OF REQUIRED SECTION -------------------------------------------------------------------------------------
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_FrontJab(e)
    
    attackComponent = GetComponent(e, "C_FrontJab")
    
    -- checking if player is able to use attack against the enemy --------------------------------------------------------
    if (attackComponent.startCheck == true) then
        
        -- run the check only once per attack
        attackComponent.startCheck = false

        if (attackComponent.attacker == -1 or attackComponent.defender == -1) then 

            print("[AttackFrontJab.lua] ERROR: Either attacker or defender is -1.")

        else
            
            -- check if player is adjacent to enemy
            if (CheckPlayerAdjacentToEnemy(attackComponent.attacker, attackComponent.defender, e) == true) then
                
                -- passed the check, allow the rest of the script to run
                attackComponent.canAttack = true
                print("[AttackFrontJab.lua] Adjacent check passed!")
            else
                
                -- failed the check, print fail message
                print("[AttackFrontJab.lua] Adjacent check failed.")
            end
            
            -- this will allow PlayerAttack.lua to proceed
            attackComponent.checkCompleted = true

        end   
    end
    -- end of checking if player is able to use attack against the enemy -------------------------------------------------
    

    -- if can use attack, turn the player and activate effects -----------------------------------------------------------
    if (attackComponent.canAttack == true) then 
        
        -- turn player to face enemy 
        transformComponent = GetTransform(attackComponent.attacker)
        transformComponent.rotation.y = attackComponent.playerRotation
        
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

function CheckPlayerAdjacentToEnemy(attacker, defender, e)
    
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