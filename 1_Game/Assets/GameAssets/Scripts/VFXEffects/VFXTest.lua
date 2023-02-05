-- This script is meant for testing VFX stuff.
-- Will delete once it's not needed anymore 
  -- Min Ye

--[IsComponent]
function C_VFXTest()
    local var = {

    }
    return function()
        return var
    end
end

--[IsSystem]
function S_VFXTest(e)

    -- just for testing (press Z)
    if (CheckKeyPress(90)) then
        local enemy = GetEntity("350DummishModel")
        -- local vfx = SpawnVFX(enemy, "VFX_Vibration", GetTransform(enemy).position.x, GetTransform(enemy).position.y, GetTransform(enemy).position.z)

        -- local enemyHitVFX = SpawnVFX(enemy, "VFX_Hit", GetTransform(enemy).position.x, GetTransform(enemy).position.y, GetTransform(enemy).position.z)
        -- GetComponent(enemyHitVFX, "C_VFXHit").parentObject = enemy

        -- local enemyDustVFX = SpawnVFX(enemy, "VFX_Dust", GetTransform(enemy).position.x, GetTransform(enemy).position.y, GetTransform(enemy).position.z)
        -- GetComponent(enemyDustVFX, "C_DustParticleSystem").parentObject = enemy

        
        
        
        
        
        local player = GetEntity("350TritonModel")
        -- local playerHitVFX = SpawnVFX(player, "VFX_Hit", GetTransform(player).position.x, GetTransform(player).position.y, GetTransform(player).position.z)
        -- GetComponent(playerHitVFX, "C_VFXHit").parentObject = player
        
        -- local playerDustVFX = SpawnVFX(player, "VFX_Dust", GetTransform(player).position.x, GetTransform(player).position.y, GetTransform(player).position.z)
        -- GetComponent(playerDustVFX, "C_DustParticleSystem").parentObject = player

        local playerAOEVFX = SpawnVFX(player, "SwirlyLightning", GetTransform(player).position.x, GetTransform(player).position.y + 50, GetTransform(player).position.z + 150)

    end
end