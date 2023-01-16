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
		
		if (#diceList < 3) then
			print("[DuceUI] ERROR! There should be 3 dice!")
			diceSettled = false
		end
		
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
				if (GetComponent(GetEntity("Player"), "C_Character").currentStamina >= 2) then
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
					
					GetComponent(GetEntity("Player"), "C_Character").currentStamina = GetComponent(GetEntity("Player"), "C_Character").currentStamina - 2
					
					-- trigger sound effect
					InstancePrefab("SFX_Reroll",0,0,0)
				end
				
			elseif c_dicemanager.button_name == "Ready" then
				local diceUIList = EntitiesWithScriptComponent("C_DiceUI")
				for i = 1, #diceUIList do
					GetComponent(diceUIList[i], "C_DiceUI").is_active = false
				end
				
				local diceList = EntitiesWithScriptComponent("C_DiceScript")
				for i = 1, #diceList do
					DiceScript_DisableDice(diceList[i], GetComponent(diceList[i], "C_DiceScript"))
				end
				
				-- trigger sound effect
				InstancePrefab("SFX_EndTurn",0,0,0)
			end
		end
	end
end

function SetDiceIcon(e, dicenumber)
	local diceList = EntitiesWithScriptComponent("C_DiceScript")
	local dice = GetComponent(diceList[dicenumber], "C_DiceScript")

	if (dice.color == 0) then
		print("[DiceUI] Error! Color should not be 0!")
	end

	if (dice.value == 0) then
		if (dice.color == 1) then
			ChangeTexture(e, "dice_jab")
		elseif (dice.color == 2) then
			ChangeTexture(e, "dice_jab_pink")
		elseif (dice.color == 3) then
			ChangeTexture(e, "dice_jab_gold")
		end
	elseif (dice.value == 1) then
		if (dice.color == 1) then
			ChangeTexture(e, "dice_swing")
		elseif (dice.color == 2) then
			ChangeTexture(e, "dice_swing_pink")
		elseif (dice.color == 3) then
			ChangeTexture(e, "dice_swing_gold")
		end
	elseif (dice.value == 2) then
		if (dice.color == 1) then
			ChangeTexture(e, "dice_smash")
		elseif (dice.color == 2) then
			ChangeTexture(e, "dice_smash_pink")
		elseif (dice.color == 3) then
			ChangeTexture(e, "dice_smash_gold")
		end
	elseif (dice.value == 3) then
		if (dice.color == 1) then
			ChangeTexture(e, "dice_bolt")
		elseif (dice.color == 2) then
			ChangeTexture(e, "dice_bolt_pink")
		elseif (dice.color == 3) then
			ChangeTexture(e, "dice_bolt_gold")
		end
	elseif (dice.value == 4) then
		if (dice.color == 1) then
			ChangeTexture(e, "dice_projectile")
		elseif (dice.color == 2) then
			ChangeTexture(e, "dice_projectile_pink")
		elseif (dice.color == 3) then
			ChangeTexture(e, "dice_projectile_gold")
		end
	elseif (dice.value == 6) then
		if (dice.color == 1) then
			ChangeTexture(e, "dice_cryogenesis")
		elseif (dice.color == 2) then
			ChangeTexture(e, "dice_cryogenesis_pink")
		elseif (dice.color == 3) then
			ChangeTexture(e, "dice_cryogenesis_gold")
		end
	end
end 