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
    print("Hi winlose")
    local winLose = GetComponent(e, "C_WinLose")
    print("Hi")
    local player = GetComponent(GetEntity("Player"), "C_Character")
    print("Hi player")
    local gui_object = GetGUIObject(e)
    print("Hi UI")
    
    if(player.isDead == true)
    then
        print("Player is dead")
        gui_object.active = true
    else
        print("Player is still alive")
    end
end