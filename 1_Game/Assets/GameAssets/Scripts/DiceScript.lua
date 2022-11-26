--[IsComponent]
function C_DiceScript()
    local var = {
		--[SerializeString]
		fodder_text = "DiceScript is attached!",
		is_init = false,
		is_rolling = false,
		value = -1,
		color = 0,
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
		rotation_z = 0.0,
		
		dice_faces = { 0, 1, 2, 3, 4, 5 },
		dice_id = 0
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_DiceScript(e)
	local c_dice = GetComponent(e, "C_DiceScript")

	if (c_dice.is_init == false) then
		c_dice.fodder_text = "DiceScript is running!"
		local diceList = EntitiesWithScriptComponent("C_DiceScript")
		for i = 1, #diceList do
			if (diceList[i] == e) then
				c_dice.dice_id = i
				if (i == 1) then
					c_dice.dice_faces = dice_1_faces
				elseif (i == 2) then
					c_dice.dice_faces = dice_2_faces
				elseif (i == 3) then
					c_dice.dice_faces = dice_3_faces
				end
				break
			end
		end
		local transform = GetTransform(e)
		c_dice.start_position_x = transform.position.x
		c_dice.start_position_y = transform.position.y
		c_dice.start_position_z = transform.position.z
		ChangeTexture(e, "Pixel_Blue")
		for i = 0,5 do
			if (c_dice.dice_faces[i+1] == 0) then
				ChangeModel(Child(e, i), "dice_jab3D")
			elseif (c_dice.dice_faces[i+1] == 1) then
				ChangeModel(Child(e, i), "dice_swing3D")
			elseif (c_dice.dice_faces[i+1] == 2) then
				ChangeModel(Child(e, i), "dice_smash3D")
			elseif (c_dice.dice_faces[i+1] == 3) then
				ChangeModel(Child(e, i), "dice_bolt3D")
			elseif (c_dice.dice_faces[i+1] == 4) then
				ChangeModel(Child(e, i), "dice_projectile3D")
			elseif (c_dice.dice_faces[i+1] == 6) then
				ChangeModel(Child(e, i), "dice_cryogenesis3D")
			end
		end
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
	local isdicesleeping = GetisSleeping(e)

	if (c_dice.is_rolling == true) then
		if (isdicesleeping == true) then 
			c_dice.is_rolling = false
			local top_index_position_y = -999
			for i = 0,5 do
				if (top_index_position_y < WorldPosition(Child(e, i)).y) then
					top_index_position_y = WorldPosition(Child(e, i)).y
					-- c_dice.value = i + 1
					c_dice.value = c_dice.dice_faces[i+1]
				end
			end
			print("[DiceScript] Dice value:", c_dice.value)
			print("[DiceScript] Dice color:", c_dice.color)				
		end
		
		if (transform.position.x > 10) then
			SetTransformPosition(e, 9.9, transform.position.y, transform.position.z)
			SetVelocity(e, -1, 0, 0)
		elseif (transform.position.x < -5) then
			SetTransformPosition(e, -4.9, transform.position.y, transform.position.z)
			SetVelocity(e, 1, 0, 0)
		end
		
		if (transform.position.z > 3) then
			SetTransformPosition(e, transform.position.x, transform.position.y, 2.9)
			SetVelocity(e, 0, 0, -1)
		elseif (transform.position.z < -11) then
			SetTransformPosition(e, transform.position.x, transform.position.y, -10.9)
			SetVelocity(e, 0, 0, 1)
		end

		c_dice.position_x = transform.position.x
		c_dice.position_y = transform.position.y
		c_dice.position_z = transform.position.z
		c_dice.rotation_x = transform.rotation.x
		c_dice.rotation_y = transform.rotation.y
		c_dice.rotation_z = transform.rotation.z
		
		c_dice.current_timer = c_dice.current_timer - (5 * GetDeltaTime())
		if (c_dice.current_timer < 0) then
			c_dice.current_timer = c_dice.current_timer + c_dice.blue_chance
			c_dice.color = 1
			ChangeTexture(e, "Pixel_Blue")
		elseif (c_dice.current_timer < c_dice.gold_chance) then
			c_dice.color = 3
			ChangeTexture(e, "Pixel_Gold")
		elseif (c_dice.current_timer < c_dice.pink_chance) then
			c_dice.color = 2
			ChangeTexture(e, "Pixel_Pink")
		end
	end
end

function DiceScript_RollDice(e, c_dice)
	print("[DiceScript] Roll Dice")
	c_dice.is_rolling = true
	c_dice.value = -1
	FreezeObject(e, false)
	local transform = GetTransform(e)
	transform.rotation.x = GenerateRandomNumberInRange(0,360)
	transform.rotation.y = GenerateRandomNumberInRange(0,360)
	transform.rotation.z = GenerateRandomNumberInRange(0,360)
	c_dice.current_timer = GenerateRandomNumberInRange(0, c_dice.blue_chance)
	if (c_dice.current_timer < c_dice.gold_chance) then
		c_dice.color = 3
		ChangeTexture(e, "Pixel_Gold")
	elseif (c_dice.current_timer < c_dice.pink_chance) then
		c_dice.color = 2
		ChangeTexture(e, "Pixel_Pink")
	else
		c_dice.color = 1
		ChangeTexture(e, "Pixel_Blue")
	end
	SetTransformPosition(e, c_dice.start_position_x, c_dice.start_position_y, c_dice.start_position_z)
end

function DiceScript_DisableDice(e, c_dice)
	print("[DiceScript] Disabled Dice")
	c_dice.is_rolling = false
	c_dice.value = -1
	c_dice.color = 0
	FreezeObject(e, true)
	SetTransformPosition(e, 999, 999, 999)

end