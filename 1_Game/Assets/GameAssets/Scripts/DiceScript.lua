--[IsComponent]
function C_DiceScript()
    local var = {
		--[SerializeString]
		fodder_text = "DiceScript is attached!",
	
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
	local transform = GetTransform(e)
	local c_dice = GetComponent(e, "C_DiceScript")
	if (c_dice.position_x == transform.position.x) then 
		if (c_dice.position_y == transform.position.y) then
			if (c_dice.position_z == transform.position.z) then
				if (c_dice.rotation_x == transform.rotation.x) then
					if (c_dice.rotation_y == transform.rotation.y) then
						if (c_dice.rotation_z == transform.rotation.z) then
							print("DICE STOPPED!")
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
	
	print("DICE MOVING!")
end