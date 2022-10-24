-- This script allows the player to end their turn.
-- Upon ending turn, this script will inform TurnOrderManager to switch to 
-- the next character's turn. 

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
        local turnOrderManagerComponent = GetComponent(turnOrderManagerEntity, "C_TurnOrderManager")        
        
        -- only end the turn if it is actually the player's turn
        if (turnOrderManagerComponent.currentTurn == playerID) then
            
            -- debug message, can comment if unneeded
            print("Ending Player's turn!")
            
            -- signal turnOrderManager to move on to the next character's turn
            turnOrderManagerComponent.nextTurn = true
        end
    end
end

--[[ PSEUDOCODE

if player ends turn (either by pressing E or pressing an EndTurn button)
    set TurnOrderManager.nextTurn = true;

--]]