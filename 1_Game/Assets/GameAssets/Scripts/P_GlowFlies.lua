--[IsComponent]
function C_P_GlowFlies()
    local var = {
        --[SerializeFloat]
        Speed = 1.0,
        Lifetime = 3.0,
        Timer = 0.0,
        VelX = 0.0,
        VelY = 1.0,
        VelZ = 0.0,
        Offset = 0.0,
        Scale = 1.0,
        isAlive = true
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_P_GlowFlies(e)
    local transform = GetTransform(e)
    local particle = GetComponent(e, "C_P_GlowFlies")
    local dt = GetDeltaTime()
    if (particle.Timer > particle.Lifetime)
    then
        isAlive = false
        RemoveInstance(e)
    else
        particle.Timer = particle.Timer + dt
        -- particle.Offset = math.sin(particle.Timer)
        transform.position.x = transform.position.x + dt * particle.Speed * particle.VelX
        transform.position.y = transform.position.y + dt * particle.Speed * particle.VelY
        transform.position.z = transform.position.z + dt * particle.Speed * particle.VelZ

        if (transform.scale.x < particle.Scale) then
            transform.scale.x = transform.scale.x + dt * 0.2
            transform.scale.y = transform.scale.y + dt * 0.2
        end
    end
end
