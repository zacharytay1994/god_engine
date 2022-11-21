--[IsComponent]
function C_QuitToMainMenu()
    local var = {
        isSelected = false
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_QuitToMainMenu(e)
    local screen = GetComponent(e, "C_QuitToMainMenu")
    local gui_object = GetGUIObject(e)

    if(gui_object.pressed and screen.isSelected == false) then
        ChangeScene("MainmenuScreen",true)
        screen.isSelected = true
    end
end