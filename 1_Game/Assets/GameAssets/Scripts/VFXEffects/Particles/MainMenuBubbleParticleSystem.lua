-- Bubbles particle system. Emits bubbles of various sizes in a 15 x 15 square centered around the emitter.

--[IsComponent]
function C_MainMenuBubbleParticleSystem()
    local var = {
        --[SerializeFloat]
        emissionRate = 0.0,

        timer = 0.0
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_MainMenuBubbleParticleSystem(e)

    local dt = GetDeltaTime()
    local bubbleEmitterComponent = GetComponent(e, "C_MainMenuBubbleParticleSystem")


    if (bubbleEmitterComponent.timer < bubbleEmitterComponent.emissionRate) then
        bubbleEmitterComponent.timer = bubbleEmitterComponent.timer + dt
        return
    end

    bubbleEmitterComponent.timer = 0.0
    
    -- generate bubble at x-coordinate
    local x = GenerateRandomNumberInRange(-100, 100) / 100
    local y = -1
    local z = 1
    
    local bubble_particle = InstancePrefabParentedNow(e, "Bubble", x, y, z)

    -- control bubble particle speed here
    local randomizeSpeed = GenerateRandomNumberInRange(0, 1)
    if (randomizeSpeed == 1) then
        GetComponent(bubble_particle, "C_BubbleParticle").Speed = 0.3
    else
        GetComponent(bubble_particle, "C_BubbleParticle").Speed = 0.1
    end

    -- control bubble particle scale here
    local csalf = GenerateRandomNumberInRange(5, 15) / 300
    GetTransform(bubble_particle).scale.x = csalf * 0.5
    GetTransform(bubble_particle).scale.y = csalf 
    GetTransform(bubble_particle).scale.z = 0.1

    GetComponent(bubble_particle, "C_BubbleParticle").Lifetime = 2.0
end