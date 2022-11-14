--[IsComponent]
function C_DiceScript()
    local var = {
		--[SerializeString]
		fodder_text = "DiceScript is attached!",
		is_init = false,
		is_rolling = false,
		value = -1,
		gold_chance = 10,
		pink_chance = 40,
		blue_chance = 100,
		current_timer = 0,
		
		start_position_x = 0.0,
		start_position_y = 0.0,
		start_position_z = 0.0,
	
		position_x = 0.0,
		position_y = 0.0,
		position_z = 0.0,
		
		rotation_x = 0.0,
		rotation_y = 0.0,
		rotation_z = 0.0
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_DiceScript(e)
	local c_dice = GetComponent(e, "C_DiceScript")
	if (c_dice.is_init == false) then
		c_dice.fodder_text = "---DiceScript is running!"
		local transform = GetTransform(e)
		c_dice.start_position_x = transform.position.x
		c_dice.start_position_y = transform.position.y
		c_dice.start_position_z = transform.position.z
		ChangeTexture(e, "Dice_Blue")
		c_dice.current_timer = GenerateRandomNumberInRange(0, c_dice.blue_chance)
		DiceScript_DisableDice(e, c_dice)
		c_dice.is_init = true
	end
	
	if(CheckKeyPress(79) == true) then
		DiceScript_RollDice(e, c_dice)
		return
	elseif(CheckKeyPress(80) == true) then
		DiceScript_DisableDice(e, c_dice)
		return
	end
	
	local transform = GetTransform(e)

	if (c_dice.is_rolling == true) then
		if (c_dice.position_x == transform.position.x) then 
			if (c_dice.position_y == transform.position.y) then
				if (c_dice.position_z == transform.position.z) then
					if (c_dice.rotation_x == transform.rotation.x) then
						if (c_dice.rotation_y == transform.rotation.y) then
							if (c_dice.rotation_z == transform.rotation.z) then
								c_dice.is_rolling = false
								local top_index_position_y = -999
								for i = 0,5 do
									if (top_index_position_y < WorldPosition(Child(e, i)).y) then
										top_index_position_y = WorldPosition(Child(e, i)).y
										c_dice.value = i + 1
									end
								end
								print("[DiceScript] Dice value:", c_dice.value)
							end
						end
					end
				end
			end
		end
		
		if (transform.position.x > 10) then
			SetTransformPosition(e, 9.9, transform.position.y, transform.position.z)
		elseif (transform.position.x < -5) then
			SetTransformPosition(e, -4.9, transform.position.y, transform.position.z)
		end
		
		if (transform.position.z > 3) then
			SetTransformPosition(e, transform.position.x, transform.position.y, 2.9)
		elseif (transform.position.z < -11) then
			SetTransformPosition(e, transform.position.x, transform.position.y, -10.9)
		end

		c_dice.position_x = transform.position.x
		c_dice.position_y = transform.position.y
		c_dice.position_z = transform.position.z
		c_dice.rotation_x = transform.rotation.x
		c_dice.rotation_y = transform.rotation.y
		c_dice.rotation_z = transform.rotation.z
		
		c_dice.current_timer = c_dice.current_timer - (10 * GetDeltaTime())
		if (c_dice.current_timer < 0) then
			c_dice.current_timer = c_dice.current_timer + c_dice.blue_chance
			ChangeTexture(e, "Dice_Blue")
		elseif (c_dice.current_timer < c_dice.gold_chance) then
			ChangeTexture(e, "Dice_Gold")
		elseif (c_dice.current_timer < c_dice.pink_chance) then
			ChangeTexture(e, "Dice_Pink")
		end
	end
end

function DiceScript_RollDice(e, c_dice)
	print("[DiceScript] Roll Dice")
	c_dice.is_rolling = true
	FreezeObject(e, false)
	local transform = GetTransform(e)
	transform.rotation.x = GenerateRandomNumberInRange(0,360)
	transform.rotation.y = GenerateRandomNumberInRange(0,360)
	transform.rotation.z = GenerateRandomNumberInRange(0,360)
	--AddForce(e, 100.0, 100.0, 100.0)
	c_dice.current_timer = GenerateRandomNumberInRange(0, c_dice.blue_chance)
	SetTransformPosition(e, c_dice.start_position_x, c_dice.start_position_y, c_dice.start_position_z)
end

function DiceScript_DisableDice(e, c_dice)
	print("[DiceScript] Disabled Dice")
	c_dice.is_rolling = false
	c_dice.value = -1
	FreezeObject(e, true)
	SetTransformPosition(e, 999, 999, 999)
end