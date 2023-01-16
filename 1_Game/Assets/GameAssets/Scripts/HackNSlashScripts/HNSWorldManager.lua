--[IsComponent]
function C_HNSWorldManager()
    local var = {
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_HNSWorldManager(e)
end

function OnLoad_HNSWorldManager(e)
    -- spawn 5 squid enemies at random x,z at height y
    print("HNS - Spawning Enemies")
    srand()
    for i = 0, 5, 1 do
        local x = GenerateRandomNumberInRange(1,100)/100.0 * 8.0
        local z = GenerateRandomNumberInRange(1,100)/100.0 * 8.0
        local squid = InstancePrefabNow("HNSEnemy_Squid",x,10,z)
        local physics_controller = GetPhysicsController(squid)
        physics_controller.speed = 0.2
    end
end
