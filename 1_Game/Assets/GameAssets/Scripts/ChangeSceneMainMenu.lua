--[IsComponent]
function C_ChangeSceneMainMenu()
    local var = {
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_ChangeSceneMainMenu(e)
    local lm = Get_350LevelManager(e);
    if lm.done == true then
        ChangeScene("SplashScreen", true)
    end
end
