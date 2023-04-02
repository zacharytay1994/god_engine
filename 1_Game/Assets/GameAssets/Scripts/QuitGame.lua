--[IsComponent]
function C_QuitGame()
    local var = {
        isSelected = false
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_QuitGame(e)
    local screen = GetComponent(e, "C_QuitGame")
    local gui_object = GetGUIObject(e)

    if(gui_object.pressed and screen.isSelected == false) then
        --StopBGM()
        QuitGame()
    end
end
