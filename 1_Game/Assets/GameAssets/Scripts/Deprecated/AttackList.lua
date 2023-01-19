-- The AttackList component contains a list of all attacks, their base damage, and other information.
-- May need to de-serialize this component to save player progress.

--[IsComponent]
function C_AttackList()
    local var = {
        
       --[SerializeString]
       AttackList = "AttackList",
       
        -- access each attack using the ID number commented above it. 
        -- each attack is an array containing { attack name, base damage, special property, component name } 
        attackList = {

            -- 1
            { "frontJabBlue", 3, 0, "C_FrontJab" },

            -- 2
            { "frontJabPink", 5, 0, "C_FrontJab" },

            -- 3
            { "frontJabGold", 7, 0, "C_FrontJab" },

            -- 4
            { "bigSwingBlue", 2, 0, "C_BigSwing" },

            -- 5
            { "bigSwingPink", 5, 0, "C_BigSwing" },

            -- 6
            { "bigSwingGold", 7, 0, "C_BigSwing" },

            -- 7
            { "groundSmashBlue", 1, 0, "C_GroundSmash" },

            -- 8
            { "groundSmashPink", 3, 0, "C_GroundSmash" },

            -- 9
            { "groundSmashGold", 5, 0, "C_GroundSmash" },

            -- 10
            { "energyBoltBlue", 2, 0, "C_EnergyBolt" },

            -- 11
            { "energyBoltPink", 4, 0, "C_EnergyBolt" },

            -- 12
            { "energyBoltGold", 6, 0, "C_EnergyBolt" },

            -- 13 (blue version has 4-no AOE)
            { "projectileBlue", 1, 0, "C_Projectile" },

            -- 14 (pink version has 4-directional AOE)
            { "projectilePink", 3, 1, "C_Projectile" },

            -- 15 (gold version has 8-directional AOE)
            { "projectileGold", 5, 2, "C_Projectile" },

            -- 16 (blue version immobilises enemy for 2 turns)
            { "corporikinesisBlue", 0, 0, "C_Corporikinesis" },

            -- 17 (pink version immobilises enemy for 3 turns)
            { "corporikinesisPink", 0, 1, "C_Corporikinesis" },

            -- 18 (gold version makes enemy attack closest character, for 3 turns)
            { "corporikinesisGold", 0, 2, "C_Corporikinesis" },

            -- 19 (blue version can only freeze empty block, melts after 2 turns)
            { "cryogenesisBlue", 0, 0, "C_Cryogenesis" },

            -- 20 (pink version can freeze an enemy, melts after 2 turns)
            { "cryogenesisPink", 0, 1, "C_Cryogenesis" },

            -- 21 (gold version can freeze an enemy and deal DoT, melts after 2 turns)
            { "cryogenesisGold", 0, 2, "C_Cryogenesis" },

            -- 22 (blue version deal DoT on a tile for 2 turns)
            { "hydrokinesisBlue", 2, 2, "C_Hydrokinesis" },

            -- 23 (pink version deal DoT on a tile for 3 turns, and lifts Triton up 1 tile-height)
            { "hydrokinesisPink", 3, 2, "C_Hydrokinesis" },

            -- 24 (gold version deal DoT on a tile for 3 turns, and lifts Triton up 2 tile-height)
            { "hydrokinesisGold", 3, 2, "C_Hydrokinesis" },

            -- 25 (blue version moves one enemy to a different tile, up to 4 tiles away)
            { "levitationBlue", 0, 4, "C_Levitation" },

            -- 26 (pink version moves one enemy to a different tile, up to 4 tiles away)
            { "levitationPink", 0, 5, "C_Levitation" },

            -- 27 (gold version moves one enemy to a different tile, up to 4 tiles away)
            { "levitationGold", 0, 6, "C_Levitation" }
        }

        -- OLD ATTACK LIST
        -- playerAttackList = {

        --     -- 1
        --     frontJabBlue = { "frontJabBlue", 3, 0 },

        --     -- 2
        --     frontJabPink = { "frontJabPink", 5, 0 },

        --     -- 3
        --     frontJabGold = { "frontJabGold", 7, 0 },

        --     -- 4
        --     bigSwingBlue = { "bigSwingBlue", 2, 0 },

        --     -- 5
        --     bigSwingPink = { "bigSwingPink", 5, 0 },

        --     -- 6
        --     bigSwingGold = { "bigSwingGold", 7, 0 },

        --     -- 7
        --     groundSmashBlue = { "groundSmashBlue", 1, 0 },

        --     -- 8
        --     groundSmashPink = { "groundSmashPink", 3, 0 },

        --     -- 9
        --     groundSmashGold = { "groundSmashGold", 5, 0 },

        --     -- 10
        --     energyBoltBlue = { "energyBoltBlue", 2, 0 },

        --     -- 11
        --     energyBoltPink = { "energyBoltPink", 4, 0 },

        --     -- 12
        --     energyBoltGold = { "energyBoltGold", 6, 0 },

        --     -- 13 (blue version has 4-no AOE)
        --     projectileBlue = { "projectileBlue", 1, 0 },

        --     -- 14 (pink version has 4-directional AOE)
        --     projectilePink = { "projectilePink", 3, 1 },

        --     -- 15 (gold version has 8-directional AOE)
        --     projectileGold = { "projectileGold", 5, 2 },

        --     -- 16 (blue version immobilises enemy for 2 turns)
        --     corporikinesisBlue = { "corporikinesisBlue", 0, 0 },

        --     -- 17 (pink version immobilises enemy for 3 turns)
        --     corporikinesisPink = { "corporikinesisPink", 0, 1 },

        --     -- 18 (gold version makes enemy attack closest character, for 3 turns)
        --     corporikinesisGold = { "corporikinesisGold", 0, 2 },

        --     -- 19 (blue version can only freeze empty block, melts after 2 turns)
        --     cryogenesisBlue = { "cryogenesisBlue", 0, 0 },

        --     -- 20 (pink version can freeze an enemy, melts after 2 turns)
        --     cryogenesisPink = { "cryogenesisPink", 0, 1 },

        --     -- 21 (gold version can freeze an enemy and deal DoT, melts after 2 turns)
        --     cryogenesisGold = { "cryogenesisGold", 0, 2 },

        --     -- 22 (blue version deal DoT on a tile for 2 turns)
        --     hydrokinesisBlue = { "hydrokinesisBlue", 2, 2 },

        --     -- 23 (pink version deal DoT on a tile for 3 turns, and lifts Triton up 1 tile-height)
        --     hydrokinesisPink = { "hydrokinesisPink", 3, 2 },

        --     -- 24 (gold version deal DoT on a tile for 3 turns, and lifts Triton up 2 tile-height)
        --     hydrokinesisGold = { "hydrokinesisGold", 3, 2 },

        --     -- 25 (blue version moves one enemy to a different tile, up to 4 tiles away)
        --     levitationBlue = { "levitationBlue", 0, 4 },

        --     -- 26 (pink version moves one enemy to a different tile, up to 4 tiles away)
        --     levitationPink = { "levitationPink", 0, 5 },

        --     -- 27 (gold version moves one enemy to a different tile, up to 4 tiles away)
        --     levitationGold = { "levitationGold", 0, 6 }
        -- }
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_AttackList(e)

    -- local attackListComponent = GetComponent(GetEntity("CombatManager"), "C_AttackList")
    -- print(#attackListComponent.attackList)

end