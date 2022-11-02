-- This script allows the player to move, but only during the Player's turn.

-- TODO:
-- 1) Allow player to move along y-axis
-- 2) 

--[IsComponent]
function C_StateMovePlayer()
    local var = {
        --[SerializeString]
        GlobalStatemachine = "GlobalStatemachine",
        State = "MovePlayer",
        Time = 0.0
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_StateMovePlayer(e)

    -- check if TurnOrderManager entity exists
    local turnOrderManagerEntity = GetEntity("TurnOrderManager")
    if (turnOrderManagerEntity ~= -1) then

        -- do some state-only action here
        local entityDataComponent = GetEntityData(e)
        local turnOrderManagerComponent = GetComponent(turnOrderManagerEntity, "C_TurnOrderManager")
        local stateMovePlayerComponent = GetComponent(e, "C_StateMovePlayer")
        
        if (turnOrderManagerComponent.currentTurn == entityDataComponent.id) then
            -- action ...

            -- do not allow movement if no more stamina
            if (GetComponent(e, "C_Character").currentStamina <= 0) then
                return
            end

            -- allow movement after 1 second has passed
            if (stateMovePlayerComponent.Time < 1.0) then
                -- accumulate time
                stateMovePlayerComponent.Time = stateMovePlayerComponent.Time + GetDeltaTime()
            else
                local pathfind = GetComponent(e, "C_Pathfind")
                
                local gm_entity = GetEntity("GridManipulate")
                if (gm_entity ~= -1) then
                    local gm = GetGridManipulate(gm_entity)
                    if (gm.clicked) then
                        pathfind.x = gm.last_clicked_cell.x
                        pathfind.y = gm.last_clicked_cell.y
                        pathfind.z = gm.last_clicked_cell.z
                        pathfind.Path = true
                    end
                end
            end
        end
    end
end

-- function S_StateMovePlayer(e)
--     -- check if global state entity exists
--     local component = GetComponent(e, "C_StateMovePlayer")
--     local gs_entity = GetEntity(component.GlobalStatemachine)
--     if (gs_entity ~= -1) then
--         local global_statemachine = GetComponent(gs_entity, "C_GlobalStatemachine")
--         -- do some state-only action here
--         if (global_statemachine.CurrentState == component.MovementState) then
--             -- action ...
--             if (component.Time < 1.0) then
--                 component.Time = component.Time + GetDeltaTime()
--             else
--                 local pathfind = GetComponent(e, "C_Pathfind")
                
--                 local gm_entity = GetEntity("GridManipulate")
--                 if (gm_entity ~= -1) then
--                     local gm = GetGridManipulate(gm_entity)
--                     if (gm.clicked) then
--                         print("hi")
--                         pathfind.x = gm.last_clicked_cell.x
--                         pathfind.y = gm.last_clicked_cell.y
--                         pathfind.z = gm.last_clicked_cell.z
--                         pathfind.Path = true
--                         print("move")
--                     end
--                 end
--             end
--         end
--     end
-- end
