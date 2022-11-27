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

local sfxTriggered = false

--[IsSystem]
function S_WinLose(e)
    local winLose = GetComponent(e, "C_WinLose")

    local gui_object = GetGUIObject(e)
    
    if(isAlive == false)
    then
		if sfxTriggered == false then
			-- trigger sound effect
			InstancePrefab("SFX_PlayerDeath",0,0,0)
			sfxTriggered = true
		end

        --print("Player is dead")
        gui_object.active = true
    else
        --print("Player is still alive")
    end
end