--[IsComponent]
function C_DiceUI()
	local var = {
		--[SerializeString]
		button_name = "DiceManager is attached!",
		starting_scale_x = 1,
		starting_scale_y = 1,
		is_init = false,
		is_active = false
	}
	
	return function()
		return var
	end
end

--[IsSystem]
function S_DiceUI(e)
	local c_dicemanager = GetComponent(e, "C_DiceUI")
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
		
		local diceList = EntitiesWithScriptComponent("C_DiceScript")
		local diceSettled = true
		for i = 1, #diceList do
            if (GetComponent(diceList[i], "C_DiceScript").value == 0) then
				print("[S_DiceUI] dice still rolling:", i)
                diceSettled = false
				break
            end
        end

		
		if(diceSettled == true) then
			c_dicemanager.is_active = true
		end
	else
		gui_object.size.x = c_dicemanager.starting_scale_x
		gui_object.size.y = c_dicemanager.starting_scale_y
	end
end