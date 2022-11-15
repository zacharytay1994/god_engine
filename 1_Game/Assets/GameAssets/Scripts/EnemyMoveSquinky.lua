-- MoveDummee contains Squinkys' movement code. 

-- How this script works:
-- It will first check if it's already directly adjacent to the Player.
-- If yes, then it won't move, because it can use the Melee Attack on the Player.
-- If no, then it will set the one of the Player's adjacent tile as its destination, and move one tile towards it each turn.
-- To limit movement to one tile per turn, I have given it a stamina of 1.
-- Squinky can move diagonally and traverse up and down.

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
        targetPath = { }
        -- End of Squinky-specific variables --------------

    }
    return function()
        return var
    end
end

--[IsSystem]
function S_EnemyMoveSquinky(e)
            
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
    if (moveComponent.Time < 0.5) then

        -- accumulate time
        moveComponent.Time = moveComponent.Time + GetDeltaTime()

    -- after 1 second has passed and pathfinding has not been initialized
    elseif (moveComponent.startedPathfind == false) then
        
        print("[EnemyMoveSquinky.lua] Start of movement for", EntityName(e), entityDataComponent.id)

        -- if enemy is already beside player or in same lane as player, don't move 
        if (EnemyMoveSquinkyAdjacentToPlayer(e, playerEntity)) then
            print("[EnemyMoveSquinky.lua] Squinky is already adjacent to Player, don't move. Returning.")
            GetComponent(e, "C_EnemyController").hasMoved = true
            moveComponent.executeMove = false
            return

        -- else, find a suitable tile for Squinky to move to
        else   
            moveComponent.targetPath = EnemyMoveSquinkySuitablePath(e)
        end

        -- if a suitable tile is found, then set it as target and move towards it
        if (#moveComponent.targetPath > 1) then 

            -- Cannot use C_Pathfind here because then Squinky will never walk diagonally
            moveComponent.startedPathfind = true
            print("[EnemyMoveSquinky.lua] Starting pathfind!")

        -- else if no suitable path is found, don't move and end turn
        else
            -- reset variables
            moveComponent.Time = 0.0
            moveComponent.startedPathfind = false

            print("[EnemyMoveSquinky.lua] No suitable tiles to move to, staying still and ending turn.")
            GetComponent(e, "C_EnemyController").hasMoved = true
            moveComponent.executeMove = false
            return
        end
    end   

    -- Note: once pathfinding has been initialized, the enemy will start moving.
    -- We will need to terminate pathfinding once the enemy has reached the desired location.


    -- once the Squinky has reached the last position in the targetPath, or if it's adjacent to the player, stop moving
    if ((enemyGridCell.x == moveComponent.targetPath[#moveComponent.targetPath][1] and 
         enemyGridCell.y == moveComponent.targetPath[#moveComponent.targetPath][2] and 
         enemyGridCell.z == moveComponent.targetPath[#moveComponent.targetPath][3]) or 
         EnemyMoveSquinkyAdjacentToPlayer(e, playerEntity) == true) then
    
        -- reset variables
        moveComponent.Time = 0.0
        moveComponent.startedPathfind = false
        moveComponent.targetPath = { }

        moveComponent.executeMove = false
        GetComponent(e, "C_EnemyController").hasMoved = true
        print("[EnemyMoveSquinky.lua] Destination reached!")

    elseif (GetComponent(e, "C_Character").currentStamina <= 0) then

        -- reset variables
        moveComponent.Time = 0.0
        moveComponent.startedPathfind = false
        moveComponent.targetPath = { }

        moveComponent.executeMove = false
        GetComponent(e, "C_EnemyController").hasMoved = true
        print("[EnemyMoveSquinky.lua] Stamina fully depleted! Returning.")    
        return

    end
end

-- checks all 8 tiles around Squinky to see which one is closest to the Player, then returns a path to that tile.
function EnemyMoveSquinkySuitablePath(Squinky)
    
    print("[EnemyMoveDummee.lua] Start of EnemyMoveDummeeSuitableTile().")

    -- getting player's location
    local playerGridCell = GetGridCell(GetEntity("Player"))
    -- print("[StateMoveEnemy.lua] Player grid location:", playerGridCell.x, playerGridCell.y, playerGridCell.z)

    -- get all tiles
    local allTiles = EntitiesWithScriptComponent("C_FloorTile")
    -- print("[StateMoveEnemy.lua] Inside SuitableTile(), checking #allTiles:", #allTiles)

    -- a list of possible adjacent tiles to set as destination. this list is prioritized over diagonalCandidates
    local adjacentCandidates = { }

    -- a list of possible diagonal tiles to set as destination. will only use this if none of the adjacentCandidates are suitable
    local diagonalCandidates = { } 

    -- loop through all tiles to find a valid adjacent tile, set destination to the first tile to be found valid
    for i = 1, #allTiles do
    
        -- location of the current tile
        local currentGridCell = GetGridCell(allTiles[i])

       -- if the current tile is directly adjacent to the player
       if (currentGridCell.x == playerGridCell.x and currentGridCell.y == playerGridCell.y -1 and currentGridCell.z == playerGridCell.z - 1 or -- behind player
           currentGridCell.x == playerGridCell.x and currentGridCell.y == playerGridCell.y -1 and currentGridCell.z == playerGridCell.z + 1 or -- in front of player
           currentGridCell.z == playerGridCell.z and currentGridCell.y == playerGridCell.y -1 and currentGridCell.x == playerGridCell.x - 1 or -- to the right
           currentGridCell.z == playerGridCell.z and currentGridCell.y == playerGridCell.y -1 and currentGridCell.x == playerGridCell.x + 1 ) then -- to the left
               
            -- add current tile to adjacentCandidates
            adjacentCandidates[#adjacentCandidates + 1] = allTiles[i]
           
        -- else if current tile is diagonal to the player
        elseif (currentGridCell.x == playerGridCell.x + 1 and currentGridCell.y == playerGridCell.y -1 and currentGridCell.z == playerGridCell.z + 1 or -- forward right
                currentGridCell.x == playerGridCell.x + 1 and currentGridCell.y == playerGridCell.y -1 and currentGridCell.z == playerGridCell.z - 1 or -- backward right
                currentGridCell.x == playerGridCell.x - 1 and currentGridCell.y == playerGridCell.y -1 and currentGridCell.z == playerGridCell.z + 1 or -- forward left
                currentGridCell.x == playerGridCell.x - 1 and currentGridCell.y == playerGridCell.y -1 and currentGridCell.z == playerGridCell.z - 1 ) then -- backward left

            -- add current tile to diagonalCandidates
            diagonalCandidates[#diagonalCandidates + 1] = allTiles[i]
       end
   end

   -- get all entities with gridcell component
   entitiesWithGridCell = EntitiesWithEngineComponent("GridCell") 
   
   -- go through adjacentCandidates first, if a tile in this list is empty, immediately return it
    for j = 1, #adjacentCandidates do

        currentTileSuitable = true
                
        -- if GetGridCell(adjacentCandidates[j]).y + 1 == the gridcell of anther object then break,
        -- because it means there is something occupying the current adjacentCandidates[j]
        for l = 1, #entitiesWithGridCell do

            adjacentCandidatesGridCell = GetGridCell(adjacentCandidates[j])
            local otherEntityGridCell = GetGridCell(entitiesWithGridCell[l])

            if (adjacentCandidatesGridCell.y + 1 == otherEntityGridCell.y and
                adjacentCandidatesGridCell.x     == otherEntityGridCell.x and
                adjacentCandidatesGridCell.z     == otherEntityGridCell.z) then 
                
                -- print("[StateMoveEnemy.lua] Current tile is unsuitable.")  
                currentTileSuitable = false
            end
        end

        if (currentTileSuitable) then
            -- if code reaches here then it means there is nothing occupying adjacentCandidates[j]
            -- and the enemy can move here
            print("[EnemyMoveDummee.lua] Returning adjacentCandidates[j]", adjacentCandidatesGridCell.x, adjacentCandidatesGridCell.y, adjacentCandidatesGridCell.z)
            return adjacentCandidates[j]
        end
    end

    -- if no tiles in adjacentCandidates are suitable, then go through diagonalCandidates
    for k = 1, #diagonalCandidates do

        currentTileSuitable = true
        
        -- if GetGridCell(diagonalCandidates[k]).y + 1 == the gridcell of anther object then break,
        -- because it means there is something occupying the current diagonalCandidates[k]
        for m = 1, #entitiesWithGridCell do
            
            diagonalCandidatesGridCell = GetGridCell(diagonalCandidates[k])
            local otherEntityGridCell = GetGridCell(entitiesWithGridCell[m])

            if (diagonalCandidatesGridCell.y + 1 == otherEntityGridCell.y and
                diagonalCandidatesGridCell.x     == otherEntityGridCell.x and
                diagonalCandidatesGridCell.z     == otherEntityGridCell.z) then 
                
                -- print("[StateMoveEnemy.lua] Current tile is unsuitable.")
                currentTileSuitable = false
            end
        end

        if (currentTileSuitable) then
            -- if code reaches here then it means there is nothing occupying diagonalCandidates[m]
            -- and the enemy can move here
            print("[EnemyMoveDummee.lua] Returning diagonalCandidates[m]", diagonalCandidatesGridCell.x, diagonalCandidatesGridCell.y, diagonalCandidatesGridCell.z)
            return diagonalCandidates[k]
        end
    end
    
    -- if the code reaches here then it means all 8 tiles surrounding the player are occupied.
    -- the enemy will just stay still
    print("[EnemyMoveDummee.lua] End of EnemyMoveDummeeSuitableTile(). Returning nil.")
    return nil
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

function EnemyMoveSquinkyWalk(squinky)

    -- todo: make squinky walk the targetPath (refer to EnemyAttackDummeeMoveDummeeTowardsPlayer())

end