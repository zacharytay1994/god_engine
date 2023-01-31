-- TODO: add some variation to the bubble movement

--[IsComponent]
function C_BubbleParticle()
    local var = {
        Lifetime = 30.0,
        Timer = 0.0,
        --[SerializeFloat]
        Speed = 3.0,
        isAlive = true
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_BubbleParticle(e)
    local transform = GetTransform(e)
    local bubble_particle = GetComponent(e, "C_BubbleParticle")
    local dt = GetDeltaTime()
    if (bubble_particle.Timer > bubble_particle.Lifetime) 
    then
        isAlive = false
        RemoveInstance(e)        
    else
        bubble_particle.Timer = bubble_particle.Timer + dt
        -- move the particle up
        transform.position.y = transform.position.y + dt * bubble_particle.Speed
    end
end
