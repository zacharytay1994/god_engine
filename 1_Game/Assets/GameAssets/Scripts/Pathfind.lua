-- This script contains path finding algorithm for characters.
-- Character stamina is also reduced as they walk.

--[IsComponent]
function C_Pathfind()
    local var = {
        x = 0,
        y = 0,
        z = 0,
        Path = false,
        --[SerializeFloat]
        Interval = 0.1,
        Timer = 0.0,
        --[SerializeInt]
        MaxDown = 1,
        --[SerializeInt]
        MaxUp = 1,
        --[SerializeInt]
        HeuristicMultiplier = 1
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_Pathfind(e)
    local pathfind = GetComponent(e, "C_Pathfind")
    if (pathfind.Path) then
        if (pathfind.Timer < pathfind.Interval) then
            pathfind.Timer = pathfind.Timer + GetDeltaTime()
        else
            -- print("finding")
            pathfind.Timer = 0.0
            local current_cell = GetGridCell(e);
            -- if cell position not match, pathfind to cell
            if (current_cell.x ~= pathfind.x or current_cell.y ~= pathfind.y or current_cell.z ~= pathfind.z) then
                local path = GetPath3D(e, pathfind.x, pathfind.y, pathfind.z, pathfind.MaxDown, pathfind.MaxUp, pathfind.HeuristicMultiplier);
                if (#path > 1) then
                           
                    -- if C_Pathfind is attached to an enemy whose is flagged to perform a forecast,
                    -- then overwrite current_cell so that the indicator moves instead of the enemy.
                    -- in the future, check from a list of enemy names instead.
                    if (EntityName(e) ~= "Player" and GetComponent(e, "C_EnemyController").movementForecast) then
                        if (GetComponent(e, "C_EnemyController").indicatorEntity ~= -1 and GetComponent(e, "C_EnemyController").indicatorEntity ~= nil) then
                            
                            current_cell = GetGridCell(GetComponent(e, "C_EnemyController").indicatorEntity)
                            --print("indicatorEntity gridcell:", current_cell.x, current_cell.y, current_cell.z)

                            current_cell.x = path[2].x
                            current_cell.y = path[2].y
                            current_cell.z = path[2].z
                            -- print("found")
                            print(current_cell.x, current_cell.y, current_cell.z)

                            local charComp = GetComponent(e, "C_Character")
                            charComp.currentStamina = charComp.currentStamina - 1
                            print("[Pathfind.lua]", EntityName(e), "'s indicator's Stamina after moving:", charComp.currentStamina)

                            -- need to do this because for some reason, using e in the following if-block 
                            -- causes the player to not be able to move.
                            local entity = e

                            if (charComp.currentStamina <= 0) then
                                print("[Pathfind.lua] Indicator's stamina has reached 0! Ending movement.")
                                pathfind.Path = false
                                pathfind.Timer = 0.0
                                
                                -- refresh the stamina for enemy after the forecast has expended it
                                charComp.currentStamina = charComp.maxStamina

                                -- -- reset enemy's movement script variables
                                local moveComponent = GetComponent(entity, "C_EnemyMoveDummee")
                                moveComponent.Time = 0.0
                                moveComponent.startedPathfind = false
                                moveComponent.executeMove = false
                                GetComponent(entity, "C_EnemyController").movementForecast = false
                                GetComponent(entity, "C_EnemyController").attackForecast = true

                                -- reset C_Pathfind variables
                                local pathfindComponent = GetComponent(e, "C_Pathfind")
                                pathfindComponent.x = 0
                                pathfindComponent.y = 0
                                pathfindComponent.z = 0

                                print("[Pathfind.lua] ForecastIndicator done moving!")
                            end
                        end
                    else
                        
                        -- -- adjust character's rotation here -----------------------------------------------------------
                        -- local characterTransform = GetTransform(e)
                        -- local child_model = Child(e,0)
                        -- local child_model_transform = GetTransform(child_model)
                        
                        -- if (path[2].x > current_cell.x) then
                        --     -- face left
                        --     child_model_transform.rotation.y = 90
                        -- elseif (path[2].x < current_cell.x) then
                        --     -- face right
                        --     child_model_transform.rotation.y = 270
                        -- elseif (path[2].z > current_cell.z) then
                        --     -- face front
                        --     child_model_transform.rotation.y = 0
                        -- else
                        --     -- face back
                        --     child_model_transform.rotation.y = 180         
                        -- end
                        -- -- end of adjusting rotation ------------------------------------------------------------------
                        
                        -- -- move the character -------------------------------------------------------------------------
                        -- set camera to track movement
                        if EntityName(e) == "Player" then
                            local world_position = WorldPosition(e)
                            local dir_x = path[2].x - current_cell.x
                            local dir_y = path[2].y - current_cell.y
                            local dir_z = path[2].z - current_cell.z
                            local dist_behind = 3
                            local dist_above = 4
                            SetCameraMoveSpeed(2.0)
                            SetCameraPanSpeed(3.0)
                            if dir_x ~= 0 then
                                if dir_x > 0 then
                                    SetCameraNextPosition(world_position.x-dist_behind, world_position.y + dir_y + dist_above, world_position.z)
                                else 
                                    SetCameraNextPosition(world_position.x+dist_behind, world_position.y + dir_y + dist_above, world_position.z)
                                end
                            else
                                if dir_z > 0 then
                                    SetCameraNextPosition(world_position.x, world_position.y + dir_y + dist_above, world_position.z-dist_behind)
                                else 
                                    SetCameraNextPosition(world_position.x, world_position.y + dir_y + dist_above, world_position.z+dist_behind)
                                end
                            end
                            SetCameraNextLookAt(world_position.x+dir_x, world_position.y + dir_y, world_position.z+dir_z)
                        end
                        
                        print("SETTING OFFSET")
                        -- Set child i.e. model position offset to start movement animation
                        -- child_model_transform.position.x = (current_cell.x - path[2].x)*2.0
                        -- child_model_transform.position.y = (current_cell.y - path[2].y)*2.0
                        -- child_model_transform.position.z = (current_cell.z - path[2].z)*2.0

                        print("OFFSET SET")

                        -- Set parent i.e. base position
                        current_cell.x = path[2].x
                        current_cell.y = path[2].y
                        current_cell.z = path[2].z

                        -- print("found")
                        print(current_cell.x, current_cell.y, current_cell.z)
                        -- -- end of moving the character ----------------------------------------------------------------
                        
                        -- -- reduce stamina by one for each step --------------------------------------------------------
                        local charComp = GetComponent(e, "C_Character")
                        charComp.currentStamina = charComp.currentStamina - 1
                        print("[Pathfind.lua]", EntityName(e), "Stamina after moving:", charComp.currentStamina)

                        if (charComp.currentStamina <= 0) then
                            pathfind.Path = false
                            pathfind.Timer = 0.0
                            
                            -- camera reset
                            if EntityName(e) == "Player" then
                                SetCameraMoveSpeed(1.0)
                                SetCameraPanSpeed(1.5)
                                SetCameraNextLookAt(1,0,-4)
                                SetCameraNextPosition(-3,12,-15)
                            end
                        end
                        -- -- end of reducing stamina --------------------------------------------------------------------
                    end
                    
                    

                else
                    pathfind.Path = false
                    pathfind.Timer = 0.0 
                    -- print("#path <= 1, path not found")
                end
            else
                pathfind.Path = true
                pathfind.Timer = 0.0
                -- print("path not found")
            end
        end
    end
end
