-- Dust particles are killed after a certain duration.
-- Trying out different methods of killing: shrinking or disappearing

-- TODO: add some variation to the dust movement

--[IsComponent]
function C_DustParticle()
    local var = {
        
        -- total duration the dust particle stays alive
        Lifetime = 0.5,

        -- timer
        Timer = 0.0,
        
        --[SerializeFloat]
        Speed = 1.0,
        
        isAlive = true,

        forwardVector = nil
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_DustParticle(e)
    
    local dustTransform = GetTransform(e)
    local dustParticleComponent = GetComponent(e, "C_DustParticle")
    local dt = GetDeltaTime()
    
    if (dustParticleComponent.Timer > dustParticleComponent.Lifetime) 
    then
        isAlive = false
        RemoveInstance(e)      
        --print(dustParticleComponent.forwardVector.x, dustParticleComponent.forwardVector.y, dustParticleComponent.forwardVector.z)
    else
        dustParticleComponent.Timer = dustParticleComponent.Timer + dt
        
        -- move the particle
        dustTransform.position.y = dustTransform.position.y + (dt * dustParticleComponent.Speed * dustParticleComponent.forwardVector.y * 0.5)
        dustTransform.position.x = dustTransform.position.x + (dt * dustParticleComponent.Speed * dustParticleComponent.forwardVector.x)
        dustTransform.position.z = dustTransform.position.z + (dt * dustParticleComponent.Speed * dustParticleComponent.forwardVector.z)


    end
end
