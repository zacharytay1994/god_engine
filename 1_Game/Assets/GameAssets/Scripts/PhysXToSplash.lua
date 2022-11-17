--[IsComponent]
function C_PhysXToSplash()
    local var = {
        isSelected = false
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_PhysXToSplash(e)

    local screen = GetComponent(e, "C_PhysXToSplash")
    local gui_object = GetGUIObject(e)

    if(gui_object.pressed and screen.isSelected == false) then
        ChangeScene("Level_1",true)
        screen.isSelected = true
    end

end