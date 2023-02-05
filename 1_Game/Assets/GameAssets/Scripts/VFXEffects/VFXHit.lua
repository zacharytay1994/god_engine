-- This script animates the on-hit VFX.
  -- Min Ye

--[IsComponent]
function C_VFXHit()
    local var = {
        particleCount = 8,
        particleEmitted = false
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_VFXHit(e)

    local emitterComponent = GetComponent(e, "C_VFXHit")

    if (emitterComponent.dustEmitted) then
        if (ChildCount(e) == 0) then
            RemoveInstance(e)
        end
        return
    end

    local emitterPosition = GetTransform(e).position

    for i = 1, emitterComponent.particleCount do

        
        -- spawn hit particle, cycle between 2 particle shapes
        local hitParticle
        if (i % 2 == 0) then
            hitParticle = InstancePrefabParentedNow(e, "HitParticle1", 0, -0.25, 0.35)
        else
            hitParticle = InstancePrefabParentedNow(e, "HitParticle2", 0, -0.25, 0.35)
        end
        
        -- each particle spawned will face a different direction (evenly spread across 360 degrees)
        GetTransform(hitParticle).rotation.z = 360 / emitterComponent.particleCount * i

        local forwardVec = HitParticleForwardVector(hitParticle)
        print(forwardVec.x, forwardVec.y, forwardVec.z)
        print()
        GetComponent(hitParticle, "C_HitParticle").forwardVector = forwardVec

        -- for debugging
        GetComponent(hitParticle, "C_HitParticle").forwardVectorX = forwardVec.x
        GetComponent(hitParticle, "C_HitParticle").forwardVectorY = forwardVec.y
        GetComponent(hitParticle, "C_HitParticle").forwardVectorZ = forwardVec.z
        GetComponent(hitParticle, "C_HitParticle").spawnSequence = i
        
    end

    emitterComponent.dustEmitted = true

end

 -- returns a normalized vector in the direction that the entity is facing.
 -- this function is modified for this case
 function HitParticleForwardVector(entity)
    
    local entityTransform = GetTransform(entity)
    --print("entity rotation is", entityTransform.rotation.x, entityTransform.rotation.y, entityTransform.rotation.z)
    
    local newX = Abs(Cos(RadianToDegree(entityTransform.rotation.z)))
    -- print("sin90 = ", Sin(90))
    -- print("RadianToDegree(90):", RadianToDegree(90))
    local newY = Abs(Sin(RadianToDegree(entityTransform.rotation.z)))

    if (entityTransform.rotation.z > 0 and entityTransform.rotation.z < 180) then
        newY = newY * -1
    end

    if (entityTransform.rotation.z > 270 or entityTransform.rotation.z < 90) then
        newX = newX * -1
    end


    return Normalize(newX, newY, 1)

end