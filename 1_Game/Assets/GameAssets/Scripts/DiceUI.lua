--[IsComponent]
function C_DiceUI()
	local var = {
		--[SerializeString]
		button_name = "DiceManager is attached!",
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
		c_dicemanager.is_init = true
	end
	
	if(c_dicemanager.is_active == false) then
		if (gui_object.active == true) then
			gui_object.active = false
		end
		
		local diceList = EntitiesWithScriptComponent("C_DiceScript")
		local diceSettled = true
		for i = 1, #diceList do
            if (GetComponent(diceList[i], "C_DiceScript").value == -1) then
                diceSettled = false
				break
            end
        end
		
		if(diceSettled == true) then
			c_dicemanager.is_active = true
		end
	else
		if (gui_object.active == false) then
			gui_object.active = true
		end

		
		if gui_object.pressed then
			if c_dicemanager.button_name == "Reroll" then
				
			elseif c_dicemanager.button_name == "Ready" then
				local diceUIList = EntitiesWithScriptComponent("C_DiceUI")
				for i = 1, #diceUIList do
					GetComponent(diceUIList[i], "C_DiceUI").is_active = false
				end
				
				local diceList = EntitiesWithScriptComponent("C_DiceScript")
				for i = 1, #diceList do
					DiceScript_DisableDice(diceList[i], GetComponent(diceList[i], "C_DiceScript"))
				end
			end
		end
	end
end