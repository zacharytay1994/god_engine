-- Dust particle system. For when characters push off from the ground / landing on tiles.

-- 1) Dust partcles (of various sizes) all spawn at the same position
-- 2) Each particle will move different (randomized by script)

--[IsComponent]
function C_DustParticleSystem()
    local var = {
        --[SerializeFloat]
        particleCount = 5,
        dustEmitted = false,
        parentObject = nil
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_DustParticleSystem(e)

    local dustEmitterComponent = GetComponent(e, "C_DustParticleSystem")

    if (dustEmitterComponent.dustEmitted) then
        if (ChildCount(e) == 0) then
            RemoveInstance(e)
        end
        return
    end

    local dustEmitterPosition = GetTransform(e).position

    for i = 1, dustEmitterComponent.particleCount do

        --spawn dust here
        local dustParticle = InstancePrefabParentedNow(e, "DustParticle", 0, 1, 0)

        if (EntityName(dustEmitterComponent.parentObject) == "350TritonModel") then
            -- increase scale by 100 times because the Triton model is 0.01
            GetTransform(dustParticle).scale.x = GetTransform(dustParticle).scale.x * 100
            GetTransform(dustParticle).scale.y = GetTransform(dustParticle).scale.y * 100
            GetTransform(dustParticle).scale.z = GetTransform(dustParticle).scale.z * 100
            GetComponent(dustParticle, "C_DustParticle").Speed = 250.0            
        end

        -- each dust particle spawned will face a different direction
        GetTransform(dustParticle).rotation.y = 360 / dustEmitterComponent.particleCount * i

        local forwardVec = ForwardVector(dustParticle)
        -- print(forwardVec.x, forwardVec.y, forwardVec.z)
        GetComponent(dustParticle, "C_DustParticle").forwardVector = forwardVec
    end

    dustEmitterComponent.dustEmitted = true
end