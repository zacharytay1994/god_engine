-- Defines lifetime and movement of a trail particle

--[IsComponent]
function C_TrailParticle()
    local var = {
        
        -- how long the particle stays before deleting itself
        Lifetime = 1.0,

        Timer = 0.0,
        
        Speed,
        
        -- direction of movement (initialized in TrailParticleSystem)
        direction,
        
        isAlive = true
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_TrailParticle(e)
    local trailParticleTransform = GetTransform(e)
    local trailParticleComponent = GetComponent(e, "C_TrailParticle")
    local dt = GetDeltaTime()
    
    if (trailParticleComponent.Timer > trailParticleComponent.Lifetime) 
    then
        isAlive = false
        RemoveInstance(e)        
    else
        trailParticleComponent.Timer = trailParticleComponent.Timer + dt
        
        -- print("movement direction is", trailParticleComponent.direction[1], trailParticleComponent.direction[2], trailParticleComponent.direction[3], "speed is ", trailParticleComponent.Speed)
        
        
        -- move the particle
        trailParticleTransform.position.x = trailParticleTransform.position.x + (dt * trailParticleComponent.Speed * trailParticleComponent.direction[1])
        trailParticleTransform.position.y = trailParticleTransform.position.y + (dt * trailParticleComponent.Speed * trailParticleComponent.direction[2])
        trailParticleTransform.position.z = trailParticleTransform.position.z + (dt * trailParticleComponent.Speed * trailParticleComponent.direction[3])
    end

end
