--[IsComponent]
function C_DiceManager()
	local var = {
		--[SerializeString]
		button_name = "DiceManager is attached!",
		starting_scale_x = 1,
		starting_scale_y = 1,
		is_init = false,
		is_active = true
	}
	
	return function()
		return var
	end
end

--[IsSystem]
function S_DiceManager(e)
	local c_dicemanager = GetComponent(e, "C_DiceManager")
	local gui_object = GetGUIObject(e)
	if(c_dicemanager.is_init == false) then
		if(c_dicemanager.button_name == "Panel") then
			c_dicemanager.starting_scale_x = 1
			c_dicemanager.starting_scale_y = 1
		else
			c_dicemanager.starting_scale_x = gui_object.size.x
			c_dicemanager.starting_scale_y = gui_object.size.y
		end
		c_dicemanager.is_init = true
	end

	if(c_dicemanager.is_active == false) then
		gui_object.size.x = 0
		gui_object.size.y = 0
	else
		gui_object.size.x = c_dicemanager.starting_scale_x
		gui_object.size.y = c_dicemanager.starting_scale_y
	end
end