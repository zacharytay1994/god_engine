--[IsComponent]
function C_LevelProgress()
    local var = {
        isLevel1 = true
    }
    return function()
        return var
    end
end

unlocked = false
--[IsSystem]
function S_LevelProgress(e)

    local progress = GetComponent(e, "C_LevelProgress")
    local gui_object = GetGUIObject(e)

    if (SceneName == "Level_2") then
        unlocked = true
        progress.isLevel1 = false
    else
        progress.isLevel1 = true
    end

    if(unlocked == true) then 
        gui_object.active = false
    end

    if(progress.isLevel1 == true and SceneName == "Mainmenu") then
        gui_object.active  = true
    end



end