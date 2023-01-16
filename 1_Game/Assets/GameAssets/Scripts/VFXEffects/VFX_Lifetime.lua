--[IsComponent]
function C_VFX_Lifetime()
    local var = {
        --[SerializeFloat]
        Lifetime = 1.0
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_VFX_Lifetime(e)
    local lifetime = GetComponent(e, "C_VFX_Lifetime")
    if lifetime.Lifetime > 0.0 then
        lifetime.Lifetime = lifetime.Lifetime - GetDeltaTime()
    else
        RemoveInstance(e)
    end
end
