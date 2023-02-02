-- This script animates the vibration VFX.
  -- Min Ye

--[IsComponent]
function C_VFXVibration()
    local var = {

    }
    return function()
        return var
    end
end

--[IsSystem]
function S_VFXVibration(e)

    -- -- just for testing (press Z)
    -- if (CheckKeyPress(90)) then
    --     SpawnVFX("VFX_Slash", 1.0, GetTransform(e).position.x, GetTransform(e).position.y, GetTransform(e).position.z)
    -- end

    local smallRing = GetChild(e, 0)
    local mediumRing = GetChild(e, 1)
    local largeRing = GetChild(e, 2)

    print(EntityName(smallRing), EntityName(mediumRing), EntityName(largeRing))

end