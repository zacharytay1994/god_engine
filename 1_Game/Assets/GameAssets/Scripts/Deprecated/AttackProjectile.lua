-- This script will allow the player to perform a projectile attack.
-- This script will be attached to CombatManager.
-- CombatManager will run this script whenever the player uses a projectile attack.

-- Projectile can only be used if player is in the same lane as the target &&
-- target is within 4 tiles away from player.
-- 4 tiles away meaning: |O| | | |X| 

--[IsComponent]
function C_Projectile()
    local var = {
        --[SerializeString]
        AttackName = "Projectile",

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
        errorMessage = "[AttackProjectile.lua] Player not in the same lane as target.",
        
        -- used to turn player to face the target
        playerRotation = 0,
        --END OF REQUIRED SECTION -------------------------------------------------------------------------------------

        -- how for the projectile can travel (x-axis)
        boltRange = 4
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_Projectile(e)
    
    local attackComponent = GetComponent(e, "C_Projectile")

    -- checking if player is able to use attack against the enemy --------------------------------------------------------
    if (attackComponent.startCheck == true) then
        
        print("[AttackProjectile.lua] Starting check!")
        
        -- run the check only once per attack
        attackComponent.startCheck = false
        
        -- check if player is in same lane as enemy
        if (CheckEnemyWithinProjectileRange(attackComponent.attacker, attackComponent.defender, e) == true) then
            
            -- passed the check, allow the rest of the script to run
            attackComponent.canAttack = true
            print("[AttackProjectile.lua] Projectile check passed!")
        else
            
            -- failed the check, print fail message
            print("[AttackProjectile.lua] Projectile check failed.")
        end
        
        -- this will allow PlayerAttack.lua to proceed
        attackComponent.checkCompleted = true
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

        -- no screenshake here (will be triggered by the Projectile entity's script)

        -- instantiate the Projectile (a prefab with a script that makes it float towards the target)
        print("[AttackProjectile.lua] Instantiating projectile!")
        local attackerPos = GetTransform(attackComponent.attacker).position
        InstancePrefab("Projectile", attackerPos.x, attackerPos.y, attackerPos.z)
        
        -- trigger particles (if any)
        
        -- play attack animation (if any)

        -- sound effect will be triggered by MoveProjectile.lua
    end
    -- end of attack effects ---------------------------------------------------------------------------------------------
    
    -- reset variables (do not reset checkCompleted and canAttack here. They will be reset by PlayerAttack.lua)
    -- should be fine if attacker and defender and rotation aren't reset (MoveEnergyBolt.lua needs them)
    attackComponent.startCheck = false
    -- attackComponent.attacker = -1
    -- attackComponent.defender = -1
    -- attackComponent.playerRotation = 0

end

-- checks whether the player and enemy are in the same lane, and if they are within range 
function CheckEnemyWithinProjectileRange(attacker, defender, e)
    
    -- init result
    result = false

    -- get attacker and defenders' locations
    attackerGrid = GetGridCell(attacker)
    defenderGrid = GetGridCell(defender)

    -- get rotation variable
    attackComponent = GetComponent(e, "C_Projectile")

    -- enemy and player on the same x-axis
    if (attackerGrid.x == defenderGrid.x) then

        -- if enemy and player are within 4 tiles of each other
        if (Abs(attackerGrid.z - defenderGrid.z) <= 4) then 

            -- enemy is behind player
            if (attackerGrid.z > defenderGrid.z) then 
                result = true 
                attackComponent.playerRotation = 180
                -- print("enemy is behind player")
            
            -- enemy is in front of player
            else
                result = true 
                attackComponent.playerRotation = 0
                -- print("enemy is in front of player")
            end

        else
            attackComponent.errorMessage = "[AttackProjectile.lua] Cannot use Projectile because enemy is too far away."
        end

    -- enemy and player on the same z-axis
    elseif (attackerGrid.z == defenderGrid.z) then

        -- if enemy and player are within 4 tiles of each other
        if (Abs(attackerGrid.x - defenderGrid.x) <= 4) then 
        
            -- enemy to the player's right
            if (attackerGrid.x > defenderGrid.x) then
                result = true 
                attackComponent.playerRotation = 270    
                -- print("enemy to the player's right")
            
            -- enemy to the player's left
            else
                result = true 
                attackComponent.playerRotation = 90
                -- print("enemy to the player's left")
            end   

        else
            attackComponent.errorMessage = "[AttackProjectile.lua] Cannot use Projectile because enemy is too far away."
        end    
        
    else
        attackComponent.errorMessage = "[AttackProjectile.lua] Cannot use Projectile because Player not in the same lane as target."
    end
    
    return result
end