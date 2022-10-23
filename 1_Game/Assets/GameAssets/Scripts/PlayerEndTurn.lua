-- This script allows the player to end their turn.
-- Upon ending turn, this script will change GlobalStatemachine's current state to 
-- the next character's turn. 

--[IsComponent]
function C_PlayerEndTurn()
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
function S_PlayerEndTurn(e)  
    -- press E to end the player's turn and get TurnOrderManager to go to next enemy's turn
    if (CheckKeyDown(69) == true) then
        
        -- debug message, can comment if unneeded
        print("Ending Player's turn!")
        
        -- getting Player's PlayerEndTurn component
        local playerEndTurnComponent = GetComponent(e, "C_PlayerEndTurn")

        -- getting TurnOrderManager object
        local turnOrderManagerEntity = GetEntity(playerEndTurnComponent.TurnOrderManager)

        if (turnOrderManagerEntity ~= -1) then           
            -- getting TurnOrderManager's TurnOrderManager component
            local turnOrderManagerComponent = GetComponent(turnOrderManagerEntity, "C_TurnOrderManager")

            -- signal turnOrderManager to move on to the next character's turn
            turnOrderManagerComponent.nextTurn = true
        end
    end
end

--[[ PSEUDOCODE

if player ends turn (either by pressing E or pressing an EndTurn button)
    set TurnOrderManager.nextTurn = true;

--]]