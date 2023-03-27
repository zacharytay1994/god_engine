--[IsComponent]
function C_MainmenuToGame()
    local var = {
        isSelected = false
    }
    return function()
        return var
    end
end

dice_1_faces = { 0, 0, 0, 0, 0, 0 }
dice_2_faces = { 0, 1, 2, 3, 4, 6 }
dice_3_faces = { 0, 1, 2, 3, 4, 6 }

--[IsSystem]
function S_MainmenuToGame(e)

    local screen = GetComponent(e, "C_MainmenuToGame")
    local gui_object = GetGUIObject(e)

    if(gui_object.pressed and screen.isSelected == false) then
        StopBGM()
        ChangeScene("350LevelScene",true)
        SceneName = "350LevelScene"
        screen.isSelected = true
    end
end