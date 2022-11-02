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
        
        local playerComponent = GetComponent(GetEntity("Player"), "C_Player")
        local playerAttackComponent = GetComponent(GetEntity("Player"), "C_PlayerAttack")
        
        if (on_hover_popup.buttonSelected) then
            
            -- release the button
            on_hover_popup.buttonSelected = false

            -- set player's selected action to nil
            playerComponent.selectedAction = nil
            playerAttackComponent.selectedAttack = nil

        else
            
            -- lock the button
            on_hover_popup.buttonSelected = true
            
            local attackList = GetComponent(GetEntity("CombatManager"), "C_AttackList").attackList
            
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
