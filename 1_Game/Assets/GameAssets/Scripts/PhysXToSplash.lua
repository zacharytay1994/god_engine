--[IsComponent]
function C_PhysXToSplash()
    local var = {
        isSelected = false,
        timer = 2.0
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_PhysXToSplash(e)

    local screen = GetComponent(e, "C_PhysXToSplash")
    local gui_object = GetGUIObject(e)
    local dt = GetDeltaTime()
    
    if(screen.timer >= 0.0)
    then
        screen.timer = screen.timer - dt
    end

    if(gui_object.pressed and screen.isSelected == false or screen.timer <= 0.0) then
        ChangeScene("MainmenuScreen",true)
        screen.isSelected = true
    end

end