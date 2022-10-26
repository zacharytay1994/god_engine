--[IsComponent]
function C_FishParticleSystem()
    local var = {
        --[SerializeFloat]
        EmissionRate = 0.2,
        Timer = 0.0
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_FishParticleSystem(e)
    local dt = GetDeltaTime();
    local fish_ps = GetComponent(e, "C_FishParticleSystem")
    if (fish_ps.Timer < fish_ps.EmissionRate) then
        fish_ps.Timer = fish_ps.Timer + dt
    else
        fish_ps.Timer = 0.0
        InstancePrefabParented(e, "FishParticle", 0.0, 0.5, 0.0)
    end

    if CheckKeyPress(65) then
        InstancePrefabParentedOnGrid(e,"TilePrefab",-2,1,3)
    end
end
