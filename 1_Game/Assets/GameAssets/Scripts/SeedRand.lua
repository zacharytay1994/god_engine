-- This script will run seed rand() only once, at the start of the program.

-- TODO: 
-- 1) Try Removing the seed rand object after seeding so that S_SeedRand() doesn't keep running

--[IsComponent]
function C_SeedRand()
    local var = {

        --[SerializeBool]
        seedAtStart = true,

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

    if (seedRandComponent.seedAtStart == false) then
        return  
    end

    -- press V to check RandomProbability()
    if (CheckKeyPress(86)) then
        print("RandomProbability:", GenerateRandomProbability())
    end

    if (seedRandComponent.seeded == false) then
        srand()
        seedRandComponent.seeded = true
        print("[SeedRand.lua] Seeded rand().\n")
    end
end
