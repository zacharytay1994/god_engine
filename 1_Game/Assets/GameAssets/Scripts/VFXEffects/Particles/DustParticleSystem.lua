-- Dust particle system. For when characters push off from the ground / landing on tiles.

-- 1) Dust partcles (of various sizes) all spawn at the same position
-- 2) Each particle will move different (randomized by script)

--[IsComponent]
function C_DustParticleSystem()
    local var = {
        --[SerializeFloat]
        particleCount = 5,
        dustEmitted = false
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_DustParticleSystem(e)

    local dustEmitterComponent = GetComponent(e, "C_DustParticleSystem")

    if (dustEmitterComponent.dustEmitted) then
        return
    end

    local dustEmitterPosition = GetTransform(e).position

    for i = 1, dustEmitterComponent.particleCount do

        --spawn dust here
        local dustParticle = InstancePrefabParentedNow(e, "DustParticle", 0, -1, 0)

        -- each dust particle spawned will face a different direction
        GetTransform(dustParticle).rotation.y = 360 / dustEmitterComponent.particleCount * i

        local forwardVec = ForwardVector(dustParticle)
        -- print(forwardVec.x, forwardVec.y, forwardVec.z)
        GetComponent(dustParticle, "C_DustParticle").forwardVector = forwardVec
    end

    dustEmitterComponent.dustEmitted = true
end