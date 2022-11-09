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
        local playerEntity = GetEntity("Player")

        if (playerEntity == -1) then
            print("[StateMoveEnemy.lua] ERROR: Player does not exist! Returning.")
            return
        end

        if (GetComponent(e, "C_Character").moved == true) then
            print("[StateMoveEnemy.lua] Enemy moved already, skipping this script!")
            return
        end
             
        -- if it's this character's turn, let it perform it actions
        if (turnOrderManagerComponent.currentTurn == entityDataComponent.id) then  
                     
            -- allow movement after 1 second has passed
            if (stateMoveEnemyComponent.Time < 1.0) then

                -- accumulate time
                stateMoveEnemyComponent.Time = stateMoveEnemyComponent.Time + GetDeltaTime()

            -- after 1 second has passed and pathfinding has not been initialized
            elseif (stateMoveEnemyComponent.Time >= 1.0 and stateMoveEnemyComponent.startedPathfind == false and GetComponent(e, "C_Character").moved == false) then
                print("[StateMoveEnemy.lua] Start of movement for", EntityName(e), entityDataComponent.id)

                -- get the enemy's C_Pathfind component
                pathfind = GetComponent(e, "C_Pathfind")

                local targetTile = nil

                -- if enemy is already beside player, don't move 
                if (CheckEnemyAdjacentToPlayer(e, playerEntity) == false) then
                    targetTile = SuitableTile()
                end

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
                    stateMoveEnemyComponent.startedPathfind = false
                    pathfind.x = 0
                    pathfind.y = 0
                    pathfind.z = 0



                    -- switch to next character's turn
                    print("[StateMoveEnemy.lua] moved = true (line 84).")
                    -- turnOrderManagerComponent.nextTurn = true
                    GetComponent(e, "C_Character").moved = true
                    print("[StateMoveEnemy.lua] No suitable tiles to move to, staying still and ending turn.")
                    print("[StateMoveEnemy.lua] End of movement for", EntityName(e), entityDataComponent.id, "\n")                 
                    return
                end
            end   

            if (GetComponent(e, "C_Character").moved == false) then
                -- once the enemy has reached its destination, or if it's adjacent to the player, stop moving
                if ((enemyGridCell.x == pathfind.x and enemyGridCell.y == pathfind.y and enemyGridCell.z == pathfind.z) or 
                    CheckEnemyAdjacentToPlayer(e, playerEntity) == true)  then

                    -- reset variables
                    stateMoveEnemyComponent.Time = 0.0
                    stateMoveEnemyComponent.startedPathfind = false
                    pathfind.x = 0
                    pathfind.y = 0
                    pathfind.z = 0

                    -- switch to next character's turn
                    print("[StateMoveEnemy.lua] moved = true (line 104).")
                    -- turnOrderManagerComponent.nextTurn = true
                    GetComponent(e, "C_Character").moved = true
                    print("[StateMoveEnemy.lua] Destination reached!")
                    print("[StateMoveEnemy.lua] End of movement for", EntityName(e), entityDataComponent.id, "\n")  
                
                elseif (GetComponent(e, "C_Character").currentStamina <= 0) then
                    
                    -- reset variables
                    stateMoveEnemyComponent.Time = 0.0
                    stateMoveEnemyComponent.startedPathfind = false
                    pathfind.x = 0
                    pathfind.y = 0
                    pathfind.z = 0

                    -- switch to next character's turn
                    print("[StateMoveEnemy.lua] moved = true (line 119).")
                    -- turnOrderManagerComponent.nextTurn = true
                    GetComponent(e, "C_Character").moved = true
                    print("[StateMoveEnemy.lua] Stamina fully depleted!")
                    print("[StateMoveEnemy.lua] End of movement for", EntityName(e), entityDataComponent.id, "\n")  
                
                end
            end
        end
    end
end

-- helper function for selecting a suitable tile to set as pathfind destination
function SuitableTile()
    
    print("[StateMoveEnemy.lua] Start of SuitableTile().")

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
            -- print("[StateMoveEnemy.lua] Adding current tile to adjacentCandidates.")
            adjacentCandidates[#adjacentCandidates + 1] = allTiles[i]
           
        -- else if current tile is diagonal to the player
        elseif (currentGridCell.x == playerGridCell.x + 1 and currentGridCell.y == playerGridCell.y -1 and currentGridCell.z == playerGridCell.z + 1 or -- forward right
                currentGridCell.x == playerGridCell.x + 1 and currentGridCell.y == playerGridCell.y -1 and currentGridCell.z == playerGridCell.z - 1 or -- backward right
                currentGridCell.x == playerGridCell.x - 1 and currentGridCell.y == playerGridCell.y -1 and currentGridCell.z == playerGridCell.z + 1 or -- forward left
                currentGridCell.x == playerGridCell.x - 1 and currentGridCell.y == playerGridCell.y -1 and currentGridCell.z == playerGridCell.z - 1 ) then -- backward left

            -- add current tile to diagonalCandidates
            -- print("[StateMoveEnemy.lua] Adding current tile to diagonalCandidates.")
            diagonalCandidates[#diagonalCandidates + 1] = allTiles[i]
       end
   end

   -- get all entities with gridcell component
   entitiesWithGridCell = EntitiesWithEngineComponent("GridCell") 
   -- print("[StateMoveEnemy.lua] Checking length of entitiesWithGridCell:", #entitiesWithGridCell)
   
   -- go through adjacentCandidates first, if a tile in this list is empty, immediately return it
    for j = 1, #adjacentCandidates do

        currentTileSuitable = true
        
        -- print("[StateMoveEnemy.lua] adjacentCandidate coordinates:", GetGridCell(adjacentCandidates[j]).x, GetGridCell(adjacentCandidates[j]).y, GetGridCell(adjacentCandidates[j]).z)
        
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
            print("[StateMoveEnemy.lua] Returning adjacentCandidates[j]", adjacentCandidatesGridCell.x, adjacentCandidatesGridCell.y, adjacentCandidatesGridCell.z)
            return adjacentCandidates[j]
        end

    end
    -- print("[StateMoveEnemy.lua] End of checking adjacentCandidates.")

    -- if no tiles in adjacentCandidates are suitable, then go through diagonalCandidates
    for k = 1, #diagonalCandidates do

        currentTileSuitable = true

        -- print("[StateMoveEnemy.lua] diagonalCandidates coordinates:", GetGridCell(diagonalCandidates[k]).x, GetGridCell(diagonalCandidates[k]).y, GetGridCell(diagonalCandidates[k]).z)
        
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
            print("[StateMoveEnemy.lua] Returning diagonalCandidates[m]", diagonalCandidatesGridCell.x, diagonalCandidatesGridCell.y, diagonalCandidatesGridCell.z)
            return diagonalCandidates[k]
        end
    end
    
    -- if the code reaches here then it means all 8 tiles surrounding the player are occupied.
    -- the enemy will just stay still
    print("[StateMoveEnemy.lua] End of SuitableTile(). Returning nil.")
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