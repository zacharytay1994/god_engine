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
        errorMessage = "[FrontJab.lua] Player not adjacent to enemy!!!",
        --END OF REQUIRED SECTION -------------------------------------------------------------------------------------
        
        -- used to turn player to face the target
        playerRotation = 0
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_FrontJab(e)
    
    frontJabComponent = GetComponent(e, "C_FrontJab")
    
    -- checking if player is able to use attack against the enemy --------------------------------------------------------
    if (frontJabComponent.startCheck == true) then
        
        -- run the check only once per attack
        frontJabComponent.startCheck = false
        
        -- check if player is adjacent to enemy
        if (CheckPlayerAdjacentToEnemy(frontJabComponent.attacker, frontJabComponent.defender, e) == true) then
            
            -- passed the check, allow the rest of the script to run
            frontJabComponent.canAttack = true
            print("[FrontJab.lua] Adjacent check passed!")
        else
            
            -- failed the check, print fail message
            print("[FrontJab.lua] Adjacent check failed!")
        end
        
        -- this will allow PlayerAttack.lua to proceed
        frontJabComponent.checkCompleted = true
    end
    -- end of checking if player is able to use attack against the enemy -------------------------------------------------
    

    -- if can use attack, turn the player and activate effects -----------------------------------------------------------
    if (frontJabComponent.canAttack == true) then 
        
        -- turn player to face enemy 
        transformComponent = GetTransform(frontJabComponent.attacker)
        transformComponent.rotation.y = frontJabComponent.playerRotation
        
        -- activate screenshake
        screenShakeEntity = GetEntity("ScreenShake")
        if (screenShakeEntity ~= -1) then
            screenShakeComponent = GetComponent(screenShakeEntity, "C_ScreenShake")
            screenShakeComponent.duration = 0.25
            screenShakeComponent.doScreenShake = true
        end
        
        -- trigger sound effect
        
        -- trigger particles (if any)
        
        -- play attack animation (if any)
    end
    -- end of attack effects ---------------------------------------------------------------------------------------------
    
    -- reset variables (do not reset checkCompleted and canAttack here. They will be reset by PlayerAttack.lua)
    frontJabComponent.startCheck = false
    frontJabComponent.attacker = -1
    frontJabComponent.defender = -1
    frontJabComponent.playerRotation = 0
end

function CheckPlayerAdjacentToEnemy(attacker, defender, e)
    
    -- init result
    result = false

    -- get attacker and defenders' locations
    attackerGrid = GetGridCell(attacker)
    defenderGrid = GetGridCell(defender)

    -- get rotation variable
    frontJabComponent = GetComponent(e, "C_FrontJab")

    if (attackerGrid.y == defenderGrid.y) then 

        -- enemy behind player
        if (attackerGrid.x == defenderGrid.x and attackerGrid.z == defenderGrid.z - 1) then
            result = true 
            frontJabComponent.playerRotation = 90

        -- enemy in front of player
        elseif (attackerGrid.x == defenderGrid.x and attackerGrid.z == defenderGrid.z + 1) then
            result = true 
            frontJabComponent.playerRotation = 270

        -- enemy to player's left
        elseif (attackerGrid.z == defenderGrid.z and attackerGrid.x == defenderGrid.x - 1) then
            result = true 
            frontJabComponent.playerRotation = 180

        -- enemy to player's right
        elseif(attackerGrid.z == defenderGrid.z and attackerGrid.x == defenderGrid.x + 1) then
            result = true 
            frontJabComponent.playerRotation = 0       
        end
    end

    return result
end