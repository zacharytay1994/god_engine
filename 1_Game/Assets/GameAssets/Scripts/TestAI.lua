--[IsComponent]
function C_TestAI()
    local var = {
        --[SerializeString]
        GlobalStatemachine = "GlobalStatemachine",
        --[SerializeString]
        ExampleStateOne = "StateOne",
        --[SerializeString]
        ExampleStateTwo = "StateTwo"
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_TestAI(e)
    -- check if global state entity exists
    local component = GetComponent(e, "C_TestAI")
    local gs_entity = GetEntity(component.GlobalStatemachine)
    if (gs_entity ~= -1) then
        local current_global_state = GetComponent(gs_entity, "C_GlobalStatemachine").CurrentState
        -- do some state-only action here
        if (current_global_state == component.ExampleStateOne) then
            -- action ...
            print("State One")
        end
        if (current_global_state == component.ExampleStateTwo) then
            print("State Two")
        end
    end
end
