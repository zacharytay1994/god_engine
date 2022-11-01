-- This script allows the enemy to end their turn.
-- Upon ending turn, this script will change GlobalStatemachine's current state to 
-- the next character's turn. 

-- TODO:
-- 1) Get enemy to its turn automatically after it has performed its turn.
--    Currently, the enemy's ends automatically after it reaches a tile adjacent to the player.
--    Will be implemented in a separate script.

--[IsComponent]
function C_EnemyEndTurn()
    local var = {
        -- --[SerializeString]
        TurnOrderManager = "TurnOrderManager",
        --MovementState = "MovePlayer",
        -- Time = 0.0
    };
    return function()
        return var
    end
end

--[IsSystem]
function S_EnemyEndTurn(e)  

    -- press R to end the Enemy's turn and get TurnOrderManager to go to next character's turn
    if (CheckKeyPress(82) == true) then

        -- getting this enemy's ID number
        local enemyID = GetEntityData(e).id

        -- getting EnemyEndTurn component
        local enemyEndTurnComponent = GetComponent(e, "C_EnemyEndTurn")

        -- getting TurnOrderManager object
        local turnOrderManagerEntity = GetEntity(enemyEndTurnComponent.TurnOrderManager)

        -- checking whether turnOrderManager is nil
        if (turnOrderManagerEntity ~= -1) then
        
            -- getting TurnOrderManager component
            local turnOrderManagerComponent = GetComponent(turnOrderManagerEntity, "C_TurnOrderManager") 
            
            -- only end the turn if it is actually the player's turn
            if (turnOrderManagerComponent.currentTurn == enemyID) then
                
                -- debug message, can comment if unneeded
                print("\n[EnemyEndTurn - START]\nEnding enemy's turn!")
                
                -- signal turnOrderManager to move on to the next character's turn
                turnOrderManagerComponent.nextTurn = true

                print("[EnemyEndTurn - END]\n\n")
            end
        end
    end
end