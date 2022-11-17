--[IsComponent]
function C_DigipenToFmod()
local var = {
    isSelected = false
}
return function()
return var
end
end

--[IsSystem]
function S_DigipenToFmod(e)
    local screen = GetComponent(e, "C_DigipenToFmod")
    local gui_object = GetGUIObject(e)

    if(gui_object.pressed and screen.isSelected == false) then
        ChangeScene("FMODScreen",true)
        screen.isSelected = true
    end

end