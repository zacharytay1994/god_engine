-- This script animates the pop up rock effect with Triton's trident smashes down into the enemy.
-- It also triggers the RockParticle effect.
  -- Min Ye

--[IsComponent]
function C_VFXRock()
    local var = {
        
        -- number of particles to spawn
        particleCount = 8,

        -- to tell emitter to emit once, then wait until all children
        -- are dead before removing itself
        particleEmitted = false,

        -- -- this is for checking whether the caller is Triton or Dummish
        -- -- for some reason Triton is huge af, so need to scale up some properties
        -- parentObject = nil
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_VFXRock(e)

    

    

end

