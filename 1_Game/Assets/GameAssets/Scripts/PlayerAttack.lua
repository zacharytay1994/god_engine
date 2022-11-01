-- This script will allow the player to perform an attack

-- TODO:
-- 1) Attacks should be selected by pressing the Attack Button UI. For now just press hotkeys.
-- 2) Click to select target entity. For now, the enemy is selected by default.

--[IsComponent]
function C_PlayerAttack()
    local var = {
        --[SerializeString]
        PlayerAttack = "PlayerAttack",

        -- currently selected attack (refer to AttackList) is an array containing { attack name, base damage, special property, component name }
        selectedAttack = nil,

        -- target entity
        targetEntity = -1,

        -- reset variables
        resetVariables = false,

        -- checks whether the attack script has performed its check
        attackCheckTriggered = false
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_PlayerAttack(e)
    
    -- check if TurnOrderManager entity exists
    local turnOrderManagerEntity = GetEntity("TurnOrderManager")
    if (turnOrderManagerEntity ~= -1) then

        -- getting the required entities and components
        local entityDataComponent = GetEntityData(e)
        turnOrderManagerComponent = GetComponent(turnOrderManagerEntity, "C_TurnOrderManager")
        local playerAttackComponent = GetComponent(e, "C_PlayerAttack")
        
        -- only run this script if its the character's turn
        if (turnOrderManagerComponent.currentTurn == entityDataComponent.id) then
                  
            -- resetting variables
            if (playerAttackComponent.resetVariables) then 
                print("[PlayerAttack.lua] Resetting variables!!!\n")
                playerAttackComponent.selectedAttack = nil
                playerAttackComponent.targetEntity = nil
                playerAttackComponent.attackCheckTriggered = false
                playerAttackComponent.resetVariables = false
            end
            
            --  check if CombatManager entity exists
            combatManagerEntity = GetEntity("CombatManager")
            if (combatManagerEntity ~= -1) then
                
                -- get C_AttackList
                attackListComponent = GetComponent(combatManagerEntity, "C_AttackList")

                if (attackListComponent ~= nil) then
                    attackList = attackListComponent.attackList
                else
                    print("[PlayerAttack.lua] ERROR: attackListComponent is nil!!!")
                end    
            end 

            -- select an attack ----------------------------------------------------------------------------------
            -- press 1 to select Front Jab (Blue)
            if (CheckKeyPress(49)) then
                playerAttackComponent.selectedAttack = attackList[1]
                print("\n[PlayerAttack.lua] Selected Player attack:", playerAttackComponent.selectedAttack[1], 
                      "Base damage:", playerAttackComponent.selectedAttack[2], 
                      "Special property:", playerAttackComponent.selectedAttack[3], "\n")
            end

            -- -- press 2 to select Big Swing (Blue)
            -- if (CheckKeyPress(50)) then
            --     playerAttackComponent.selectedAttack = attackList[4]
            --     print("[PlayerAttack.lua] Selected Player attack:", playerAttackComponent.selectedAttack[1], 
            --           "Base damage:", playerAttackComponent.selectedAttack[2], 
            --           "Special property:", playerAttackComponent.selectedAttack[3], "\n")
            -- end

            -- press 3 to select Energy Bolt (Blue)
            if (CheckKeyPress(51)) then
                playerAttackComponent.selectedAttack = attackList[10]
                print("[PlayerAttack.lua] Selected Player attack:", playerAttackComponent.selectedAttack[1], 
                      "Base damage:", playerAttackComponent.selectedAttack[2], 
                      "Special property:", playerAttackComponent.selectedAttack[3], "\n")
            end
            -- end of selecting attack ---------------------------------------------------------------------------------
        
            -- select a target -----------------------------------------------------------------------------------------
            playerAttackComponent.targetEntity = EntitiesWithScriptComponent("C_StateMoveEnemy")[1]
            -- print(EntityName(playerAttackComponent.targetEntity))
            -- end of selecting target ---------------------------------------------------------------------------------

            -- if attack type and attack target are chosen, then initiate combat
            if (playerAttackComponent.selectedAttack ~= nil and playerAttackComponent.targetEntity ~= -1) then
                        
                -- check whether player is able to attack the enemy (within range / no obstructions / etc) --------------
                if (playerAttackComponent.attackCheckTriggered == false) then
                    selectedAttackComponent = GetComponent(combatManagerEntity, playerAttackComponent.selectedAttack[4])
                    selectedAttackComponent.attacker = e
                    selectedAttackComponent.defender = playerAttackComponent.targetEntity
                    selectedAttackComponent.startCheck = true
                    playerAttackComponent.attackCheckTriggered = true
                    print("[PlayerAttack.lua] Triggering", playerAttackComponent.selectedAttack[4], "to perform check!")
                end
                ---end of range check -----------------------------------------------------------------------------------
                
                if (selectedAttackComponent.checkCompleted) then 
                                        
                    if (selectedAttackComponent.canAttack) then
                                            
                        -- pass this entity, attacktype, and target to CombatManager
                        combatManagerComponent = GetComponent(combatManagerEntity, "C_CombatManager")
                        combatManagerComponent.attacker = e
                        combatManagerComponent.attackType = playerAttackComponent.selectedAttack
                        combatManagerComponent.defender = playerAttackComponent.targetEntity
                        print("PlayerAttack.lua ---> CombatManager.lua")

                        -- resetting FrontJab variable
                        selectedAttackComponent.canAttack = false
                        
                        -- CombatManager will handle the rest of damage calculation and application     

                    else 
                        -- print error message from attack script
                        print(selectedAttackComponent.errorMessage)
                    end

                    -- reset variables
                    playerAttackComponent.resetVariables = true

                    -- reset FrontJab variable
                    selectedAttackComponent.checkCompleted = false
                end
            end
        end
    end
end