-- Each time GlobalStatemachine switches to StateRandomEvent, there will be a 25% chance of a random event happening.
-- RandomEventManager will then choose a random randomevent from a list and run its script.
-- When a randomevent is set to occur, RandomEventManager will not proc another randomevent.
-- Game flow: [level start] > [characters' turns] > [random event (if any)] > [characters' turns] > ... > [level end]

-- While GlobalStatemachine.CurrentState is StateRandomEvent, this script will be responsible for randomising a randomevent,
-- before changing GlobalStatemachine.CurrentState to CharacterTurnState.

-- TODO:
-- 1) Run specific randomevent scripts after they have been selected

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
        eventHappening = false,
        
        -- list of possible randomevents
        randomEventList = { "eventEarthquake", "eventTsunami", "eventVolcano" }
    };
    return function()
        return var
    end
end

--[IsSystem]
function S_RandomEventManager(e)
    
    -- getting TurnOrderManager entity and component
    local randomEventManagerEntity = GetEntity("RandomEventManager")
    local randomEventManagerComponent = GetComponent(randomEventManagerEntity, "C_RandomEventManager")

    -- -- press C to check whether RandomEventManager is working
    -- if (CheckKeyPress(67)) then
    --     if (randomEventManagerComponent ~= nil) then
    --         print("RandomEventManager is alive and well")  
    --     else
    --         print("ERROR: RandomEventManager is not working!!!")  
    --     end
    -- end
 
    -- getting GlobalStatemachine entity and component
    local globalStateMachineEntity = GetEntity("GlobalStatemachine")
    local globalStateMachineComponent = GetComponent(globalStateMachineEntity, "C_GlobalStatemachine")

    -- print(globalStateMachineComponent.CurrentState, "vs", randomEventManagerComponent.RandomEventState)

    -- only run the rest of this script if globalStateMachine allows it
    if (globalStateMachineComponent.CurrentState == randomEventManagerComponent.RandomEventState) then
            
        -- press M to check whether the script passes the globalStateMachineComponent.CurrentState check
        if (CheckKeyPress(77)) then
            print("RandomEventManager script entered main chunk")
            print("RandomProbability:", GenerateRandomProbability())
            print("GenerateRandomNumberInRange(1, #randomEventList)", GenerateRandomNumberInRange(1, #randomEventManagerComponent.randomEventList))
        end
        
        -- only allow random event if no other random event is occuring
        if (randomEventManagerComponent.eventHappening == false) then
            print("\n[RandomEventManager - START]")
            if (GenerateRandomProbability() < 0.25) then
                print("RandomEvent activated! 25% chance.")
                print("Activated random event:", randomEventManagerComponent.randomEventList[GenerateRandomNumberInRange(1, #randomEventManagerComponent.randomEventList)])
                randomEventManagerComponent.eventHappening = true
                -- TODO: randomly pick an event and run that script
            else
                print("RandomEvent not activated! 75% chance.")            
            end
        end
        
        globalStateMachineComponent.CurrentState = "StateCharacterTurn"
        print("CurrentState = StateCharacterTurn")
        print("[RandomEventManager - END]\n\n")
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