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
    
    local GlobalStatemachineEntity = GetEntity("GlobalStatemachine")
    local GlobalStatemachineComponent = GetComponent(GlobalStatemachineEntity, "C_GlobalStatemachine")
    
    -- press 1 to change state to StateCharacterTurn
    if (CheckKeyDown(49) == true) then
        GlobalStatemachineComponent.CurrentState = "StateCharacterTurn"
        print("CurrentState = StateCharacterTurn")
    end

    -- press 2 to change state to StateRandomEvent
    if (CheckKeyDown(50) == true) then
        GlobalStatemachineComponent.CurrentState = "StateRandomEvent"
        print("CurrentState = StateRandomEvent")

    end
end
