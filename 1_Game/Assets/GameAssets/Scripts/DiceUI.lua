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
			
			if c_dicemanager.button_name == "DiceIcon1" then
				SetDiceIcon(e, 1)
			elseif c_dicemanager.button_name == "DiceIcon2" then
				SetDiceIcon(e, 2)
			elseif c_dicemanager.button_name == "DiceIcon3" then
				SetDiceIcon(e, 3)
			end
		end

		
		if gui_object.pressed then
			if c_dicemanager.button_name == "Reroll" then
				local diceUIList = EntitiesWithScriptComponent("C_DiceUI")
				for i = 1, #diceUIList do
					GetComponent(diceUIList[i], "C_DiceUI").is_active = false
				end
				
				local diceList = EntitiesWithScriptComponent("C_DiceScript")
				for i = 1, #diceList do
					DiceScript_DisableDice(diceList[i], GetComponent(diceList[i], "C_DiceScript"))
					DiceScript_RollDice(diceList[i], GetComponent(diceList[i], "C_DiceScript"))
				end
				
				GetComponent(GetEntity("Level1GUI"), "C_UIManager").diceRolled = false
				GetComponent(GetEntity("Level1GUI"), "C_UIManager").actionButtonList = {}
				print("[DiceUI] UIManager shit works!")
				
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

function SetDiceIcon(e, dicenumber)
	local diceList = EntitiesWithScriptComponent("C_DiceScript")
	if (GetComponent(diceList[dicenumber], "C_DiceScript").value == 1) then
		ChangeTexture(e, "dice_jab")
	elseif (GetComponent(diceList[dicenumber], "C_DiceScript").value == 2) then
		ChangeTexture(e, "dice_jab")
	elseif (GetComponent(diceList[dicenumber], "C_DiceScript").value == 3) then
		ChangeTexture(e, "dice_jab")
	elseif (GetComponent(diceList[dicenumber], "C_DiceScript").value == 4) then
		ChangeTexture(e, "dice_bolt")
	elseif (GetComponent(diceList[dicenumber], "C_DiceScript").value == 5) then
		ChangeTexture(e, "dice_bolt")
	elseif (GetComponent(diceList[dicenumber], "C_DiceScript").value == 6) then
		ChangeTexture(e, "dice_bolt")
	end
end 