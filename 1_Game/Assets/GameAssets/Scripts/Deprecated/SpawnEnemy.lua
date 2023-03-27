-- Attached to SpawnEnemy entity.
-- At the start of each level, this script will spawn enemies at random locations.
-- After spawning, SpawnEnemy entity will destroy itself.

-- TODO: 
-- 1) Create a list of enemies and automate the spawn instead of having different loops for each type of enemy

--[IsComponent]
function C_SpawnEnemy()
    local var = {
        
        --[SerializeInt]
        numberOfDummee = 0,

        --[SerializeInt]
        numberOfSquinky = 0

    }
    return function()
        return var
    end
end

--[IsSystem]
function S_SpawnEnemy(e)

    local spawnEnemyComponent = GetComponent(e, "C_SpawnEnemy")
    local unoccupiedTileList = AllUnoccupiedFloorTiles()
    print("[SpawnEnemy.lua] Spawning enemies!")

    -- spawn Dummees
    for i = 1, spawnEnemyComponent.numberOfDummee do
    
        -- first, get a random tile
        local randomTile = RandomUnoccupiedFloorTile()
        local randomTileGridCell = GetGridCell(randomTile)

        -- then instance a dummee on that tile
        InstancePrefabParentedOnGridNow(GetEntity("Floor"), "Dummee", randomTileGridCell.x, randomTileGridCell.y + 1, randomTileGridCell.z)    
    end

    -- then do the same for Squinkies
    for i = 1, spawnEnemyComponent.numberOfSquinky do
    
        -- first, get a random tile
        local randomTile = RandomUnoccupiedFloorTile()
        local randomTileGridCell = GetGridCell(randomTile)

        -- then instance a dummee on that tile
        InstancePrefabParentedOnGridNow(GetEntity("Floor"), "Squinky", randomTileGridCell.x, randomTileGridCell.y + 1, randomTileGridCell.z)
    end

    print("[SpawnEnemy.lua] Removing SpawnEnemy entity!")

    -- remove itself after spawning enemies
    RemoveInstance(e)
end
