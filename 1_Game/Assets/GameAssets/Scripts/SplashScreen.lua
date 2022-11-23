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
local minRows = 0
local maxRows = 10
local minColumns = -5
local maxColumns = 20
local centerX = 10

-- random generated seaweeds
function GenerateRandomSeaweeds(arr)

    for i = 0, 150, 1 do
        local rng_pos_x = GenerateRandomNumberInRange(minColumns, maxColumns)
        local rng_pos_z = GenerateRandomNumberInRange(minRows, maxRows)

        if (arr[rng_pos_z * maxColumns + rng_pos_x] == false) then

            -- set seaweed position
            local seaweed_entity = InstancePrefabNow("SS_Seaweed", rng_pos_x, 0, rng_pos_z)

            ChangeModel(seaweed_entity, "Plant_Seaweed0" .. tostring(GenerateRandomNumberInRange(1, 4)))
            local seaweed_transform = GetTransform(seaweed_entity)

            --set seaweed scale
            local rng_scale = GenerateRandomNumberInRange(2, 8) / 10.0
            seaweed_transform.scale.x = rng_scale
            seaweed_transform.scale.y = rng_scale
            seaweed_transform.scale.z = rng_scale
            seaweed_transform.position.y = 1.0 * seaweed_transform.scale.y

            arr[rng_pos_z * maxColumns + rng_pos_x] = true
        end
    end
end

-- random generated rocks
function GenerateRandomRocks(arr)

    -- generate random position data
    for i = 0, 100, 1 do
        local rng_pos_x = GenerateRandomNumberInRange(minColumns, maxColumns)
        local rng_pos_z = GenerateRandomNumberInRange(minRows, maxRows)

        -- create rock
        if (arr[rng_pos_z * maxColumns + rng_pos_x] == false) then
            local rock_entity = InstancePrefabNow("SS_Rock", rng_pos_x, 0, rng_pos_z)
            ChangeModel(rock_entity, "rock0" .. tostring(GenerateRandomNumberInRange(1, 4)))

            -- get rock transformation data
            local rock_transform = GetTransform(rock_entity)

            -- set transform scale to rock
            local rng_scale = GenerateRandomNumberInRange(1, 10) / 10.0
            rock_transform.scale.x = rng_scale
            rock_transform.scale.y = rng_scale
            rock_transform.scale.z = rng_scale
            rock_transform.position.y = 1.0 * (rock_transform.scale.y / 2.0);

            arr[rng_pos_z * maxColumns + rng_pos_x] = true
        end
    end
end

function GenerateRandomCorals(arr)
    -- lua array starts from 1
    corals = { "plant02_CoralSticks", "Plant_Coral01", "Plant_Coral02" }

    -- generate random position data
    for i = 0, 100, 1 do
        local rng_pos_x = GenerateRandomNumberInRange(minColumns, maxColumns)
        local rng_pos_z = GenerateRandomNumberInRange(minRows, maxRows)
        -- create corals
        if (arr[rng_pos_z * maxColumns + rng_pos_x] == false) then
            local coral_entity = InstancePrefabNow("SS_Coral", rng_pos_x, 1.01, rng_pos_z)

            ChangeModel(coral_entity, corals[(GenerateRandomNumberInRange(1, 3))])

            -- get corals transformation data
            local coral_transform = GetTransform(coral_entity)

            -- set transform scale to corals
            local rng_scale = GenerateRandomNumberInRange(2, 10) / 10.0

            coral_transform.scale.x = rng_scale
            coral_transform.scale.y = rng_scale
            coral_transform.scale.z = rng_scale
            coral_transform.position.y = coral_transform.scale.y
            coral_transform.rotation.y = (GenerateRandomNumberInRange(1, 18) * 10)

            arr[rng_pos_z * maxColumns + rng_pos_x] = true
        end
    end
end

function PlantBubbleEmitter(arr)
    for row = minRows, maxRows, 1 do
        for col = minColumns, maxColumns, 1 do
            if (arr[row * maxColumns + col] == false) then
                local plant_bubble_entity = InstancePrefabNow("SS_FloraBubbleEmitter", col, 1, row)

                ChangeTexture(plant_bubble_entity, "2D_Flora_0" .. tostring(GenerateRandomNumberInRange(2, 5)))


                local plant_bubble_transform = GetTransform(plant_bubble_entity)

                local rng_scale = GenerateRandomNumberInRange(4, 8) / 10.0

                plant_bubble_transform.scale.x = rng_scale
                plant_bubble_transform.scale.y = rng_scale
                plant_bubble_transform.scale.z = rng_scale

                plant_bubble_transform.position.y = plant_bubble_transform.scale.y
                arr[row * maxColumns + col] = true
            end
        end
    end
end

