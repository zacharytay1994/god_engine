--[IsComponent]
function C_RandomBubbles()
	local var = {
	}
	return function()
		return var
	end
end

function GenerateRandomSFX()
	local sfx = { "SFX_Bloop", "SFX_Click", "SFX_Jab" }
	
	local rng = GenerateRandomNumberInRange(1, 3)
	InstancePrefab(sfx[rng],0,0,0)
end

local coolDown = 0

--[IsSystem]
function S_RandomBubbles(e)
	local weights = { 3, 5, 10 }
	local sum = 0
	for index = 1, 3 do
		sum = sum + weights[index]
	end
	
	if coolDown <= 0 then
		local rng = GenerateRandomNumberInRange(1, sum)
		for index = 1, 3 do
			if rng < weights[index] then
				coolDown = weights[index]
				GenerateRandomSFX()
				break
			end
		end
	end
	
	coolDown = coolDown - GetDeltaTime()
end