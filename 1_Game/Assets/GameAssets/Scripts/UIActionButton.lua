-- What ActionButton.lua does:
-- 1) When an Action Button is clicked, set C_Player.selectedAction to whatever that ActionButton represents, and mark that 
--    Action Button as selected. All other Action Buttons will be un-selected.
-- 2) If the Player performs an attack while an Action Button is marked as selected, that Action Button's texture will be reset.
--    C_Player.selectedAction will be set to nil, and C_Player.playerAttacked will be set to false.

-- IMPORTANT: Texture name for the action buttons must follow a format.
--            Which is just the name of the attack.
--            { FrontJab, BigSwing, GroundSmash, EnergyBolt, Projectile, Corporikinesis, Cryogenesis, Hydrokinesis, Levitation }


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
    
    -- getting action button component and object
    local actionButtonComponent = GetComponent(e, "C_ActionButton")
    local actionButtonObject = GetGUIObject(e)

    -- making sure playing entity exists and getting PlayerAttack and Character components
    local playerEntity = GetEntity("Player")
    local playerAttackComponent = nil
    local playerComponent = nil
    if (playerEntity ~= -1) then
        playerAttackComponent = GetComponent(playerEntity, "C_PlayerAttack")
        playerComponent = GetComponent(playerEntity, "C_Player")
    else
        print("[ActionButton.lua] Player Entity not found! Returning.")
        return
    end

    if actionButtonObject.pressed then
        
        print("[ActionButton.lua]", EntityName(e), "pressed!")
        
        -- getting the AttackList ----------------------------------------------------------
        local attackList = nil
        local combatManagerEntity = GetEntity("CombatManager")
        if (combatManagerEntity ~= -1) then
            attackList = GetComponent(combatManagerEntity, "C_AttackList").attackList
        else
            print("[ActionButton.lua] CombatManager Entity not found! Returning.")
            return
        end
        
        -- set player's selected attack to whatever this button represents -----------------
        playerComponent.selectedAction = TextureName(e)
        print("[ActionButton.lua] playerComponent.selectedAction:", playerComponent.selectedAction)
        
        -- hardcoded (will update soon)
		if (playerComponent.selectedAction == "Move") then
			HighlightTiles(playerEntity, GetComponent(playerEntity, "C_Character").currentStamina)
			
        elseif (playerComponent.selectedAction == "FrontJab") then
            playerAttackComponent.selectedAttack = attackList[1]
			ResetHighlightTiles()
            print("[ActionButton.lua] Selected Player attack:", playerAttackComponent.selectedAttack[1], 
                        "Base damage:", playerAttackComponent.selectedAttack[2], 
                        "Special property:", playerAttackComponent.selectedAttack[3], "\n")
        
        elseif (playerComponent.selectedAction == "EnergyBolt") then
            playerAttackComponent.selectedAttack = attackList[10]
			ResetHighlightTiles()
            print("[ActionButton.lua] Selected Player attack:", playerAttackComponent.selectedAttack[1], 
                        "Base damage:", playerAttackComponent.selectedAttack[2], 
                        "Special property:", playerAttackComponent.selectedAttack[3], "\n")
            
        end

        -- -- un-hardcoded solution 
        -- -- (diceValue will be a value from 0 to 8, colorModifier will be a value from 1 to 3.
        -- -- On the dice side, need to map 0 to FrontJab, 1 to BigSwing, 2 to GroundSmash, 3 to EnergyBolt, etc.)
        -- playerAttackComponent.selectedAttack = attackList[(diceValue * 3) + colorModifier]
        -- end of setting selected action --------------------------------------------------

        -- set this button as currently selected
        actionButtonComponent.buttonSelected = true

        -- set all other Action Buttons as unselected
        print("[ActionButton.lua] Un-selecting all other Action Buttons!")
        local actionButtonList = EntitiesWithScriptComponent("C_ActionButton")
        for i = 1, #actionButtonList do
            if (actionButtonList[i] ~= e) then
                GetComponent(actionButtonList[i], "C_ActionButton").buttonSelected = false
            end
        end
    end

    -- empty the button after the attack is used
    if (actionButtonComponent.buttonSelected == true and playerComponent.playerAttacked == true) then
        
        print("[ActionButton.lua]", EntityName(e), "has been used! Resetting to empty button.")
        
        -- reset the button texture and scale
        ChangeTexture(e, "empty_btn")
        playerComponent.playerAttacked = false
        playerComponent.selectedAction = nil
        actionButtonComponent.buttonSelected = false
    end
end
