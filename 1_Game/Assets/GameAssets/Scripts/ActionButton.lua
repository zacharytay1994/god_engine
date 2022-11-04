-- Click on an Action Button to set it as the Player's selectedAction.

-- IMPORTANT: Texture name for the action buttons must follow a format.
--            Which is just the name of the attack.
--            e.g. FrontJab, EnergyBolt, BigSweep, etc.


--[IsComponent]
function C_ActionButton()
    local var = {
        
        -- marks the selected button 
        buttonSelected = false
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_ActionButton(e)
    
    local actionButtonComponent = GetComponent(e, "C_ActionButton")
    local actionButtonObject = GetGUIObject(e)

    -- making sure playing entity exists -----------------------------------------------
    local playerEntity = GetEntity("Player")
    local playerComponent
    local playerAttackComponent
    
    if (playerEntity ~= -1) then
        playerComponent = GetComponent(playerEntity, "C_Player")
        playerAttackComponent = GetComponent(playerEntity, "C_PlayerAttack")
    else
        print("[ActionButton.lua] Player Entity not found! Returning.")
        return
    end
    -- end of checking player entity ---------------------------------------------------

    if actionButtonObject.pressed then
        
        actionButtonComponent.buttonSelected = true
        
        -- getting the AttackList ----------------------------------------------------------
        local attackList
        local combatManagerEntity = GetEntity("CombatManager")
        if (combatManagerEntity ~= -1) then
            attackList = GetComponent(combatManagerEntity, "C_AttackList").attackList
        end
        -- end of getting the AttackList ---------------------------------------------------
        
        -- set player's selected attack to whatever this button represents -----------------
        playerComponent.selectedAction = TextureName(e)
        print("[ActionButton.lua] playerComponent.selectedAction:", playerComponent.selectedAction)
        
        -- hardcoded
        if (playerComponent.selectedAction == "FrontJab") then
            playerAttackComponent.selectedAttack = attackList[1]
        elseif (playerComponent.selectedAction == "EnergyBolt") then
            playerAttackComponent.selectedAttack = attackList[10]
        end
        -- end of setting selected action --------------------------------------------------

        -- set all other Action Buttons as unselected --------------------------------------
        local actionButtonList = EntitiesWithScriptComponent("C_ActionButton")
        for i = 1, #actionButtonList do
            if (actionButtonList[i] ~= e) then
                GetComponent(actionButtonList[i], "C_ActionButton").buttonSelected = false
            end
        end
        -- end of unselecting other Action Buttons -----------------------------------------
    end

    -- empty the button after the attack is used
    if (actionButtonComponent.buttonSelected == true and playerComponent.playerAttacked == true) then
        
        -- reset the button texture and scale
        ChangeTexture(e, "empty_btn")
        playerComponent.playerAttacked = false
        actionButtonComponent.buttonSelected = false
    end
end
