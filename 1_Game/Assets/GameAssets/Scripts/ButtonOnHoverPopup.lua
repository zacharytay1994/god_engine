-- This script makes buttons pop up when hovered over.
-- Button size becomes locked when the player clicks on it. Need to click it again to revert.
-- Also plays a cute sound effect when hovered over or clicked on.

--[IsComponent]
function C_ButtonOnHoverPopup()
    local var = {
        PopUpAmount = 1.2,
        InitialScaleX = 0.0,
        InitialScaleY = 0.0,
    }
    return function()
        return var
    end
end

function GenerateRandomHoverSFX()
	local sfx = 
	{ 
		"SFX_Hover01", "SFX_Hover02", "SFX_Hover03", "SFX_Hover04", "SFX_Hover05",
		"SFX_Hover06", "SFX_Hover07", "SFX_Hover08", "SFX_Hover09", "SFX_Hover10"
	}
	
	local rng = GenerateRandomNumberInRange(1, 10)
	InstancePrefab(sfx[rng],0,0,0)
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
		GenerateRandomHoverSFX()
        -- InstancePrefab("SFX_Bloop",0,0,0)
    end

    if gui_object.exit or ~gui_object.active then
        gui_object.size.x = on_hover_popup.InitialScaleX
        gui_object.size.y = on_hover_popup.InitialScaleY
    end

    if gui_object.pressed then
		InstancePrefab("SFX_Click",0,0,0)
    end
end
