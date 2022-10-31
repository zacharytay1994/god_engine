-- Going through a list of all Tiles, each tile has a 10% chance of getting moved. 
-- If a tile is to be moved, some of its surrounding tiles will also move.
-- Each moved tile has a 50% chance of moving up and 50% chance of moving down.
-- Max number of moved tiles will be 3, so as not to change the map too much.
-- Any character standing on a moving tile will received damage.
-- If there are tiles above a tile that is moving, those tiles on top will not be affected.
-- Earthquake will only affect tiles on floor0, so players will have to get to higher ground within 3 turns to avoid damage.

-- TODO:
-- 1) Implement UI to warn player of earthquake every turn

--[IsComponent]
function C_RandomEventEarthquake()
    local var = {
        --[SerializeString]
        RandomEventManager = "RandomEventManager",
        
        -- name of random event
        RandomEvent = "eventEarthquake",

        -- probability of the earthquake affecting each tile
        earthquakeTileRate = 0.1,

        -- -- probability of an effected tile moving up or down
        tileDirectionRate = 0.5,
        
        -- max number of affected tiles
        maxAffectedTiles = 3,

        -- counts the number of affected tiles for this earthquake
        affectedTilesCounter = 0,

        -- number of turns before earthquake happens
        eventCountdown = 3,

        -- earthquake will occur at the start of this turn no.
        earthquakeExecuteTurn = 0,

        -- turn no. of the turn when the countdown started
        startOfCountdown = 0,

        -- bool to run earthquake initialization once
        earthquakeInitialized = false, 

        -- remind the player about earthquake every turn
        playerWarned = false,

        -- damage to characters standing on affected tiles during earthquake
        earthquakeDamage = 3
    };
    return function()
        return var
    end
end

