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
        local vfx = SpawnVFX("VFX_Vibration", 0.5, GetTransform(enemy).position.x, GetTransform(enemy).position.y - 0.2, GetTransform(enemy).position.z + 0.5)
        GetTransform(vfx).scale.x = 2
        GetTransform(vfx).scale.y = 2
        GetTransform(vfx).scale.z = 2

    end

end