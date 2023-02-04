-- modified to randomize swimming direction

--[IsComponent]
function C_FishParticle()
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

        -- move the fish swim horizontally, every third fish will swim in the opposite direction
        if (GetEntityData(e).id % 3 == 0) then
            transform.position.z = transform.position.z + (dt * fish_particle.Speed * -1)
            transform.rotation.y = 180
        else
            transform.position.z = transform.position.z + dt * fish_particle.Speed
        end
    end
end