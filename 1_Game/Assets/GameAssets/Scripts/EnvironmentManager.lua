--[IsComponent]
function C_EnvironmentManager()
    local var = {
    }
    return function()
        return var
    end
end

function OnLoad_EnvironmentManager(e)
    print("Setting up environment.")

    -- find all TilePrefab (recognized by C_FloorTile) and instance Env_Flora_1 on it
    local grass = "2D_Flora_Grass"
    local corals = "2D_Flora_0"
    local tiles = EntitiesWithScriptComponent("C_FloorTile")
    for i = 1, #tiles do
        -- spawn 2-5 grass
        -- random amount number
        local rng_amount = GenerateRandomNumberInRange(5, 10)
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
        -- spawn 0-2 corals
        local to_spawn_corals = GenerateRandomNumberInRange(1, 10)
        if to_spawn_corals < 4 then
            rng_amount = GenerateRandomNumberInRange(0, 1)
            for n = 1, rng_amount do
                -- get tile transform half vertical extent
                local tile_transform = GetTransform(tiles[i])
                local env = InstancePrefabParentedNow(tiles[i], "Env_Flora_Wave", 0, tile_transform.scale.y, 0)
                local env_transform = GetTransform(env)

                -- random grass
                local rng_grass = GenerateRandomNumberInRange(2, 5)
                ChangeTexture(env, corals .. rng_grass)

                -- random scale
                local rng_scale_x = (GenerateRandomNumberInRange(1, 100) / 200.0 + 0.5) / 1.0
                local rng_scale_y = (GenerateRandomNumberInRange(1, 100) / 200.0 + 0.5) / 1.0
                env_transform.scale.x = rng_scale_x
                env_transform.scale.y = rng_scale_y
                env_transform.position.y = env_transform.position.y + env_transform.scale.y

                -- random x & z
                local rng_position_x = (0.5 - GenerateRandomNumberInRange(1, 100) / 100.0)
                local rng_position_z = (0.5 - GenerateRandomNumberInRange(1, 100) / 100.0)
                env_transform.position.x = rng_position_x
                env_transform.position.z = rng_position_z
            end
        end
    end

    -- randomly spawn 3 env flora 1 on 3 tiles
    for i = 0, 2 do
        local tile_index = GenerateRandomNumberInRange(1, #tiles) - 1
        local tile_transform = GetTransform(tiles[tile_index])
        local glowy_plant = InstancePrefabParentedNow(tiles[tile_index], "Env_Flora_1", 0, tile_transform.scale.y, 0)
        local glowy_plant_transform = GetTransform(glowy_plant)
        glowy_plant_transform.position.y = glowy_plant_transform.position.y + glowy_plant_transform.scale.y
    end

    -- randomly spawn big corals around board
    for i = 0, 50 do
        local env = InstancePrefabParentedNow(e, "Env_Flora_Wave", 0, 0, 0)
        local env_transform = GetTransform(env)

        -- random grass
        local rng_grass = GenerateRandomNumberInRange(2, 5)
        ChangeTexture(env, corals .. rng_grass)

        -- random scale
        local rng_scale_x = (GenerateRandomNumberInRange(1, 100) / 150.0 + 0.2) * 3.0
        local rng_scale_y = (GenerateRandomNumberInRange(1, 100) / 150.0 + 0.2) * 6.0
        env_transform.scale.x = rng_scale_x
        env_transform.scale.y = rng_scale_y
        env_transform.position.y = env_transform.position.y + env_transform.scale.y

        -- random x & z
        local board_side = GenerateRandomNumberInRange(1, 4)
        if board_side == 1 then
            local rng_position_x = GenerateRandomNumberInRange(1, 300) / 100.0 + 12
            local rng_position_z = GenerateRandomNumberInRange(1, 1600) / 100 - 12
            env_transform.position.x = rng_position_x
            env_transform.position.z = rng_position_z
        elseif board_side == 2 then
            local rng_position_x = -8 - GenerateRandomNumberInRange(1, 300) / 100.0
            local rng_position_z = GenerateRandomNumberInRange(1, 1600) / 100 - 12
            env_transform.position.x = rng_position_x
            env_transform.position.z = rng_position_z
        elseif board_side == 3 then
            local rng_position_x = GenerateRandomNumberInRange(1, 1600) / 100.0 - 7
            local rng_position_z = -14 - GenerateRandomNumberInRange(1, 300) / 100
            env_transform.position.x = rng_position_x
            env_transform.position.z = rng_position_z
        elseif board_side == 4 then
            local rng_position_x = GenerateRandomNumberInRange(1, 1600) / 100.0 - 7
            local rng_position_z = GenerateRandomNumberInRange(1, 300) / 100 + 5
            env_transform.position.x = rng_position_x
            env_transform.position.z = rng_position_z
        end

    end

    SetCameraPosition(0, 60, 200)
    SetCameraLookAt(0, 100, -50)
    SetCameraMoveSpeed(1.0)
    SetCameraPanSpeed(0.7)
    SetCameraNextLookAt(1, 0, -4)
    SetCameraNextPosition(-3, 12, -15)

    print("Finsihed setting up environment.")
end

--[IsSystem]
function S_EnvironmentManager(e)
    -- shortcut to switch to main menu
    if CheckKeyPress(77) then
        print("test")
        ChangeScene("MainmenuScreen", true)
    end
end
