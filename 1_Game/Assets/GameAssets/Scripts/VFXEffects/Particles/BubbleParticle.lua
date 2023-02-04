-- Bubbles are killed after travelling a certain distance. 
-- This is to ensure that they disappear only when they are off-screen.

-- TODO: add some variation to the bubble movement

--[IsComponent]
function C_BubbleParticle()
    local var = {
        
        -- total distance the bubble is allowed to travel
        Lifetime = 150.0,

        -- unused
        --Timer = 0.0,
        
        --[SerializeFloat]
        Speed = 3.0,
        
        -- y-coordinate of bubble's spawn position
        initialHeight = 0.0,
        
        isAlive = true
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_BubbleParticle(e)
    
    local bubbleTransform = GetTransform(e)
    local bubbleParticleComponent = GetComponent(e, "C_BubbleParticle")
    local dt = GetDeltaTime()
    
    -- if (bubbleParticleComponent.Timer > bubbleParticleComponent.Lifetime) 
    -- then
    --     isAlive = false
    --     RemoveInstance(e)        
    -- else
    --     bubble_particle.Timer = bubble_particle.Timer + dt
    --     -- move the particle up
    --     transform.position.y = transform.position.y + dt * bubble_particle.Speed
    -- end

    if (bubbleTransform.position.y > bubbleParticleComponent.Lifetime + bubbleParticleComponent.initialHeight) 
    then
        bubbleParticleComponent.isAlive = false
        RemoveInstance(e)        
    else
        -- move the particle up
        bubbleTransform.position.y = bubbleTransform.position.y + dt * bubbleParticleComponent.Speed
    end
end
