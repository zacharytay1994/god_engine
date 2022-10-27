-- This script allows the enemy to move, but only during the enemy's turn.
-- Currently only moves in one direction.

--[IsComponent]
function C_StateMoveEnemy()
    local var = {
        --[SerializeString]
        GlobalStatemachine = "GlobalStatemachine",
        State = "MoveEnemy",
        Time = 0.0
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_StateMoveEnemy(e)
    
    -- check if TurnOrderManager entity exists
    local turnOrderManagerEntity = GetEntity("TurnOrderManager")  
    if (turnOrderManagerEntity ~= -1) then
        
        -- do some state-only action here
        local entityDataComponent = GetEntityData(e)
        local turnOrderManagerComponent = GetComponent(turnOrderManagerEntity, "C_TurnOrderManager")
        local stateMoveEnemyComponent = GetComponent(e, "C_StateMovePlayer")
             
        if (turnOrderManagerComponent.currentTurn == entityDataComponent.id) then
            -- action ...
            
            -- press V to check if script passes currentTurn check
            if (CheckKeyPress(86)) then
                print("It is indeed the enemy's turn")
            end          
                     
            -- make the enemy move one tile, then end its turn 
            if (stateMoveEnemyComponent.Time < 1.0) then
                -- accumulate time
                stateMoveEnemyComponent.Time = stateMoveEnemyComponent.Time + GetDeltaTime()
            else
                print("\n[StateEnemyMove - START]")
                
                -- move the enemy by one tile
                local cell = GetGridCell(e);
                cell.x = cell.x + 1
                
                -- switch to next character's turn
                turnOrderManagerComponent.nextTurn = true

                print("Ending enemy's turn!")

                -- reset timer
                stateMoveEnemyComponent.Time = 0.0

                print("[StateEnemyMove - END]\n\n")
            end
        end
    end
end

-- function S_StateMoveEnemy(e)
--     -- check if global state entity exists
--     local component = GetComponent(e, "C_StateMoveEnemy")
--     local gs_entity = GetEntity(component.GlobalStatemachine)
--     if (gs_entity ~= -1) then
--         local global_statemachine = GetComponent(gs_entity, "C_GlobalStatemachine")
--         -- do some state-only action here
--         if (global_statemachine.CurrentState == component.State) then
--             -- action ...
--             if (component.Time < 1.0) then
--                 component.Time = component.Time + GetDeltaTime()
--             else
--                 local cell = GetGridCell(e);
--                 cell.x = cell.x + 1
--                 global_statemachine.CurrentState = "MovePlayer"
--                 component.Time = 0.0
--             end
--         end
--     end
-- end