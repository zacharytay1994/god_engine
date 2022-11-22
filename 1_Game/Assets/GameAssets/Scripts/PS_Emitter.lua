--[IsComponent]
function C_PS_Emitter()
    local var = {
        --[SerializeFloat]
        EmissionRate = 1.5,
        Timer = 0.0
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_PS_Emitter(e)
    local dt = GetDeltaTime()
    local ps = GetComponent(e, "C_PS_Emitter")
    local ps_position = GetTransform(e).position.x

    if (ps.Timer < ps.EmissionRate) then
        ps.Timer = ps.Timer + dt
    else
        ps.Timer = GenerateRandomNumberInRange(0, 10) / 10.0

        local x = GenerateRandomNumberInRange(0, 100) / 100 - 0.5
        local y = GenerateRandomNumberInRange(0, 100) / 100
        local z = GenerateRandomNumberInRange(0, 100) / 100 - 0.5
        local s = GenerateRandomNumberInRange(0, 100) / 100
        local particle = InstancePrefabParentedNow(e, "Env_GlowFlies", x, y + 0.5, z)
        local transform = GetTransform(particle)
        transform.scale.x = 0.0
        transform.scale.y = 0.0
        local glow_flies = GetComponent(particle, "C_P_GlowFlies")
        glow_flies.Scale = 0.2 * s;
        glow_flies.Speed = s
    end
end
