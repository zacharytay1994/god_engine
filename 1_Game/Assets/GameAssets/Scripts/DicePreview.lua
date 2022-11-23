--[IsComponent]
function C_DicePreview()
	local var = {
		is_init = false,
		dice_faces = {0, 0, 0, 0, 0, 0}
	}
	return function()
		return var
	end
end

--[IsSystem]
function S_DicePreview(e)
	local c_dice = GetComponent(e, "C_DicePreview")
	if (c_dice.is_init == false) then

		if (EntityName(e) == "DicePreview" or EntityName(e) == "DicePreviewEdit") then
			c_dice.dice_faces = dice_1_faces
		elseif (EntityName(e) == "DicePreview2" or EntityName(e) == "DicePreviewEdit2") then
			c_dice.dice_faces = dice_2_faces
		elseif (EntityName(e) == "DicePreview3" or EntityName(e) == "DicePreviewEdit3") then
			c_dice.dice_faces = dice_3_faces
		end

		if (EntityName(e) == "DicePreviewEdit" or EntityName(e) == "DicePreviewEdit2" or EntityName(e) == "DicePreviewEdit3") then
			for i = 0,5 do
				if (c_dice.dice_faces[i+1] == 0) then
					ChangeTexture(Child(e, i), "dice_jab")
				elseif (c_dice.dice_faces[i+1] == 1) then
					ChangeTexture(Child(e, i), "dice_swing")
				elseif (c_dice.dice_faces[i+1] == 2) then
					ChangeTexture(Child(e, i), "dice_smash")
				elseif (c_dice.dice_faces[i+1] == 3) then
					ChangeTexture(Child(e, i), "dice_bolt")
				elseif (c_dice.dice_faces[i+1] == 4) then
					-- ChangeTexture(Child(e, i), "dice_projectile3D")
				elseif (c_dice.dice_faces[i+1] == 5) then
					-- ChangeTexture(Child(e, i), "dice_corporikinesis3D")
				end
			end
		else
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
					-- ChangeModel(Child(e, i), "dice_projectile3D")
				elseif (c_dice.dice_faces[i+1] == 5) then
					-- ChangeModel(Child(e, i), "dice_corporikinesis3D")
				end
			end
		end

		c_dice.is_init = true
	end
	
	if (EntityName(e) == "DicePreview" or EntityName(e) == "DicePreview2" or EntityName(e) == "DicePreview3") then
		local transform = GetTransform(e)
		transform.rotation.x = transform.rotation.x + 15.0 * GetDeltaTime()
		transform.rotation.y = transform.rotation.y + 15.0 * GetDeltaTime()
		transform.rotation.z = transform.rotation.z + 15.0 * GetDeltaTime()
	end
	
end