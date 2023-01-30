-- Bubbles particle system. Emits bubbles of various sizes.

--[IsComponent]
function C_BubbleParticleSystem()
    local var = {
        --[SerializeFloat]
        largeEmissionRate = 7.0,
        --[SerializeFloat]
        mediumEmissionRate = 5.0,
        --[SerializeFloat]
        smallEmissionRate = 3.0,
        
        largeTimer = 0.0,
        mediumTimer = 0.0,
        smallTimer = 0.0
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_BubbleParticleSystem(e)
    local dt = GetDeltaTime()
    local bubbleEmitterComponent = GetComponent(e, "C_BubbleParticleSystem")

    GenerateSmallBubbles(e, dt, bubbleEmitterComponent)
    GenerateMediumBubbles(e, dt, bubbleEmitterComponent)
    GenerateLargeBubbles(e, dt, bubbleEmitterComponent)
end

function GenerateSmallBubbles(bubbleEmitterEntity, deltaTime, bubbleEmitterComponent)

    if (bubbleEmitterComponent.smallTimer < bubbleEmitterComponent.smallEmissionRate) then
        bubbleEmitterComponent.smallTimer = bubbleEmitterComponent.smallTimer + deltaTime
        return
    end

    -- print("GenerateSmallBubbles() called!")
    
    bubbleEmitterComponent.smallTimer = 0.0
    
    -- generate bubble at random location (near the emitter)
    local x = GenerateRandomNumberInRange(-20, 20)
    local y = 0
    local z = GenerateRandomNumberInRange(-20, 20)
    local bubble_particle = InstancePrefabParentedNow(bubbleEmitterEntity, "BubbleParticle", x, y, z)
    
    -- make small bubbles move faster
    GetComponent(bubble_particle, "C_BubbleParticle").Speed = 6.0

    -- make small bubbles look small
    GetTransform(bubble_particle).scale.x = 0.1
    GetTransform(bubble_particle).scale.y = 0.1
    GetTransform(bubble_particle).scale.z = 0.1
end

function GenerateMediumBubbles(bubbleEmitterEntity, deltaTime, bubbleEmitterComponent)

    if (bubbleEmitterComponent.mediumTimer < bubbleEmitterComponent.mediumEmissionRate) then
        bubbleEmitterComponent.mediumTimer = bubbleEmitterComponent.mediumTimer + deltaTime
        return
    end

    bubbleEmitterComponent.mediumTimer = 0.0
    
    print("GenerateMediumBubbles() called!")
    
    -- generate bubble at random location (near the emitter)
    local x = GenerateRandomNumberInRange(-20, 20)
    local y = 0
    local z = GenerateRandomNumberInRange(-20, 20)
    local bubble_particle = InstancePrefabParentedNow(bubbleEmitterEntity, "BubbleParticle", x, y, z)

    -- let medium bubbles move at default speed (which is 3.0)
    -- and leave the scale at 0.2
end

function GenerateLargeBubbles(bubbleEmitterEntity, deltaTime, bubbleEmitterComponent)

    if (bubbleEmitterComponent.largeTimer < bubbleEmitterComponent.largeEmissionRate) then
        bubbleEmitterComponent.largeTimer = bubbleEmitterComponent.largeTimer + deltaTime
        return
    end

    bubbleEmitterComponent.largeTimer = 0.0
    
    print("GenerateLargeBubbles() called!")
    
    -- generate bubble at random location (near the emitter)
    local x = GenerateRandomNumberInRange(-20, 20)
    local y = 0
    local z = GenerateRandomNumberInRange(-20, 20)
    local bubble_particle = InstancePrefabParentedNow(bubbleEmitterEntity, "BubbleParticle", x, y, z)
    
    -- make large bubbles move slower
    GetComponent(bubble_particle, "C_BubbleParticle").Speed = 2.5

    -- make large bubbles look large
    GetTransform(bubble_particle).scale.x = 0.5
    GetTransform(bubble_particle).scale.y = 0.5
    GetTransform(bubble_particle).scale.z = 0.5
end