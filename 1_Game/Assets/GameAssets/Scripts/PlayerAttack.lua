-- This script will allow the player to select an enemy as an attack target.

-- TODO:
-- 1) 

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
        attackCheckTriggered = false,

        -- used to cycle through enemy targets, won't be used anymore once UI to select enemy is done
        enemyCycle = 1
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
        local turnOrderManagerComponent = GetComponent(turnOrderManagerEntity, "C_TurnOrderManager")
        local playerAttackComponent = GetComponent(e, "C_PlayerAttack")
        
        -- only run this script if its the character's turn
        if (turnOrderManagerComponent.currentTurn == entityDataComponent.id) then
            
            -- resetting variables
            if (playerAttackComponent.resetVariables) then 
                print("[PlayerAttack.lua] Resetting variables!\n")
                playerAttackComponent.selectedAttack = nil
                playerAttackComponent.targetEntity = nil
                playerAttackComponent.attackCheckTriggered = false
                playerAttackComponent.resetVariables = false
            end
            
            --  getting the attack list
            combatManagerEntity = GetEntity("CombatManager")
            if (combatManagerEntity ~= -1) then
                
                attackListComponent = GetComponent(combatManagerEntity, "C_AttackList")

                if (attackListComponent ~= nil) then
                    attackList = attackListComponent.attackList
                else
                    print("[PlayerAttack.lua] ERROR: attackListComponent is nil.")
                end    
            end 

            
        
            -- select a target -----------------------------------------------------------------------------------------           
            if (playerComponent.selectedAction ~= "Move") then
                
                local gridManipulateEntity = GetEntity("GridManipulate")
                    
                if (gridManipulateEntity ~= -1) then
                    
                    local gridManipulateComponent = GetGridManipulate(gridManipulateEntity)
                    
                    if (gridManipulateComponent.clicked) then
                        
                        -- note: last_clicked_cell.y will +1 automatically, so need to minus one first
                        local enemyGridx = gridManipulateComponent.last_clicked_cell.x
                        local enemyGridy = gridManipulateComponent.last_clicked_cell.y - 1
                        local enemyGridz = gridManipulateComponent.last_clicked_cell.z

                        local characterList = EntitiesWithScriptComponent("C_Character")

                        for i = 1, #characterList do
                        
                            -- skip the player
                            if (characterList[i] ~= e) then
                            
                                local currentEntityGridCell = GetGridCell(characterList[i])

                                if (currentEntityGridCell.x == enemyGridx and currentEntityGridCell.y == enemyGridy and currentEntityGridCell.z == enemyGridz) then
                                    print("[PlayerAttack.lua] Selected target:", EntityName(characterList[i]), GetEntityData(characterList[i]).id)
                                    playerAttackComponent.targetEntity = characterList[i]
                                    break
                                end
                            end
                        end
                    end
                else
                    print("[PlayerAttack.lua] ERROR: GridManipulate entity does not exist!")
                end
            end
            -- end of selecting target ---------------------------------------------------------------------------------

            -- if attack type and attack target are chosen, then initiate combat
            if (playerAttackComponent.selectedAttack ~= nil and playerAttackComponent.targetEntity ~= -1 and playerAttackComponent.targetEntity ~= nil) then
                        
                local selectedAttackComponent = GetComponent(combatManagerEntity, playerAttackComponent.selectedAttack[4])
                
                -- check whether player is able to attack the enemy (within range / no obstructions / etc) --------------
                if (playerAttackComponent.attackCheckTriggered == false) then
                    
                    -- note: selectedAttack[4] is the attack's component name (refer to AttackList.lua)
                    -- selectedAttackComponent = GetComponent(combatManagerEntity, playerAttackComponent.selectedAttack[4])
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
                        print("[PlayerAttack.lua] PlayerAttack.lua ---> CombatManager.lua")

                        -- resetting FrontJab variable
                        selectedAttackComponent.canAttack = false

                        -- setting cue to reset action button texture
                        GetComponent(e, "C_Player").playerAttacked = true
                        
                        -- CombatManager will handle the rest of damage calculation and application     

                    else 
                        -- print error message from attack script
                        print(selectedAttackComponent.errorMessage)
                    end

                    -- reset variables
                    playerAttackComponent.resetVariables = true
                    selectedAttackComponent.checkCompleted = false
                end
            end
        end
    end
end