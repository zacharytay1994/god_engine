--[IsComponent]
function C_DicePreviewEdit()
	local var = {
		is_init = false,
		dice_faces = {0, 0, 0, 0, 0, 0}
	}
	return function()
		return var
	end
end

--[IsSystem]
function S_DicePreviewEdit(e)
	local c_dice = GetComponent(e, "C_DicePreviewEdit")
	if (c_dice.is_init == false) then
		
		if (EntityName(e) == "DicePreviewEdit") then
			dice_faces = dice_1_faces
		elseif (EntityName(e) == "DicePreviewEdit2") then
			dice_faces = dice_2_faces
		elseif (EntityName(e) == "DicePreviewEdit3") then
			dice_faces = dice_3_faces
		end
		
		for i = 0,5 do
			if (dice_faces[i] == 0) then
				ChangeTexture(Child(e, i), "dice_jab")
			elseif (dice_faces[i] == 1) then
				ChangeTexture(Child(e, i), "dice_swing")
			elseif (dice_faces[i] == 2) then
				ChangeTexture(Child(e, i), "dice_smash")
			elseif (dice_faces[i] == 3) then
				ChangeTexture(Child(e, i), "dice_bolt")
			elseif (dice_faces[i] == 4) then
				-- ChangeTexture(Child(e, i), "dice_projectile3D")
			elseif (dice_faces[i] == 5) then
				-- ChangeTexture(Child(e, i), "dice_corporikinesis3D")
			end
		end

		c_dice.is_init = true
	end
	
	if gui_object.pressed then
		
	end
	
end