--[IsSystem]
function S_RandomEventEarthquake(e)
    
    -- getting RandomEventManager
    local randomEventManagerEntity = GetEntity("RandomEventManager")
    if (randomEventManagerEntity ~= -1) then
        randomEventManagerComponent = GetComponent(randomEventManagerEntity, "C_RandomEventManager")
    end

    -- getting C_RandomEventEarthquake component
    local randomEventEarthquakeComponent = GetComponent(randomEventManagerEntity, "C_RandomEventEarthquake")

    -- only go on if RandomEventManager says the current event is an earthquake
    if (randomEventManagerComponent.currentEvent == randomEventEarthquakeComponent.RandomEvent) then 
            
        -- getting TurnOrderManager to check the turn counter
        local turnOrderManagerEntity = GetEntity("TurnOrderManager")
        if (turnOrderManagerEntity ~= -1) then
            turnOrderManagerComponent = GetComponent(turnOrderManagerEntity, "C_TurnOrderManager")
        end

        -- determine the turn no. to execute the earthquake
        if (randomEventEarthquakeComponent.earthquakeInitialized == false) then
            
            -- earthquakeExecuteTurn will be current turn number + eventCountdown
            randomEventEarthquakeComponent.earthquakeExecuteTurn = turnOrderManagerComponent.turnCycleCounter + randomEventEarthquakeComponent.eventCountdown

            -- storing the turn number of the turn where the earthquake was confirmed
            randomEventEarthquakeComponent.startOfCountdown = turnOrderManagerComponent.turnCycleCounter
            
            -- set earthquakeInitialized to true so script will not overwrite earthquakeExecuteTurn
            randomEventEarthquakeComponent.earthquakeInitialized = true
            
            print("\n[Earthquake announcement - START]")
            print("Current turn is turn no.", turnOrderManagerComponent.turnCycleCounter)
            print("Earthquake will execute on turn no.", randomEventEarthquakeComponent.earthquakeExecuteTurn)
            print("[Earthquake announcement - END]\n\n")
        end

        --countdown, warn the player, let them know how many more turns until earthquake
        if (randomEventEarthquakeComponent.playerWarned == false) then 
            
            -- insert UI to warn player here
            
            -- for now, just print a message
            print("\n[Earthquake warning - START]")
            print("EARTHQUAKE HAPPENING IN", randomEventEarthquakeComponent.eventCountdown, "TURNS")
            print("[Earthquake warning - END]\n\n")

            -- reset playerWarned, and decrement eventCountdown
            randomEventEarthquakeComponent.playerWarned = true
            randomEventEarthquakeComponent.eventCountdown = randomEventEarthquakeComponent.eventCountdown - 1
        end

        -- if the turnCycleCounter has increased, warn the player again
        if (randomEventEarthquakeComponent.startOfCountdown ~= turnOrderManagerComponent.turnCycleCounter) then 
            randomEventEarthquakeComponent.startOfCountdown = turnOrderManagerComponent.turnCycleCounter
            randomEventEarthquakeComponent.playerWarned = false
        end

        -- when it's time to execute the earthquake
        if (turnOrderManagerComponent.turnCycleCounter == randomEventEarthquakeComponent.earthquakeExecuteTurn) then 
            
            print("\n[RandomEventEarthquake - START]")

            -- get a list of all tiles
            local tileList = EntitiesWithScriptComponent("C_FloorTile")
            print("number of tiles", #tileList)

            -- iterate through each tile in tileList
            for i = 1, #tileList do
                
                -- get a random number between 0 and 1
                local randomProb = GenerateRandomProbability()
                --print(randomProb)

                -- if this tile is affected by the earthquake (limit to floor0 tiles because higher tiles may end up floating)
                if (randomProb <= randomEventEarthquakeComponent.earthquakeTileRate and (GetGridCell(tileList[i]).y == 0)) then
                    
                    -- create a new list to contain tiles that will be moved
                    local affectedTileList = { tileList[i] }
                    -- print("1. number of tiles in affectedTileList:", #affectedTileList) -- this should print 1
                    
                    -- getting the x,y,z grid coordinates for the center tile
                    local mainTileCoordinates = GetGridCell(tileList[i])
                    print("mainTileCoordinates:", mainTileCoordinates.x, mainTileCoordinates.y, mainTileCoordinates.z)

                    -- manually add surrounding tiles into affectedTileList, by iterating through tileList again to look for tiles with correct coords
                    for k = 1, #tileList do
                        
                        -- get the coordinates for each tile
                        adjacentTileCoordinates = GetGridCell(tileList[k])
                        
                        -- if this tile is not in the same y-axis, then just skip it
                        if (adjacentTileCoordinates.y == mainTileCoordinates.y) then
                            
                            -- compare x and z (8 scenarios)
                            if ((adjacentTileCoordinates.x == mainTileCoordinates.x and adjacentTileCoordinates.z == mainTileCoordinates.z - 1) or -- behind
                                (adjacentTileCoordinates.x == mainTileCoordinates.x and adjacentTileCoordinates.z == mainTileCoordinates.z + 1) or -- in front
                                (adjacentTileCoordinates.z == mainTileCoordinates.z and adjacentTileCoordinates.x == mainTileCoordinates.x - 1) or -- right
                                (adjacentTileCoordinates.z == mainTileCoordinates.z and adjacentTileCoordinates.x == mainTileCoordinates.x + 1)) then -- left
                                --(adjacentTileCoordinates.x == mainTileCoordinates.x + 1 and adjacentTileCoordinates.z == mainTileCoordinates.z + 1) or -- front + left
                                --(adjacentTileCoordinates.x == mainTileCoordinates.x - 1 and adjacentTileCoordinates.z == mainTileCoordinates.z + 1) or -- front + right
                                --(adjacentTileCoordinates.x == mainTileCoordinates.x + 1 and adjacentTileCoordinates.z == mainTileCoordinates.z - 1) or -- behind + left
                                --(adjacentTileCoordinates.x == mainTileCoordinates.x - 1 and adjacentTileCoordinates.z == mainTileCoordinates.z - 1)) then -- behind + right
                                print("inserting a tile in affectedTileList", adjacentTileCoordinates.x, adjacentTileCoordinates.y, adjacentTileCoordinates.z)
                                affectedTileList[#affectedTileList+1] = tileList[k]
                            end 
                        end
                    end

                    print("2. number of tiles in affectedTileList:", #affectedTileList, "\n")

                    -- activate screenshake here
                    local screenShakeEntity = GetEntity("ScreenShake")
                    if (screenShake ~= -1) then
                        screenShakeComponent = GetComponent(screenShakeEntity, "C_ScreenShake")
                    end
                    screenShakeComponent.duration = 0.75
                    screenShakeComponent.doScreenShake = true
                    
                    -- get another randomprob to determine move up or down
                    local directionProb = GenerateRandomProbability()

                    -- get all entities with C_Character component
                    local allCharacters = EntitiesWithScriptComponent("C_Character")

                    -- for each tile in affectedTileList, move either up or down, according to directionProb
                    -- and also move any characters standing on them
                    for j = 1, #affectedTileList do 
                        
                        -- GridCell component for each tile
                        local tileGridCell = GetGridCell(affectedTileList[j])

                        -- marks whether a character is standing on the affected tile
                        local standingOnTile = false
                        local affectedCharacterGridCell = nil
                        
                        -- looping through all characters to check if any of them are standing on this affected tile
                        for l = 1, #allCharacters do
                            if (GetGridCell(allCharacters[l]).y == tileGridCell.y + 1 and 
                                GetGridCell(allCharacters[l]).x == tileGridCell.x and 
                                GetGridCell(allCharacters[l]).z == tileGridCell.z) then 
                                standingOnTile = true
                                affectedCharacterGridCell = GetGridCell(allCharacters[l])
                                GetComponent(allCharacters[l], "C_Character").currentHP = GetComponent(allCharacters[l], "C_Character").currentHP - randomEventEarthquakeComponent.earthquakeDamage
                            end
                        end
                        
                        -- make the tile go up or down, according to directionProb
                        if (directionProb >= randomEventEarthquakeComponent.tileDirectionRate) then 
                            -- go up
                            tileGridCell.y = tileGridCell.y + 1
                            if (standingOnTile) then 
                                affectedCharacterGridCell.y = affectedCharacterGridCell.y + 1
                            end
                        else
                            -- go down
                            tileGridCell.y = tileGridCell.y - 1
                            if (standingOnTile) then 
                                affectedCharacterGridCell.y = affectedCharacterGridCell.y - 1
                            end
                        end
                    end
                    
                    -- increment number of affected tiles
                    randomEventEarthquakeComponent.affectedTilesCounter = randomEventEarthquakeComponent.affectedTilesCounter + 1
                end

                -- break the loop after hitting the cap for affected tiles
                if (randomEventEarthquakeComponent.affectedTilesCounter == randomEventEarthquakeComponent.maxAffectedTiles) then
                    print("Maximum affected tiles hit! Breaking loop") 
                    break
                end
            end

            -- reset variables
            randomEventEarthquakeComponent.affectedTilesCounter = 0
            randomEventEarthquakeComponent.eventCountdown = 3
            randomEventEarthquakeComponent.earthquakeExecuteTurn = 0
            randomEventEarthquakeComponent.startOfCountdown = 0
            randomEventEarthquakeComponent.earthquakeInitialized = false
            randomEventEarthquakeComponent.playerWarned = false

            -- reset randomeventmanager.currentEvent
            randomEventManagerComponent.currentEvent = nil

            print("RandomEventEarthquake - END]\n\n")
        end
    end
end


--[[ PSEUDOCODE

if (RandomEventManager.currentEvent = eventEarthquake)
{
    tileList = a list of all tiles
    
    for each tile in tileList
    {
        if (randomprob <= 0.1)
        {
            1. tile will be affected! 
            2. push this tile and its surrounding 8 tiles into another list
            3. get another randomprob to determine move up or down
            4. for each tile in this new list, move either up or down, according to no. 3
            5. ++affecttiles
        }        

        if affectedtiles >= 3, stop
    }

    reset variables
    reset randomeventmanager.currentEvent
}

--]]