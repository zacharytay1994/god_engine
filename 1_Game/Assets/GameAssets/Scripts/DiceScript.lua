--[IsComponent]
function C_DiceScript()
    local var = {
		--[SerializeString]
		fodder_text = "DiceScript is attached!",
		is_init = false,
		is_rolling = false,
		value = 0,
		
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
		c_dice.is_init = true
	end
	
	if(CheckKeyPress(79) == true) then
		DiceScript_RollDice(e, c_dice)
		return
	--elseif(CheckKeyPress(80) == true) then
	--	SetTransformPosition(e, c_dice.start_position_x, c_dice.start_position_y, c_dice.start_position_z)
	--	--DiceScript_DisableDice(e, c_dice)
	--	return
	end
	
	local transform = GetTransform(e)

	if (c_dice.position_x == transform.position.x) then 
		if (c_dice.position_y == transform.position.y) then
			if (c_dice.position_z == transform.position.z) then
				if (c_dice.rotation_x == transform.rotation.x) then
					if (c_dice.rotation_y == transform.rotation.y) then
						if (c_dice.rotation_z == transform.rotation.z) then
							if (c_dice.is_rolling == true) then
								c_dice.is_rolling = false
								print("Dice Stopped!")
								local top_index_position_y = -999
								for i = 0,5 do
									if (top_index_position_y < GetTransform(Child(e, i)).position.y) then
										top_index_position_y = GetTransform(Child(e, i)).position.y
										c_dice.value = i + 1
									end
								end
								print(c_dice.value)
								ChangeTexture(e, "Blue")
								return
							end
						end
					end
				end
			end
		end
	end
	
	c_dice.position_x = transform.position.x
	c_dice.position_y = transform.position.y
	c_dice.position_z = transform.position.z
	c_dice.rotation_x = transform.rotation.x
	c_dice.rotation_y = transform.rotation.y
	c_dice.rotation_z = transform.rotation.z
end

function DiceScript_RollDice(e, c_dice)
	c_dice.is_rolling = true
	local transform = GetTransform(e)
	transform.rotation.x = GenerateRandomNumberInRange(0,360)
	transform.rotation.y = GenerateRandomNumberInRange(0,360)
	transform.rotation.z = GenerateRandomNumberInRange(0,360)
	--AddForce(e, 100.0, 100.0, 100.0)
	SetTransformPosition(e, c_dice.start_position_x, c_dice.start_position_y, c_dice.start_position_z)
end

function DiceScript_DisableDice(e, c_dice)
	c_dice.is_rolling = false
	--FreezeObject(e, true)
end