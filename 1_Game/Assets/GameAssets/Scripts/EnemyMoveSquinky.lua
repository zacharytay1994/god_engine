-- MoveDummee contains Squinkys' movement code. 

-- How this script works:
-- It will first check if it's already directly adjacent to the Player.
-- If yes, then it won't move, because it can use the Melee Attack on the Player.
-- If no, then it will set the one of the Player's adjacent tile as its destination, and move one tile towards it each turn.
-- To limit movement to one tile per turn, I have given it a stamina of 1.
-- Squinky can move diagonally and traverse up and down.

-- TODO:
-- 1) 
-- 2)  
-- 3) 

--[IsComponent]
function C_EnemyMoveSquinky()
    local var = {

        -- cue to start moving Squinky
        executeMove = false,
        
        -- a little pause before the enemy starts moving
        Time = 0.0,

        -- set to true once pathfinding has been initialized
        startedPathfind = false,

        -- Squinky-specific variables ---------------------
        -- the tile that Squinky will move to for current turn
        targetTile = nil
        -- End of Squinky-specific variables --------------

    }
    return function()
        return var
    end
end

--[IsSystem]
function S_EnemyMoveSquinky(e)
            
    -- print("[EnemyMoveSquinky.lua] Start of S_EnemyMoveSquinky()")
    
    -- get movement component
    local moveComponent = GetComponent(e, "C_EnemyMoveSquinky")
    
    -- if enemy is not cued to move, then don't run this script
    if (moveComponent.executeMove == false) then
        return
    end

    -- getting necessary entities and components
    local entityDataComponent = GetEntityData(e)
    local enemyGridCell = GetGridCell(e)
    local playerEntity = GetEntity("Player")
    local pathfindComponent = GetComponent(e, "C_Pathfind")
    
    -- stop the script here if Player entity does not exist as Squinky will have nowhere to go
    if (playerEntity == -1) then
        print("[EnemyMoveSquinky.lua] ERROR: Player does not exist! Returning.")
        return
    end
                    
    -- allow movement after 1 second has passed
    if (moveComponent.Time < 0.1) then

        -- accumulate time
        moveComponent.Time = moveComponent.Time + GetDeltaTime()

    -- after 1 second has passed and pathfinding has not been initialized
    elseif (moveComponent.startedPathfind == false) then
        
        if (GetComponent(e, "C_EnemyController").movementForecast) then
            print("[EnemyMoveSquinky.lua] Start of FORECAST movement for", EntityName(e), entityDataComponent.id)
        else
            print("[EnemyMoveSquinky.lua] Start of movement for", EntityName(e), entityDataComponent.id)
        end
        
        -- if enemy is already beside player, don't move 
        if (EnemyMoveSquinkyAdjacentToPlayer(e, playerEntity)) then
            if (GetComponent(e, "C_Character").isActive == true) then
                print("[EnemyMoveSquinky.lua] Squinky is already adjacent to Player, don't move. Returning.")
                moveComponent.Time = 0
                moveComponent.executeMove = false
                GetComponent(e, "C_EnemyController").hasMoved = true
                return
            -- if character is not active, that means it's trying to forecast movement
            else
                print("[EnemyMoveSquinky.lua] Squinky is already adjacent to Player, don't move. Returning.")
                moveComponent.Time = 0
                moveComponent.executeMove = false
                GetComponent(e, "C_EnemyController").movementForecast = false
                GetComponent(e, "C_EnemyController").attackForecast = true
                return
            end

        -- else, find a suitable tile for Squinky to move to
        else   
            moveComponent.targetTile = EnemyMoveSquinkySuitablePath(e, playerEntity)
        end

        -- if a suitable tile is found, then set it as target and move towards it
        if (moveComponent.targetTile ~= nil) then 

            if (GetComponent(e, "C_EnemyController").movementForecast) then
                
                local indicatorEntity = GetComponent(e, "C_EnemyController").indicatorEntity
                if (indicatorEntity ~= -1 and indicatorEntity ~= nil) then

                    print("[EnemyMoveSquinky.lua] ForecastIndicator starting to move.")
                    local targetTileGrid = GetGridCell(moveComponent.targetTile)
                    local indicatorGrid = GetGridCell(indicatorEntity)
                    indicatorGrid.x = targetTileGrid.x
                    indicatorGrid.y = targetTileGrid.y + 1
                    indicatorGrid.z = targetTileGrid.z
                    
                    --print("CHECK HERE")
                    moveComponent.Time = 0
                    moveComponent.startedPathfind = false
                    moveComponent.executeMove = false
                    GetComponent(e, "C_EnemyController").movementForecast = false
                    GetComponent(e, "C_EnemyController").attackForecast = true
                    print("[EnemyMoveSquinky.lua] ForecastIndicator done moving!")
                end
            
            else
                -- Cannot use C_Pathfind here because then Squinky will never walk diagonally
            
                -- TODO: just teleport squinky here
                local targetTileGrid = GetGridCell(moveComponent.targetTile)
                enemyGridCell.x = targetTileGrid.x
                enemyGridCell.y = targetTileGrid.y + 1
                enemyGridCell.z = targetTileGrid.z

                -- reset variables
                moveComponent.Time = 0.0
                moveComponent.startedPathfind = false
                moveComponent.targetTile = nil

                moveComponent.executeMove = false
                GetComponent(e, "C_EnemyController").hasMoved = true
                print("[EnemyMoveSquinky.lua] Destination reached!")
                
            
            end
            

        -- else if no suitable target tile is found, don't move and end turn
        else
            if (GetComponent(e, "C_EnemyController").movementForecast) then

                -- reset variables
                moveComponent.Time = 0
                moveComponent.startedPathfind = false

                GetComponent(entity, "C_EnemyController").movementForecast = false
                GetComponent(entity, "C_EnemyController").attackForecast = true

                moveComponent.executeMove = false
                print("[EnemyMoveSquinky.lua] Forecast Indicator has suitable tiles to move to, staying still.")
                return
            
            else

                -- reset variables
                moveComponent.Time = 0
                moveComponent.startedPathfind = false

                moveComponent.executeMove = false
                GetComponent(e, "C_EnemyController").hasMoved = true
                print("[EnemyMoveSquinky.lua] No suitable tiles to move to, staying still and ending turn.")
                return
            end
        end
    end
