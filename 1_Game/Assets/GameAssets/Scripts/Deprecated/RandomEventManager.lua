-- Each time GlobalStatemachine switches to StateRandomEvent, there will be a 25% chance of a random event happening.
-- RandomEventManager will then choose a random randomevent from a list and run its script.
-- When a randomevent is set to occur, RandomEventManager will not proc another randomevent.
-- Game flow: [level start] > [characters' turns] > [random event (if any)] > [characters' turns] > ... > [level end]

-- While GlobalStatemachine.CurrentState is StateRandomEvent, this script will be responsible for randomising a randomevent,
-- before changing GlobalStatemachine.CurrentState to CharacterTurnState.

-- TODO:
-- 1) Implement eventTsunami and eventVolcano

--[IsComponent]
function C_RandomEventManager()
    local var = {
        --[SerializeString]
        RandomEventManager = "RandomEventManager",

        -- name of the TurnOrder state, to be checked against GlobalStatemachine's CurrentState
        RandomEventState = "StateRandomEvent",

        -- probability of a randomevent occuring 
        randomEventRate = 0.25,

        -- false if there is no random event occuring at the moment
        currentEvent = nil,
        
        -- list of possible randomevents
        -- randomEventList = { "eventEarthquake", "eventTsunami", "eventVolcano" }
        randomEventList = { "eventEarthquake" }
    };
    return function()
        return var
    end
end

--[IsSystem]
function S_RandomEventManager(e)
    
    -- getting TurnOrderManager entity and component
    local randomEventManagerEntity = GetEntity("RandomEventManager")
    local randomEventManagerComponent
    if (randomEventManagerEntity ~= -1) then
        randomEventManagerComponent = GetComponent(randomEventManagerEntity, "C_RandomEventManager")
    end
 
    -- getting GlobalStatemachine entity and component
    local globalStateMachineEntity = GetEntity("GlobalStatemachine")
    local globalStateMachineComponent
    if (globalStateMachineEntity ~= -1) then
        globalStateMachineComponent = GetComponent(globalStateMachineEntity, "C_GlobalStatemachine")
    end

    -- only run the rest of this script if globalStateMachine allows it
    if (globalStateMachineComponent.CurrentState == randomEventManagerComponent.RandomEventState) then
             
        -- only allow random event if no other random event is occuring
        if (randomEventManagerComponent.currentEvent == nil) then
            if (GenerateRandomProbability() < randomEventManagerComponent.randomEventRate) then
                print("[RandomEventManager.lua] RandomEvent activated! 25% chance.")
                
                local randomNumber = GenerateRandomNumberInRange(1, #randomEventManagerComponent.randomEventList)
                print("[RandomEventManager.lua] Activated random event:", randomEventManagerComponent.randomEventList[randomNumber], "\n")
                randomEventManagerComponent.currentEvent = randomEventManagerComponent.randomEventList[randomNumber]
            else
                print("[RandomEventManager.lua] RandomEvent not activated! 75% chance.\n")
            end
        end
        
        globalStateMachineComponent.CurrentState = "StateCharacterTurn"        
    end
end