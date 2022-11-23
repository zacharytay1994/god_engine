--[IsComponent]
function C_MainmenuToDice()
    local var = {
        isSelected = false
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_MainmenuToDice(e)

    local screen = GetComponent(e, "C_MainmenuToDice")
    local gui_object = GetGUIObject(e)

    if(gui_object.pressed and screen.isSelected == false) then
        ChangeScene("DiceCustomization",true)
        screen.isSelected = true
    end
end