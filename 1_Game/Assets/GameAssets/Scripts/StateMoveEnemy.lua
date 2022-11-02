-- How this script works:
-- 1) On the enemy's turn, it will set one of the player's adjacent tiles as the destination
-- 2) The enemy will start moving towards the destination using pathfinding.

-- Note: if there are more than 8 enemies on the map, some enemies will remain stationary during their turn.

-- TODO:
-- 1) Allow the enemy to move along y-axis
-- 2) Define different movement types (don't move, move beside player, move to a distance away from player)
-- 3)

--[IsComponent]
function C_StateMoveEnemy()
    local var = {
        --[SerializeString]
        GlobalStatemachine = "GlobalStatemachine",
        State = "MoveEnemy",
        Time = 0.0,
        startedPathfind = false
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_StateMoveEnemy(e)

    -- check if TurnOrderManager entity exists
    local turnOrderManagerEntity = GetEntity("TurnOrderManager")  
    if (turnOrderManagerEntity ~= -1) then
        
        -- getting necessary entities and components
        local entityDataComponent = GetEntityData(e)
        local turnOrderManagerComponent = GetComponent(turnOrderManagerEntity, "C_TurnOrderManager")
        local stateMoveEnemyComponent = GetComponent(e, "C_StateMoveEnemy")
        local enemyGridCell = GetGridCell(e)
             
        -- if it's this character's turn, let it perform it actions
        if (turnOrderManagerComponent.currentTurn == entityDataComponent.id) then  
                     
            -- allow movement after 1 second has passed
            if (stateMoveEnemyComponent.Time < 1.0) then

                -- accumulate time
                stateMoveEnemyComponent.Time = stateMoveEnemyComponent.Time + GetDeltaTime()

            -- after 1 second has passed and pathfinding has not been initialized
            elseif (stateMoveEnemyComponent.Time >= 1.0 and stateMoveEnemyComponent.startedPathfind == false) then
                print("\n[StateEnemyMove - START]", EntityName(e), entityDataComponent.id)

                -- get the enemy's C_Pathfind component
                pathfind = GetComponent(e, "C_Pathfind")
                                
                -- get player entity
                playerEntity = (EntitiesWithScriptComponent("C_PlayerEndTurn"))[1]

                -- checking that player entity exists
                if (playerEntity ~= -1) then 

                    -- -- sanity check
                    -- print("Player ID", GetEntityData(playerEntity).id)
                    
                    local targetTile = SuitableTile()

                    -- if a suitable tile is found, then set it as target and move towards it
                    if (targetTile ~= nil) then 

                        local targetGridCell = GetGridCell(targetTile)

                        -- start the pathfinding
                        pathfind.x = targetGridCell.x
                        pathfind.y = targetGridCell.y + 1
                        pathfind.z = targetGridCell.z
                        pathfind.Path = true

                        stateMoveEnemyComponent.startedPathfind = true

                    -- else if no suitable tile is found, don't move and end turn
                    else
                        -- reset variables
                        stateMoveEnemyComponent.Time = 0.0
                        -- switch to next character's turn
                        print("[nextTurn = true --- StateMoveEnemy line 85")
                        turnOrderManagerComponent.nextTurn = true
                        print("[StateEnemyMove] No suitable tiles to move to, staying still and ending turn")
                        print("[StateEnemyMove - END]\n\n")
                        
                        return
                    end
                end
            end   

            -- once the enemy has reached its destination, end its turn
            if (enemyGridCell.x == pathfind.x and enemyGridCell.y == pathfind.y and enemyGridCell.z == pathfind.z) then

                -- reset variables
                stateMoveEnemyComponent.Time = 0.0
                stateMoveEnemyComponent.startedPathfind = false
                pathfind.x = 0
                pathfind.y = 0
                pathfind.z = 0

                -- switch to next character's turn
                print("[nextTurn = true --- StateMoveEnemy line 106")
                turnOrderManagerComponent.nextTurn = true
                print("Destination reached!")
                print("[StateEnemyMove - END]\n\n")
            
            elseif (GetComponent(e, "C_Character").currentStamina <= 0) then
                
                -- reset variables
                stateMoveEnemyComponent.Time = 0.0
                stateMoveEnemyComponent.startedPathfind = false
                pathfind.x = 0
                pathfind.y = 0
                pathfind.z = 0

                -- switch to next character's turn
                print("[nextTurn = true --- StateMoveEnemy line 121")
                turnOrderManagerComponent.nextTurn = true
                print("Stamina depleted!")
                print("[StateEnemyMove - END]\n\n")
            
            end
        end
    end
end

-- helper function for selecting a suitable tile to set as pathfind destination
function SuitableTile()
    
    print("[SuitableTile() - START]")

    -- getting player's location
    -- print("inside SuitableTile(), checking player Entity's name:", EntityName(playerEntity))
    local playerGridCell = GetGridCell(playerEntity)
    print("player grid location:", playerGridCell.x, playerGridCell.y, playerGridCell.z)

    -- get all tiles
    local allTiles = EntitiesWithScriptComponent("C_FloorTile")
    -- print("inside SuitableTile(), checking #allTiles:", #allTiles)

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
            print("adding current tile to adjacentCandidates")
            -- currentGridCell.y = currentGridCell.y + 1 -- to see which tiles are added
            adjacentCandidates[#adjacentCandidates + 1] = allTiles[i]
           
        -- else if current tile is diagonal to the player
        elseif (currentGridCell.x == playerGridCell.x + 1 and currentGridCell.y == playerGridCell.y -1 and currentGridCell.z == playerGridCell.z + 1 or -- forward right
                currentGridCell.x == playerGridCell.x + 1 and currentGridCell.y == playerGridCell.y -1 and currentGridCell.z == playerGridCell.z - 1 or -- backward right
                currentGridCell.x == playerGridCell.x - 1 and currentGridCell.y == playerGridCell.y -1 and currentGridCell.z == playerGridCell.z + 1 or -- forward left
                currentGridCell.x == playerGridCell.x - 1 and currentGridCell.y == playerGridCell.y -1 and currentGridCell.z == playerGridCell.z - 1 ) then -- backward left

            -- add current tile to diagonalCandidates
            print("adding current tile to diagonalCandidates")
            -- currentGridCell.y = currentGridCell.y + 1 -- to see which tiles are added
            diagonalCandidates[#diagonalCandidates + 1] = allTiles[i]
       end
   end

   -- get all entities with gridcell component
   entitiesWithGridCell = EntitiesWithEngineComponent("GridCell") 
   print("checking length of entitiesWithGridCell:", #entitiesWithGridCell)
   
   -- go through adjacentCandidates first, if a tile in this list is empty, immediately return it
    for j = 1, #adjacentCandidates do

        currentTileSuitable = true
        
        print("adjacentCandidate coordinates:", GetGridCell(adjacentCandidates[j]).x, GetGridCell(adjacentCandidates[j]).y, GetGridCell(adjacentCandidates[j]).z)
        
        -- if GetGridCell(adjacentCandidates[j]).y + 1 == the gridcell of anther object then break,
        -- because it means there is something occupying the current adjacentCandidates[j]
        for l = 1, #entitiesWithGridCell do

            adjacentCandidatesGridCell = GetGridCell(adjacentCandidates[j])
            local otherEntityGridCell = GetGridCell(entitiesWithGridCell[l])

            -- if (adjacentCandidatesGridCell == nil) then
            --     print("ERRO: adjacentCandidatesGridCell is nil") 
            -- else
            --     print("adjacentCandidatesGridCell IS NOT NIL")
            --     print(adjacentCandidatesGridCell.x, adjacentCandidatesGridCell.y, adjacentCandidatesGridCell.z)
            -- end

            -- if (otherEntityGridCell == nil) then
            --     print("ERRO: otherEntityGridCell is nil") 
            -- else
            --     print("otherEntityGridCell IS NOT NIL")
            --     print("name of other entity:", EntityName(entitiesWithGridCell[l]))
            --     print(otherEntityGridCell.x, otherEntityGridCell.y, otherEntityGridCell.z)
            --     print(GetGridCell(entitiesWithGridCell[l]).x, GetGridCell(entitiesWithGridCell[l]).y, GetGridCell(entitiesWithGridCell[l]).z)
            -- end

            -- if (GetEntityData(entitiesWithGridCell[l]).id == 82) then
            --     print("checking enemy 82's location:", otherEntityGridCell.x, otherEntityGridCell.y, otherEntityGridCell.z)
            -- end

            -- print("\n", adjacentCandidatesGridCell.y + 1, adjacentCandidatesGridCell.x, adjacentCandidatesGridCell.z, "vs" )
            -- print(otherEntityGridCell.y, otherEntityGridCell.x, otherEntityGridCell.z)

            if (adjacentCandidatesGridCell.y + 1 == otherEntityGridCell.y and
                adjacentCandidatesGridCell.x     == otherEntityGridCell.x and
                adjacentCandidatesGridCell.z     == otherEntityGridCell.z) then 
                
                print("current tile is unsuitable")  
                currentTileSuitable = false
            end
        end

        if (currentTileSuitable) then
            -- if code reaches here then it means there is nothing occupying adjacentCandidates[j]
            -- and the enemy can move here
            print("returning adjacentCandidates[j]", adjacentCandidatesGridCell.x, adjacentCandidatesGridCell.y, adjacentCandidatesGridCell.z)
            return adjacentCandidates[j]
        end

    end
    print("end of checking adjacentCandidates")

    -- if no tiles in adjacentCandidates are suitable, then go through diagonalCandidates
    for k = 1, #diagonalCandidates do

        currentTileSuitable = true

        print("diagonalCandidates coordinates:", GetGridCell(diagonalCandidates[k]).x, GetGridCell(diagonalCandidates[k]).y, GetGridCell(diagonalCandidates[k]).z)
        
        -- if GetGridCell(diagonalCandidates[k]).y + 1 == the gridcell of anther object then break,
        -- because it means there is something occupying the current diagonalCandidates[k]
        for m = 1, #entitiesWithGridCell do
            
            diagonalCandidatesGridCell = GetGridCell(diagonalCandidates[k])
            local otherEntityGridCell = GetGridCell(entitiesWithGridCell[m])

            if (diagonalCandidatesGridCell.y + 1 == otherEntityGridCell.y and
                diagonalCandidatesGridCell.x     == otherEntityGridCell.x and
                diagonalCandidatesGridCell.z     == otherEntityGridCell.z) then 
                
                print("current tile is unsuitable")  
                currentTileSuitable = false
            end
        end

        if (currentTileSuitable) then
            
            -- if code reaches here then it means there is nothing occupying diagonalCandidates[m]
            -- and the enemy can move here
            print("returning diagonalCandidates[m]", diagonalCandidatesGridCell.x, diagonalCandidatesGridCell.y, diagonalCandidatesGridCell.z)
            return diagonalCandidates[k]
        end
    end
    
    -- if the code reaches here then it means all 8 tiles surrounding the player are occupied.
    -- the enemy will just stay still
    return nil
end

-- currently unused
function CheckEnemyAdjacentToPlayer(enemy, player)
    
    -- init result
    result = false

    -- get attacker and defenders' locations
    enemyGrid = GetGridCell(enemy)
    playerGrid = GetGridCell(player)

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