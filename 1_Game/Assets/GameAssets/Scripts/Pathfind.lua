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
        Interval = 1.0,
        Timer = 0.0
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
                local path = GetPath(e, pathfind.x, pathfind.y, pathfind.z);
                if (#path > 1) then
                    current_cell.x = path[2].x
                    current_cell.y = path[2].y
                    current_cell.z = path[2].z
                    -- print("found")
                    print(current_cell.x, current_cell.y, current_cell.z)

                    -- reduce stamina by one for each step
                    charComp = GetComponent(e, "C_Character")
                    charComp.currentStamina = charComp.currentStamina - 1
                    print("[Pathfind.lua] Stamina after moving:", charComp.currentStamina)

                    if (charComp.currentStamina <= 0) then
                        pathfind.Path = false
                        pathfind.Timer = 0.0
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
