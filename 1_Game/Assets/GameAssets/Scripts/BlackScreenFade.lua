--[IsComponent]
function C_BlackScreenFade()
    local var = {
        --[SerializeFloat]
        FadeSpeed = 1.0,
        FadeIn = true,
        SceneToGo = "None"
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_BlackScreenFade(e)
    local fade = GetComponent(e,"C_BlackScreenFade")
    local gui_object = GetGUIObject(e)
    local dt = GetDeltaTime()
    if fade.FadeIn then
        if gui_object.alpha > 0 then
            gui_object.alpha = gui_object.alpha - fade.FadeSpeed * dt
        else
            RemoveInstance(e)
        end
    else
        if gui_object.alpha < 1 then
            gui_object.alpha = gui_object.alpha + fade.FadeSpeed * dt
        else
            if fade.SceneToGo ~= "None" then
                ChangeScene(fade.SceneToGo,true)
            else
                RemoveInstance(e)
            end
        end
    end
end
