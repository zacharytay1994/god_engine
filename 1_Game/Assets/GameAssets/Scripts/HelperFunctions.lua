-- LIST OF FUNCTIONS (IN ALPHABETICAL ORDER)

-- AllUnoccupiedFloorTiles()
    -- returns a list of unoccupied floor tiles

-- ForwardVector(e)
    -- returns a normalized vector in the direction that the entity is facing.
    -- THIS FUNCTION IGNORES THE Y-AXIS!!!

-- NothingInGridAbove(entity) 
    -- checks if the grid cell above the input entity is empty.
    -- returns true if empty, false if not empty (occupied by another entity).

-- RandomUnoccupiedFloorTile()
    -- returns a random unoccupied floor tile 
    -- aka a tile without an entity (with a GridCell component) standing on it



-- returns a list of empty floor tiles
function AllUnoccupiedFloorTiles()

    local tileList = EntitiesWithScriptComponent("C_FloorTile")
    -- print("total number of tiles:", #tileList)
    
    local unoccupiedTileList = { }

    for i = 1, #tileList do
        if (NothingInGridAbove(tileList[i]) == true) then
            unoccupiedTileList[#unoccupiedTileList + 1] = tileList[i]
        end
    end

    -- print("number of unoccupied tiles:", #unoccupiedTileList)
    return unoccupiedTileList
end

 -- returns a normalized vector in the direction that the entity is facing.
 -- THIS FUNCTION IGNORES THE Y-AXIS!!!
 function ForwardVector(entity)
    local entityTransform = GetTransform(entity)
    -- print("entity y-rotation is", entityTransform.rotation.y)
    local newX = Sin(entityTransform.rotation.y)
    local newZ = Cos(entityTransform.rotation.y)
    return Normalize(newX, 1, newZ)

end

-- checks if the grid cell above the input entity is empty.
-- returns true if empty, false if not empty (occupied by another entity).
function NothingInGridAbove(entity)

    if (entity == nil or entity == -1) then
        print("[HelperFunctions.lua] NothingInGridAbove(entity): entity is nil!")
        return
    end

    local entityGridCell = GetGridCell(entity)
    local gridCellEntities = EntitiesWithEngineComponent("GridCell")

    for i = 1, #gridCellEntities do
    
        local otherEntityGridCell = GetGridCell(gridCellEntities[i])

        if (entityGridCell.x == otherEntityGridCell.x and 
            entityGridCell.y == otherEntityGridCell.y - 1 and
            entityGridCell.z == otherEntityGridCell.z) then
                -- print("occupied tile at:", entityGridCell.x, entityGridCell.y, entityGridCell.z)
                -- print("NothingInGridAbove(entity) returning false")
                return false
    
        end
    end
    -- print("NothingInGridAbove(entity) returning true")
    return true
end

-- returns a random floor tile
function RandomUnoccupiedFloorTile()

    local unoccupiedTileList = AllUnoccupiedFloorTiles()

    randomNumber = GenerateRandomNumberInRange(1, #unoccupiedTileList)

    return unoccupiedTileList[randomNumber]

end