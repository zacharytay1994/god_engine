--[IsComponent]
function C_DiceScript()
    local var = {
		--[SerializeString]
		fodder_text = "DiceScript is attached!",
		is_init = false,
		is_rolling = false,
		
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

local function DiceScript_RollDice(c_dice)
	c_dice.is_rolling = true
	print("ROLLING...")
	FreezeObject(e, false)
	rotation_x = math.random(0,360)
	rotation_y = math.random(0,360)
	rotation_z = math.random(0,360)
	SetTransformPosition(e, c_dice.start_position_x, c_dice.start_position_y, c_dice.start_position_z)
end

local function DiceScript_DisableDice(c_dice)
	c_dice.is_rolling = false
end

--[IsSystem]
function S_DiceScript(e)
	local c_dice = GetComponent(e, "C_DiceScript")
	if (c_dice.is_init == false) then
		c_dice.fodder_text = "DiceScript is running!"
		DiceScript_RollDice(c_dice)
		local transform = GetTransform(e)
		start_position_x = transform.position.x
		start_position_y = transform.position.y
		start_position_z = transform.position.z
		c_dice.is_init = true
	end
	
	local transform = GetTransform(e)

	if (c_dice.position_x == transform.position.x) then 
		if (c_dice.position_y == transform.position.y) then
			if (c_dice.position_z == transform.position.z) then
				if (c_dice.rotation_x == transform.rotation.x) then
					if (c_dice.rotation_y == transform.rotation.y) then
						if (c_dice.rotation_z == transform.rotation.z) then
							print("DICE STOPPED!")
							c_dice.is_rolling = false
							DiceScript_RollDice(c_dice)
							return
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

