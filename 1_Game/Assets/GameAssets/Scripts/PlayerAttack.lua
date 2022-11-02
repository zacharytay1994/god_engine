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
        turnOrderManagerComponent = GetComponent(turnOrderManagerEntity, "C_TurnOrderManager")
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
            
            --  check if CombatManager entity exists
            combatManagerEntity = GetEntity("CombatManager")
            if (combatManagerEntity ~= -1) then
                
                -- get C_AttackList
                attackListComponent = GetComponent(combatManagerEntity, "C_AttackList")

                if (attackListComponent ~= nil) then
                    attackList = attackListComponent.attackList
                else
                    print("[PlayerAttack.lua] ERROR: attackListComponent is nil.")
                end    
            end 

            -- press G to check currently selected enemy
            if (CheckKeyPress(71)) then
                print("G:", playerAttackComponent.targetEntity)
                
                -- if (playerAttackComponent.targetEntity ~= -1) then
                --     print("Currently target enemy is", GetEntityData(playerAttackComponent.targetEntity).id)
                -- else
                --     print("G:", playerAttackComponent.targetEntity)
                -- end
            end

            -- select an attack ----------------------------------------------------------------------------------
            local playerComponent = GetComponent(e, "C_Player")

            if (CheckKeyPress(49) or CheckKeyPress(50) or CheckKeyPress(51)) then
                -- -- press 1 to select Front Jab (Blue)
                -- if (CheckKeyPress(49)) then
                --     playerAttackComponent.selectedAttack = attackList[1]
                --     print("[PlayerAttack.lua] Selected Player attack:", playerAttackComponent.selectedAttack[1], 
                --           "Base damage:", playerAttackComponent.selectedAttack[2], 
                --           "Special property:", playerAttackComponent.selectedAttack[3], "\n")
                -- end
                if (playerComponent.selectedAction == "FrontJab") then
                    playerAttackComponent.selectedAttack = attackList[1]
                    print("[PlayerAttack.lua] Selected Player attack:", playerAttackComponent.selectedAttack[1], 
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
                -- if (playerComponent.selectedAction == "BigSwing") then
                --     playerAttackComponent.selectedAttack = attackList[4]
                --     print("[PlayerAttack.lua] Selected Player attack:", playerAttackComponent.selectedAttack[1], 
                --           "Base damage:", playerAttackComponent.selectedAttack[2], 
                --           "Special property:", playerAttackComponent.selectedAttack[3], "\n")
                -- end

                -- press 3 to select Energy Bolt (Blue)
                -- if (CheckKeyPress(51)) then
                --     playerAttackComponent.selectedAttack = attackList[10]
                --     print("[PlayerAttack.lua] Selected Player attack:", playerAttackComponent.selectedAttack[1], 
                --           "Base damage:", playerAttackComponent.selectedAttack[2], 
                --           "Special property:", playerAttackComponent.selectedAttack[3], "\n")
                -- end
                if (playerComponent.selectedAction == "EnergyBolt") then
                    playerAttackComponent.selectedAttack = attackList[10]
                    print("[PlayerAttack.lua] Selected Player attack:", playerAttackComponent.selectedAttack[1], 
                        "Base damage:", playerAttackComponent.selectedAttack[2], 
                        "Special property:", playerAttackComponent.selectedAttack[3], "\n")
                end
            end
            -- end of selecting attack ---------------------------------------------------------------------------------
        
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
                            if (EntityName(characterList[i]) ~= "Player") then
                            
                                local currentEntityGridCell = GetGridCell(characterList[i])

                                if (currentEntityGridCell.x == enemyGridx and currentEntityGridCell.y == enemyGridy and currentEntityGridCell.z == enemyGridz) then
                                    print("Selected target:", EntityName(characterList[i]), GetEntityData(characterList[i]).id)
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
            
            -- press 4 to cycle through enemy targets
            -- if (CheckKeyPress(52)) then 
                
            --     local enemyList = EntitiesWithScriptComponent("C_StateMoveEnemy")
            --     local enemyRemaining = false

            --     for i = 1, #enemyList do
            --         if (GetComponent(enemyList[i], "C_Character").isDead == false) then
            --             enemyRemaining = true
            --             break
            --         end
            --     end

            --     if (enemyRemaining == false) then
            --         print("[PlayerAttack.lua] No enemies to target, all enemies are dead!")
            --     else

            --         if (playerAttackComponent.enemyCycle > #enemyList) then
            --             playerAttackComponent.enemyCycle = 1
            --         end

            --         while (GetComponent(enemyList[playerAttackComponent.enemyCycle], "C_Character").isDead == true) do
            --             playerAttackComponent.enemyCycle = playerAttackComponent.enemyCycle + 1

            --             if (playerAttackComponent.enemyCycle > #enemyList) then
            --                 playerAttackComponent.enemyCycle = 1
            --             end
            --         end

            --         -- target the next enemy in the list
            --         playerAttackComponent.targetEntity = enemyList[playerAttackComponent.enemyCycle]

            --         playerAttackComponent.enemyCycle = playerAttackComponent.enemyCycle + 1
            --     end

            --     print("[PlayerAttack.lua] Currently target enemy is:", EntityName(playerAttackComponent.targetEntity), "ID no.", GetEntityData(playerAttackComponent.targetEntity).id)
            -- end
            -- end of selecting target ---------------------------------------------------------------------------------

            -- if attack type and attack target are chosen, then initiate combat
            if (playerAttackComponent.selectedAttack ~= nil and playerAttackComponent.targetEntity ~= -1 and playerAttackComponent.targetEntity ~= nil) then
                        
                -- check whether player is able to attack the enemy (within range / no obstructions / etc) --------------
                if (playerAttackComponent.attackCheckTriggered == false) then
                    -- print("[PlayerAttack.lua] playerAttackComponent.attackCheckTriggered == false")
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

                    -- reset FrontJab variable
                    selectedAttackComponent.checkCompleted = false
                end
            end
        end
    end
end