-- How Combat will work:
-- 1) Only one character will be able to attack at a time (i.e. during their turn).
-- 2) When a character wants to attack a target, its CharacterAttack script will have to pass 3 variables to CombatManager, which are the attacker entity, the target entity, and the attack type.
-- 3) CombatManager will then check if both the attacker and targer entities have the C_Character components before continuing.
-- 4) CombatManager will then calculate damage by referencing the AttackList (which will be stored in another script) and applying it into the damage equation.
-- 5) The calculated damage will be temporarily stored in CombatManager.damage before it is deducted from the target's C_Character component.

-- Note: if something doesn't work, try changing attacker and target nil value to nil instead of -1

-- TODO:
-- 1) Implement CharacterAttack script
-- 2) Implement AttackList script

--[IsComponent]
function C_CombatManager()
    local var = {
        --[SerializeString]
        CombatManager = "CombatManager",

        -- damage for current attack
        damage = 0,

        -- attacking entity
        attacker = -1,

        -- defending entity
        defender = -1,

        -- attack type is a number corresponding to an attack in the AttackList script
        -- 0 is nothing. the index starts from 1.
        attackType = 0,

        -- scales the damage of a critical hit
        critDamageMultiplier = 1.5,

        -- calculate damage once per attack.
        -- CharacterAttack script will use this variable as a sign to retrieve the calculated damage
        damageCalculated = false,

        -- CharacterAttack script will be responsible for telling CombatManager to reset variables 
        resetVariables = false
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

    if (combatManagerComponent.resetVariables) then 
        combatManagerComponent.damage = 0
        combatManagerComponent.attacker = -1
        combatManagerComponent.defender = -1
        combatManagerComponent.attackType = 0
        combatManagerComponent.damageCalculated = false
        combatManagerComponent.resetVariables = false
    end
    
    -- calculate damage once per attack
    if (combatManagerComponent.damageCalculated == false) then 
    
        -- only calculate damage if all the required components are available
        if (combatManagerComponent.attacker ~= -1 and combatManagerComponent.defender ~= -1 and combatManagerComponent.attackType ~= 0) then
            
            local attackerCharacterComponent = GetComponent(attackerCharacterComponent, "C_Character")
            local defenderCharacterComponent = GetComponent(defenderCharacterComponent, "C_Character")

            -- checking if both the attacker and defender have the C_Character component
            if (attackerCharacterComponent ~= nil and defenderCharacterComponent ~= nil) then 
                
                -- get attacker strength
                local attackStrength = attackerCharacterComponent.strength

                -- get defender defence
                local defenderDefence = defenderCharacterComponent.defence

                -- get attack type base damage
                -- TODO

                combatManagerComponent.damage = ((attackStrength + --[[base damage]]) * combatManagerComponent.critDamageMultiplier) - defenderDefence

                combatManagerComponent.damageCalculated = true
            end
        end
    end 
end

-- local bob_spin = GetComponent(e, "C_BobSpin")
    -- local transform = GetTransform(e)
    -- local dt = GetDeltaTime()
    -- transform.rotation.y = transform.rotation.y + bob_spin.SpinRate * dt
    -- bob_spin.BobCounter = bob_spin.BobCounter + bob_spin.BobRate * dt
    -- transform.position.y = math.sin(bob_spin.BobCounter) / 2.0

    -- if CheckKeyPress(65) then
    --     InstancePrefabParented("DownArrow", e)
    -- end
    -- if CheckKeyPress(66) then
    --     RemoveInstance(e)
    -- end