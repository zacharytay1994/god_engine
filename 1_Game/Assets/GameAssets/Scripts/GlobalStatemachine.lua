-- GlobalStatemachine will manage higher-level states, such as:
-- 1) StateCharacterTurn
-- 2) StateRandomEvent
-- where each of them will manage their own lower-level states such as StatePlayerMove / StateEnemyMove

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
    
    -- getting entity and component
    local globalStateMachineEntity = GetEntity("GlobalStatemachine")
    if (globalStateMachineEntity ~= -1) then 
        globalStateMachineComponent = GetComponent(globalStateMachineEntity, "C_GlobalStatemachine")
    end
    
    -- -- press 1 to change state to StateCharacterTurn
    -- if (CheckKeyPress(49) == true) then
    --     globalStateMachineComponent.CurrentState = "StateCharacterTurn"
    --     print("CurrentState = StateCharacterTurn")
    -- end

    -- -- press 2 to change state to StateRandomEvent
    -- if (CheckKeyPress(50) == true) then
    --     globalStateMachineComponent.CurrentState = "StateRandomEvent"
    --     print("CurrentState = StateRandomEvent")
    -- end

    -- press Z to print the current State
    if (CheckKeyPress(90) == true) then
        print(globalStateMachineComponent.CurrentState)
    end
end
