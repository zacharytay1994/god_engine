--[IsComponent]
function C_ButtonOnHoverPopup()
    local var = {
        PopUpAmount = 1.2,
        InitialScaleX = 0.0,
        InitialScaleY = 0.0
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_ButtonOnHoverPopup(e)
    local on_hover_popup = GetComponent(e, "C_ButtonOnHoverPopup")
    local gui_object = GetGUIObject(e)

    if gui_object.enter then
        on_hover_popup.InitialScaleX = gui_object.size.x
        on_hover_popup.InitialScaleY = gui_object.size.y
        gui_object.size.x = gui_object.size.x * on_hover_popup.PopUpAmount
        gui_object.size.y = gui_object.size.y * on_hover_popup.PopUpAmount
    end

    if gui_object.exit then
        gui_object.size.x = on_hover_popup.InitialScaleX
        gui_object.size.y = on_hover_popup.InitialScaleY
    end
end
