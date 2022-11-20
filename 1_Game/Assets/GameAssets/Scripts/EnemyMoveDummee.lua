-- MoveDummee contains Dummees' movement code. 

-- How this script works:
-- It will first check if Dummee is in the same y-axis as Player. If no then it will not move.
-- It will then check if it's in the same lane as the Player.
-- If yes, then it won't move, because it can use the Charging Attack on the Player.
-- If no, then it will set the Player's position as its destination, and move one tile towards it each turn.
-- To limit movement to one tile per turn, I have given it a stamina of 1.

-- Note: Dummees cannot climb up or down.

--[IsComponent]
function C_EnemyMoveDummee()
    local var = {

        -- cue to start moving Dummee
        executeMove = false,
        
        -- a little pause before the enemy starts moving
        Time = 0.0,

        -- set to true once pathfinding has been initialized
        startedPathfind = false

    }
    return function()
        return var
    end
end

--[IsSystem]
function S_EnemyMoveDummee(e)
    
    -- get MoveDummee component
    local moveComponent = GetComponent(e, "C_EnemyMoveDummee")
    
    -- if enemy is not cued to move, then don't run this script
    if (moveComponent.executeMove == false) then
        return
    end

    -- getting necessary entities and components
    local entityDataComponent = GetEntityData(e)
    local enemyGridCell = GetGridCell(e)
    local playerEntity = GetEntity("Player")
    local pathfindComponent = GetComponent(e, "C_Pathfind")
    
    -- stop the script here if Player entity does not exist as Dummee will have nowhere to go :(
    if (playerEntity == -1) then
        print("[EnemyMoveDummee.lua] ERROR: Player does not exist! Returning.")
        return
    end
                    
    -- allow movement after 1 second has passed
    if (moveComponent.Time < 0.1) then

        -- accumulate time
        moveComponent.Time = moveComponent.Time + GetDeltaTime()

    -- after 1 second has passed and pathfinding has not been initialized
    elseif (moveComponent.startedPathfind == false) then
        
        if (GetComponent(e, "C_EnemyController").movementForecast) then
            print("[EnemyMoveDummee.lua] Start of FORECAST movement for", EntityName(e), entityDataComponent.id)
        else
            print("[EnemyMoveDummee.lua] Start of movement for", EntityName(e), entityDataComponent.id)
        end

        local targetTile = nil

        -- if enemy is already beside player or in same lane as player, don't move 
        if (EnemyMoveDummeeAdjacentToPlayer(e, playerEntity) or EnemyMoveDummeeSameLane(e, playerEntity)) then
            if (GetComponent(e, "C_Character").isActive == true) then
                print("[EnemyMoveDummee.lua] Dummee is already adjacent to or in same lane as Player. Returning.")
                GetComponent(e, "C_EnemyController").hasMoved = true
                moveComponent.executeMove = false
                return
            -- if character is not active, that means it's trying to forecast movement.
            else
                moveComponent.executeMove = false
                GetComponent(e, "C_EnemyController").movementForecast = false
                return
            end
        
            -- if enemy is on different y-axis as player, don't move
        elseif (enemyGridCell.y ~= GetGridCell(playerEntity).y) then
            if (GetComponent(e, "C_Character").isActive == true) then
                print("[EnemyMoveDummee.lua] Dummee is not on the same level as player. Returning.")
                GetComponent(e, "C_EnemyController").hasMoved = true
                moveComponent.executeMove = false
                return
            else
                moveComponent.executeMove = false
                GetComponent(e, "C_EnemyController").movementForecast = false
                return
            end
            

        else   
            targetTile = EnemyMoveDummeeSuitableTile()
        end

        -- if a suitable tile is found, then set it as target and move towards it
        if (targetTile ~= nil) then 

            local targetGridCell = GetGridCell(targetTile)

            -- start the pathfinding
            pathfindComponent.x = targetGridCell.x
            pathfindComponent.y = targetGridCell.y + 1
            pathfindComponent.z = targetGridCell.z
            pathfindComponent.Path = true

            moveComponent.startedPathfind = true
            print("[EnemyMoveDummee.lua] Starting pathfind!")

        -- else if no suitable tile is found, don't move and end turn
        else
            -- reset variables
            moveComponent.Time = 0.0
            moveComponent.startedPathfind = false
            pathfindComponent.x = 0
            pathfindComponent.y = 0
            pathfindComponent.z = 0

            print("[EnemyMoveDummee.lua] No suitable tiles to move to, staying still and ending turn.")
            GetComponent(e, "C_EnemyController").hasMoved = true
            moveComponent.executeMove = false
            return
        end
    end   

    -- Note: once pathfinding has been initialized, the enemy will start moving.
    -- We will need to terminate pathfinding once the enemy has reached the desired location.

    -- once the enemy has reached its destination, or if it's adjacent to the player, stop moving
    if ((enemyGridCell.x == pathfindComponent.x and enemyGridCell.y == pathfindComponent.y and enemyGridCell.z == pathfindComponent.z) or 
        EnemyMoveDummeeAdjacentToPlayer(e, playerEntity) == true) then

        -- reset variables
        moveComponent.Time = 0.0
        moveComponent.startedPathfind = false
        pathfindComponent.x = 0
        pathfindComponent.y = 0
        pathfindComponent.z = 0

        GetComponent(e, "C_EnemyController").hasMoved = true
        moveComponent.executeMove = false
        print("[EnemyMoveDummee.lua] Destination reached!")
    
    elseif (GetComponent(e, "C_Character").currentStamina <= 0) then
        
        -- reset variables
        moveComponent.Time = 0.0
        moveComponent.startedPathfind = false
        pathfindComponent.x = 0
        pathfindComponent.y = 0
        pathfindComponent.z = 0

        moveComponent.executeMove = false
        GetComponent(e, "C_EnemyController").hasMoved = true
        print("[EnemyMoveDummee.lua] Stamina fully depleted! Returning.")    
        return
    end 
end

-- helper function for selecting a suitable tile to set as pathfind destination
function EnemyMoveDummeeSuitableTile()
    
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

function EnemyMoveDummeeAdjacentToPlayer(enemy, player)
    
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

function EnemyMoveDummeeSameLane(enemyEntity, playerEntity)
    
    -- init result
    local result = false

    -- get attacker and defenders' locations
    local enemyGrid = GetGridCell(enemyEntity)
    local playerGrid = GetGridCell(playerEntity)

    if (enemyGrid.y == playerGrid.y) then 
        if (enemyGrid.x == playerGrid.x or enemyGrid.z == playerGrid.z) then
            result = true 
        end
    end

    return result
end

-- function S_StateMoveEnemy(e)
--     -- check if global state entity exists
--     local component = GetComponent(e, "C_StateMoveEnemy")
--     local gs_entity = GetEntity(component.GlobalStatemachine)
--     if (gs_entity ~= -1) then
--         local global_statemachine = GetComponent(gs_entity, "C_GlobalStatemachine")
--         -- do some state-only action here
--         if (global_statemachine.CurrentState == component.State) then
--             -- action ...
--             if (component.Time < 1.0) then
--                 component.Time = component.Time + GetDeltaTime()
--             else
--                 local cell = GetGridCell(e);
--                 cell.x = cell.x + 1
--                 global_statemachine.CurrentState = "MovePlayer"
--                 component.Time = 0.0
--             end
--         end
--     end
-- end