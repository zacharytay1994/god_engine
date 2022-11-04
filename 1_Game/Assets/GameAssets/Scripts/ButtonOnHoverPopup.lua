--[IsComponent]
function C_ButtonOnHoverPopup()
    local var = {
        PopUpAmount = 1.2,
        InitialScaleX = 0.0,
        InitialScaleY = 0.0,
        buttonSelected = false,
        resetSize = false -- used by release Move button cheat in Character.lua
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_ButtonOnHoverPopup(e)
    local on_hover_popup = GetComponent(e, "C_ButtonOnHoverPopup")
    local gui_object = GetGUIObject(e)

    -- only allow button to pop up when it's not already selected
    if (on_hover_popup.buttonSelected == false) then
        if gui_object.enter then
            on_hover_popup.InitialScaleX = gui_object.size.x
            on_hover_popup.InitialScaleY = gui_object.size.y
            gui_object.size.x = gui_object.size.x * on_hover_popup.PopUpAmount
            gui_object.size.y = gui_object.size.y * on_hover_popup.PopUpAmount
            InstancePrefab("SFX_Bloop",0,0,0)
        end
    end

    if gui_object.pressed then
        
        -- making sure playing entity exists
        local playerComponent
        local playerAttackComponent
        if (GetEntity("Player") ~= -1) then
            playerComponent = GetComponent(GetEntity("Player"), "C_Player")
            playerAttackComponent = GetComponent(GetEntity("Player"), "C_PlayerAttack")
        else
            print("[ButtonOnHoverPopup.lua] Player Entity not found! Returning.")
            return
        end
        
        if (on_hover_popup.buttonSelected) then
            
            -- release the button
            on_hover_popup.buttonSelected = false

            -- set player's selected action to nil
            playerComponent.selectedAction = nil
            playerAttackComponent.selectedAttack = nil

        else
            
            -- lock the button
            on_hover_popup.buttonSelected = true
            
            local attackList
            local combatManagerEntity = GetEntity("CombatManager")
            if (combatManagerEntity ~= -1 then)
                GetComponent(combatManagerEntity, "C_AttackList").attackList
            end
            
            -- set player's selected attack to whatever this button represents
            playerComponent.selectedAction = TextureName(e)
            print("playerComponent.selectedAction:", playerComponent.selectedAction)

            -- hardcode
            if (playerComponent.selectedAction == "FrontJab") then
                playerAttackComponent.selectedAttack = attackList[1]
            elseif (playerComponent.selectedAction == "EnergyBolt") then
                playerAttackComponent.selectedAttack = attackList[10]
            end

        end

        if (EntityName(e) == "EndTurnButton") then
            -- only end the turn if it is actually the player's turn
            if (GetComponent(GetEntity("TurnOrderManager"), "C_TurnOrderManager").currentTurn == GetEntityData(GetEntity("Player")).id) then
                
                -- signal turnOrderManager to move on to the next character's turn
                GetComponent(GetEntity("TurnOrderManager"), "C_TurnOrderManager").nextTurn = true
            end
        end
    end

    if (on_hover_popup.buttonSelected == false) then 
        if gui_object.exit then
            gui_object.size.x = on_hover_popup.InitialScaleX
            gui_object.size.y = on_hover_popup.InitialScaleY
        end
    end

    if (on_hover_popup.buttonSelected == true and GetComponent(GetEntity("Player"), "C_Player").playerAttacked == true) then
        
        -- reset the button texture and scale
        ChangeTexture(e, "empty_btn")
        GetComponent(GetEntity("Player"), "C_Player").playerAttacked = false
        on_hover_popup.buttonSelected = false

        gui_object.size.x = on_hover_popup.InitialScaleX
        gui_object.size.y = on_hover_popup.InitialScaleY

    end

    if (on_hover_popup.resetSize) then
        gui_object.size.x = on_hover_popup.InitialScaleX
        gui_object.size.y = on_hover_popup.InitialScaleY
        on_hover_popup.resetSize = false
        on_hover_popup.buttonSelected = false
    end

    if gui_object.pressed then
		InstancePrefab("SFX_Click",0,0,0)
    end
end
