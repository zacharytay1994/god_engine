-- This script animates erupting rocks when trident hits the ground.

-- TODO:
-- 1) Crumbling rock particles (from the tip of the rocks) when the rock erupts

--[IsComponent]
function C_VFXRock()
    local var = {
        
        
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_VFXRock(e)

    -- local emitterComponent = GetComponent(e, "C_VFXRock")

    -- -- if particles have already been emitted, just stop the code here
    -- if (emitterComponent.particleEmitted) then
    --     if (ChildCount(e) == 0) then
    --         --print("END OF VFX_HIT")
    --         RemoveInstance(e)
    --     end
    --     return
    -- end

    -- --print("START OF VFX_HIT")

    -- local emitterPosition = GetTransform(e).position

    -- for i = 1, emitterComponent.particleCount do

    --     -- y-coordinate for each particles' spawn location depends on whether
    --     -- the VFX is for Triton or Dummish
    --     local ySpawn
    --     if (EntityName(emitterComponent.parentObject) == "350TritonModel") then
    --         ySpawn = 150
    --     else
    --         ySpawn = 1.5
    --     end

    --     -- spawn hit particle, cycle between 2 particle shapes
    --     local hitParticle
    --     if (i % 2 == 0) then
    --         hitParticle = InstancePrefabParentedNow(e, "HitParticle1", 0, ySpawn, 0.35)
    --     else
    --         hitParticle = InstancePrefabParentedNow(e, "HitParticle2", 0, ySpawn, 0.35)
    --     end
        
    --     -- each particle spawned will face a different direction (evenly spread across 360 degrees)
    --     GetTransform(hitParticle).rotation.z = 360 / emitterComponent.particleCount * i

    --     -- scale up particle size for player model
    --     if (EntityName(emitterComponent.parentObject) == "350TritonModel") then
    --         GetTransform(hitParticle).scale.x = GetTransform(hitParticle).scale.x * 200
    --         GetTransform(hitParticle).scale.y = GetTransform(hitParticle).scale.y * 200
    --         GetTransform(hitParticle).scale.z = GetTransform(hitParticle).scale.z * 200
    --         GetComponent(hitParticle, "C_HitParticle").Speed = 1500.0
    --     end
    --     -- print(EntityName(emitterComponent.parentObject))

    --     -- calculate the direction that the particle should move towards
    --     local forwardVec = HitParticleForwardVector(hitParticle)
    --     GetComponent(hitParticle, "C_HitParticle").forwardVector = forwardVec
    -- end

    -- emitterComponent.particleEmitted = true

    

end

