--[IsComponent]
function C_VFX_Expand()
    local var = {
        --[SerializeFloat]
        Rate = 1.0
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_VFX_Expand(e)
    local transform = GetTransform(e)
    local expand = GetComponent(e, "C_VFX_Expand")
    local dt = GetDeltaTime()
    transform.scale.x = transform.scale.x + expand.Rate * dt
    transform.scale.y = transform.scale.y + expand.Rate * dt
end
