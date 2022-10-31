-- This script will move the energy bolt object towards the target.
-- Upon reaching target, it will trigger a screenshake.

--[IsComponent]
function C_MoveEnergyBolt()
    local var = {
        
        --[SerializeString]
        MoveEnergyBolt = "MoveEnergyBolt",

        -- speed of the energy bolt
        speed = 2.0,

        -- destination will be set by AttackEnergyBolt.lua
        destination = nil,

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
            
            
            
            attackComponent = GetComponent(GetEntity("CombatManager"), "C_EnergyBolt")
            energyBoltComponent.destination = GetTransform(attackComponent.defender).position

            if (attackComponent.playerRotation == 270) then
                energyBoltComponent.zDirection = -1
            elseif (attackComponent.playerRotation == 90) then
                energyBoltComponent.zDirection = 1
            elseif (attackComponent.playerRotation == 0) then
                energyBoltComponent.xDirection = -1
            elseif (attackComponent.playerRotation == 180) then
                energyBoltComponent.xDirection = 1
            end

            energyBoltComponent.initialized = true

            print("initializing energy bolt")
        end

        energyBoltPosition = GetTransform(e).position
        energyBoltPosition.x = energyBoltPosition.x + energyBoltComponent.xDirection * energyBoltComponent.speed
        energyBoltPosition.z = energyBoltPosition.z + energyBoltComponent.zDirection * energyBoltComponent.speed

        if (energyBoltPosition.x == energyBoltComponent.destination.x and energyBoltPosition.z == energyBoltComponent.destination.z) then
            RemoveInstance(e)
        end

    end
    
end
