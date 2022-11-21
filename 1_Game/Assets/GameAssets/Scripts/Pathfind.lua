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
                        -- -- move the character -------------------------------------------------------------------------
                        current_cell.x = path[2].x
                        current_cell.y = path[2].y
                        current_cell.z = path[2].z
                        -- print("found")
                        print(current_cell.x, current_cell.y, current_cell.z)
                        -- -- end of moving the character ----------------------------------------------------------------
                        
                        -- -- adjust character's rotation here -----------------------------------------------------------
                        local characterTransform = GetTransform(e)
                        
                        if (path[2].x > current_cell.x) then
                            -- face left
                            characterTransform.rotation.y = 90
                        elseif (path[2].x < current_cell.x) then
                            -- face right
                            characterTransform.rotation.y = 270
                        elseif (path[2].z > current_cell.z) then
                            -- face front
                            characterTransform.rotation.y = 0
                        else
                            -- face back
                            characterTransform.rotation.y = 180         
                        end
                        -- -- end of adjusting rotation ------------------------------------------------------------------
                        
                        -- -- reduce stamina by one for each step --------------------------------------------------------
                        local charComp = GetComponent(e, "C_Character")
                        charComp.currentStamina = charComp.currentStamina - 1
                        print("[Pathfind.lua]", EntityName(e), "Stamina after moving:", charComp.currentStamina)

                        if (charComp.currentStamina <= 0) then
                            pathfind.Path = false
                            pathfind.Timer = 0.0
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
