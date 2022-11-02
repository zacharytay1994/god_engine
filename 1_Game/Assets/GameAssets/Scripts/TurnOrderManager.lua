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
    
    -- press L to remove instance of TestSubject
    if (CheckKeyPress(76) == true) then
        RemoveInstance(GetEntity("TestSubject"))
    end

    -- getting TurnOrderManager entity and component
    local turnOrderManagerEntity = GetEntity("TurnOrderManager")
    local turnOrderManagerComponent = GetComponent(turnOrderManagerEntity, "C_TurnOrderManager")
    
    -- getting GlobalStatemachine entity and component
    local globalStateMachineEntity = GetEntity("GlobalStatemachine")
    local globalStateMachineComponent = GetComponent(globalStateMachineEntity, "C_GlobalStatemachine")

    -- only run the rest of this script if globalStateMachine allows it
    if (globalStateMachineComponent.CurrentState == turnOrderManagerComponent.TurnOrderState) then
            
        -- press M to check ID of current active character
        if (CheckKeyPress(77)) then
            print("current active character:", EntityName(turnOrderManagerComponent.turnQueue[turnOrderManagerComponent.queueIndex]), "    ID no.", turnOrderManagerComponent.currentTurn)
        end
        -- press X to check the number of turn cycles so far
        if (CheckKeyPress(78)) then
            print("Currently at turn no.", turnOrderManagerComponent.turnCycleCounter)
        end

        -- if player is dead, don't do anything because the game is over
        if (GetEntity("Player") == -1) then
            
            -- trigger any game over events here

            return
        end
        
        -- if starting a new turn cycle, build the turn queue 
        if (turnOrderManagerComponent.buildTurnQueue == true) then        

            print("[TurnOrderManager.lua] Start of building turnQueue")

            -- incrementing the turn counter
            turnOrderManagerComponent.turnCycleCounter = turnOrderManagerComponent.turnCycleCounter + 1

            -- reset nextTurn
            -- print("Resetting nextTurn!!!")
            turnOrderManagerComponent.nextTurn = false
            
            -- adding all entities with C_Character script into the turnQueue and print result
            print("[TurnOrderManager.lua] Entities added into turnQueue:")
            turnOrderManagerComponent.turnQueue = EntitiesWithScriptComponent("C_Character")
            for k = 1, #turnOrderManagerComponent.turnQueue do              
                print(GetEntityData(turnOrderManagerComponent.turnQueue[k]).id)
            end
            print() -- print line break

            -- sort all characters in turnQueue by remaining stamina (Selection Sort)//////////////
            if (#turnOrderManagerComponent.turnQueue > 1) then 
                                
                local arrayLength = #turnOrderManagerComponent.turnQueue
        
                for i = 1, #turnOrderManagerComponent.turnQueue - 1 do
                    
                    if (i <= arrayLength) then 
                                            
                        indexLargest = i

                        for j = i + 1, #turnOrderManagerComponent.turnQueue do
                            if (j <= arrayLength) then
                                if (GetComponent(turnOrderManagerComponent.turnQueue[j], "C_Character").currentStamina > GetComponent(turnOrderManagerComponent.turnQueue[indexLargest], "C_Character").currentStamina) then
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

            -- push player to the front as much as possible (e.g. if a bunch of characters have 0 stamina, the player will go first among the 0-stamina-ers) ------------------------------------------            
            print("[TurnOrderManager.lua] Pushing player to the front.")
            local playerIndex = 0
            for n = 1, #turnOrderManagerComponent.turnQueue do
                if (turnOrderManagerComponent.turnQueue[n] == GetEntityData(GetEntity("Player")).id) then
                    playerIndex = n
                    break;
                end
            end

            while ((playerIndex - 1) > 1) do
                
                if (GetComponent(turnOrderManagerComponent.turnQueue[playerIndex], "C_Character").currentStamina == GetComponent(turnOrderManagerComponent.turnQueue[playerIndex - 1], "C_Character").currentStamina) then
                
                    temp = turnOrderManagerComponent.turnQueue[playerIndex]
                    turnOrderManagerComponent.turnQueue[playerIndex] = turnOrderManagerComponent.turnQueue[playerIndex - 1]
                    turnOrderManagerComponent.turnQueue[playerIndex - 1] = temp

                    playerIndex = playerIndex - 1
                else
                    break
                end
            end
            -- end of pushing player to the front -----------------------------------------------------------------------------------------------------------------------------------------------------

            -- print sort queue, and also refresh characters' stamina
            print("[TurnOrderManager.lua] Sorted queue:")
            for l = 1, #turnOrderManagerComponent.turnQueue do              
                print(l, EntityName(turnOrderManagerComponent.turnQueue[l]), GetEntityData(turnOrderManagerComponent.turnQueue[l]).id)
                GetComponent(turnOrderManagerComponent.turnQueue[l], "C_Character").currentStamina = GetComponent(turnOrderManagerComponent.turnQueue[l], "C_Character").maxStamina
            end
            -- END OF SELECTION SORT/////////////////////////////////////////////////////////////////////

            -- reset to false so it doesn't keep running this chunk
            turnOrderManagerComponent.buildTurnQueue = false

            print("[TurnOrderManager.lua] End of building turnQueue.\n")
        end
     
        -- while there are still characters who have not taken their turn for this cycle
        if (turnOrderManagerComponent.turnQueue[turnOrderManagerComponent.queueIndex] ~= nil) then
                     
            -- skip dead character's turn
            if (GetComponent(turnOrderManagerComponent.turnQueue[turnOrderManagerComponent.queueIndex], "C_Character").isDead) then
                turnOrderManagerComponent.nextTurn = true
            end
            
            -- allow current character do perform their turn
            turnOrderManagerComponent.currentTurn = GetEntityData(turnOrderManagerComponent.turnQueue[turnOrderManagerComponent.queueIndex]).id

            -- turnOrderManagerComponent.nextTurn will be toggled to true when the current active character ends their turn
            if (turnOrderManagerComponent.nextTurn == true) then
                
                print("[TurnOrderManager.lua]", EntityName(turnOrderManagerComponent.turnQueue[turnOrderManagerComponent.queueIndex]), "has set nextTurn to true.")
                
                -- reset to false
                turnOrderManagerComponent.nextTurn = false    
                -- increment queueIndex (next character in turnQueue will become active)
                turnOrderManagerComponent.queueIndex = turnOrderManagerComponent.queueIndex + 1

                if (turnOrderManagerComponent.turnQueue[turnOrderManagerComponent.queueIndex] ~= nil) then
                    print("[TurnOrderManager.lua] It is now", EntityName(turnOrderManagerComponent.turnQueue[turnOrderManagerComponent.queueIndex]), "turn.")
                    turnOrderManagerComponent.currentTurn = GetEntityData(turnOrderManagerComponent.turnQueue[turnOrderManagerComponent.queueIndex]).id
                end

            end
        -- once all characters have taken their turn, reset variables and change global state to randomevents 
        else          
            turnOrderManagerComponent.buildTurnQueue = true
            turnOrderManagerComponent.queueIndex = 1
            turnOrderManagerComponent.currentTurn = 0

            -- remove all the dead characters
            print("[TurnOrderManager.lua] Removing dead characters.")
            characterList = EntitiesWithScriptComponent("C_Character")
            for m = 1, #characterList do
                if (GetComponent(characterList[m], "C_Character").isDead) then
                    print("[TurnOrderManager.lua]", EntityName(characterList[m]), GetEntityData(characterList[m]).id , "removed.")
                    RemoveInstance(characterList[m])
                end
            end

            globalStateMachineComponent.CurrentState = "StateRandomEvent"
            print("[TurnOrderManager.lua] Setting globalStatemachine to StateRandomEvent.")
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