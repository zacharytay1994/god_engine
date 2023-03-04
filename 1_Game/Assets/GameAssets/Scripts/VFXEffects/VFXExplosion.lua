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
        expandContractTime = 0.05,

        -- number of times the explosion expands and contracts
        -- e.g. 1 expand and 1 contract is 2 repetitions
        repetitions = 4,

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
        print("expandState before changing:", vfxExplosionComponent.expandState)
        --vfxExplosionComponent.expandState = ~(vfxExplosionComponent.expandState)
        if (vfxExplosionComponent.expandState) then
            vfxExplosionComponent.expandState = false
        else
            vfxExplosionComponent.expandState = true
        end
        print("expandState after changing:", vfxExplosionComponent.expandState)
        vfxExplosionComponent.repetitionCounter = vfxExplosionComponent.repetitionCounter + 1
        vfxExplosionComponent.timer = 0.0
    end

    if (vfxExplosionComponent.expandState) then
        print("expanding")
        GetTransform(e).scale.x = GetTransform(e).scale.x + 5
        GetTransform(e).scale.y = GetTransform(e).scale.y + 5
        GetTransform(e).scale.z = GetTransform(e).scale.z + 5
    else
        print("contracting")
        GetTransform(e).scale.x = GetTransform(e).scale.x - 5
        GetTransform(e).scale.y = GetTransform(e).scale.y - 5
        GetTransform(e).scale.z = GetTransform(e).scale.z - 5
    end

    -- spawn particles once done with repetitions
    if (vfxExplosionComponent.repetitionCounter == vfxExplosionComponent.repetitions) then
        -- TODO: spawn particles here

        -- destroy this instance
        RemoveInstance(e)
    end
end