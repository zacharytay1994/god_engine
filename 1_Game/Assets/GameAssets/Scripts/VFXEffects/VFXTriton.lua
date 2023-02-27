-- This script checks Triton's animation frame to display VFX at the right time.
-- Use this script to spawn VFX that is related to Triton's attacks.

--[IsComponent]
function C_VFXTriton()
    local var = {
        playedOnce = false,
        timer = 0.0,
        cooldown = 3.0
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_VFXTriton(e)

    local currentFrame = CurrentAnimationFrame(e)
    --print(currentFrame)
    local tritonPosition = GetTransform(e).position

    vfxComponent = GetComponent(e, "C_VFXTriton")

    if (vfxComponent.playedOnce) then

        vfxComponent.timer = vfxComponent.timer + GetDeltaTime()

        if (vfxComponent.timer > vfxComponent.cooldown) then
            vfxComponent.playedOnce = false
            vfxComponent.timer = 0.0
        end

        return
    end

    -- trigger various VFX by using triton's animation timing
    if (currentFrame > 137.0 and currentFrame < 140.0) then
        local playerAOEVFX = SpawnVFX(e, "SwirlyLightning", tritonPosition.x, tritonPosition.y + 50, tritonPosition.z + 100)
        print("SwirlyLightning animation played!!!")
        print("player ID:", GetEntityData(e).id)
        vfxComponent.playedOnce = true

        -- attempt to insert VFXRock inside here
        local rockVFX = SpawnVFX(e, "VFX_Rock", tritonPosition.x, tritonPosition.y + 50, tritonPosition.z + 100)
        print("Rock spawned!!!")

        -- activate screenshake
        local screenShakeEntity = GetEntity("ScreenShake")
        if (screenShakeEntity ~= -1) then
            screenShakeComponent = GetComponent(screenShakeEntity, "C_ScreenShake")
            screenShakeComponent.duration = 0.25
            screenShakeComponent.doScreenShake = true
        end
    end

    -- for triggering on-hit vfx (when hit by enemy)
    local dummishList = EntitiesWithScriptComponent("C_VFXDummish")
    for i = 1, #dummishList do
        local dummishFrame = CurrentAnimationFrame(dummishList[i])
        if (dummishFrame > 35.0 and dummishFrame < 37.0) then
            local playerHitVFX = SpawnVFX(e, "VFX_Hit", tritonPosition.x, tritonPosition.y - 50, tritonPosition.z)
            GetComponent(playerHitVFX, "C_VFXHit").parentObject = e
            print("playerHitVFX animation played!!!")
            print("player ID:", GetEntityData(e).id)
            --vfxComponent.playedOnce = true
        end
    end

    -- for triggering dust effect when triton faints
    if (currentFrame > 195.0 and currentFrame < 197.0) then
        local playerDustVFX = SpawnVFX(e, "VFX_Dust", tritonPosition.x, tritonPosition.y, tritonPosition.z - 80)
        GetComponent(playerDustVFX, "C_DustParticleSystem").parentObject = e

        print("playerDustVFX animation played!!!")
        print("player ID:", GetEntityData(e).id)
        vfxComponent.playedOnce = true
    end

end