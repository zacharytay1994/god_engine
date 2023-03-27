-- This script animates the vibration VFX.
  -- Min Ye

--[IsComponent]
function C_VFXVibration()
    local var = {
        secondRingDelay = 0.128,
        thirdRingDelay = 0.320,
        timer = 0.0,

        smallRing = nil,
        mediumRing = nil,
        largeRing = nil
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_VFXVibration(e)

    -- local smallRing = Child(e, 0)


    local vibrationTransform = GetTransform(e)
    local vibrationComponent = GetComponent(e, "C_VFXVibration")
    
    -- spawn the small ring first
    if (vibrationComponent.smallRing == nil) then
        vibrationComponent.smallRing = InstancePrefabParentedNow(e, "Ring", vibrationTransform.position.x, vibrationTransform.position.y + 1.9, vibrationTransform.position.z + 0.25)
        GetTransform(vibrationComponent.smallRing).scale.x = 0.1
        GetTransform(vibrationComponent.smallRing).scale.y = 0.1
        GetTransform(vibrationComponent.smallRing).scale.z = 0.1
        --print("spawned SMALL RING")
    end

    -- then spawn the second ring after a certain duration
    if (vibrationComponent.timer > vibrationComponent.secondRingDelay and vibrationComponent.mediumRing == nil) then
        vibrationComponent.mediumRing = InstancePrefabParentedNow(e, "Ring", vibrationTransform.position.x, vibrationTransform.position.y + 1.9, vibrationTransform.position.z + 0.25 + 0.01)
        GetTransform(vibrationComponent.mediumRing).scale.x = 0.15
        GetTransform(vibrationComponent.mediumRing).scale.y = 0.15
        GetTransform(vibrationComponent.mediumRing).scale.z = 0.15
        --print("spawned MEDIUM RING")
    end

    -- then spawn the third ring after a certain duration
    if (vibrationComponent.timer > vibrationComponent.thirdRingDelay and vibrationComponent.largeRing == nil) then
        vibrationComponent.largeRing = InstancePrefabParentedNow(e, "Ring", vibrationTransform.position.x, vibrationTransform.position.y + 1.9, vibrationTransform.position.z + 0.25 + 0.10)
        GetTransform(vibrationComponent.largeRing).scale.x = 0.2
        GetTransform(vibrationComponent.largeRing).scale.y = 0.2
        GetTransform(vibrationComponent.largeRing).scale.z = 0.2
        --print("spawned LARGE RING")
    end

    -- update timer
    vibrationComponent.timer = vibrationComponent.timer + GetDeltaTime()
    --print(vibrationComponent.timer)

    MoveForward(vibrationComponent.smallRing, 0.1)
    MoveForward(vibrationComponent.mediumRing, 0.25)
    MoveForward(vibrationComponent.largeRing, 0.4)

end

function MoveForward(e, speed)

    if (e == nil) then
        return
    end
    
    local transform = GetTransform(e)

    transform.position.z = transform.position.z + (speed * GetDeltaTime())

end