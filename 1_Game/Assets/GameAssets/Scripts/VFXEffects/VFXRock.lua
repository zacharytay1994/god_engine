-- This script animates the pop up rock effect with Triton's trident smashes down into the enemy.
-- It also triggers the RockParticle effect.
  -- Min Ye

-- TODO: may have to lengthen fadeDelay in VFXSHOW, must match with the explosion
-- TODO: add code to trigger explosion vfx

--[IsComponent]
function C_VFXRock()
    local var = {
        
        -- number of particles to spawn
        particleCount = 8,

        -- to tell emitter to emit once, then wait until all children
        -- are dead before removing itself
        particleEmitted = false,

        -- -- this is for checking whether the caller is Triton or Dummish
        -- -- for some reason Triton is huge af, so need to scale up some properties
        -- parentObject = nil

        timer = 0.0,

        pauseTimer = 0.2,

        emissiveLimit = 2.0,

        explosionTriggered = false
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_VFXRock(e)

    local vfxRockComponent = GetComponent(e, "C_VFXRock")
    local vfxShowComponent = GetComponent(e, "C_VFXShow")

    

    -- if (vfxRockComponent.timer < vfxRockComponent.pauseTimer) then
    --     vfxRockComponent.timer = vfxRockComponent.timer + GetDeltaTime()
    -- else
    --     -- emissive will start increasing the moment the rocks are spawned
    --     GetRenderable3D(Child(e, 0)).emissive = GetRenderable3D(Child(e, 0)).emissive + 1
    --     GetRenderable3D(Child(e, 1)).emissive = GetRenderable3D(Child(e, 1)).emissive + 1
    --     GetRenderable3D(Child(e, 2)).emissive = GetRenderable3D(Child(e, 2)).emissive + 1
    -- end
    
    
    
    -- -- if explosion already triggered then stop here
    -- if (vfxRockComponent.explosionTriggered) then
    --     return
    -- end

    -- -- Trigger an explosion vfx after emissive hits the emissiveLimit.
    -- -- At around 3 frames before the rocks disappear, if emissive has not reached 
    -- -- emissiveLimit yet, then just force an explosion.
    -- local vfxRockComponent = GetComponent(e, "C_VFXRock")
    -- if (GetRenderable3D(Child(e, 0)).emissive > vfxRockComponent.emissiveLimit) then
    --     print("spawning natural explosion")    
    --     -- SPAWN EXPLOSION VFX HERE
    --     local rockPosition = GetTransform(e).position
        
    --     local vfxExplosion = SpawnVFX(GetEntity("350TritonModel"), "VFX_Explosion", rockPosition.x, rockPosition.y + 50, rockPosition.z + 100)
    --     -- local vfxExplosion = SpawnVFX(GetEntity("350TritonModel"), "VFX_Explosion", 0, 0, 0)
    --     -- InstancePrefabNow("VFX_Explosion", 0, 0, 0)
    --     print("seconds taken to trigger explosion:", vfxRockComponent.timer)

    --     vfxRockComponent.explosionTriggered = true
    -- elseif (vfxShowComponent.fadeDelay - vfxRockComponent.timer < (3 * GetDeltaTime())) then
    --     print("trigger backup explosion")
    --     -- SPAWN EXPLOSION VFX HERE
    --     -- local rockPosition = GetTransform(e).position
    --     -- local vfxExplosion = SpawnVFX(e, "VFX_Explosion", rockPosition.x, rockPosition.y + 50, rockPosition.z + 100)
    --     -- local vfxExplosion = SpawnVFX(GetEntity("350TritonModel"), "VFX_Explosion", 0, 0, 0)
    --     local vfxExplosion = SpawnVFX(GetEntity("350TritonModel"), "VFX_Explosion", rockPosition.x, rockPosition.y + 50, rockPosition.z + 100)
    --     print("seconds taken to trigger explosion:", vfxRockComponent.timer)
    --     vfxRockComponent.explosionTriggered = true
    -- end
end