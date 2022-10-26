

-- TODO:
-- 1) Set RandomEventManager.eventHappening = false at the end of the earthquake

--[IsComponent]
function C_RandomEventEarthquake()
    local var = {
        -- --[SerializeString]
        -- TurnOrderManager = "TurnOrderManager",

        -- -- name of the TurnOrder state, to be checked against GlobalStatemachine's CurrentState
        -- StateRandomEvent = "StateRandomEvent",

        -- -- probability of a randomevent occuring 
        -- randomEventRate = 0.25,
        
        -- -- list of possible randomevents
        -- randomEventList = { "eventEarthquake", "eventTsunami" }
    };
    return function()
        return var
    end
end

--[IsSystem]
function S_RandomEventEarthquake(e)
    
    -- -- getting TurnOrderManager entity and component
    -- local stateRandomEventEntity = GetEntity("TurnOrderManager")
    -- local turnOrderManagerComponent = GetComponent(turnOrderManagerEntity, "C_TurnOrderManager")
    
    -- -- for checking if nextTurn is true or false, by pressing K
    -- if (CheckKeyPress(75)) then
    --     if (turnOrderManagerComponent.nextTurn == true) then
    --         print("nextTurn is true")  
    --     else
    --         print("nextTurn is false")  
    --     end
    -- end
    
    -- -- getting GlobalStatemachine entity and component
    -- local globalStateMachineEntity = GetEntity("GlobalStatemachine")
    -- local globalStateMachineComponent = GetComponent(globalStateMachineEntity, "C_GlobalStatemachine")

    -- -- only run the rest of this script if globalStateMachine allows it
    -- if (globalStateMachineComponent.CurrentState == turnOrderManagerComponent.TurnOrderState) then
            
    --     -- press M to check whether the script passes the globalStateMachineComponent.CurrentState check
    --     if (CheckKeyPress(77)) then
    --         print("TurnOrderManager script entered main chunk")
    --     end
    --     -- press N to check whether buildTurnQueue is true or false
    --     if (CheckKeyPress(78)) then
    --         print("buildTurnQueue is ", turnOrderManagerComponent.buildTurnQueue)
    --     end
    --     -- press B to check ID of current active character
    --     if (CheckKeyPress(66)) then
    --         print("current active character is ", turnOrderManagerComponent.currentTurn)
    --     end
        
    --     -- if starting a new turn cycle, build the turn queue 
    --     if (turnOrderManagerComponent.buildTurnQueue == true) then        

    --         -- adding all entities with C_Character script into the turnQueue and print result
    --         print("Entities added into turnQueue:")
    --         turnOrderManagerComponent.turnQueue = EntitiesWithComponent("C_Character")
    --         for k = 1, #turnOrderManagerComponent.turnQueue do              
    --             print(GetEntityData(turnOrderManagerComponent.turnQueue[k]).id)
    --         end
    --         print() -- print line break

    --         -- TODO: sort all characters in turnQueue by remaining stamina

    --         -- reset to false so it doesn't keep running this chunk
    --         turnOrderManagerComponent.buildTurnQueue = false

    --         print("turnQueue has been built, setting buildTurnQueue to false!")
    --     end
     
    --     -- while there are still characters who have not taken their turn for this cycle
    --     if (turnOrderManagerComponent.turnQueue[turnOrderManagerComponent.queueIndex] ~= nil) then
                     
    --         -- TODO: allow current character do perform their turn
    --         turnOrderManagerComponent.currentTurn = GetEntityData(turnOrderManagerComponent.turnQueue[turnOrderManagerComponent.queueIndex]).id

    --         -- turnOrderManagerComponent.nextTurn will be toggled to true when the current active character ends their turn
    --         if (turnOrderManagerComponent.nextTurn == true) then
    --             -- reset to false
    --             turnOrderManagerComponent.nextTurn = false    
    --             -- increment queueIndex (next character in turnQueue will become active)
    --             turnOrderManagerComponent.queueIndex = turnOrderManagerComponent.queueIndex + 1
    --         end
    --     -- once all characters have taken their turn, reset variables and change global state to randomevents 
    --     else          
    --         turnOrderManagerComponent.buildTurnQueue = true
    --         turnOrderManagerComponent.queueIndex = 1
    --         globalStateMachineComponent.CurrentState = "RandomEventState"
    --     end
    -- end
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