-- This script animates the door open vfx.
  -- Min Ye

--[IsComponent]
function C_VFXDoorClose()
    local var = {
        
        -- number of particles to spawn
        particleCount = 8,

        -- to tell emitter to emit once, then wait until all children
        -- are dead before removing itself
        particleEmitted = false,

        -- this is for checking whether the caller is Triton or Dummish
        -- for some reason Triton is huge af, so need to scale up some properties
        parentObject = nil
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_VFXDoorClose(e)

    local emitterComponent = GetComponent(e, "C_VFXDoorClose")

    -- if particles have already been emitted, just stop the code here
    -- and remove emitter if all children have removed themselves
    if (emitterComponent.particleEmitted) then
        if (ChildCount(e) == 0) then
            RemoveInstance(e)
        end
        return
    end

    local emitterPosition = GetTransform(e).position

    for i = 1, emitterComponent.particleCount do
    
        local xSpawn
        local ySpawn
        
        local doorParticle
        -- imma hard code this im sry
        -- if (i == 1) then
        --     xSpawn = 1
        --     ySpawn = 1
        --     doorParticle = InstancePrefabParentedNow(e, "DoorParticle2", xSpawn, ySpawn, 0.35)
        -- elseif (i ==2) then
        --     xSpawn = 0
        --     ySpawn = 2
        --     doorParticle = InstancePrefabParentedNow(e, "DoorParticle1", xSpawn, ySpawn, 0.35)
        -- elseif (i ==3) then
        --     xSpawn = -1
        --     ySpawn = 1
        --     doorParticle = InstancePrefabParentedNow(e, "DoorParticle2", xSpawn, ySpawn, 0.35)
        -- elseif (i ==4) then
        --     xSpawn = -2
        --     ySpawn = 0
        --     doorParticle = InstancePrefabParentedNow(e, "DoorParticle1", xSpawn, ySpawn, 0.35)
        -- elseif (i == 5) then
        --     xSpawn = -1
        --     ySpawn = -1
        --     doorParticle = InstancePrefabParentedNow(e, "DoorParticle2", xSpawn, ySpawn, 0.35)
        -- elseif (i ==6) then
        --     xSpawn = 0
        --     ySpawn = -2
        --     doorParticle = InstancePrefabParentedNow(e, "DoorParticle1", xSpawn, ySpawn, 0.35)
        -- elseif (i ==7) then
        --     xSpawn = 1
        --     ySpawn = -1
        --     doorParticle = InstancePrefabParentedNow(e, "DoorParticle2", xSpawn, ySpawn, 0.35)
        -- elseif (i ==8) then
        --     xSpawn = 2
        --     ySpawn = 0
        --     doorParticle = InstancePrefabParentedNow(e, "DoorParticle1", xSpawn, ySpawn, 0.35)
        -- end

        --invert this
        if (i == 1) then
            xSpawn = -1
            ySpawn = -1
            doorParticle = InstancePrefabParentedNow(e, "DoorParticle2", xSpawn, ySpawn, 0.35)
        elseif (i ==2) then
            xSpawn = 0
            ySpawn = -2
            doorParticle = InstancePrefabParentedNow(e, "DoorParticle1", xSpawn, ySpawn, 0.35)
        elseif (i ==3) then
            xSpawn = 1
            ySpawn = -1
            doorParticle = InstancePrefabParentedNow(e, "DoorParticle2", xSpawn, ySpawn, 0.35)
        elseif (i ==4) then
            xSpawn = 2
            ySpawn = 0
            doorParticle = InstancePrefabParentedNow(e, "DoorParticle1", xSpawn, ySpawn, 0.35)
        elseif (i == 5) then
            xSpawn = 1
            ySpawn = 1
            doorParticle = InstancePrefabParentedNow(e, "DoorParticle2", xSpawn, ySpawn, 0.35)
        elseif (i ==6) then
            xSpawn = 0
            ySpawn = 2
            doorParticle = InstancePrefabParentedNow(e, "DoorParticle1", xSpawn, ySpawn, 0.35)
        elseif (i ==7) then
            xSpawn = -1
            ySpawn = 1
            doorParticle = InstancePrefabParentedNow(e, "DoorParticle2", xSpawn, ySpawn, 0.35)
        elseif (i ==8) then
            xSpawn = -2
            ySpawn = 0
            doorParticle = InstancePrefabParentedNow(e, "DoorParticle1", xSpawn, ySpawn, 0.35)
        end

        local doorParticleScale = GetTransform(doorParticle).scale
        doorParticleScale.x = doorParticleScale.x * 5
        doorParticleScale.y = doorParticleScale.y * 5
        doorParticleScale.z = doorParticleScale.z * 5
        
        -- each particle spawned will face a different direction (evenly spread across 360 degrees)
        GetTransform(doorParticle).rotation.z = 360 / emitterComponent.particleCount * i

        -- calculate the direction that the particle should move towards
        local forwardVec = DoorParticleBackwardVector(doorParticle)
        GetComponent(doorParticle, "C_VFXDoorParticle").forwardVector = forwardVec

        -- slow it down
        GetComponent(doorParticle, "C_VFXDoorParticle").Speed = 7.25
        GetComponent(doorParticle, "C_VFXDoorParticle").Lifetime = 0.20

        GetComponent(doorParticle, "C_VFXDoorParticle").moveForward = false
        
    end

    emitterComponent.particleEmitted = true

end

 -- returns a normalized vector in the direction that the entity is facing.
 -- this function is modified for this case
 function DoorParticleBackwardVector(entity)
    
    local entityTransform = GetTransform(entity)
    
    local newX = Abs(Cos(RadianToDegree(entityTransform.rotation.z)))
    local newY = Abs(Sin(RadianToDegree(entityTransform.rotation.z)))

    if (entityTransform.rotation.z > 0 and entityTransform.rotation.z < 180) then
        newY = newY * -1
    end

    if (entityTransform.rotation.z > 270 or entityTransform.rotation.z < 90) then
        newX = newX * -1
    end

    return Normalize(newX, newY, 0)
end