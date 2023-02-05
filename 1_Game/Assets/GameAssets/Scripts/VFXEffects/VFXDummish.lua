-- This script checks Triton's animation frame to display VFX at the right time.

--[IsComponent]
function C_VFXDummish()
    local var = {
        playedOnce = false,
        timer = 0.0,
        cooldown = 5.0
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_VFXDummish(e)

    local currentFrame = CurrentAnimationFrame(e)
    --print(currentFrame)
    local enemyPosition = GetTransform(e).position

    vfxComponent = GetComponent(e, "C_VFXDummish")

    if (vfxComponent.playedOnce) then

        vfxComponent.timer = vfxComponent.timer + GetDeltaTime()

        if (vfxComponent.timer > vfxComponent.cooldown) then
            vfxComponent.playedOnce = false
            vfxComponent.timer = 0.0
        end

        return
    end

    -- trigger various VFX by using dummish's animation timing
    
    -- DUMMISH ATTACK ANIMATION
    if (currentFrame > 33.0 and currentFrame < 36.0) then
        local dummishVibrationVFX = SpawnVFX(e, "VFX_Vibration", enemyPosition.x, enemyPosition.y, enemyPosition.z + 0.1)
        print("VFX_Vibration animation played!!!")     
    end
    if (currentFrame > 34.0 and currentFrame < 36.0) then
        -- activate screenshake (slight delay after vibration vfx)
        local screenShakeEntity = GetEntity("ScreenShake")
        if (screenShakeEntity ~= -1) then
            screenShakeComponent = GetComponent(screenShakeEntity, "C_ScreenShake")
            screenShakeComponent.duration = 0.25
            screenShakeComponent.doScreenShake = true
        end
        vfxComponent.playedOnce = true
    end

    -- for triggering dust effect when dummish faints
    if (currentFrame > 60.0 and currentFrame < 62.0) then
        local enemyDustVFX = SpawnVFX(e, "VFX_Dust", enemyPosition.x, enemyPosition.y, enemyPosition.z)
        GetComponent(enemyDustVFX, "C_DustParticleSystem").parentObject = e

        print("enemyDustVFX animation played!!!")
        vfxComponent.playedOnce = true
    end
    

end