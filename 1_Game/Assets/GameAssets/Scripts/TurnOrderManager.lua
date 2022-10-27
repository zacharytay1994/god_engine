-- TurnOrderManager determines the turn orders of the player and enemies based on their remaining stamina.
-- Game flow: [level start] > [characters' turns] > [random event (if any)] > [characters' turns] > ... > [level end]
-- While GlobalStatemachine.CurrentState is CharacterTurnState, this script will be responsible for switching to each 
-- character's turn, before changing GlobalStatemachine.CurrentState to RandomEventState.

-- TODO:
-- 1) Sort the turnQueue by remaining stamina (currently sorts by ID to test sorting)

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

        -- indicates which character in the queue is performing their turn (lua index starts from 1, not 0)
        queueIndex = 1,

        -- turnQueue is an array containing all entities with a C_Character component
        turnQueue = {},

        -- counts the number of turn cycles 
        turnCycleCounter = 0
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
        -- press X to check the number of turn cycles so far
        if (CheckKeyPress(88)) then
            print("Currently at turn no.", turnOrderManagerComponent.turnCycleCounter)
        end
        
        -- if starting a new turn cycle, build the turn queue 
        if (turnOrderManagerComponent.buildTurnQueue == true) then        

            print("\n[TurnOrderManager: buildTurnQueue - START]")

            -- incrementing the turn counter
            turnOrderManagerComponent.turnCycleCounter = turnOrderManagerComponent.turnCycleCounter + 1
            
            -- adding all entities with C_Character script into the turnQueue and print result
            print("Entities added into turnQueue:")
            turnOrderManagerComponent.turnQueue = EntitiesWithComponent("C_Character")
            for k = 1, #turnOrderManagerComponent.turnQueue do              
                print(GetEntityData(turnOrderManagerComponent.turnQueue[k]).id)
            end
            print() -- print line break

            -- TODO: sort all characters in turnQueue by remaining stamina (Selection Sort)//////////////
            if (#turnOrderManagerComponent.turnQueue > 1) then 
                
                local arrayLength = #turnOrderManagerComponent.turnQueue
        
                for i = 1, #turnOrderManagerComponent.turnQueue do
                    
                    if (i < arrayLength - 1) then 
                    
                        indexLargest = i

                        for j = i + 1, #turnOrderManagerComponent.turnQueue do
                            if (j < arrayLength) then
                                 if (GetEntityData(turnOrderManagerComponent.turnQueue[j]).id > GetEntityData(turnOrderManagerComponent.turnQueue[indexLargest]).id) then
                                --if (GetComponent(turnOrderManagerComponent.turnQueue[j]).currentStamina > GetComponent(turnOrderManagerComponent.turnQueue[indexLargest]).currentStamina) then
                                    indexLargest = j
                                end
                            end
                        end 

                        temp = turnOrderManagerComponent.turnQueue[indexLargest]
                        turnOrderManagerComponent.turnQueue[indexLargest] = turnOrderManagerComponent.turnQueue[i]
                        turnOrderManagerComponent.turnQueue[i] = temp

                    end
                end
            end

            print("Sorted queue:")
            for l = 1, #turnOrderManagerComponent.turnQueue do              
                print(GetEntityData(turnOrderManagerComponent.turnQueue[l]).id)
            end
            -- END OF SELECTION SORT/////////////////////////////////////////////////////////////////////


            -- reset to false so it doesn't keep running this chunk
            turnOrderManagerComponent.buildTurnQueue = false

            print("[TurnOrderManager: buildTurnQueue - END]\n\n")
        end
     
        -- while there are still characters who have not taken their turn for this cycle
        if (turnOrderManagerComponent.turnQueue[turnOrderManagerComponent.queueIndex] ~= nil) then
                     
            -- TODO: allow current character do perform their turn
            turnOrderManagerComponent.currentTurn = GetEntityData(turnOrderManagerComponent.turnQueue[turnOrderManagerComponent.queueIndex]).id

            -- turnOrderManagerComponent.nextTurn will be toggled to true when the current active character ends their turn
            if (turnOrderManagerComponent.nextTurn == true) then
                -- reset to false
                turnOrderManagerComponent.nextTurn = false    
                -- increment queueIndex (next character in turnQueue will become active)
                turnOrderManagerComponent.queueIndex = turnOrderManagerComponent.queueIndex + 1
            end
        -- once all characters have taken their turn, reset variables and change global state to randomevents 
        else          
            turnOrderManagerComponent.buildTurnQueue = true
            turnOrderManagerComponent.queueIndex = 1
            turnOrderManagerComponent.currentTurn = 0

            globalStateMachineComponent.CurrentState = "StateRandomEvent"
            print("\n[TurnOrderManager - END]")
            print("CurrentState = StateRandomEvent")
            print("[TurnOrderManager - END]\n\n")
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