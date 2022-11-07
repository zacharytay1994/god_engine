-- DEPRECATED SCRIPT --

-- This script allows the player to end their turn.
-- Upon ending turn, this script will inform TurnOrderManager to switch to 
-- the next character's turn. 

-- TODO:
-- 1) 

--[IsComponent]
function C_PlayerEndTurn()
    local var = {
        --[SerializeString]
        TurnOrderManager = "TurnOrderManager",
    };
    return function()
        return var
    end
end

--[IsSystem]
function S_PlayerEndTurn(e)  

    -- press E to end the player's turn and get TurnOrderManager to go to next character's turn
    if (CheckKeyPress(69) == true) then

        -- getting Player's ID number
        local playerID = GetEntityData(e).id

        -- getting Player's PlayerEndTurn component
        local playerEndTurnComponent = GetComponent(e, "C_PlayerEndTurn")

        -- getting TurnOrderManager object and component
        local turnOrderManagerEntity = GetEntity(playerEndTurnComponent.TurnOrderManager)
              
        -- checking whether turnOrderManager is nil
        if (turnOrderManagerEntity ~= -1) then

            -- getting TurnOrderManager component
            local turnOrderManagerComponent = GetComponent(turnOrderManagerEntity, "C_TurnOrderManager")  
        
            -- only end the turn if it is actually the player's turn
            if (turnOrderManagerComponent.currentTurn == playerID) then
                
                -- debug message, can comment if unneeded
                print("\n[PlayerEndTurn - START]\nEnding Player's turn!\n[PlayerEndTurn - END]\n\n")
                
                -- signal turnOrderManager to move on to the next character's turn
                turnOrderManagerComponent.nextTurn = true
            end
        end
    end
end