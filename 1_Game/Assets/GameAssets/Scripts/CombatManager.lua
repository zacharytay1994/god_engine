-- How Combat will work:
-- 1) Only one character will be able to attack at a time (i.e. during their turn).
-- 2) When a character wants to attack a target, its attack script will have to pass 3 variables to CombatManager, which are the attacker entity, the target entity, and the attack type.
-- 3) CombatManager will then check if both the attacker and targer entities have the C_Character components before continuing.
-- 4) CombatManager will then calculate damage.
-- 5) The calculated damage will be temporarily stored in CombatManager.damage before it is deducted from the target's C_Character component.

-- Note: Player will have a PlayerAttack script, while each type of enemy will have a its own attack script (because they have different sets of attacks)

-- TODO:
-- 1) 

--[IsComponent]
function C_CombatManager()
    local var = {
        --[SerializeString]
        CombatManager = "CombatManager",

        --[SerializeBool]
        attackListAttached = false,

        -- damage for current attack
        damage = 0,

        -- attacking entity
        attacker = -1,

        -- defending entity
        defender = -1,

        -- attack type is an array containing attack name, base damage, special property
        attackType = nil,

        -- scales the damage of a critical hit
        critDamageMultiplier = 1.5,

        -- set to true if player manages to pass the crit hit mini game
        critSuccess = false,

        -- damage dealt to characters that fall off a ledge
        fallDamage = 3
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_CombatManager(e)
    
    -- getting CombatManager entity and component
    local combatManagerEntity = GetEntity("CombatManager")
    if (combatManagerEntity ~= -1) then 
        combatManagerComponent = GetComponent(combatManagerEntity, "C_CombatManager")
    end

    if (GetComponent(combatManagerEntity, "C_AttackList") ~= nil) then
        combatManagerComponent.attackListAttached = true
    end

    -- only calculate damage if all the required components are available
    if (combatManagerComponent.attacker ~= -1 and combatManagerComponent.defender ~= -1 and combatManagerComponent.attackType ~= nil) then
                    
        print("[CombatManager.lua] CombatManager received all info, proceeding to calculate damage.")
        
        -- getting the attacker and defenders' C_Character components
        local attackerCharacterComponent = GetComponent(combatManagerComponent.attacker, "C_Character")
        local defenderCharacterComponent = GetComponent(combatManagerComponent.defender, "C_Character")

        -- checking if both the attacker and defender have the C_Character component
        if (attackerCharacterComponent ~= nil and defenderCharacterComponent ~= nil) then 
                            
            -- get attacker strength
            local attackStrength = attackerCharacterComponent.strength

            -- get defender defence
            local defenderDefence = defenderCharacterComponent.defence

            -- get attack type base damage
            local baseDamage = combatManagerComponent.attackType[2]

            -- calculate damage and print results
            if (combatManagerComponent.critSuccess) then 
                combatManagerComponent.damage = ((attackStrength + baseDamage) * combatManagerComponent.critDamageMultiplier) - defenderDefence
                print("[CombatManager.lua] [(", attackStrength, "+", baseDamage, ") *", combatManagerComponent.critDamageMultiplier, "] -", defenderDefence, "=", combatManagerComponent.damage)
            else
                combatManagerComponent.damage = (attackStrength + baseDamage) - defenderDefence
                print("[CombatManager.lua] (", attackStrength, "+", baseDamage, ") -", defenderDefence, "=", combatManagerComponent.damage)
            end

            -- deduct defender's current HP
            defenderCharacterComponent.currentHP = defenderCharacterComponent.currentHP - combatManagerComponent.damage
            print("[CombatManager.lua]", EntityName(combatManagerComponent.attacker), "dealt", combatManagerComponent.damage, "damage to", EntityName(combatManagerComponent.defender), ".\n")

            -- reset variables
            combatManagerComponent.damage = 0
            combatManagerComponent.attacker = -1
            combatManagerComponent.defender = -1
            combatManagerComponent.attackType = nil

        else
            print("[CombatManager.lua] ERROR: Either the attacking entity or defending entity does not have C_Character component!!!")
        end
    end
end