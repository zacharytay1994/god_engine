--[IsComponent]
function C_GlobalStatemachine()
    local var = {
        --[SerializeString]
        CurrentState = "StateOne"
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_GlobalStatemachine(e)
end
