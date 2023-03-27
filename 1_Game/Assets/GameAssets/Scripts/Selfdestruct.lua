--[IsComponent]
function C_Selfdestruct()
    local var = {     

        --[SerializeFloat]
        selfDestructTimer = 0.0,

        timer = 0.0

    };
    return function()
        return var
    end
end

--[IsSystem]
function S_Selfdestruct(e)
    
    local c = GetComponent(e, "C_Selfdestruct")

    if (c.timer < c.selfDestructTimer) then
        c.timer = c.timer + GetDeltaTime()
    else
        RemoveInstance(e)
    end

end