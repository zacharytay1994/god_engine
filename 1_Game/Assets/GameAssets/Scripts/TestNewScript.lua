--[IsComponent]
function C_TestNewScript()
    local var = {
        --[SerializeInt]
        z = 4
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_TestNewScript(e)
end
