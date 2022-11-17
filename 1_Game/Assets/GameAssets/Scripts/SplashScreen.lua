--[IsComponent]
function C_SplashScreen()
    local var = {
        x = 0,
        y = 0,
        z = 0
    }
    print("[SplashScreen.lua] C_SplashScreen")
    return function() return var
    end
end

-- number of corals to spawn
local corals_row_min_ = 0
local corals_row_max_ = 20
local corals_column_min_ = 0
local corals_column_max_ = 5

-- initialization of data before play button is trigged
function OnLoad_SplashScreen()

    local camera      = FindCameraObject()
    camera.position.x = 0
    camera.position.y = 2
    camera.position.z = 10



    local c_tbl =
    {
        [1] = print "1",
        [2] = print "2"
    }
    choice = 1
    print("choice", choice)
    local func = c_tbl[choice]
    if (func) then
        func()
    else
        print " The program has been terminated."
        print " Thank you!";
    end

    -- create corals based on row and column
    for j = corals_column_min_, corals_column_max_, 1 do
        for i = corals_row_min_, corals_row_max_, 1 do
            InstancePrefab("SS_Coral", i - (corals_row_max_ / 2), 0, j + 0.5)
        end
    end

    -- create directional light at the door
    InstancePrefab("SS_DirectionalLight", 0, 6, -6)

    -- create door at the end of the corals
    InstancePrefab("SS_Door", 0, 0, -corals_column_max_)

    -- create terrain floor
    InstancePrefab("SS_Terrain", 0, 0, -38)

    -- create rectagular plain floor / using precreated prefab
    InstancePrefab("FloorPrefab", 0, 0, 0)

    print("[SplashScreen.lua] OnLoad_SplashScreen")
end

--[IsSystem]
function S_SplashScreen(e)
    local dt = GetDeltaTime()
    local speed = 0.1 * dt

    -- get all corals in the scene
    local corals_in_scene = EntitiesWithScriptComponent("C_CoralData")

    -- for each coral in the scene
    for i = 1, #corals_in_scene do
        local coral_transform = GetTransform(corals_in_scene[i])

        -- set transform scale to coral scale
        coral_transform.scale.x = math.abs(coral_transform.position.x) * 0.1
        coral_transform.scale.y = math.abs(coral_transform.position.x) * 0.1
        coral_transform.scale.z = math.abs(coral_transform.position.x) * 0.1
        coral_transform.position.y = coral_transform.scale.y;

    end

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
    if door_transform.scale.x < 4.5 and not IsMoving then
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
