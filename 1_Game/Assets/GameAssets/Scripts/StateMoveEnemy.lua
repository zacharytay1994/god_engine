-- How this script works:
-- 1) On the enemy's turn, it will set one of the player's adjacent tiles as the destination
-- 2) The enemy will start moving towards the destination using pathfinding.

-- TODO:
-- 1) Allow the enemy to move along y-axis
-- 2) Decrease enemy stamina as they move
-- 3) Define different movement types (don't move, move beside player, move to a distance away from player)

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
                print("\n[StateEnemyMove - START]")

                -- get the enemy's C_Pathfind component
                pathfind = GetComponent(e, "C_Pathfind")
                                
                -- get player entity
                local playerEntity = (EntitiesWithScriptComponent("C_PlayerEndTurn"))[1]

                -- checking that player entity exists
                if (playerEntity ~= -1) then 

                    -- -- sanity check
                    -- print("Player ID", GetEntityData(playerEntity).id)
                    
                    -- getting player's location
                    local playerGridCell = GetGridCell(playerEntity)

                    -- get all tiles
                    local allTiles = EntitiesWithScriptComponent("C_FloorTile")

                    -- loop through all tiles to find a valid adjacent tile, set destination to the first tile to be found valid
                    for i = 1, #allTiles do
                        local currentGridCell = GetGridCell(allTiles[i])

                        -- if the current tile is directly adjacent to the player
                        if (currentGridCell.x == playerGridCell.x and currentGridCell.y == playerGridCell.y -1 and currentGridCell.z == playerGridCell.z - 1 or -- behind player
                            currentGridCell.x == playerGridCell.x and currentGridCell.y == playerGridCell.y -1 and currentGridCell.z == playerGridCell.z + 1 or -- in front of player
                            currentGridCell.z == playerGridCell.z and currentGridCell.y == playerGridCell.y -1 and currentGridCell.x == playerGridCell.x - 1 or -- to the right
                            currentGridCell.z == playerGridCell.z and currentGridCell.y == playerGridCell.y -1 and currentGridCell.x == playerGridCell.x + 1 ) then -- to the left
                                
                            -- store the location of the adjacent tile and break the loop
                            targetGridCell = currentGridCell
                            break
                        end
                    end

                    -- start the pathfinding
                    pathfind.x = targetGridCell.x
                    pathfind.y = targetGridCell.y + 1
                    pathfind.z = targetGridCell.z
                    pathfind.Path = true

                    stateMoveEnemyComponent.startedPathfind = true
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
                turnOrderManagerComponent.nextTurn = true

                print("[StateEnemyMove - END]\n\n")
            end
        end
    end
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