--[IsComponent]
function C_UIRestartButton()
local var = {
    isSelected = false
}
    return function()
        return var
    end
end

--[IsSystem]
function S_UIRestartButton(e)

    local restartButton = GetComponent(e, "C_UIRestartButton")
    local actionButtonObject = GetGUIObject(e)

    if (actionButtonObject.pressed and restartButton.isSelected == false) then
        ChangeScene("Level_1", true)
        restartButton.isSelected = true
    end
end