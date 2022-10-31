-- This script will move the energy bolt object towards the target.
-- Upon reaching target, it will trigger a screenshake.

--[IsComponent]
function C_MoveEnergyBolt()
    local var = {
        
        --[SerializeString]
        MoveEnergyBolt = "MoveEnergyBolt",

        -- speed of the energy bolt
        speed = 0.5,

        xStart = 0,

        zStart = 0,

        distanceTravelled = 0,

        distanceToTravel = 0,

        -- set by AttackEnergyBolt.lua
        xDirection = 0,

        -- set by AttackEnergyBolt.lua
        zDirection = 0,

        initialized = false
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_MoveEnergyBolt(e)
    
    if (e ~= nil and e ~= -1) then

        energyBoltComponent = GetComponent(e, "C_MoveEnergyBolt")

        if (energyBoltComponent.initialized == false) then 
            
            local combatManagerEntity = GetEntity("CombatManager")
            if (combatManagerEntity ~= -1) then
                attackComponent = GetComponent(combatManagerEntity, "C_EnergyBolt")
            end

            if (attackComponent.defender ~= -1 and attackComponent.attacker ~= -1) then
                defenderGrid = GetGridCell(attackComponent.defender)
                attackerGrid = GetGridCell(attackComponent.attacker)
            else
                print("[MoveEnergyBolt.lua] attackComponent.defender == -1 or attackComponent.attacker == -1")
            end

            -- storing starting position
            --print("Storing energy bolt starting position!!!")
            energyBoltComponent.xStart = GetTransform(e).position.x
            energyBoltComponent.zStart = GetTransform(e).position.z

            -- note to self: xDirection = 1 goes to the left
            if (attackComponent.playerRotation == 270) then
                
                --print("behind")
                energyBoltComponent.zDirection = -1
                energyBoltComponent.distanceToTravel = Abs(defenderGrid.z - attackerGrid.z)

            elseif (attackComponent.playerRotation == 90) then
                
                -- print("front")
                energyBoltComponent.zDirection = 1
                energyBoltComponent.distanceToTravel = Abs(defenderGrid.z - attackerGrid.z)

            elseif (attackComponent.playerRotation == 0) then
                
                -- print("right")
                energyBoltComponent.xDirection = -1
                energyBoltComponent.distanceToTravel = Abs(defenderGrid.x - attackerGrid.x)

            elseif (attackComponent.playerRotation == 180) then
                
                -- print("left")
                energyBoltComponent.xDirection = 1
                energyBoltComponent.distanceToTravel = Abs(defenderGrid.x - attackerGrid.x)
            end

            -- energyBoltComponent.distanceToTravel = energyBoltComponent.distanceToTravel / 2
            
            energyBoltComponent.initialized = true

            print("[MoveEnergyBolt.lua] Initialized energy bolt")
        end

        energyBoltPosition = GetTransform(e).position
        energyBoltPosition.x = energyBoltPosition.x + energyBoltComponent.xDirection * energyBoltComponent.speed
        energyBoltPosition.z = energyBoltPosition.z + energyBoltComponent.zDirection * energyBoltComponent.speed
        
        -- print(Abs(energyBoltPosition.x), "vs", Abs(energyBoltPosition.xStart), "+", energyBoltComponent.distanceToTravel)
        -- print(Abs(energyBoltPosition.z), "vs", Abs(energyBoltPosition.zStart), "+", energyBoltComponent.distanceToTravel)
        -- print("energyBoltComponent.distanceToTravel", energyBoltComponent.distanceToTravel)

        -- print("Abs(energyBoltPosition.z)", Abs(energyBoltPosition.z))
        -- print("Abs(energyBoltComponent.zStart)", Abs(energyBoltComponent.zStart))

        if (Abs(energyBoltPosition.x) >= Abs(energyBoltComponent.xStart) + energyBoltComponent.distanceToTravel or 
            Abs(energyBoltPosition.z) >= Abs(energyBoltComponent.zStart) + energyBoltComponent.distanceToTravel) then
            
            if (energyBoltComponent.xDirection == 0) then
                print("energyBoltComponent.distanceToTravel", energyBoltComponent.distanceToTravel)
                print("Abs(energyBoltComponent.zStart)",Abs(energyBoltComponent.zStart))
                print("Abs(energyBoltPosition.z)", Abs(energyBoltPosition.z))
                print() --line break
            else
                print("energyBoltComponent.distanceToTravel", energyBoltComponent.distanceToTravel)
                print("Abs(energyBoltComponent.xStart)",Abs(energyBoltComponent.xStart))
                print("Abs(energyBoltPosition.x)", Abs(energyBoltPosition.x))
                print() --line break
            end            
                RemoveInstance(e)
        end

    end
    
end
