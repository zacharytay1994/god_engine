-- The AttackList component contains a list of all attacks, their base damage, and other information.
-- May need to de-serialize this component to save player progress.

--[IsComponent]
function C_CombatManager()
    local var = {
        
        -- access each attack using the ID number commented above it. 
        -- each attack is an array containing { attack name, base damage, special property } 
        attackList = {

            -- 1
            frontJabBlue = { "frontJabBlue", 3, 0 },

            -- 2
            frontJabPink = { "frontJabPink", 5, 0 },

            -- 3
            frontJabGold = { "frontJabGold", 7, 0 },

            -- 4
            bigSwingBlue = { "bigSwingBlue", 2, 0 },

            -- 5
            bigSwingPink = { "bigSwingPink", 5, 0 },

            -- 6
            bigSwingGold = { "bigSwingGold", 7, 0 },

            -- 7
            groundSmashBlue = { "groundSmashBlue", 1, 0 },

            -- 8
            groundSmashPink = { "groundSmashPink", 3, 0 },

            -- 9
            groundSmashGold = { "groundSmashGold", 5, 0 },

            -- 10
            energyBoltBlue = { "energyBoltBlue", 2, 0 },

            -- 11
            energyBoltPink = { "energyBoltPink", 4, 0 },

            -- 12
            energyBoltGold = { "energyBoltGold", 6, 0 },

            -- 13 (blue version has 4-no AOE)
            projectileBlue = { "projectileBlue", 1, 0 },

            -- 14 (pink version has 4-directional AOE)
            projectilePink = { "projectilePink", 3, 1 },

            -- 15 (gold version has 8-directional AOE)
            projectileGold = { "projectileGold", 5, 2 },

            -- 16 (blue version immobilises enemy for 2 turns)
            corporikinesisBlue = { "corporikinesisBlue", 0, 0 },

            -- 17 (pink version immobilises enemy for 3 turns)
            corporikinesisPink = { "corporikinesisPink", 0, 1 },

            -- 18 (gold version makes enemy attack closest character, for 3 turns)
            corporikinesisGold = { "corporikinesisGold", 0, 2 },

            -- 19 (blue version can only freeze empty block, melts after 2 turns)
            cryogenesisBlue = { "cryogenesisBlue", 0, 0 },

            -- 20 (pink version can freeze an enemy, melts after 2 turns)
            cryogenesisPink = { "cryogenesisPink", 0, 1 },

            -- 21 (gold version can freeze an enemy and deal DoT, melts after 2 turns)
            cryogenesisGold = { "cryogenesisGold", 0, 2 },

            -- 22 (blue version deal DoT on a tile for 2 turns)
            hydrokinesisBlue = { "hydrokinesisBlue", 2, 2 },

            -- 23 (pink version deal DoT on a tile for 3 turns, and lifts Triton up 1 tile-height)
            hydrokinesisPink = { "hydrokinesisPink", 3, 2 },

            -- 24 (gold version deal DoT on a tile for 3 turns, and lifts Triton up 2 tile-height)
            hydrokinesisGold = { "hydrokinesisGold", 3, 2 },

            -- 25 (blue version moves one enemy to a different tile, up to 4 tiles away)
            levitationBlue = { "levitationBlue", 0, 4 },

            -- 26 (pink version moves one enemy to a different tile, up to 4 tiles away)
            levitationPink = { "levitationPink", 0, 5 },

            -- 27 (gold version moves one enemy to a different tile, up to 4 tiles away)
            levitationGold = { "levitationGold", 0, 6 }
        }

    }
    return function()
        return var
    end
end