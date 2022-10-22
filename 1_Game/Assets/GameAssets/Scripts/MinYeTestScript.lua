--[IsComponent]
function C_MinyeTestFunction()
    local var = {
        --[SerializeFloat]
        p = 13.4
    };
    return function()
        return var
    end
end

--[IsSystem]
function S_MinyeTestFunction(e)
    
    if (Window.Keydown(GLFW_KEY_LEFT_CONTROL)) then
        print("min")
    end
end