-- This script will move the energy bolt object towards the target.
-- Upon reaching target, it will trigger a screenshake.

--[IsComponent]
function C_MoveEnergyBolt()
    local var = {
        
        --[SerializeString]
        MoveEnergyBolt = "MoveEnergyBolt",

        -- speed of the energy bolt
        speed = 0.5,

        -- starting x position of the energy bolt
        xStart = 0,

        -- starting x position of the energy bolt
        zStart = 0,

        -- how far the energy bolt needs to travel before hitting the target
        distanceToTravel = 0,

        -- set by AttackEnergyBolt.lua
        xDirection = 0,

        -- set by AttackEnergyBolt.lua
        zDirection = 0,

        -- initialize the variables once
        initialized = false
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_MoveEnergyBolt(e)
    
    -- if the energy bolt instance exists
    if (e ~= nil and e ~= -1) then

        -- get the component
        energyBoltComponent = GetComponent(e, "C_MoveEnergyBolt")

        -- if not initialized yet, then initialize
        if (energyBoltComponent.initialized == false) then 
            
            -- getting the main EnergyBolt component to get the attacker and defender entities + playerRotation
            local combatManagerEntity = GetEntity("CombatManager")
            if (combatManagerEntity ~= -1) then
                attackComponent = GetComponent(combatManagerEntity, "C_EnergyBolt")
            end

            -- getting attacker and defender grid locations
            if (attackComponent.defender ~= -1 and attackComponent.attacker ~= -1) then
                defenderGrid = GetGridCell(attackComponent.defender)
                attackerGrid = GetGridCell(attackComponent.attacker)
            else
                print("[MoveEnergyBolt.lua] attackComponent.defender == -1 or attackComponent.attacker == -1")
            end

            -- storing starting position
            energyBoltComponent.xStart = GetTransform(e).position.x
            energyBoltComponent.zStart = GetTransform(e).position.z

            -- determining the direction of energy sphere based on player's direction
            -- note to self: xDirection = 1, the sphere goes to the left
            if (attackComponent.playerRotation == 180) then
                
                --print("sphere goes behind")
                energyBoltComponent.zDirection = -1
                energyBoltComponent.distanceToTravel = Abs(defenderGrid.z - attackerGrid.z)

            elseif (attackComponent.playerRotation == 0) then
                
                -- print("sphere goes front")
                energyBoltComponent.zDirection = 1
                energyBoltComponent.distanceToTravel = Abs(defenderGrid.z - attackerGrid.z)

            elseif (attackComponent.playerRotation == 270) then
                
                -- print("sphere goes right")
                energyBoltComponent.xDirection = -1
                energyBoltComponent.distanceToTravel = Abs(defenderGrid.x - attackerGrid.x)

            elseif (attackComponent.playerRotation == 90) then
                
                -- print("sphere goes left")
                energyBoltComponent.xDirection = 1
                energyBoltComponent.distanceToTravel = Abs(defenderGrid.x - attackerGrid.x)
            end
            
            -- each tile is 2 units long
            energyBoltComponent.distanceToTravel = energyBoltComponent.distanceToTravel * 2
            
            energyBoltComponent.initialized = true

            print("[MoveEnergyBolt.lua] Initialized energy bolt")
        end

        -- moving the energy bolt
        energyBoltPosition = GetTransform(e).position
        energyBoltPosition.x = energyBoltPosition.x + energyBoltComponent.xDirection * energyBoltComponent.speed
        energyBoltPosition.z = energyBoltPosition.z + energyBoltComponent.zDirection * energyBoltComponent.speed

        -- if it's moving front or left, then destroy when position > starting position + distance to travel
        if (energyBoltComponent.zDirection == 1 or energyBoltComponent.xDirection == 1) then
            if (energyBoltPosition.x >= energyBoltComponent.xStart + energyBoltComponent.distanceToTravel or 
                energyBoltPosition.z >= energyBoltComponent.zStart + energyBoltComponent.distanceToTravel) then
                    
                    -- trigger a screenshake
                    screenShakeEntity = GetEntity("ScreenShake")
                    if (screenShakeEntity ~= -1) then
                        screenShakeComponent = GetComponent(screenShakeEntity, "C_ScreenShake")
                        screenShakeComponent.duration = 0.25
                        screenShakeComponent.doScreenShake = true
                    end

                    -- destroy energy bolt
                    RemoveInstance(e)
            end
        
        -- if it's moving back or right, then destroy when position < starting position - distance to travel
        elseif (energyBoltComponent.zDirection == -1 or energyBoltComponent.xDirection == -1) then
            if (energyBoltPosition.x <= energyBoltComponent.xStart - energyBoltComponent.distanceToTravel or 
                energyBoltPosition.z <= energyBoltComponent.zStart - energyBoltComponent.distanceToTravel) then   
                    
                    -- trigger a screenshake
                    screenShakeEntity = GetEntity("ScreenShake")
                    if (screenShakeEntity ~= -1) then
                        screenShakeComponent = GetComponent(screenShakeEntity, "C_ScreenShake")
                        screenShakeComponent.duration = 0.25
                        screenShakeComponent.doScreenShake = true
                    end
                    
                    -- destroy energy bolt
                    RemoveInstance(e)
            end
        end
    end    
end
