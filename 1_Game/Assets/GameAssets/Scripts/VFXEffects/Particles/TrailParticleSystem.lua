-- The object that has this script attached to it will move smoothly from source to destination
-- while emitting a trail of sparkly particles.

--[IsComponent]
function C_TrailParticleSystem()
    local var = {
        emissionRate = 0.0167,
        
        emissionTimer = 0.0,
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_TrailParticleSystem(e)
    local dt = GetDeltaTime()
    --print("dt", dt)
    local trailEmitter = GetComponent(e, "C_TrailParticleSystem")

    -- movement using I J K L
    local emitterPosition = GetTransform(e).position
    local movementMagnitude = 1.0
    if (CheckKeyPress(73)) then
        emitterPosition.x = emitterPosition.x - movementMagnitude
    end
    if (CheckKeyPress(74)) then
        emitterPosition.z = emitterPosition.z + movementMagnitude
    end
    if (CheckKeyPress(75)) then
        emitterPosition.x = emitterPosition.x + movementMagnitude
    end
    if (CheckKeyPress(76)) then
        emitterPosition.z = emitterPosition.z - movementMagnitude
    end

    -- if (trailEmitter.emissionTimer < trailEmitter.emissionRate) then
    --     trailEmitter.emissionTimer = trailEmitter.emissionTimer + dt
    --     return
    -- end

    -- trailEmitter.emissionTimer = 0.0

    
    -- generate a sparkle at emitter position
    local emitterPos = GetTransform(e).position
    local sparkle_particle = InstancePrefabNow("SparkleParticle", emitterPos.x, emitterPos.y, emitterPos.z)
    
    -- randomize sparkle particle direction
    -- print(GenerateRandomNumberInRange(-100, 100) / 100)
    local xDirection = GenerateRandomNumberInRange(-100, 100) / 100
    local yDirection = GenerateRandomNumberInRange(-100, 100) / 100
    local zDirection = GenerateRandomNumberInRange(-100, 100) / 100
    GetComponent(sparkle_particle, "C_TrailParticle").direction = { xDirection, yDirection, zDirection }

    -- randomize sparkle particle speed
    GetComponent(sparkle_particle, "C_TrailParticle").Speed = GenerateRandomNumberInRange(1, 20) / 20

    -- make sparkle particle look small
    GetTransform(sparkle_particle).scale.x = 0.05
    GetTransform(sparkle_particle).scale.y = 0.05
    GetTransform(sparkle_particle).scale.z = 0.05
end