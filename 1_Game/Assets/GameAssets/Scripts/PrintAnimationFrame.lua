--[IsComponent]
function C_PrintAnimationFrame()
    local var = {
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_PrintAnimationFrame(e)
    print(CurrentAnimationFrame(e))
end
