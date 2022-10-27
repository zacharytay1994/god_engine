-- CameraShake will be attached to an entity whose only job is to shake the screen.
-- When you want to screenshake:
-- 1) Get the screenshake entity
-- 2) Access its C_ScreenShake
-- 3) Set the duration first, then set doScreenShake to true
-- 4) The script will reset itself after the shake

--[IsComponent]
function C_ScreenShake()
    local var = {
        
        --[SerializeString]
        ScreenShake = "ScreenShake",
        
        -- duration of the shake
        duration = 0.0,

        -- the magnitude of the shake
        amplitude = 10.0,

        -- how much the magnitude decreases by
        amplitudeChange = -1.0,

        -- counts the duration of the shake
        timer = 0.0,

        -- bool to activate the shake
        doScreenShake = false
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_ScreenShake(e)

    local cameraShakeEntity = GetEntity("ScreenShake")
    if (cameraShakeEntity ~= -1) then
        cameraShakeComponent = GetComponent(cameraShakeEntity, "C_ScreenShake")
    end
    
    if (CheckKeyPress(90)) then
        print("Starting screen shake!")
        cameraShakeComponent.duration = 5.0
        cameraShakeComponent.doScreenShake = true
    end
    
    if (cameraShakeComponent.doScreenShake) then 

        -- get camera object
        local cameraObject = FindCameraObject()

        if (cameraShakeComponent.amplitude > 0) then
            cameraShakeComponent.timer = cameraShakeComponent.timer + GetDeltaTime()
            cameraShakeComponent.amplitude = cameraShakeComponent.amplitude + (cameraShakeComponent.amplitudeChange * GetDeltaTime())
            -- camera.position.y = camera.position.y + sin(amplitude)
            cameraObject.position = cameraObject.position + Sin(cameraShakeComponent.amplitude)
        end
    end
end
