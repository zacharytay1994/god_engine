--[IsComponent]
function C_SplashScreen()
    local var =
    {
    }
    print("[SplashScreen.lua] C_SplashScreen")
    return function() return var
    end
end

-- object location in scene
local objs_row_min_ = 0
local objs_row_max_ = 20
local objs_column_min_ = -2
local objs_column_max_ = 8
local objs_row_max_div2_ = objs_row_max_ / 2

-- random generated rocks
function GenerateRandomRocks(x, y, z)

    -- generate random position data
    local rng_pos = GenerateRandomNumberInRange(1, 5)

    -- create rock
    local rock_entity = InstancePrefabNow("SS_Rock", rng_pos + x, y, rng_pos + z)

    -- random generated seeds
    ChangeModel(rock_entity, "rock0" .. tostring(GenerateRandomNumberInRange(1, 4)))

    -- get rock transformation data
    local rock_transform = GetTransform(rock_entity)

    -- set transform scale to rock
    local rng_scale = GenerateRandomNumberInRange(1, 10) / 20.0
    rock_transform.scale.x = rng_scale
    rock_transform.scale.y = rng_scale
    rock_transform.scale.z = rng_scale
    rock_transform.position.y = rock_transform.position.y + (rock_transform.scale.y / 2.0);
end

-- random generated seaweeds
function GenerateRandomSeaweeds(x, y, z)

    -- generate random position
    local rng_pos = GenerateRandomNumberInRange(1, 10)

    if rng_pos + x ~= objs_row_max_div2_ then
        local seaweed_entity = InstancePrefabNow("SS_Seaweed", rng_pos + x, y, rng_pos + z)

        ChangeModel(seaweed_entity, "Plant_Seaweed0" .. tostring(GenerateRandomNumberInRange(1, 3)))

        local seaweed_transform = GetTransform(seaweed_entity)

        local rng_scale = GenerateRandomNumberInRange(1, 10) / 10.0
        seaweed_transform.scale.x = rng_scale
        seaweed_transform.scale.y = rng_scale
        seaweed_transform.scale.z = rng_scale
        seaweed_transform.position.y = 1.0 * seaweed_transform.scale.y
    end

end

-- initialization of data before play button is trigged
function OnLoad_SplashScreen()

    local camera      = FindCameraObject()
    camera.position.x = objs_row_max_div2_
    camera.position.y = 2
    camera.position.z = 10

    -- create corals based on row and column
    for j = objs_column_min_, objs_column_max_, 1 do
        for i = objs_row_min_, objs_row_max_, 1 do
            GenerateRandomRocks(i, 0, j)
            GenerateRandomSeaweeds(i, 0, j)
        end
    end

    -- create directional light at the door
    InstancePrefab("SS_DirectionalLight", objs_row_max_div2_, 6, -6)

    InstancePrefab("SS_RockFlat", objs_row_max_div2_, 0, -7)

    -- create door at the end of the corals
    InstancePrefab("SS_Door", objs_row_max_div2_, 2.5, -7)

    -- create terrain floor
    InstancePrefab("SS_Terrain", objs_row_max_div2_, 0, -38)

    -- create rectagular plain floor / using precreated prefab
    InstancePrefab("FloorPrefab", objs_row_max_div2_, 0, 0)

    print("[SplashScreen.lua] OnLoad_SplashScreen")
end

--[IsSystem]
function S_SplashScreen(e)
    local dt = GetDeltaTime()
    local speed = 0.1 * dt

    -- to get identity of 1 object
    local door_in_scene = EntitiesWithScriptComponent("C_DoorData")[1]
    local door_transform = GetTransform(door_in_scene)

    -- // this is how to print id of the object
    -- print(GetEntityData(door_in_scene).id)

    IsMoving = false
    local camera = FindCameraObject()
    -- move back camera
    if camera.position.z > 5 then
        camera.position.z = camera.position.z - 0.4 * dt
    else
        IsMoving = true
    end

    -- shift the door position up to stay on top of 0
    door_transform.position.y = door_transform.scale.y

    -- scale the door
    if door_transform.scale.x < 5 and not IsMoving then
        door_transform.scale.x = door_transform.scale.x + speed
        door_transform.scale.y = door_transform.scale.y + speed
        door_transform.scale.z = door_transform.scale.z + speed
    end
end

--[IsComponent]
function C_CoralData()
    local var = {
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_CoralData(e)
end

--[IsComponent]
function C_DoorData()
    local var = {
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_DoorData(e)
end

--[IsComponent]
function C_TerrainData()
    local var = {
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_TerrainData(e)
end
