--[IsComponent]
function C_System3()
    local var = {
        --[SerializeFloat]
        p = 13.4
    };
    return function()
        return var
    end
end

--[IsSystem]
function S_System3(e)
    -- c1 = GetComponent(e, "C_test1")
    -- c1.x = c1.x - 2
    -- GetPosition(e)
end