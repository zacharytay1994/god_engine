-- This script checks Triton's animation frame to display VFX at the right time.

--[IsComponent]
function C_VFXTriton()
    local var = {
        playedOnce = false,
        attackEffectsPlayed = false,
        explosionEffectsPlayed = false,
        onHitEffectsPlayed = false,
        dustEffectPlayed = false,
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

    vfxComponent.timer = vfxComponent.timer + GetDeltaTime()

    if (vfxComponent.timer > vfxComponent.cooldown) then
        vfxComponent.playedOnce = false
        vfxComponent.attackEffectsPlayed = false
        vfxComponent.explosionEffectsPlayed = false
        vfxComponent.onHitEffectsPlayed = false
        vfxComponent.dustEffectPlayed = false
        vfxComponent.timer = 0.0
    end


    -- trigger various VFX by using triton's animation timing
    
    -- during Triton AOE Attack Animation (trident flies towards the ground)
    if (currentFrame > 137.0 and currentFrame < 140.0 and vfxComponent.attackEffectsPlayed == false) then
        
        -- will not enter into this block of code while in cooldown
        vfxComponent.attackEffectsPlayed = true
        
        -- swirly lightning effect around the enemy
        local playerAOEVFX = SpawnVFX(e, "SwirlyLightning", tritonPosition.x, tritonPosition.y + 50, tritonPosition.z + 100)
        --print(GetTransform(playerAOEVFX).scale.x)
        --print("SwirlyLightning animation played!!!")
        --print("player ID:", GetEntityData(e).id)
        
        -- activate screenshake
        local screenShakeEntity = GetEntity("ScreenShake")
        if (screenShakeEntity ~= -1) then
            screenShakeComponent = GetComponent(screenShakeEntity, "C_ScreenShake")
            screenShakeComponent.duration = 0.25
            screenShakeComponent.doScreenShake = true
        end

        -- rock effect around enemy
        local rockVFX = SpawnVFX(e, "VFX_Rock", tritonPosition.x - 100, tritonPosition.y + 50, tritonPosition.z + 100)
        --print(GetTransform(rockVFX).scale.x)
        --print("Rock animation played!!!")

    end

    -- play explosion vfx shortly after Triton AOE attack animation
    if (currentFrame > 147.0 and currentFrame < 150.0 and vfxComponent.explosionEffectsPlayed == false) then
        vfxComponent.explosionEffectsPlayed = true
        local playerAOEVFX = SpawnVFX(e, "VFX_Explosion", tritonPosition.x, tritonPosition.y + 50, tritonPosition.z + 125)

        -- activate screenshake
        local screenShakeEntity = GetEntity("ScreenShake")
        if (screenShakeEntity ~= -1) then
            screenShakeComponent = GetComponent(screenShakeEntity, "C_ScreenShake")
            screenShakeComponent.duration = 0.50
            screenShakeComponent.doScreenShake = true
        end

    end

    -- for triggering on-hit vfx (when hit by enemy)
    local dummishList = EntitiesWithScriptComponent("C_VFXDummish")
    for i = 1, #dummishList do
        local dummishFrame = CurrentAnimationFrame(dummishList[i])
        if (dummishFrame > 35.0 and dummishFrame < 37.0 and vfxComponent.onHitEffectsPlayed == false) then
            
            local playerHitVFX = SpawnVFX(e, "VFX_Hit", tritonPosition.x, tritonPosition.y - 50, tritonPosition.z)
            GetComponent(playerHitVFX, "C_VFXHit").parentObject = e
            print("playerHitVFX animation played!!!")
            print("player ID:", GetEntityData(e).id)
            vfxComponent.onHitEffectsPlayed = true
        end
    end

    -- for triggering dust effect when triton faints
    if (currentFrame > 195.0 and currentFrame < 197.0 and vfxComponent.dustEffectPlayed == false) then
        local playerDustVFX = SpawnVFX(e, "VFX_Dust", tritonPosition.x, tritonPosition.y, tritonPosition.z - 80)
        GetComponent(playerDustVFX, "C_DustParticleSystem").parentObject = e

        --print("playerDustVFX animation played!!!")
        --print("player ID:", GetEntityData(e).id)
        vfxComponent.dustEffectPlayed = true
    end

end