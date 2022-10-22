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
    
    --print("outside")
    if (GetKeyDown(77) == true) then
        print("input received")
    end

end