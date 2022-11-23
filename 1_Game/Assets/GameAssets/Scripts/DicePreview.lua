--[IsComponent]
function C_DicePreview()
	local var = {
		--[SerializeString]
		fodder_text = "DicePreview is attached!",
	}
	return function()
		return var
	end
end

--[IsSystem]
function S_DicePreview(e)
	local transform = GetTransform(e)
	transform.rotation.x = transform.rotation.x + 1
	transform.rotation.y = transform.rotation.y + 1
	transform.rotation.z = transform.rotation.z + 1
end