end

-- checks all 8 tiles around Squinky to see which one is closest to the Player, then returns a path to that tile.
function EnemyMoveSquinkySuitablePath(Squinky, targetEntity)

    print("[EnemyMoveSquinky.lua] Start of EnemyMoveSquinkySuitablePath().")

    -- getting target's location
    local destinationGridCell = GetGridCell(targetEntity)
    local squinkyGridCell = GetGridCell(Squinky)

    -- get all tiles
    local allTiles = EntitiesWithScriptComponent("C_FloorTile")

    -- the tile that the target is standing on
    local tileBelowTarget = nil
    local tileToMoveTo = nil

    -- a list of possible adjacent tiles to set as destination
    local allCandidates = { }

    -- loop through all tiles to find a valid adjacent tile, set destination to the first tile to be found valid
    for i = 1, #allTiles do
    
        -- location of the current tile
        local currentGridCell = GetGridCell(allTiles[i])

        -- if the current tile is the one that the target is standing on, store it in tileBelowTarget
        if (currentGridCell.x == destinationGridCell.x and currentGridCell.y == destinationGridCell.y - 1 and currentGridCell.z == destinationGridCell.z) then
            tileBelowTarget = allTiles[i]

       -- elseif the current tile is directly adjacent or diagonal to Squinky
        elseif (currentGridCell.x == squinkyGridCell.x and currentGridCell.y == squinkyGridCell.y -1 and currentGridCell.z == squinkyGridCell.z - 1 or -- behind player
           currentGridCell.x == squinkyGridCell.x and currentGridCell.y == squinkyGridCell.y -1 and currentGridCell.z == squinkyGridCell.z + 1 or -- in front of player
           currentGridCell.z == squinkyGridCell.z and currentGridCell.y == squinkyGridCell.y -1 and currentGridCell.x == squinkyGridCell.x - 1 or -- to the right
           currentGridCell.z == squinkyGridCell.z and currentGridCell.y == squinkyGridCell.y -1 and currentGridCell.x == squinkyGridCell.x + 1 or
           currentGridCell.x == squinkyGridCell.x + 1 and currentGridCell.y == squinkyGridCell.y -1 and currentGridCell.z == squinkyGridCell.z + 1 or -- forward right
           currentGridCell.x == squinkyGridCell.x + 1 and currentGridCell.y == squinkyGridCell.y -1 and currentGridCell.z == squinkyGridCell.z - 1 or -- backward right
           currentGridCell.x == squinkyGridCell.x - 1 and currentGridCell.y == squinkyGridCell.y -1 and currentGridCell.z == squinkyGridCell.z + 1 or -- forward left
           currentGridCell.x == squinkyGridCell.x - 1 and currentGridCell.y == squinkyGridCell.y -1 and currentGridCell.z == squinkyGridCell.z - 1) then -- to the left
               
            -- add current tile to adjacentCandidates
            allCandidates[#allCandidates + 1] = allTiles[i]            
       end  
    end

    local shortestDistance = 1000
    for j = 1, #allCandidates do
        if (EnemyMoveSquinkyCalculateDistance(allCandidates[j], tileBelowTarget) < shortestDistance and EnemyMoveSquinkyCandidateUnoccupied(allCandidates[j])) then
            shortestDistance = EnemyMoveSquinkyCalculateDistance(allCandidates[j], tileBelowTarget)
            tileToMoveTo = allCandidates[j]
        end
    end

    return tileToMoveTo
end

function EnemyMoveSquinkyCandidateUnoccupied(candidateTile)
    
    local candidateGrid = GetGridCell(candidateTile)

    local entitiesWithGridCell = EntitiesWithEngineComponent("GridCell")

    for i = 1, #entitiesWithGridCell do
    
        local currentEntityGrid = GetGridCell(entitiesWithGridCell[i])

        if (candidateGrid.x == currentEntityGrid.x and candidateGrid.y == currentEntityGrid.y - 1 and candidateGrid.z == currentEntityGrid.z) then
            return false
        end
    end

    return true
end

function EnemyMoveSquinkyAdjacentToPlayer(enemy, player)
    
    -- init result
    local result = false

    -- get attacker and defenders' locations
    local enemyGrid = GetGridCell(enemy)
    local playerGrid = GetGridCell(player)

    if (enemyGrid.y == playerGrid.y) then 

        -- enemy behind player
        if     (enemyGrid.x == playerGrid.x and enemyGrid.z == playerGrid.z - 1) then
            result = true 

        -- enemy in front of player
        elseif (enemyGrid.x == playerGrid.x and enemyGrid.z == playerGrid.z + 1) then
            result = true 

        -- enemy to player's left
        elseif (enemyGrid.z == playerGrid.z and enemyGrid.x == playerGrid.x - 1) then
            result = true 

        -- enemy to player's right
        elseif (enemyGrid.z == playerGrid.z and enemyGrid.x == playerGrid.x + 1) then
            result = true  
        end
    end

    return result
end

-- calculate and return the grid distance between two tile entities
function EnemyMoveSquinkyCalculateDistance(firstTile, secondTile)

    local firstTileGrid = GetGridCell(firstTile)
    local secondTileGrid = GetGridCell(secondTile)

    local totalDistance = 0

    totalDistance = totalDistance + Abs(firstTileGrid.x - secondTileGrid.x)
    totalDistance = totalDistance + Abs(firstTileGrid.y - secondTileGrid.y)
    totalDistance = totalDistance + Abs(firstTileGrid.z - secondTileGrid.z)

    return totalDistance
end
