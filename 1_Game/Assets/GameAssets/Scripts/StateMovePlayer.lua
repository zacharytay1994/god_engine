--[IsComponent]
function C_StateMovePlayer()
    local var = {
        --[SerializeString]
        GlobalStatemachine = "GlobalStatemachine",
        MovementState = "MovePlayer",
        Time = 0.0
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_StateMovePlayer(e)
    -- check if global state entity exists
    local component = GetComponent(e, "C_StateMovePlayer")
    local gs_entity = GetEntity(component.GlobalStatemachine)
    if (gs_entity ~= -1) then
        local global_statemachine = GetComponent(gs_entity, "C_GlobalStatemachine")
        -- do some state-only action here
        if (global_statemachine.CurrentState == component.MovementState) then
            -- action ...
            if (component.Time < 1.0) then
                component.Time = component.Time + GetDeltaTime()
            else
                local cell = GetGridCell(e);
                cell.x = cell.x + 1
                global_statemachine.CurrentState = "MoveEnemy"
                component.Time = 0.0
            end
        end
    end
end
