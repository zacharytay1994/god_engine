-- This script checks Triton's animation frame to display VFX at the right time.

--[IsComponent]
function C_VFXTriton()
    local var = {
        playedOnce = false
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_VFXTriton(e)

    local currentFrame = CurrentAnimationFrame(e)
    -- print(currentFrame)
    local tritonPosition = GetTransform(e).position

    vfxComponent = GetComponent(e, "C_VFXTriton")

    if (vfxComponent.playedOnce) then
        return
    end

    -- trigger various VFX by using triton's animation timing
    if (currentFrame > 80.0 and currentFrame < 160.0) then
        local playerAOEVFX = SpawnVFX(e, "SwirlyLightning", tritonPosition.x, tritonPosition.y + 50, tritonPosition.z + 150)
        print("animation played!!!")
        vfxComponent.playedOnce = true
    end

end