-- This script animates the exclamation mark effect when Squinky sees Triton.
  -- Min Ye

--[IsComponent]
function C_VFXSquinkyAttention()
    local var = {
        halfwayTimer = 0.05,
        lifetime = 0.1,
        timer = 0.0,
        speed = 40.0,
        enabled = false,
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_VFXSquinkyAttention(e)

    local vfxPosition = GetTransform(e).position
    local vfxComponent = GetComponent(e, "C_VFXSquinkyAttention")
    local dt = GetDeltaTime()

    -- if (CheckKeyPress(88)) then
    --     vfxComponent.enabled = true
    -- end

    -- if (vfxComponent.enabled == false) then
    --     return
    -- end
    
    if (vfxComponent.timer < vfxComponent.halfwayTimer) then
        vfxPosition.y = vfxPosition.y + (vfxComponent.speed * dt)
    else 
        vfxPosition.y = vfxPosition.y - (vfxComponent.speed * dt)
    end

    if (vfxComponent.timer > vfxComponent.lifetime) then
        RemoveInstance(e)
    end

    vfxComponent.timer = vfxComponent.timer + dt

end