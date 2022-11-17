-- Attached to GameManager entity.
-- At the start of each level, this script will spawn enemies at random locations.

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
    local tileList = EntitiesWithScriptComponent("C_FloorTile")

    while (spawnEnemyComponent.numberOfDummee > 0) do
    
        -- spawn dummees here
    
    end

    while (spawnEnemyComponent.numberOfSquinky > 0) do
    
        -- spawn squinkies here
    
    end

end
