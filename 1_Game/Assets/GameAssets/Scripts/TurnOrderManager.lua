-- TurnOrderManager determines the turn orders of the player and enemies based on their remaining stamina.
-- Game flow: [level start] > [characters' turns] > [random event (if any)] > [characters' turns] > ... > [level end]
-- While GlobalStatemachine.CurrentState is CharacterTurnState, this script will be responsible for switching to each 
-- character's turn, before changing GlobalStatemachine.CurrentState to RandomEventState.

-- TODO:
-- 1) Trigger game over when player dies (can't trigger from Player.lua because it will stop working once Player entity is removed)

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
        turnCycleCounter = 0,

        -- turn order UI will be refreshed at the start of each round
        refreshTurnOrderUI = false
    };
    return function()
        return var
    end
end

--[IsSystem]
function S_TurnOrderManager(e)

    -- getting TurnOrderManager entity and component ----------------------------------------------------------------
    local turnOrderManagerEntity = GetEntity("TurnOrderManager")
    local turnOrderManagerComponent
    if (turnOrderManagerEntity ~= -1) then
        turnOrderManagerComponent = GetComponent(turnOrderManagerEntity, "C_TurnOrderManager")
    else
        print("[TurnOrderManager.lua] ERROR: turnOrderManager entity is missing!")
    end
    -----------------------------------------------------------------------------------------------------------------
    
    -- getting GlobalStatemachine entity and component --------------------------------------------------------------
    local globalStateMachineEntity = GetEntity("GlobalStatemachine")
    local globalStateMachineComponent
    if (globalStateMachineEntity ~= -1) then
        globalStateMachineComponent = GetComponent(globalStateMachineEntity, "C_GlobalStatemachine")
    else
        print("[TurnOrderManager.lua] ERROR: globalStateMachineEntity entity is missing!")
    end
    -----------------------------------------------------------------------------------------------------------------

    -- only run the rest of this script if globalStateMachine allows it
    if (globalStateMachineComponent.CurrentState == turnOrderManagerComponent.TurnOrderState) then
            
        -- Debugging hotkeys and cheats -----------------------------------------------------------------------------
        -- press M to check name and ID of current active character, as well as the current round number
        if (CheckKeyPress(77)) then
            print("[TurnOrderManager.lua] Current active character:", EntityName(turnOrderManagerComponent.turnQueue[turnOrderManagerComponent.queueIndex]), turnOrderManagerComponent.currentTurn)
            print("[TurnOrderManager.lua] Currently at turn no.", turnOrderManagerComponent.turnCycleCounter, "\n")
        end
        -------------------------------------------------------------------------------------------------------------

        -- if player is dead, don't do anything because the game is over --------------------------------------------
        -- TODO: Trigger game over when player dies
        -- if (GetEntity("Player") == -1) then
        --     -- trigger any game over events here
        --     return
        -- end
        -------------------------------------------------------------------------------------------------------------
        
        -- if starting a new turn cycle, build the turn queue -------------------------------------------------------
        if (turnOrderManagerComponent.buildTurnQueue == true) then        

            print("[TurnOrderManager.lua] Start of building turnQueue")

            -- incrementing the turn counter
            turnOrderManagerComponent.turnCycleCounter = turnOrderManagerComponent.turnCycleCounter + 1
            
            -- adding all entities with C_Character script into the turnQueue and print result
            print("[TurnOrderManager.lua] Entities added into turnQueue:")
            turnOrderManagerComponent.turnQueue = EntitiesWithScriptComponent("C_Character")
            for k = 1, #turnOrderManagerComponent.turnQueue do              
                print(EntityName(turnOrderManagerComponent.turnQueue[k]), GetEntityData(turnOrderManagerComponent.turnQueue[k]).id)
            end

            -- sort all characters in turnQueue by remaining stamina (Selection Sort)------------------------------------
            -- if (#turnOrderManagerComponent.turnQueue > 1) then                           
            --     local arrayLength = #turnOrderManagerComponent.turnQueue        
            --     for i = 1, #turnOrderManagerComponent.turnQueue - 1 do                  
            --         if (i <= arrayLength) then                                         
            --             indexLargest = i
            --             for j = i + 1, #turnOrderManagerComponent.turnQueue do
            --                 if (j <= arrayLength) then
            --                     if (GetComponent(turnOrderManagerComponent.turnQueue[j], "C_Character").currentStamina > GetComponent(turnOrderManagerComponent.turnQueue[indexLargest], "C_Character").currentStamina) then
            --                         indexLargest = j
            --                     end
            --                 end
            --             end 

            --             temp = turnOrderManagerComponent.turnQueue[indexLargest]
            --             turnOrderManagerComponent.turnQueue[indexLargest] = turnOrderManagerComponent.turnQueue[i]
            --             turnOrderManagerComponent.turnQueue[i] = temp
            --         end
            --     end
            -- end 
            -- end of selection sort ------------------------------------------------------------------------------------

            -- push player to the front as much as possible (e.g. if a bunch of characters have 0 stamina, the player will go first among the 0-stamina-ers) ------------------------------------------            
            print("[TurnOrderManager.lua] Pushing player to the front.")
            
            -- getting player's index in the turnQueue
            local playerIndex = 0
            local playerEntity = GetEntity("Player")
            if (playerEntity ~= -1) then
                for n = 1, #turnOrderManagerComponent.turnQueue do
                    if (turnOrderManagerComponent.turnQueue[n] == GetEntityData(playerEntity).id) then
                        playerIndex = n
                        break;
                    end
                end
            else
                print("[TurnOrderManager.lua] Player Entity is missing!")
            end

            -- swap player to first in the turnQueue ----------------------------------------------------------------
            local temp = turnOrderManagerComponent.turnQueue[playerIndex]
            turnOrderManagerComponent.turnQueue[playerIndex] = turnOrderManagerComponent.turnQueue[1]
            turnOrderManagerComponent.turnQueue[1] = temp
            -- end of swapping --------------------------------------------------------------------------------------

            -- while characters in front of the player have the same stamina as the player, swap their queue positions
            -- while ((playerIndex - 1) > 1) do
                
            --     if (GetComponent(turnOrderManagerComponent.turnQueue[playerIndex], "C_Character").currentStamina == GetComponent(turnOrderManagerComponent.turnQueue[playerIndex - 1], "C_Character").currentStamina) then
                
            --         temp = turnOrderManagerComponent.turnQueue[playerIndex]
            --         turnOrderManagerComponent.turnQueue[playerIndex] = turnOrderManagerComponent.turnQueue[playerIndex - 1]
            --         turnOrderManagerComponent.turnQueue[playerIndex - 1] = temp

            --         playerIndex = playerIndex - 1
            --     else
            --         break
            --     end
            -- end
            -- end of pushing player to the front -----------------------------------------------------------------------------------------------------------------------------------------------------

            -- print sort queue, and also refresh characters' stamina
            print("[TurnOrderManager.lua] Sorted queue:")
            for l = 1, #turnOrderManagerComponent.turnQueue do              
                print(l, EntityName(turnOrderManagerComponent.turnQueue[l]), GetEntityData(turnOrderManagerComponent.turnQueue[l]).id)
                GetComponent(turnOrderManagerComponent.turnQueue[l], "C_Character").currentStamina = GetComponent(turnOrderManagerComponent.turnQueue[l], "C_Character").maxStamina
            end
            
            -- reset to false so it doesn't build the queue again for this round
            turnOrderManagerComponent.buildTurnQueue = false

            -- cue UIManager to refresh the turn order icons 
            turnOrderManagerComponent.refreshTurnOrderUI = true

            print("[TurnOrderManager.lua] End of building turnQueue.\n")

            -- announce the first character's turn
            print("[TurnOrderManager.lua] It is now", EntityName(turnOrderManagerComponent.turnQueue[turnOrderManagerComponent.queueIndex]), "turn.")
        end
        -- end of building turn queue -------------------------------------------------------------------------------
     
        -- cycling through character turns --------------------------------------------------------------------------
        if (turnOrderManagerComponent.turnQueue[turnOrderManagerComponent.queueIndex] ~= nil) then
                     
            -- skip dead character's turn
            if (GetComponent(turnOrderManagerComponent.turnQueue[turnOrderManagerComponent.queueIndex], "C_Character").isDead) then
                turnOrderManagerComponent.nextTurn = true
            else         
                -- allow current character do perform their turn
                turnOrderManagerComponent.currentTurn = GetEntityData(turnOrderManagerComponent.turnQueue[turnOrderManagerComponent.queueIndex]).id
            end

            -- turnOrderManagerComponent.nextTurn will be toggled to true when the current active character ends their turn
            if (turnOrderManagerComponent.nextTurn == true) then
                
                print("[TurnOrderManager.lua]", EntityName(turnOrderManagerComponent.turnQueue[turnOrderManagerComponent.queueIndex]), "has set nextTurn to true.")
                
                -- reset nextTurn to false
                turnOrderManagerComponent.nextTurn = false    
                -- increment queueIndex (next character in turnQueue will become active)
                turnOrderManagerComponent.queueIndex = turnOrderManagerComponent.queueIndex + 1

                -- activate the next character and print announcement
                if (turnOrderManagerComponent.turnQueue[turnOrderManagerComponent.queueIndex] ~= nil) then
                    print("[TurnOrderManager.lua] It is now", EntityName(turnOrderManagerComponent.turnQueue[turnOrderManagerComponent.queueIndex]), "turn.")
                    turnOrderManagerComponent.currentTurn = GetEntityData(turnOrderManagerComponent.turnQueue[turnOrderManagerComponent.queueIndex]).id
                else
                    print("[TurnOrderManager.lua] Reached end of turnQueue, setting currentTurn to 0.")
                    turnOrderManagerComponent.currentTurn = 0
                end
            end
        -- once all characters have taken their turn, reset variables and change global state to randomevents 
        else          
            
            print("[TurnOrderManager.lua] End of round", turnOrderManagerComponent.turnCycleCounter)

            -- reset variables
            turnOrderManagerComponent.buildTurnQueue = true
            turnOrderManagerComponent.currentTurn = 0
            turnOrderManagerComponent.queueIndex = 1

            -- remove all the dead characters
            print("[TurnOrderManager.lua] Removing dead characters.")
            characterList = EntitiesWithScriptComponent("C_Character")
            for m = 1, #characterList do
                if (GetComponent(characterList[m], "C_Character").isDead) then
                    print("[TurnOrderManager.lua]", EntityName(characterList[m]), GetEntityData(characterList[m]).id , "removed.")
                    RemoveInstance(characterList[m])
                end
            end

            -- hand over control to RandomEventManager
            globalStateMachineComponent.CurrentState = "StateRandomEvent"
            print("[TurnOrderManager.lua] Setting globalStatemachine to StateRandomEvent.")
        end
        -- end of cycling through character turns -------------------------------------------------------------------
    end
end