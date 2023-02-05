-- This script checks Triton's animation frame to display VFX at the right time.

--[IsComponent]
function C_VFXTriton()
    local var = {
        
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_VFXTriton(e)

    print(CurrentAnimationFrame(e))

end