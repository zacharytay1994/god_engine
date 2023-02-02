-- This script animates the vibration VFX.
  -- Min Ye

-- To fix infinitely spawning medium and large rings:
-- 1) create a prefab for each ring size 
-- 2) write a "move forward script" and attach to all ring prefabs
-- 3) spawn them here (with delay in between)

--[IsComponent]
function C_VFXVibration()
    local var = {
        secondRingDelay = 0.016,
        thirdRingDelay = 0.032,
        timer = 0.0
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_VFXVibration(e)

    -- local smallRing = Child(e, 0)
    -- local mediumRing = Child(e, 1)
    -- local largeRing = Child(e, 2)
    -- -- MoveForward(smallRing, 1.5)
    -- -- MoveForward(mediumRing, 1.5)
    -- -- MoveForward(largeRing, 1.5)

    local vibrationTransform = GetTransform(e)
    local vibrationComponent = GetComponent(e, "C_VFXVibration")
    local smallRing
    local mediumRing
    local largeRing
    
    -- spawn the small ring first
    if (vibrationComponent.timer == 0.0) then
        smallRing = InstancePrefabParentedNow(e, "Ring", vibrationTransform.position.x, vibrationTransform.position.y, vibrationTransform.position.z)
        mediumRing = nil
        largeRing = nil
        print("spawned SMALL RING")
    end

    -- then spawn the second ring after a certain duration
    if (vibrationComponent.timer > vibrationComponent.secondRingDelay and mediumRing == nil) then
        mediumRing = InstancePrefabParentedNow(e, "Ring", vibrationTransform.position.x, vibrationTransform.position.y, vibrationTransform.position.z + 1)
        print(EntityName(mediumRing))
        print("spawned MEDIUM RING")
    end

    -- then spawn the third ring after a certain duration
    if (vibrationComponent.timer > vibrationComponent.thirdRingDelay and largeRing == nil) then
        largeRing = InstancePrefabParentedNow(e, "Ring", vibrationTransform.position.x, vibrationTransform.position.y, vibrationTransform.position.z + 3)
        print(EntityName(largeRing))
        print("spawned LARGE RING")
    end

    -- update timer
    vibrationComponent.timer = vibrationComponent.timer + GetDeltaTime()
    print(vibrationComponent.timer)

end

function MoveForward(e, speed)

    local transform = GetTransform(e)

    transform.position.z = transform.position.z + (speed * GetDeltaTime())

end