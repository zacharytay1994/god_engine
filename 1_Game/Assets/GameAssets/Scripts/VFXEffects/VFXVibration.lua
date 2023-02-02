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

    --print("start of S_VFXVibration")
    local smallRing = Child(e, 0)
    local mediumRing = Child(e, 1)
    local largeRing = Child(e, 2)
    --print(EntityName(smallRing), EntityName(mediumRing), EntityName(largeRing))

    MoveForward(smallRing, 0.1)
    MoveForward(mediumRing, 0.2)
    MoveForward(largeRing, 0.3)

end

function MoveForward(e, speed)

    local transform = GetTransform(e)

    transform.position.z = transform.position.z + (speed * GetDeltaTime())

end