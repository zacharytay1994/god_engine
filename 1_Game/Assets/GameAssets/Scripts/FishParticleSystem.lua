-- modified to make fish particles spawn at random locations

--[IsComponent]
function C_FishParticleSystem()
    local var = {
        --[SerializeFloat]
        EmissionRate = 1.5,
        Timer = 0.0
    }
    return function()
        return var
    end
end

-- function S_FishParticleSystem(e)
--     local dt = GetDeltaTime()
--     local fish_ps = GetComponent(e, "C_FishParticleSystem")
--     --local fish_particle = GetComponent(e,"C_FishParticle")
--     --print(EntityName(e))
--     --print(fish_particle.isAlive)
--     print(GetEntityData(e).id)
--     local fish_position = GetTransform(e).position.x

--     if (fish_ps.Timer < fish_ps.EmissionRate) then
--         fish_ps.Timer = fish_ps.Timer + dt
--     else
--         fish_ps.Timer = 0.0
--         --if (fish_particle.isAlive == true) then
--         InstancePrefabParented(e, "FishParticle", fish_position * 0.5, 0.0, 0.0)
--         --end
--     end

--     --if CheckKeyPress(65) then
--         --InstancePrefabParentedOnGrid(e,"TilePrefab", -2,3,1)
--     --end
-- end

--[IsSystem]
function S_FishParticleSystem(e)
    local dt = GetDeltaTime()
    local fish_ps = GetComponent(e, "C_FishParticleSystem")

    if (fish_ps.Timer < fish_ps.EmissionRate) then
        fish_ps.Timer = fish_ps.Timer + dt
    else
        fish_ps.Timer = 0.0

        -- generate random location (near the emitter) and scale 
        local x = GenerateRandomNumberInRange(0, 10)
        local y = GenerateRandomNumberInRange(0, 10)
        local z = GenerateRandomNumberInRange(0, 10)
        local fish_particle = InstancePrefabParentedNow(e, "FishParticle", x, y, z)
        --print(GetEntityData(fish_particle).id)
    end

    --if CheckKeyPress(65) then
        --InstancePrefabParentedOnGrid(e,"TilePrefab", -2,3,1)
    --end
end