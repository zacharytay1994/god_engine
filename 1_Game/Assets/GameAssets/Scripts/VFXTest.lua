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
        SpawnVFX("VFX_Slash", 1.0, GetTransform(e).position.x, GetTransform(e).position.y, GetTransform(e).position.z)
    end

end