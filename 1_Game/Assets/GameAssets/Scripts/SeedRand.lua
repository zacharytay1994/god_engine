-- This script will run seed rand() only once.

--[IsComponent]
function C_SeedRand()
    local var = {
        --[SerializeString]
        SeedRand = "SeedRand",

        --[SerializeBool]
        seeded = false
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_SeedRand(e)
    
    local seedRandEntity = GetEntity("SeedRand")
    if (seedRandEntity ~= -1) then
        seedRandComponent = GetComponent(seedRandEntity, "C_SeedRand")
    end

    if (seedRandComponent.seeded == false) then
        srand()
        seedRandComponent.seeded = true
        print("seeded rand()")
    end
end
