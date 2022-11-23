--[IsComponent]
function C_ShrinkExpand()
    local var = {
        ExpandCounter = 0.0
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_ShrinkExpand(e)
    local transform = GetTransform(e)
    local shrink_expand = GetComponent(e, "C_ShrinkExpand")
    local dt = GetDeltaTime()
    shrink_expand.ExpandCounter = shrink_expand.ExpandCounter + dt * 2.0
    local offset = math.sin(shrink_expand.ExpandCounter) / 2.0
    if offset < 0 then
        transform.scale.x = 1.0 - offset
        transform.scale.y = 1.0 - offset
    else 
        transform.scale.x = 1.0 + offset
        transform.scale.y = 1.0 + offset
    end
end
