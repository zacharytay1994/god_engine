-- This script will move the energy bolt object towards the target.
-- Upon reaching target, it will trigger a screenshake.

--[IsComponent]
function C_MoveProjectile()
    local var = {
        
        --[SerializeString]
        MoveEnergyBolt = "MoveProjectile",

        -- speed of the energy bolt
        speed = 0.5,

        -- starting x position of the projectile
        xStart = 0,

        -- starting z position of the projectile
        zStart = 0,

        -- starting y position of the projectile
        yStart = 0,

        -- how far the energy bolt needs to travel before hitting the target
        distanceToTravel = 0,

        -- set by AttackProjectile.lua
        xDirection = 0,

        -- set by AttackProjectile.lua
        zDirection = 0,

        -- NOT set by AttackProjectile.lua
        yDirection = 1,

        -- speed of sine movement
        frequency = 1.0,

        -- size of sine movement
        magnitude = 1.0,

        -- height of the sine movement
        amplitude = 5.0,

        accumTime = 0,
        distanceTravelled = 0,

        -- initialize the variables once
        initialized = false
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_MoveProjectile(e)
    
    -- if the projectile instance exists
    if (e ~= nil and e ~= -1) then

        -- get the component
        moveProjectileComponent = GetComponent(e, "C_MoveProjectile")

        -- if not initialized yet, then initialize -----------------------------------------------------------------------------
        if (moveProjectileComponent.initialized == false) then 
            
            -- getting the main Projectile component to get the attacker and defender entities + playerRotation
            local combatManagerEntity = GetEntity("CombatManager")
            if (combatManagerEntity ~= -1) then
                attackComponent = GetComponent(combatManagerEntity, "C_Projectile")
            end

            -- getting attacker and defender grid locations
            if (attackComponent.defender ~= -1 and attackComponent.attacker ~= -1) then
                defenderGrid = GetGridCell(attackComponent.defender)
                attackerGrid = GetGridCell(attackComponent.attacker)
            else
                print("[MoveProjectile.lua] attackComponent.defender == -1 or attackComponent.attacker == -1")
            end

            -- storing starting position
            moveProjectileComponent.xStart = GetTransform(e).position.x
            moveProjectileComponent.zStart = GetTransform(e).position.z
            moveProjectileComponent.yStart = GetTransform(e).position.y

            -- determining the direction of energy sphere based on player's direction
            -- note to self: xDirection = 1, the sphere goes to the left
            if (attackComponent.playerRotation == 180) then
                
                --print("sphere goes behind")
                moveProjectileComponent.zDirection = -1
                moveProjectileComponent.distanceToTravel = Abs(defenderGrid.z - attackerGrid.z)

            elseif (attackComponent.playerRotation == 0) then
                
                -- print("sphere goes front")
                moveProjectileComponent.zDirection = 1
                moveProjectileComponent.distanceToTravel = Abs(defenderGrid.z - attackerGrid.z)

            elseif (attackComponent.playerRotation == 270) then
                
                -- print("sphere goes right")
                moveProjectileComponent.xDirection = -1
                moveProjectileComponent.distanceToTravel = Abs(defenderGrid.x - attackerGrid.x)

            elseif (attackComponent.playerRotation == 90) then
                
                -- print("sphere goes left")
                moveProjectileComponent.xDirection = 1
                moveProjectileComponent.distanceToTravel = Abs(defenderGrid.x - attackerGrid.x)
            end
            
            -- each tile is 2 units long
            moveProjectileComponent.distanceToTravel = moveProjectileComponent.distanceToTravel * 2
            
            moveProjectileComponent.initialized = true

            print("[MoveProjectile.lua] Initialized projectile")
        end
        -- end of initializing the projectile ----------------------------------------------------------------------------------

        -- moving the projectile -----------------------------------------------------------------------------------------------
        energyBoltPosition = GetTransform(e).position
        
        local piFactor = moveProjectileComponent.distanceToTravel / 3.4
        
        -- print("moveProjectileComponent.distanceTravelled:", moveProjectileComponent.distanceTravelled);
        
        if (moveProjectileComponent.zDirection == 1 or moveProjectileComponent.zDirection == -1) then
            energyBoltPosition.z = energyBoltPosition.z + moveProjectileComponent.zDirection * moveProjectileComponent.speed
            moveProjectileComponent.distanceTravelled = Abs(energyBoltPosition.z - moveProjectileComponent.zStart) / piFactor   
        elseif (moveProjectileComponent.xDirection == 1 or moveProjectileComponent.xDirection == -1) then
            energyBoltPosition.x = energyBoltPosition.x + moveProjectileComponent.xDirection * moveProjectileComponent.speed  
            moveProjectileComponent.distanceTravelled = Abs(energyBoltPosition.x - moveProjectileComponent.xStart) / piFactor     
        end
         
        energyBoltPosition.y = moveProjectileComponent.yStart + (Sin(moveProjectileComponent.distanceTravelled) * moveProjectileComponent.amplitude)
        -- end of moving the projectile ----------------------------------------------------------------------------------------



        -- terminating the projectile when it reaches its destination  ---------------------------------------------------------
        -- if it's moving front or left, then destroy when position > starting position + distance to travel
        if (moveProjectileComponent.zDirection == 1 or moveProjectileComponent.xDirection == 1) then
            if (energyBoltPosition.x >= moveProjectileComponent.xStart + moveProjectileComponent.distanceToTravel or 
                energyBoltPosition.z >= moveProjectileComponent.zStart + moveProjectileComponent.distanceToTravel) then
                    
                    -- trigger a screenshake
                    screenShakeEntity = GetEntity("ScreenShake")
                    if (screenShakeEntity ~= -1) then
                        screenShakeComponent = GetComponent(screenShakeEntity, "C_ScreenShake")
                        screenShakeComponent.duration = 0.25
                        screenShakeComponent.doScreenShake = true
                    end

                    -- trigger sound effect
                    InstancePrefab("SFX_Jab",0,0,0)

                    -- destroy energy bolt
                    RemoveInstance(e)
            end
        
        -- if it's moving back or right, then destroy when position < starting position - distance to travel
        elseif (moveProjectileComponent.zDirection == -1 or moveProjectileComponent.xDirection == -1) then
            if (energyBoltPosition.x <= moveProjectileComponent.xStart - moveProjectileComponent.distanceToTravel or 
                energyBoltPosition.z <= moveProjectileComponent.zStart - moveProjectileComponent.distanceToTravel) then   
                    
                    -- trigger a screenshake
                    screenShakeEntity = GetEntity("ScreenShake")
                    if (screenShakeEntity ~= -1) then
                        screenShakeComponent = GetComponent(screenShakeEntity, "C_ScreenShake")
                        screenShakeComponent.duration = 0.25
                        screenShakeComponent.doScreenShake = true
                    end
                    
                    -- trigger sound effect
                    InstancePrefab("SFX_Jab",0,0,0)
                    
                    -- destroy energy bolt
                    RemoveInstance(e)
            end
        end
        -- end of terminating projectile ---------------------------------------------------------------------------------------
    end    
end
