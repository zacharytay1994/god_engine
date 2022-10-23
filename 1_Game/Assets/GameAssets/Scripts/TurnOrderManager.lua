-- TurnOrderManager determines the turn orders of the player and enemies based on their remaining stamina.
-- Game flow: [level start] > [characters' turns] > [random event (if any)] > [characters' turns] > ... > [level end]
-- While GlobalStatemachine.CurrentState is CharacterTurnState, this script will be responsible for switching to each 
-- character's turn, before changing GlobalStatemachine.CurrentState to RandomEventState.

--[IsComponent]
function C_TurnOrderManager()
    local var = {
        --[SerializeString]
        TurnOrderManager = "TurnOrderManager",

        -- name of the TurnOrder state, to be checked against GlobalStatemachine's CurrentState
        TurnOrderState = "StateCharacterTurn",
        
        -- if set to true, will give control to the next character in the turnQueue
        nextTurn = false,

        -- if set to true, will add all currently alive characters into the turnQueue
        buildTurnQueue = true,

        currentTurn = 10,

        -- turnQueue is an array containing the Enitity IDs of all entities with a C_Character
        turnQueue = {}
    };
    return function()
        return var
    end
end

--[IsSystem]
function S_TurnOrderManager(e)
    
    local turnOrderManagerEntity = GetEntity("TurnOrderManager")
    local turnOrderManagerComponent = GetComponent(turnOrderManagerEntity, "C_TurnOrderManager")
    -- if (turnOrderManagerComponent.nextTurn == true) then
    --     print("nextTurn has been set to true")  
    -- else
    --     print("nextTurn is false")  
    -- end
    
    local gs_entity = GetEntity("GlobalStatemachine")
    local global_statemachine = GetComponent(gs_entity, "C_GlobalStatemachine")
    if (global_statemachine.CurrentState == TurnOrderState) then
    
        -- if starting a new turn cycle, build the 
        if (buildTurnQueue == true) then 
            -- TODO: add all remaining characters into the turnQueue

            -- hardcoded way of building the turnQueue. 44 is Player's ID, 100 is Enemy's ID
            local component = GetComponent(e, "C_TurnOrderManager")
            component.turnQueue = { 44, 100 }

            print(component.turnQueue)

            -- TODO: sort according to stamina

            -- reset to false so it doesn't keep running this chunk
            buildTurnQueue = false
        end
        
        local i = 1
        local component = GetComponent(e, "C_TurnOrderManager")
        while (component.turnQueue[i] ~= nil) do
            
            -- TODO: allow current character do perform their turn
            currentTurn = component.turnQueue[i]

            if (nextTurn == true) then
                nextTurn = false    
                i = i + 1
            end
        end

        buildTurnQueue = true

        local gs_entity = GetEntity("GlobalStatemachine")
        local global_statemachine = GetComponent(gs_entity, "C_GlobalStatemachine")
        global_statemachine.CurrentState = "RandomEventState"

    end


end




--[[ PSEUDOCODE

if starting a new turn cycle
    init the turnQueue array
    buildTurnQueue = false

if (GlobalStatemachine.CurrentState = CharacterTurn)
    while not every character has performed their turn
        perform turn
        if (nextTurn)
            nextTurn = false
            go to next character

buildTurnQueue = true
GlobalStatemachine.CurrentState = RandomEventState

--]]