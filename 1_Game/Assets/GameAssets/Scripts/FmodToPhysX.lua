--[IsComponent]
function C_FmodToPhysX()
local var = {
    isSelected = false,
    timer = 2.0
}
    return function()
        return var
    end
end

--[IsSystem]
function S_FmodToPhysX(e)

    local screen = GetComponent(e, "C_FmodToPhysX")
    local gui_object = GetGUIObject(e)
    local dt = GetDeltaTime()

    if(screen.timer >= 0.0)
    then
        screen.timer = screen.timer - dt
    end

    if(gui_object.pressed and screen.isSelected == false or screen.timer <= 0.0) then
        ChangeScene("PhysXScreen",true)
        screen.isSelected = true
    end

end