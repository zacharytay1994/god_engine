-- This script doesn't do anything. I just need to add this component to the TilePrefab so I can
-- use EntitiesWithComponent() to get a list of all tiles.

--[IsComponent]
function C_FloorTile()
    local var = {
        --[SerializeString]
        Tile = "Tile"
    }
    return function()
        return var
    end
end
