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
    local grass = "2D_Flora_Grass"
    local tiles = EntitiesWithScriptComponent("C_FloorTile")
    for i = 1, #tiles do
        -- random amount number
        local rng_amount = GenerateRandomNumberInRange(10, 20)
        for n = 1, rng_amount do
            -- get tile transform half vertical extent
            local tile_transform = GetTransform(tiles[i])
            local env = InstancePrefabParentedNow(tiles[i], "Env_Flora_Wave", 0, tile_transform.scale.y, 0)
            local env_transform = GetTransform(env)

            -- random grass
            local rng_grass = GenerateRandomNumberInRange(1, 6)
            ChangeTexture(env, grass .. rng_grass)

            -- random scale
            local rng_scale_x = (GenerateRandomNumberInRange(1, 100) / 200.0 + 0.5) / 2.0
            local rng_scale_y = (GenerateRandomNumberInRange(1, 100) / 200.0 + 0.5) / 2.0
            env_transform.scale.x = rng_scale_x
            env_transform.scale.y = rng_scale_y
            env_transform.position.y = env_transform.position.y + env_transform.scale.y

            -- random x & z
            local rng_position_x = (0.5 - GenerateRandomNumberInRange(1, 100) / 100.0)
            local rng_position_z = (0.5 - GenerateRandomNumberInRange(1, 100) / 100.0)
            env_transform.position.x = rng_position_x
            env_transform.position.z = rng_position_z
        end

        -- local tile_transform = GetTransform(tiles[i])
        -- InstancePrefabParentedNow(tiles[i], "Env_Flora_1", 0, tile_transform.scale.y, 0)

    end

    print("Finsihed setting up environment.")
end

--[IsSystem]
function S_EnvironmentManager(e)
end
