--[IsComponent]
function C_FishParticle()
    local var = {
        Lifetime = 3.0,
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
function S_FishParticle(e)
    local transform = GetTransform(e)
    --print("hi fish")
    local fish_particle = GetComponent(e, "C_FishParticle")
    local dt = GetDeltaTime()
    if (fish_particle.Timer > fish_particle.Lifetime) 
    then
        isAlive = false
        RemoveInstance(e)
        
        --print("remove fish")
    else
        fish_particle.Timer = fish_particle.Timer + dt
        -- move the particle up
        --transform.position.y = transform.position.y + dt * fish_particle.Speed

        -- move the fish swim vertically
        transform.position.z = transform.position.z + dt * fish_particle.Speed
    end
end
