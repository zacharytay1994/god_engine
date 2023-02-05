-- This script animates the on-hit VFX.
  -- Min Ye

--[IsComponent]
function C_VFXHit()
    local var = {
        
        -- number of particles to spawn
        particleCount = 8,

        -- to tell emitter to emit once, then wait until all children
        -- are dead before removing itself
        particleEmitted = false,

        -- this is for checking whether the caller is Triton or Dummish
        -- for some reason Triton is huge af, so need to scale up some properties
        parentObject = nil
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_VFXHit(e)

    local emitterComponent = GetComponent(e, "C_VFXHit")

    -- if particles have already been emitted, just stop the code here
    if (emitterComponent.particleEmitted) then
        if (ChildCount(e) == 0) then
            RemoveInstance(e)
        end
        return
    end

    local emitterPosition = GetTransform(e).position

    for i = 1, emitterComponent.particleCount do

        -- y-coordinate for each particles' spawn location depends on whether
        -- the VFX is for Triton or Dummish
        local ySpawn
        if (EntityName(emitterComponent.parentObject) == "350TritonModel") then
            ySpawn = 150
        else
            ySpawn = 1.5
        end

        -- spawn hit particle, cycle between 2 particle shapes
        local hitParticle
        if (i % 2 == 0) then
            hitParticle = InstancePrefabParentedNow(e, "HitParticle1", 0, ySpawn, 0.35)
        else
            hitParticle = InstancePrefabParentedNow(e, "HitParticle2", 0, ySpawn, 0.35)
        end
        
        -- each particle spawned will face a different direction (evenly spread across 360 degrees)
        GetTransform(hitParticle).rotation.z = 360 / emitterComponent.particleCount * i

        -- scale up particle size for player model
        if (EntityName(emitterComponent.parentObject) == "350TritonModel") then
            GetTransform(hitParticle).scale.x = GetTransform(hitParticle).scale.x * 400
            GetTransform(hitParticle).scale.y = GetTransform(hitParticle).scale.y * 400
            GetTransform(hitParticle).scale.z = GetTransform(hitParticle).scale.z * 400
            GetComponent(hitParticle, "C_HitParticle").Speed = 1500.0
        end
        -- print(EntityName(emitterComponent.parentObject))

        -- calculate the direction that the particle should move towards
        local forwardVec = HitParticleForwardVector(hitParticle)
        GetComponent(hitParticle, "C_HitParticle").forwardVector = forwardVec
    end

    emitterComponent.particleEmitted = true

end

 -- returns a normalized vector in the direction that the entity is facing.
 -- this function is modified for this case
 function HitParticleForwardVector(entity)
    
    local entityTransform = GetTransform(entity)
    
    local newX = Abs(Cos(RadianToDegree(entityTransform.rotation.z)))
    local newY = Abs(Sin(RadianToDegree(entityTransform.rotation.z)))

    if (entityTransform.rotation.z > 0 and entityTransform.rotation.z < 180) then
        newY = newY * -1
    end

    if (entityTransform.rotation.z > 270 or entityTransform.rotation.z < 90) then
        newX = newX * -1
    end

    return Normalize(newX, newY, 1)
end