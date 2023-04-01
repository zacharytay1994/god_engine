-- This script animates the exclamation mark effect when Squinky sees Triton.
  -- Min Ye

--[IsComponent]
function C_VFXSquinkyAttention()
    local var = {
        moveUpTime = 0.05,
        moveDownTime = 0.1,
        staystillTime = 0.5,
        timer = 0.0,
        speed = 20.0,
        enabled = false,
        fixedFrameTime = 1/60,
        accumTime = 0.0,
        yOffset = 0.7,
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

    vfxComponent.accumTime = vfxComponent.accumTime + dt
    --print("vfxComponent.accumTime:", vfxComponent.accumTime)
    
    if (vfxComponent.accumTime < vfxComponent.fixedFrameTime) then
        --print("vfxComponent.accumTime < vfxComponent.fixedFrameTime")
        return
    else
        vfxComponent.timer = vfxComponent.timer + vfxComponent.accumTime
        --print("vfxComponent.timer:", vfxComponent.timer)
        vfxComponent.accumTime = 0  
    end
    

    -- if (CheckKeyPress(88)) then
    --     vfxComponent.enabled = true
    -- end

    -- if (vfxComponent.enabled == false) then
    --     return
    -- end


    
    if (vfxComponent.timer < vfxComponent.moveUpTime) then
        vfxPosition.y = vfxPosition.y + (vfxComponent.speed * vfxComponent.fixedFrameTime) + vfxComponent.yOffset
        --print("moving up")
    elseif (vfxComponent.timer < vfxComponent.moveDownTime) then
        vfxPosition.y = vfxPosition.y - (vfxComponent.speed * vfxComponent.fixedFrameTime) + vfxComponent.yOffset
        --print("moving down")
    end

    if (vfxComponent.timer > vfxComponent.staystillTime) then
        --print("removing instance")
        RemoveInstance(e)
    end
end