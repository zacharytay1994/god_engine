--[IsComponent]
function C_EnvironmentManager()
    local var = {
    }
    return function()
        return var
    end
end

function OnLoad_EnvironmentManager()
    print("Setting up environment.")
    -- find all TilePrefab (recognized by C_FloorTile) and instance Env_Flora_1 on it
    local tiles = EntitiesWithScriptComponent("C_FloorTile")
    for i = 1, #tiles do
        -- get tile transform half vertical extent
        local tile_transform = GetTransform(tiles[i])
        local env = InstancePrefabParentedNow(tiles[i], "Env_Flora_1", 0, tile_transform.scale.y, 0)
        local env_transform = GetTransform(env)
        env_transform.position.y = env_transform.position.y + env_transform.scale.y
    end

    print("Finsihed setting up environment.")
end

--[IsSystem]
function S_EnvironmentManager(e)
end
