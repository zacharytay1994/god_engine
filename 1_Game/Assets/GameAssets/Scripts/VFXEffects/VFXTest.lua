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
        local enemy = GetEntity("350Pawn")
        --local vfx = SpawnVFX("VFX_Vibration", GetTransform(enemy).position.x, GetTransform(enemy).position.y, GetTransform(enemy).position.z)
        local hitVFX = SpawnVFX("VFX_Hit", GetTransform(enemy).position.x, GetTransform(enemy).position.y, GetTransform(enemy).position.z)

        local player = GetEntity("350Triton")
        local dust = SpawnVFX("VFX_Dust", GetTransform(player).position.x, GetTransform(player).position.y, GetTransform(player).position.z)
    end
end