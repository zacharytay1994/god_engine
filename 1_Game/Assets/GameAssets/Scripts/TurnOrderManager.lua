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

        -- currentTurn will be set to the ID number of whichever character's turn it currently is
        currentTurn = 0,

        -- indicates which character in the queue is performing their turn
        queueIndex = 1,

        -- turnQueue is an array containing the Enitity IDs of all entities with a C_Character
        turnQueue = {}
    };
    return function()
        return var
    end
end

--[IsSystem]
function S_TurnOrderManager(e)
    
    -- getting TurnOrderManager entity and component
    local turnOrderManagerEntity = GetEntity("TurnOrderManager")
    local turnOrderManagerComponent = GetComponent(turnOrderManagerEntity, "C_TurnOrderManager")
    
    -- for checking if nextTurn is true or false, by pressing K
    if (CheckKeyPress(75)) then
        if (turnOrderManagerComponent.nextTurn == true) then
            print("nextTurn is true")  
        else
            print("nextTurn is false")  
        end
    end
    
    -- getting GlobalStatemachine entity and component
    local globalStateMachineEntity = GetEntity("GlobalStatemachine")
    local globalStateMachineComponent = GetComponent(globalStateMachineEntity, "C_GlobalStatemachine")

    -- only run the rest of this script if globalStateMachine allows it
    if (globalStateMachineComponent.CurrentState == turnOrderManagerComponent.TurnOrderState) then
            
        -- press M to check whether the script passes the globalStateMachineComponent.CurrentState check
        if (CheckKeyPress(77)) then
            print("TurnOrderManager script entered main chunk")
        end
        -- press N to check whether buildTurnQueue is true or false
        if (CheckKeyPress(78)) then
            print("buildTurnQueue is ", turnOrderManagerComponent.buildTurnQueue)
        end
        -- press B to check ID of current active character
        if (CheckKeyPress(66)) then
            print("current active character is ", turnOrderManagerComponent.currentTurn)
        end
        
        -- if starting a new turn cycle, build the turn queue 
        if (turnOrderManagerComponent.buildTurnQueue == true) then        

            -- TODO: add all remaining characters into the turnQueue (currently hard-coded)
            local playerID = GetEntityData(GetEntity("Player")).id
            local enemyID = GetEntityData(GetEntity("Enemy")).id

            -- for some reason, tables don't work
            -- table.insert(turnOrderManagerComponent.turnQueue, playerID)
            -- table.insert(turnOrderManagerComponent.turnQueue, enemyID)
            turnOrderManagerComponent.turnQueue[1] = playerID
            turnOrderManagerComponent.turnQueue[2] = enemyID

            -- TODO: sort all characters in turnQueue by remaining stamina

            -- printing the turnQueue (currently hard-coded)
            -- print(turnOrderManagerComponent.turnQueue[1])
            -- print(turnOrderManagerComponent.turnQueue[2])

            -- reset to false so it doesn't keep running this chunk
            -- print("setting buildTurnQueue to false!")
            turnOrderManagerComponent.buildTurnQueue = false
        end
     
        if (turnOrderManagerComponent.turnQueue[turnOrderManagerComponent.queueIndex] ~= nil) then
            
            -- TODO: allow current character do perform their turn
            turnOrderManagerComponent.currentTurn = turnOrderManagerComponent.turnQueue[turnOrderManagerComponent.queueIndex]

            if (turnOrderManagerComponent.nextTurn == true) then
                turnOrderManagerComponent.nextTurn = false    
                turnOrderManagerComponent.queueIndex = turnOrderManagerComponent.queueIndex + 1
            end
        else
            turnOrderManagerComponent.buildTurnQueue = true
            turnOrderManagerComponent.queueIndex = 1
            globalStateMachineComponent.CurrentState = "RandomEventState"
        end
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