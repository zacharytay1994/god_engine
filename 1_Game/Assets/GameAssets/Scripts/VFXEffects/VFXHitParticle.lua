--[IsComponent]
function C_HitParticle()
    local var = {
        
        -- total duration the dust particle stays alive
        Lifetime = 0.1,

        -- timer
        Timer = 0.0,
        
        Speed = 15.0,
        
        isAlive = true,

        forwardVector = nil,
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_HitParticle(e)
    
    local particleTransform = GetTransform(e)
    local particleComponent = GetComponent(e, "C_HitParticle")
    local dt = GetDeltaTime()
    --print(particleComponent.Speed)
    
    if (particleComponent.Timer > particleComponent.Lifetime) 
    then
        isAlive = false
        RemoveInstance(e)      
    else
        particleComponent.Timer = particleComponent.Timer + dt
        
        -- move the particle
        particleTransform.position.y = particleTransform.position.y + (dt * particleComponent.Speed * particleComponent.forwardVector.y)
        particleTransform.position.x = particleTransform.position.x + (dt * particleComponent.Speed * particleComponent.forwardVector.x)
        particleTransform.position.z = particleTransform.position.z + (dt * particleComponent.Speed * particleComponent.forwardVector.z * 0.5)
    end
end
