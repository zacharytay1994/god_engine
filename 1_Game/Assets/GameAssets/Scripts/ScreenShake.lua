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
        amplitude = 180,

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
    
    -- press B to trigger a screenshake
    if (CheckKeyPress(66)) then
        print("Starting screen shake!")
        cameraShakeComponent.duration = 1.0
        cameraShakeComponent.doScreenShake = true
    end
    
    if (cameraShakeComponent.doScreenShake) then 

        -- get camera object
        local cameraObject = FindCameraObject()

        -- if (cameraShakeComponent.amplitude > 0) then
        if (cameraShakeComponent.duration > 0) then
            
            -- decrease the timer
            cameraShakeComponent.duration = cameraShakeComponent.duration - GetDeltaTime()

            -- change the camera position
            cameraObject.position.y = cameraObject.position.y + Sin(cameraShakeComponent.duration * cameraShakeComponent.amplitude)
        else
            
            -- reset variables
            cameraShakeComponent.doScreenShake = false
            cameraShakeComponent.duration = 0.0
        end
    end
end
