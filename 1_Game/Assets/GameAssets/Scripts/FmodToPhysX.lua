--[IsComponent]
function C_FmodToPhysX()
local var = {
    isSelected = false
}
    return function()
        return var
    end
end

--[IsSystem]
function S_FmodToPhysX(e)

    local screen = GetComponent(e, "C_FmodToPhysX")
    local gui_object = GetGUIObject(e)

    if(gui_object.pressed and screen.isSelected == false) then
        ChangeScene("PhysXScreen",true)
        screen.isSelected = true
    end

end