-- create a pathway towards the door
function GenerateStonePathway()
    for i = 0, 4, 1 do
        InstancePrefabNow("SS_StonePath", 10.0, 0, 10.0 + (-4 * i))
    end
end

-- create few seahorses in scene
function GenerateSeahorse(vel_x, vel_y, vel_z, pos_x, pos_y, pos_z, index)

    for i = 0, index, 1 do
        local rng_pos_y = GenerateRandomNumberInRange(1, 10) / 10.0
        local seahorse = InstancePrefabNow("SS_Seahorse", pos_x + (i * 1), pos_y + rng_pos_y, pos_z)
        local seahorse_data = GetComponent(seahorse, "C_SeahorseData")
        seahorse_data.velocity_x_ = vel_x
        seahorse_data.velocity_y_ = vel_y
        seahorse_data.velocity_z_ = vel_z
        seahorse_data.position_y_ = pos_y + rng_pos_y
        seahorse_data.timer_ = i
        local seahorse_transform = GetTransform(seahorse)
        if vel_x < 0 then
            seahorse_transform.rotation.y = -80
        end
        local rng_scale = GenerateRandomNumberInRange(5, 10) / 10.0
        seahorse_transform.scale.x = rng_scale
        seahorse_transform.scale.y = rng_scale
        seahorse_transform.scale.z = rng_scale
    end
end

-- initialization of data before play button is trigged
function OnLoad_SplashScreen()

    local camera      = FindCameraObject()
    camera.position.x = centerX
    camera.position.y = 2.5
    camera.position.z = 10

    objs_placement = {} -- new array

    for row = minRows, maxRows, 1 do
        for col = minColumns, maxColumns, 1 do
            if (col > 8 and col < 12) then
                objs_placement[row * maxColumns + col] = true; -- array initialization
            else
                objs_placement[row * maxColumns + col] = false; -- array initialization
            end
        end
    end

    -- create random rocks in the scene
    GenerateRandomRocks(objs_placement)

    -- create random seaweeds in the scene
    GenerateRandomSeaweeds(objs_placement)

    GenerateRandomCorals(objs_placement)

    PlantBubbleEmitter(objs_placement)

    -- create directional light at the door
    InstancePrefab("SS_DirectionalLight", centerX, 10, -10)

    -- create rock below the door
    InstancePrefab("SS_RockFlat", centerX, 0, -7)

    -- create door at the end of the corals
    InstancePrefab("SS_Door", centerX, 1.8, -7)

    -- create terrain floor
    InstancePrefab("SS_Terrain", centerX, 0, -38)

    -- create rectagular plain floor / using precreated prefab
    InstancePrefab("FloorPrefab", centerX, 0, 0)

    -- create large kelp
    InstancePrefab("SS_Kelp", 13, 0.5, -2.8)

    InstancePrefab("SS_TreeBranch", 7.5, 1.5, 5.0)

    GenerateStonePathway()

    GenerateSeahorse(1, 0, 0, 4, 2, -2, 3)

    GenerateSeahorse(-1, 0, 0, 14, 2, -2, 3)

    InstancePrefab("SS_Starfish", centerX, 0.2, 4.0)

    print("[SplashScreen.lua] OnLoad_SplashScreen")
end

--[IsSystem]
function S_SplashScreen(e)
    local dt = GetDeltaTime()
    local speed = 0.1 * dt


    local seahorse_in_scene = EntitiesWithScriptComponent("C_SeahorseData")
    for i = 1, #seahorse_in_scene do
        local seahorse_transform = GetTransform(seahorse_in_scene[i])
        local seahorse_data = GetComponent(seahorse_in_scene[i], "C_SeahorseData")

        seahorse_transform.position.x = seahorse_transform.position.x + seahorse_data.velocity_x_ * speed
        seahorse_transform.position.y = seahorse_transform.position.y + seahorse_data.velocity_y_ * speed
        seahorse_transform.position.z = seahorse_transform.position.z + seahorse_data.velocity_z_ * speed

        seahorse_data.timer_ = seahorse_data.timer_ + dt

        if seahorse_data.velocity_x_ < 0 then
            seahorse_transform.position.y = seahorse_data.position_y_ - math.sin(seahorse_data.timer_) / 2.0
        else
            seahorse_transform.position.y = seahorse_data.position_y_ + math.cos(seahorse_data.timer_) / 2.0
        end
    end

    -- to get identity of 1 object
    local door_in_scene = EntitiesWithScriptComponent("C_DoorData")[1]
    local door_transform = GetTransform(door_in_scene)

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
function C_SeahorseData()
    local var = {
        velocity_x_ = 0,
        velocity_y_ = 0,
        velocity_z_ = 0,
        position_y_ = 0,
        timer_ = 0
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_SeahorseData(e)
end
