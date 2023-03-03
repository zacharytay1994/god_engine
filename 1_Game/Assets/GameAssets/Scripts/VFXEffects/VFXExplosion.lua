-- This script animates an explosion effect.
  -- Min Ye

--[IsComponent]
function C_VFXExplosion()
    local var = {
        
        -- flag, for spawning the red sphere
        explosionInitialized = false,

        -- reference to the red sphere entity
        redBall = nil,

        -- timer for the animation
        timer = 0.0,

        -- duration of each expand / contract animation
        expandContractTime = 0.25,

        -- number of times the explosion expands and contracts
        -- e.g. 1 expand and 1 contract is 2 repetitions
        repetitions = 6,

        -- counts the number of repetitions done
        repetitionCounter = 0,

        -- current state of the red sphere
        expandState = true,
    
        -- number of particles that shoots out from the explosion (idk what it's called)
        strayParticles = 8
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_VFXExplosion(e)

    local vfxExplosionComponent = GetComponent(e, "C_VFXExplosion")
    local vfxShowComponent = GetComponent(e, "C_VFXShow")

    -- play explosion animation (expand and contract, then maybe add particles)
    
    -- start of animation: instantiate 
    if (vfxExplosionComponent.explosionInitialized == false) then
        local explosionPosition = GetTransform(e).position 
        vfxExplosionComponent.redBall = InstancePrefabParentedNow(e, "RedBall", explosionPosition.x, explosionPosition.y, explosionPosition.z)
        vfxExplosionComponent.explosionInitialized = true
    end

    -- alternate between expand and contract states
    if (vfxExplosionComponent.timer < vfxExplosionComponent.expandContractTime) then
        vfxExplosionComponent.timer = vfxExplosionComponent.timer + GetDeltaTime()
    else
        vfxExplosionComponent.expandState = ~vfxExplosionComponent.expandState
        vfxExplosionComponent.repetitionCounter = vfxExplosionComponent.repetitionCounter + 1
    end

    -- spawn particles once done with repetitions
    if (vfxExplosionComponent.repetitionCounter == vfxExplosionComponent.repetitions) then
        -- TODO: spawn particles here

        -- destroy this instance
        RemoveInstance(e)
    end
end