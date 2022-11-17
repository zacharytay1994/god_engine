--[IsComponent]
function C_WinLose()
    local var = {
    timer = 0.0,
    lifetime = 2.0
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_WinLose(e)
   
    local winLose = GetComponent(e, "C_WinLose")

    local gui_object = GetGUIObject(e)
    
    if(isAlive == false)
    then
        --print("Player is dead")
        gui_object.active = true
    else
        --print("Player is still alive")
    end
end